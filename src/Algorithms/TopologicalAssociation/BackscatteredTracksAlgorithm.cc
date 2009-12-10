/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/BackscatteredTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the backscattered tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/BackscatteredTracksAlgorithm.h"

using namespace pandora;

StatusCode BackscatteredTracksAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iterI = pClusterList->begin(); iterI != pClusterList->end();)
    {
        Cluster *pDaughterCluster = *iterI;
        ++iterI;

        if (pDaughterCluster->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(pDaughterCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        if (!pDaughterCluster->GetFitToAllHitsResult().IsFitSuccessful() || (pDaughterCluster->GetFitToAllHitsResult().GetRms() > m_fitToAllHitsRmsCut))
            continue;

        // Fit all but the n outermost layers of the daughter cluster candidate
        const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());
        const PseudoLayer daughterOuterLayer(pDaughterCluster->GetOuterPseudoLayer());
        const PseudoLayer daughterOuterFitLayer((daughterOuterLayer > m_nOuterFitExclusionLayers) ? daughterOuterLayer - m_nOuterFitExclusionLayers : 0);

        ClusterHelper::ClusterFitResult daughterClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitLayers(pDaughterCluster, daughterInnerLayer, daughterOuterFitLayer, daughterClusterFitResult))
            continue;

        Cluster *pBestParentCluster = NULL;
        float minFitDistanceToClosestHit = std::numeric_limits<float>::max();

        // Find the most plausible parent cluster, with the smallest distance to the projection of the daughter cluster fit
        for (ClusterList::const_iterator iterJ = pClusterList->begin(); iterJ != pClusterList->end(); ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            if (pDaughterCluster == pParentCluster)
                continue;

            if (!ClusterHelper::CanMergeCluster(pParentCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
                continue;

            if ((pParentCluster->GetOuterPseudoLayer() <= daughterOuterLayer) || (pParentCluster->GetInnerPseudoLayer() >= daughterOuterLayer))
                continue;

            // Cut on the closest approach within a layer between parent cluster candidate and the daughter cluster
            float intraLayerDistance(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetClosestIntraLayerDistance(pParentCluster, pDaughterCluster, intraLayerDistance))
                continue;

            if (intraLayerDistance > m_maxIntraLayerDistance)
                continue;

            // Calculate the smallest distance between the projected daughter cluster fit and the parent cluster candidate
            const PseudoLayer fitProjectionInnerLayer((daughterOuterLayer > m_nFitProjectionLayers) ? daughterOuterLayer - m_nFitProjectionLayers : 0);
            const PseudoLayer fitProjectionOuterLayer(daughterOuterLayer + m_nFitProjectionLayers);

            const float fitDistanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(daughterClusterFitResult, pParentCluster, fitProjectionInnerLayer, fitProjectionOuterLayer));
            if (fitDistanceToClosestHit < minFitDistanceToClosestHit)
            {
                minFitDistanceToClosestHit = fitDistanceToClosestHit;
                pBestParentCluster = pParentCluster;
            }
        }

        // If parent cluster found, merge the clusters
        if (pBestParentCluster != NULL)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BackscatteredTracksAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_canMergeMinMipFraction = 0.7;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_minCaloHitsPerCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerCluster", m_minCaloHitsPerCluster));

    m_fitToAllHitsRmsCut = 10.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitToAllHitsRmsCut", m_fitToAllHitsRmsCut));

    m_nOuterFitExclusionLayers = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NOuterFitExclusionLayers", m_nOuterFitExclusionLayers));

    m_nFitProjectionLayers = 2; // TODO alter this name
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NFitProjectionLayers", m_nFitProjectionLayers));

    m_maxIntraLayerDistance = 100.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxIntraLayerDistance", m_maxIntraLayerDistance));

    return STATUS_CODE_SUCCESS;
}
