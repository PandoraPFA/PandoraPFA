/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/ShowerMipMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the shower mip merging algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/ShowerMipMergingAlgorithm.h"

using namespace pandora;

StatusCode ShowerMipMergingAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iterI = pClusterList->begin(); iterI != pClusterList->end(); ++iterI)
    {
        Cluster *pClusterI = *iterI;

        if (pClusterI->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(pClusterI, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        // Determine whether cluster is a plausible mip candidate
        if (pClusterI->GetMipFraction() < m_mipFractionCut)
            continue;

        if (!pClusterI->GetFitToAllHitsResult().IsFitSuccessful() || (pClusterI->GetFitToAllHitsResult().GetRms() > m_fitToAllHitsRmsCut))
            continue;

        ClusterHelper::ClusterFitResult clusterFitResultI;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitEnd(pClusterI, m_nPointsToFit, clusterFitResultI))
            continue;

        const PseudoLayer innerLayerI(pClusterI->GetInnerPseudoLayer());
        const PseudoLayer outerLayerI(pClusterI->GetOuterPseudoLayer());

        // Compare this mip candidate cluster with all other clusters
        for (ClusterList::const_iterator iterJ = pClusterList->begin(); iterJ != pClusterList->end(); ++iterJ)
        {
            Cluster *pClusterJ = *iterJ;

            if (pClusterI == pClusterJ)
                continue;

            if (pClusterJ->GetNCaloHits() < m_minCaloHitsPerCluster)
                continue;

            if (!ClusterHelper::CanMergeCluster(pClusterJ, m_canMergeMinMipFraction, m_canMergeMaxRms))
                continue;

            // Check mip candidate cluster has origin closest to IP
            const PseudoLayer innerLayerJ(pClusterJ->GetInnerPseudoLayer());

            if (innerLayerJ >= innerLayerI)
                continue;

            // Cut on physical separation of clusters
            const CartesianVector centroidDifference(pClusterI->GetCentroid(outerLayerI) - pClusterJ->GetCentroid(innerLayerJ));
            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Cut on distance between projected fit result and nearest cluster hit
            const float distanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(clusterFitResultI, pClusterJ, outerLayerI, outerLayerI + m_nFitProjectionLayers));
            if (distanceToClosestHit > m_maxDistanceToClosestHit)
                continue;

            // Also cut on distance between projected fit result and nearest cluster centroid
            const float distanceToClosestCentroid(ClusterHelper::GetDistanceToClosestCentroid(clusterFitResultI, pClusterJ, outerLayerI, outerLayerI + m_nFitProjectionLayers));
            if (distanceToClosestCentroid < m_maxDistanceToClosestCentroid)
            {
                // TODO decide whether to continue loop over daughter cluster candidates after merging
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pClusterI, pClusterJ));
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_canMergeMinMipFraction = 0.7;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_nPointsToFit = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NPointsToFit", m_nPointsToFit));

    m_minCaloHitsPerCluster = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerCluster", m_minCaloHitsPerCluster));

    m_mipFractionCut = 0.5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipFractionCut", m_mipFractionCut));

    m_fitToAllHitsRmsCut = 10.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitToAllHitsRmsCut", m_fitToAllHitsRmsCut));

    m_maxCentroidDifference = 1000.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    m_nFitProjectionLayers = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NFitProjectionLayers", m_nFitProjectionLayers));

    m_maxDistanceToClosestHit = 100.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxDistanceToClosestHit", m_maxDistanceToClosestHit));

    m_maxDistanceToClosestCentroid = 25.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxDistanceToClosestCentroid", m_maxDistanceToClosestCentroid));

    return STATUS_CODE_SUCCESS;
}
