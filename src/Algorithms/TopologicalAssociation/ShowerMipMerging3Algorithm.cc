/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/ShowerMipMerging3Algorithm.cc
 * 
 *  @brief  Implementation of the shower mip merging 3 algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/ShowerMipMerging3Algorithm.h"

#include <limits>

using namespace pandora;

StatusCode ShowerMipMerging3Algorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iterI = pClusterList->begin(); iterI != pClusterList->end();)
    {
        Cluster *pDaughterCluster = *iterI;
        ++iterI;

        // Identify a possible mip-stub daughter cluster
        if (pDaughterCluster->GetNCaloHits() < m_minCaloHitsPerDaughterCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(pDaughterCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        ClusterHelper::ClusterFitResult daughterClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitEnd(pDaughterCluster, m_nPointsToFit, daughterClusterFitResult))
            continue;

        // Mip stub cluster must be consistent with a straight line fit
        if (daughterClusterFitResult.GetChi2() > m_maxFitChi2)
            continue;

        Cluster *pBestParentCluster = NULL;
        float closestClusterApproach = std::numeric_limits<float>::max();

        const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

        // Find the closest plausible parent cluster, with the smallest cluster approach distance
        for (ClusterList::const_iterator iterJ = pClusterList->begin(); iterJ != pClusterList->end(); ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            if (pDaughterCluster == pParentCluster)
                continue;

            if (pParentCluster->GetNCaloHits() < m_minCaloHitsPerParentCluster)
                continue;

            // We are looking for small mip stub clusters emerging from the parent shower-like cluster
            const PseudoLayer parentOuterLayer(pParentCluster->GetOuterPseudoLayer());

            if (daughterInnerLayer < parentOuterLayer)
                continue;

            // Cut on distance between projected fit result and nearest cluster hit
            const PseudoLayer fitProjectionInnerLayer((parentOuterLayer > m_nFitProjectionLayers) ? parentOuterLayer - m_nFitProjectionLayers : 0);
            const float fitDistanceToClosestHit(ClusterHelper::GetDistanceToClosestHit(daughterClusterFitResult, pParentCluster, fitProjectionInnerLayer, parentOuterLayer));
            if (fitDistanceToClosestHit < m_maxFitDistanceToClosestHit)
                continue;

            // Cluster approach is the smallest distance between a hit in daughter cluster and a hit in parent cluster
            const float clusterApproach(ClusterHelper::GetDistanceToClosestHit(pDaughterCluster, pParentCluster));
            if (clusterApproach < closestClusterApproach)
            {
                closestClusterApproach = clusterApproach;
                pBestParentCluster = pParentCluster;
            }
        }

        // If parent cluster found, within threshold approach distance, merge the clusters
        if ((closestClusterApproach < m_closestClusterApproachCut) && (pBestParentCluster != NULL))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMerging3Algorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_minCaloHitsPerDaughterCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerDaughterCluster", m_minCaloHitsPerDaughterCluster));

    m_minCaloHitsPerParentCluster = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerParentCluster", m_minCaloHitsPerParentCluster));

    m_nPointsToFit = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NPointsToFit", m_nPointsToFit));

    m_maxFitChi2 = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitChi2", m_maxFitChi2));

    m_nFitProjectionLayers = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NFitProjectionLayers", m_nFitProjectionLayers));

    m_maxFitDistanceToClosestHit = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitDistanceToClosestHit", m_maxFitDistanceToClosestHit));

    m_closestClusterApproachCut = 30.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestClusterApproachCut", m_closestClusterApproachCut));

    return STATUS_CODE_SUCCESS;
}
