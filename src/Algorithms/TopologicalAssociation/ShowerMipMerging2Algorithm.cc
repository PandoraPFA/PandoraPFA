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

    const GeometryHelper *const pGeometryHelper(GeometryHelper::GetInstance());

    for (ClusterList::const_iterator iterI = pClusterList->begin(); iterI != pClusterList->end(); ++iterI)
    {
        Cluster *pClusterI = *iterI;

        if (pClusterI->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(pClusterI, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        if (!pClusterI->GetFitToAllHitsResult().IsFitSuccessful() || (pClusterI->GetFitToAllHitsResult().GetChi2() > m_fitToAllHitsChi2Cut))
            continue;

        ClusterHelper::ClusterFitResult clusterFitResultI;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitEnd(pClusterI, m_nPointsToFit, clusterFitResultI))
            continue;

        const PseudoLayer outerLayerI(pClusterI->GetOuterPseudoLayer());

        // Compare this successfully fitted cluster with all others
        for (ClusterList::const_iterator iterJ = pClusterList->begin(); iterJ != pClusterList->end();)
        {
            Cluster *pClusterJ = *iterJ;
            ++iterJ;

            if (pClusterI == pClusterJ)
                continue;

            if (pClusterJ->GetNCaloHits() < m_minCaloHitsPerCluster)
                continue;

            if (!ClusterHelper::CanMergeCluster(pClusterJ, m_canMergeMinMipFraction, m_canMergeMaxRms))
                continue;

            // Cut on layer separation between the two clusters
            const PseudoLayer innerLayerJ(pClusterJ->GetInnerPseudoLayer());

            if ((innerLayerJ <= outerLayerI) || ((innerLayerJ - outerLayerI) > m_maxLayerDifference))
                continue;

            // Also cut on physical separation between the two clusters
            const CartesianVector centroidDifference(pClusterI->GetCentroid(outerLayerI) - pClusterJ->GetCentroid(innerLayerJ));
            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Require clusters to point at one another
            if (centroidDifference.GetUnitVector().GetDotProduct(clusterFitResultI.GetDirection()) > m_maxFitDirectionDotProduct)
                continue;

            // Cut on perpendicular distance between fit direction and centroid difference vector.
            const bool isOutsideECalJ(pGeometryHelper->IsOutsideECal(pClusterJ->GetCentroid(innerLayerJ)));
            const float perpendicularDistanceCut(isOutsideECalJ ? m_perpendicularDistanceCutHcal : m_perpendicularDistanceCutEcal);

            const CartesianVector fitICrossCentroidDifference(clusterFitResultI.GetDirection().GetCrossProduct(centroidDifference));
            const float fitIPerpendicularDistance(fitICrossCentroidDifference.GetMagnitude());

            if (fitIPerpendicularDistance < perpendicularDistanceCut)
            {
                // TODO decide whether to continue loop over daughter cluster candidates after merging
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pClusterI, pClusterJ));
            }
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

    m_minCaloHitsPerCluster = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerCluster", m_minCaloHitsPerCluster));

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
