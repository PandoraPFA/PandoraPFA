/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/BackscatteredTracks2Algorithm.cc
 * 
 *  @brief  Implementation of the backscattered tracks 2 algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/BackscatteredTracks2Algorithm.h"

#include <limits>

using namespace pandora;

StatusCode BackscatteredTracks2Algorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    // Apply preselection and order clusters by inner layer
    ClusterVector clusterVector;
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        if (ClusterHelper::CanMergeCluster(*iter, m_canMergeMinMipFraction, m_canMergeMaxRms))
            clusterVector.push_back(*iter);
    }

    std::sort(clusterVector.begin(), clusterVector.end(), Cluster::SortByInnerLayer);

    // Loop over candidate daughter/parent cluster combinations
    for (ClusterVector::const_iterator iterI = clusterVector.begin(); iterI != clusterVector.end(); ++iterI)
    {
        Cluster *pParentCluster = *iterI;

        // Check to see if cluster has already been changed
        if (NULL == pParentCluster)
            continue;

        if (pParentCluster->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        // Fit the parent cluster candidate from its innermost layer to its showerstart layer
        const PseudoLayer parentInnerLayer(pParentCluster->GetInnerPseudoLayer());
        const PseudoLayer parentShowerStartLayer(pParentCluster->GetShowerStartLayer());

        ClusterHelper::ClusterFitResult parentClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitLayers(pParentCluster, parentInnerLayer, parentShowerStartLayer, parentClusterFitResult))
            continue;

        if (parentClusterFitResult.GetRms() > m_maxFitRms)
            continue;

        float minFitDistanceToClosestHit(std::numeric_limits<float>::max());
        ClusterVector::iterator bestDaughterClusterIter(clusterVector.end());

        // Find a compatible daughter cluster
        for (ClusterVector::iterator iterJ = clusterVector.begin(); iterJ != clusterVector.end(); ++iterJ)
        {
            Cluster *pDaughterCluster = *iterJ;

            // Check to see if cluster has already been changed
            if ((NULL == pDaughterCluster) || (pParentCluster == pDaughterCluster))
                continue;

            // Backscattered particle is expected to be daughter of a parent mip section; cut on overlap between relevant layers
            const PseudoLayer daughterOuterLayer(pDaughterCluster->GetOuterPseudoLayer());

            if ((parentShowerStartLayer <= daughterOuterLayer) || (parentInnerLayer >= daughterOuterLayer))
                continue;

            // Cut on the closest approach within a layer between parent cluster and the daughter cluster candidate
            float intraLayerDistance(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetClosestIntraLayerDistance(pParentCluster, pDaughterCluster, intraLayerDistance))
                continue;

            if (intraLayerDistance > m_maxIntraLayerDistance)
                continue;

            // Cut on the distance of closest approach between the fit to the parent cluster and the daughter cluster candidate
            const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());
            const PseudoLayer fitProjectionOuterLayer((daughterOuterLayer > m_nFitProjectionLayers) ? daughterOuterLayer - m_nFitProjectionLayers : 0);

            const float fitDistanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(parentClusterFitResult, pDaughterCluster,
                daughterInnerLayer, fitProjectionOuterLayer));

            if ((fitDistanceToClosestHit < m_maxFitDistanceToClosestHit) && (fitDistanceToClosestHit < minFitDistanceToClosestHit))
            {
                bestDaughterClusterIter = iterJ;
                minFitDistanceToClosestHit = fitDistanceToClosestHit;
            }
        }

        if (bestDaughterClusterIter != clusterVector.end())
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, *bestDaughterClusterIter));
            *bestDaughterClusterIter = NULL;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BackscatteredTracks2Algorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_minCaloHitsPerCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerCluster", m_minCaloHitsPerCluster));

    m_maxFitRms = 15.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitRms", m_maxFitRms));

    m_nFitProjectionLayers = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NFitProjectionLayers", m_nFitProjectionLayers));

    m_maxFitDistanceToClosestHit = 30.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitDistanceToClosestHit", m_maxFitDistanceToClosestHit));

    m_maxIntraLayerDistance = 1000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxIntraLayerDistance", m_maxIntraLayerDistance));

    return STATUS_CODE_SUCCESS;
}
