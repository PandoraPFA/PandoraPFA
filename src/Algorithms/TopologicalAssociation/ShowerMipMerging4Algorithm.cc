/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/ShowerMipMerging4Algorithm.cc
 * 
 *  @brief  Implementation of the shower mip merging 4 algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/ShowerMipMerging4Algorithm.h"

#include <limits>

using namespace pandora;

const float ShowerMipMerging4Algorithm::FLOAT_MAX = std::numeric_limits<float>::max();

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMerging4Algorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iterI = pClusterList->begin(); iterI != pClusterList->end();)
    {
        Cluster *pDaughterCluster = *iterI;
        ++iterI;

        if (pDaughterCluster->GetNCaloHits() < m_minCaloHitsPerDaughterCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(pDaughterCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        Cluster *pBestParentCluster = NULL;
        float minProjectionDistance = std::numeric_limits<float>::max();

        const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

        // Find the closest plausible parent cluster, with the smallest cluster approach distance
        for (ClusterList::const_iterator iterJ = pClusterList->begin(); iterJ != pClusterList->end(); ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            if (pDaughterCluster == pParentCluster)
                continue;

            if (pParentCluster->GetNCaloHits() < m_minCaloHitsPerParentCluster)
                continue;

            // Cut on layer separation between the two clusters
            const PseudoLayer parentOuterLayer(pParentCluster->GetOuterPseudoLayer());

            if ((daughterInnerLayer > parentOuterLayer) || ((parentOuterLayer - daughterInnerLayer) > m_maxLayerDifference))
                continue;

            // Get the smallest distance between a hit in the daughter cluster and the projected initial direction of the parent cluster
            const float projectionDistance(this->GetDistanceFromInitialProjection(pParentCluster, pDaughterCluster));
            if (projectionDistance < minProjectionDistance)
            {
                minProjectionDistance = projectionDistance;
                pBestParentCluster = pParentCluster;
            }
        }

        if ((pBestParentCluster == NULL) || (minProjectionDistance > m_maxProjectionDistance))
            continue;

        // Check closest approach within a layer between best parent cluster and the daughter cluster
        float intraLayerDistance(FLOAT_MAX);
        if (STATUS_CODE_SUCCESS != ClusterHelper::GetClosestIntraLayerDistance(pBestParentCluster, pDaughterCluster, intraLayerDistance))
            continue;

        if (intraLayerDistance < m_maxIntraLayerDistance)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ShowerMipMerging4Algorithm::GetDistanceFromInitialProjection(const Cluster *const pClusterToProject, const Cluster *const pClusterToExamine) const
{
    float minDistance(FLOAT_MAX);
    const OrderedCaloHitList &orderedCaloHitList(pClusterToExamine->GetOrderedCaloHitList());

    const CartesianVector innerCentroidI(pClusterToProject->GetCentroid(pClusterToProject->GetInnerPseudoLayer()));
    const CartesianVector &projectedDirection(pClusterToProject->GetInitialDirection());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector separation((*hitIter)->GetPositionVector() - innerCentroidI);
            const CartesianVector directionCrossSeparation(projectedDirection.GetCrossProduct(separation));

            const float perpendicularDistance(directionCrossSeparation.GetMagnitude());
            const float parallelDistance(projectedDirection.GetDotProduct(separation));

            if ((0 == parallelDistance) || ((perpendicularDistance / parallelDistance) > m_maxProjectionDistanceRatio))
                continue;

            if (perpendicularDistance < minDistance)
            {
                minDistance = perpendicularDistance;
            }
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ShowerMipMerging4Algorithm::ReadSettings(const TiXmlHandle xmlHandle)
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

    m_minCaloHitsPerParentCluster = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerParentCluster", m_minCaloHitsPerParentCluster));

    m_maxLayerDifference = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayerDifference", m_maxLayerDifference));

    m_maxProjectionDistance = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxProjectionDistance", m_maxProjectionDistance));

    m_maxProjectionDistanceRatio = 0.9f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxProjectionDistanceRatio", m_maxProjectionDistanceRatio));

    m_maxIntraLayerDistance = 500.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxIntraLayerDistance", m_maxIntraLayerDistance));

    return STATUS_CODE_SUCCESS;
}
