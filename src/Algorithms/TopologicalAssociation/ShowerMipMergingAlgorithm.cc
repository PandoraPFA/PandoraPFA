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

        if ((pClusterI->GetNCaloHits() < m_minCaloHitsPerCluster) || !this->CanMergeCluster(pClusterI))
            continue;

        // Determine whether cluster is a plausible mip candidate
        if ((pClusterI->GetMipFraction() < m_mipFractionCut) || (pClusterI->GetFitToAllHitsResult().GetRms() > m_fitToAllHitsRmsCut))
            continue;

        ClusterFitResult clusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitEnd(pClusterI, m_nPointsToFit, clusterFitResult))
            continue;

        const PseudoLayer innerLayerI(pClusterI->GetInnerPseudoLayer());
        const PseudoLayer outerLayerI(pClusterI->GetOuterPseudoLayer());

        // Compare this mip candidate cluster with all other clusters
        for (ClusterList::const_iterator iterJ = pClusterList->begin(); iterJ != pClusterList->end(); ++iterJ)
        {
            Cluster *pClusterJ = *iterJ;

            if (pClusterI == pClusterJ)
                continue;

            if ((pClusterJ->GetNCaloHits() < m_minCaloHitsPerCluster) || !this->CanMergeCluster(pClusterJ))
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
            const float distanceToClosestHit(this->GetDistanceToClosestHit(clusterFitResult, pClusterJ, outerLayerI, outerLayerI + m_nFitProjectionLayers));
            if (distanceToClosestHit > m_maxDistanceToClosestHit)
                continue;

            // Also cut on distance between projected fit result and nearest cluster centroid
            const float distanceToClosestCentroid(this->GetDistanceToClosestCentroid(clusterFitResult, pClusterJ, outerLayerI, outerLayerI + m_nFitProjectionLayers));
            if (distanceToClosestCentroid < m_maxDistanceToClosestCentroid)
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pClusterI, pClusterJ));
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ShowerMipMergingAlgorithm::CanMergeCluster(Cluster *const pCluster) const
{
    const bool canMerge(!pCluster->IsPhoton() ||
        (pCluster->GetMipFraction() > m_canMergeMinMipFraction) ||
        (pCluster->GetFitToAllHitsResult().GetRms() < m_canMergeMaxRms));

    return canMerge;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ShowerMipMergingAlgorithm::GetDistanceToClosestHit(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
    PseudoLayer startLayer, PseudoLayer endLayer) const
{
    float minDistance = std::numeric_limits<float>::max();
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const CartesianVector interceptDifference(pCaloHit->GetPositionVector() - clusterFitResult.GetIntercept());
            const float distance(interceptDifference.GetCrossProduct(clusterFitResult.GetDirection()).GetMagnitude());

            if (distance < minDistance)
            {
                minDistance = distance;
            }
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ShowerMipMergingAlgorithm::GetDistanceToClosestCentroid(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
    PseudoLayer startLayer, PseudoLayer endLayer) const
{
    float minDistance = std::numeric_limits<float>::max();
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        const CartesianVector interceptDifference(pCluster->GetCentroid(iLayer) - clusterFitResult.GetIntercept());
        const float distance(interceptDifference.GetCrossProduct(clusterFitResult.GetDirection()).GetMagnitude());

        if (distance < minDistance)
        {
            minDistance = distance;
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMergingAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
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
