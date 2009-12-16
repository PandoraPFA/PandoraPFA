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

    for (ClusterList::const_iterator iterI = pClusterList->begin(); iterI != pClusterList->end(); ++iterI)
    {
        Cluster *pParentCluster = *iterI;

        if (pParentCluster->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(pParentCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        // Fit the parent cluster candidate from its innermost layer to its showermax layer
        const PseudoLayer parentInnerLayer(pParentCluster->GetInnerPseudoLayer());
        const PseudoLayer parentShowerMaxLayer(pParentCluster->GetShowerMaxLayer());

        ClusterHelper::ClusterFitResult parentClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitLayers(pParentCluster, parentInnerLayer, parentShowerMaxLayer, parentClusterFitResult))
            continue;

        if (parentClusterFitResult.GetRms() > m_maxFitRms)
            continue;

        // Find a compatible daughter cluster
        for (ClusterList::const_iterator iterJ = pClusterList->begin(); iterJ != pClusterList->end();)
        {
            Cluster *pDaughterCluster = *iterJ;
            ++iterJ;

            if (pParentCluster == pDaughterCluster)
                continue;

            if (!ClusterHelper::CanMergeCluster(pDaughterCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
                continue;

            // Backscattered particle is expected to be daughter of a parent mip section; cut on overlap between relevant layers
            const PseudoLayer daughterOuterLayer(pDaughterCluster->GetOuterPseudoLayer());

            if ((parentShowerMaxLayer <= daughterOuterLayer) || (parentInnerLayer >= daughterOuterLayer))
                continue;

            // Cut on the distance of closest approach between the fit to the parent cluster and the daughter cluster candidate
            const PseudoLayer fitProjectionInnerLayer((daughterOuterLayer > m_nFitProjectionLayers) ? daughterOuterLayer - m_nFitProjectionLayers : 0);
            const float fitDistanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(parentClusterFitResult, pDaughterCluster, fitProjectionInnerLayer, daughterOuterLayer));

            if (fitDistanceToClosestHit > m_maxFitDistanceToClosestHit)
                continue;

            // Cut on the closest approach within a layer between parent cluster and the daughter cluster candidate
            float intraLayerDistance(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetClosestIntraLayerDistance(pParentCluster, pDaughterCluster, intraLayerDistance))
                continue;

            if (intraLayerDistance < m_maxIntraLayerDistance)
            {
                // TODO decide whether to continue loop over daughter cluster candidates after merging
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, pDaughterCluster));
            }
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
