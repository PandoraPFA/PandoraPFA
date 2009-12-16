/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the broken tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.h"

#include <cmath>

using namespace pandora;

StatusCode BrokenTracksAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    const GeometryHelper *const pGeometryHelper(GeometryHelper::GetInstance());

    // Fit a straight line to start and end of all clusters in the current list
    ClusterFitResultMap startClusterFitResultMap;
    ClusterFitResultMap endClusterFitResultMap;

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        ClusterFitResult startClusterFitResult;
        (void) ClusterHelper::FitStart(*iter, m_nStartLayersToFit, startClusterFitResult);

        if (startClusterFitResult.IsFitSuccessful() && (startClusterFitResult.GetRms() < m_maxFitRms))
        {
            if (!startClusterFitResultMap.insert(ClusterFitResultMap::value_type(*iter, startClusterFitResult)).second)
                return STATUS_CODE_FAILURE;
        }

        ClusterFitResult endClusterFitResult;
        (void) ClusterHelper::FitEnd(*iter, m_nEndLayersToFit, endClusterFitResult);

        if (endClusterFitResult.IsFitSuccessful() && (endClusterFitResult.GetRms() < m_maxFitRms))
        {
            if (!endClusterFitResultMap.insert(ClusterFitResultMap::value_type(*iter, endClusterFitResult)).second)
                return STATUS_CODE_FAILURE;
        }
    }

    // Loop over cluster combinations, comparing fit results to determine whether clusters should be merged
    for (ClusterFitResultMap::const_iterator iterI = endClusterFitResultMap.begin(); iterI != endClusterFitResultMap.end(); ++iterI)
    {
        Cluster *pClusterI = iterI->first;

        if (!ClusterHelper::CanMergeCluster(pClusterI, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        const ClusterFitResult &clusterFitResultI = iterI->second;
        const PseudoLayer outerLayerI(pClusterI->GetOuterPseudoLayer());

        // For each end fit, examine start fits for all other clusters
        for (ClusterFitResultMap::const_iterator iterJ = startClusterFitResultMap.begin(); iterJ != startClusterFitResultMap.end();)
        {
            Cluster *pClusterJ = iterJ->first;
            const ClusterFitResult &clusterFitResultJ = iterJ->second;
            ++iterJ;

            if (pClusterI == pClusterJ)
                continue;

            if (!ClusterHelper::CanMergeCluster(pClusterJ, m_canMergeMinMipFraction, m_canMergeMaxRms))
                continue;

            const PseudoLayer innerLayerJ(pClusterJ->GetInnerPseudoLayer());

            // Cut on layer separation between the two clusters
            if ((innerLayerJ > outerLayerI) || ((outerLayerI - innerLayerJ) > m_maxLayerDifference))
                continue;

            // Also cut on physical separation between the two clusters
            const CartesianVector centroidDifference(pClusterI->GetCentroid(outerLayerI) - pClusterJ->GetCentroid(innerLayerJ));
            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Check that cluster fit directions are compatible
            const float fitDirectionDotProduct(clusterFitResultI.GetDirection().GetDotProduct(clusterFitResultJ.GetDirection()));
            if (fitDirectionDotProduct > m_fitDirectionDotProductCut)
                continue;

            // Cut on distance of closest approach between start and end fits
            const bool isOutsideECalJ(pGeometryHelper->IsOutsideECal(pClusterJ->GetCentroid(innerLayerJ)));
            const float trackMergeCut(isOutsideECalJ ? m_trackMergeCutHcal : m_trackMergeCutEcal);

            if (ClusterHelper::GetFitResultsClosestApproach(clusterFitResultI, clusterFitResultJ) > trackMergeCut)
                continue;

            // Cut on perpendicular distance between fit directions and centroid difference vector.
            const float trackMergePerpCut(isOutsideECalJ ? m_trackMergePerpCutHcal : m_trackMergePerpCutEcal);

            const CartesianVector fitICrossCentroidDifference(clusterFitResultI.GetDirection().GetCrossProduct(centroidDifference));
            const float fitIPerpendicularDistance(fitICrossCentroidDifference.GetMagnitude());

            const CartesianVector fitJCrossCentroidDifference(clusterFitResultJ.GetDirection().GetCrossProduct(centroidDifference));
            const float fitJPerpendicularDistance(fitJCrossCentroidDifference.GetMagnitude());

            if ((fitIPerpendicularDistance < trackMergePerpCut) || (fitJPerpendicularDistance < trackMergePerpCut))
            {
                // TODO decide whether to continue loop over daughter cluster candidates after merging
                // Cluster to be enlarged is that for which the end fit was used
                endClusterFitResultMap.erase(pClusterJ);
                startClusterFitResultMap.erase(pClusterJ);
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pClusterI, pClusterJ));
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BrokenTracksAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_nStartLayersToFit = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NStartLayersToFit", m_nStartLayersToFit));

    m_nEndLayersToFit = 8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NEndLayersToFit", m_nEndLayersToFit));

    m_maxFitRms = 15.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitRms", m_maxFitRms));

    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_maxLayerDifference = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayerDifference", m_maxLayerDifference));

    m_maxCentroidDifference = 2000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    m_fitDirectionDotProductCut = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitDirectionDotProductCut", m_fitDirectionDotProductCut));

    m_trackMergeCutEcal = 25.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergeCutEcal", m_trackMergeCutEcal));

    m_trackMergeCutHcal = 25.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergeCutHcal", m_trackMergeCutHcal));

    m_trackMergePerpCutEcal = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergePerpCutEcal", m_trackMergePerpCutEcal));

    m_trackMergePerpCutHcal = 75.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergePerpCutHcal", m_trackMergePerpCutHcal));

    return STATUS_CODE_SUCCESS;
}
