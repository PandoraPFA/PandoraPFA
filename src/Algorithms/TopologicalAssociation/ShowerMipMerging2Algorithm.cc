/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/ShowerMipMerging2Algorithm.cc
 * 
 *  @brief  Implementation of the shower mip merging 2 algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/ShowerMipMerging2Algorithm.h"

#include <cmath>

using namespace pandora;

StatusCode ShowerMipMerging2Algorithm::Run()
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

    static const GeometryHelper *const pGeometryHelper(GeometryHelper::GetInstance());

    // Loop over all candidate parent clusters
    for (ClusterVector::const_iterator iterI = clusterVector.begin(); iterI != clusterVector.end(); ++iterI)
    {
        Cluster *pParentCluster = *iterI;

        // Check to see if cluster has already been changed
        if (NULL == pParentCluster)
            continue;

        if ((pParentCluster->GetNCaloHits() < m_minHitsInCluster) || (pParentCluster->GetOrderedCaloHitList().size() < m_minOccupiedLayersInCluster))
            continue;

        if (!pParentCluster->GetFitToAllHitsResult().IsFitSuccessful() || (pParentCluster->GetFitToAllHitsResult().GetChi2() > m_fitToAllHitsChi2Cut))
            continue;

        ClusterHelper::ClusterFitResult parentClusterFitResult;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitEnd(pParentCluster, m_nPointsToFit, parentClusterFitResult))
            continue;

        const PseudoLayer parentOuterLayer(pParentCluster->GetOuterPseudoLayer());
        const CartesianVector parentOuterCentroid(pParentCluster->GetCentroid(parentOuterLayer));

        float minPerpendicularDistance(std::numeric_limits<float>::max());
        ClusterVector::iterator bestDaughterClusterIter(clusterVector.end());

        // Compare this successfully fitted cluster with all others
        for (ClusterVector::iterator iterJ = clusterVector.begin(); iterJ != clusterVector.end(); ++iterJ)
        {
            Cluster *pDaughterCluster = *iterJ;

            // Check to see if cluster has already been changed
            if ((NULL == pDaughterCluster) || (pParentCluster == pDaughterCluster))
                continue;

            // Cut on layer separation between the two clusters
            const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

            if ((daughterInnerLayer <= parentOuterLayer) || ((daughterInnerLayer - parentOuterLayer) > m_maxLayerDifference))
                continue;

            // Also cut on physical separation between the two clusters
            const CartesianVector daughterInnerCentroid(pDaughterCluster->GetCentroid(daughterInnerLayer));
            const CartesianVector centroidDifference(parentOuterCentroid - daughterInnerCentroid);

            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Require clusters to point at one another
            if (centroidDifference.GetUnitVector().GetDotProduct(parentClusterFitResult.GetDirection()) > m_maxFitDirectionDotProduct)
                continue;

            // Cut on perpendicular distance between fit direction and centroid difference vector.
            const bool isDaughterOutsideECal(pGeometryHelper->IsOutsideECal(daughterInnerCentroid));
            const float perpendicularDistanceCut(isDaughterOutsideECal ? m_perpendicularDistanceCutHcal : m_perpendicularDistanceCutEcal);

            const CartesianVector parentCrossProduct(parentClusterFitResult.GetDirection().GetCrossProduct(centroidDifference));
            const float perpendicularDistance(parentCrossProduct.GetMagnitude());

            if ((perpendicularDistance < perpendicularDistanceCut) && (perpendicularDistance < minPerpendicularDistance))
            {
                bestDaughterClusterIter = iterJ;
                minPerpendicularDistance = perpendicularDistance;
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

StatusCode ShowerMipMerging2Algorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_minHitsInCluster = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    m_minOccupiedLayersInCluster = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersInCluster", m_minOccupiedLayersInCluster));

    m_fitToAllHitsChi2Cut = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitToAllHitsChi2Cut", m_fitToAllHitsChi2Cut));

    m_nPointsToFit = 8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NPointsToFit", m_nPointsToFit));

    m_maxLayerDifference = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayerDifference", m_maxLayerDifference));

    m_maxCentroidDifference = 2000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    m_maxFitDirectionDotProduct = -0.8f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitDirectionDotProduct", m_maxFitDirectionDotProduct));

    m_perpendicularDistanceCutEcal = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PerpendicularDistanceCutEcal", m_perpendicularDistanceCutEcal));

    m_perpendicularDistanceCutHcal = 75.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PerpendicularDistanceCutHcal", m_perpendicularDistanceCutHcal));

    return STATUS_CODE_SUCCESS;
}
