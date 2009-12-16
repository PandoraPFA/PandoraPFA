/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/LoopingTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the looping tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/LoopingTracksAlgorithm.h"

#include "Helpers/GeometryHelper.h"

#include <cmath>
#include <limits>

using namespace pandora;

StatusCode LoopingTracksAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    const GeometryHelper *const pGeometryHelper(GeometryHelper::GetInstance());

    // Fit a straight line to the last n occupied pseudo layers in each cluster and store results
    typedef std::map<Cluster *, ClusterHelper::ClusterFitResult> ClusterFitResultMap;
    ClusterFitResultMap clusterFitResultMap;

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        ClusterHelper::ClusterFitResult clusterFitResult;

        if (STATUS_CODE_SUCCESS != ClusterHelper::FitEnd(*iter, m_nLayersToFit, clusterFitResult))
            continue;

        if (clusterFitResult.GetChi2() > m_fitChi2Cut)
            continue;

        if (!clusterFitResultMap.insert(ClusterFitResultMap::value_type(*iter, clusterFitResult)).second)
            return STATUS_CODE_FAILURE;
    }

    ClusterList deletedClusterList;

    // Loop over cluster combinations, comparing fit results to determine whether clusters should be merged
    for (ClusterFitResultMap::const_iterator iterI = clusterFitResultMap.begin(); iterI != clusterFitResultMap.end(); ++iterI)
    {
        Cluster *pClusterI = iterI->first;

        if (!ClusterHelper::CanMergeCluster(pClusterI, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        const PseudoLayer outerLayerI(pClusterI->GetOuterPseudoLayer());
        const bool isOutsideECalI(pGeometryHelper->IsOutsideECal(pClusterI->GetCentroid(outerLayerI)));

        const ClusterHelper::ClusterFitResult &clusterFitResultI = iterI->second;

        ClusterFitResultMap::const_iterator iterJ = iterI;
        ++iterJ;

        while (iterJ != clusterFitResultMap.end())
        {
            Cluster *pClusterJ = iterJ->first;
            const ClusterHelper::ClusterFitResult &clusterFitResultJ = iterJ->second;
            ++iterJ;

            if (!ClusterHelper::CanMergeCluster(pClusterJ, m_canMergeMinMipFraction, m_canMergeMaxRms))
                continue;

            // Are both clusters outside of the ecal region? If so, relax cluster compatibility checks.
            const PseudoLayer outerLayerJ(pClusterJ->GetOuterPseudoLayer());

            const bool isOutsideECalJ(pGeometryHelper->IsOutsideECal(pClusterJ->GetCentroid(outerLayerJ)));
            const bool isOutsideECal(isOutsideECalI && isOutsideECalJ);

            // Apply loose cuts to examine suitability of merging clusters before proceeding
            const PseudoLayer outerLayerDifference((outerLayerI > outerLayerJ) ? (outerLayerI - outerLayerJ) : (outerLayerJ - outerLayerI));
            if (outerLayerDifference > m_maxOuterLayerDifference)
                continue;

            const CartesianVector centroidDifference(pClusterI->GetCentroid(outerLayerI) - pClusterJ->GetCentroid(outerLayerJ));
            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Check that cluster fit directions are compatible with looping track hypothesis
            const float fitDirectionDotProductCut(isOutsideECal ? m_fitDirectionDotProductCutHCal : m_fitDirectionDotProductCutECal);

            const float fitDirectionDotProduct(clusterFitResultI.GetDirection().GetDotProduct(clusterFitResultJ.GetDirection()));
            if (fitDirectionDotProduct > fitDirectionDotProductCut)
                continue;

            if (centroidDifference.GetDotProduct(clusterFitResultJ.GetDirection() - clusterFitResultI.GetDirection()) <= 0.)
                continue;

            // Cut on distance of closest approach between hits in outer layers of the two clusters
            const float closestHitDistanceCut(isOutsideECal ? m_closestHitDistanceCutHCal : m_closestHitDistanceCutECal);

            if (this->GetClosestDistanceBetweenOuterLayerHits(pClusterI, pClusterJ) > closestHitDistanceCut)
                continue;

            // Cut on distance of closest approach between fit extrapolations
            const float fitResultsClosestApproachCut(isOutsideECal ? m_fitResultsClosestApproachCutHCal : m_fitResultsClosestApproachCutECal);

            if (ClusterHelper::GetFitResultsClosestApproach(clusterFitResultI, clusterFitResultJ) > fitResultsClosestApproachCut)
                continue;

            // Merge clusters if they are in HCal, otherwise look for "good" features (bit ad hoc) ...
            unsigned int nGoodFeatures(0);

            if (!isOutsideECal)
            {
                if(fitDirectionDotProduct < m_goodFeaturesMaxFitDotProduct)
                    nGoodFeatures++;

                if(fitResultsClosestApproachCut < m_goodFeaturesMaxFitApproach)
                    nGoodFeatures++;

                if(outerLayerDifference < m_goodFeaturesMaxLayerDifference)
                    nGoodFeatures++;

                if((pClusterI->GetMipFraction() > m_goodFeaturesMinMipFraction) && (pClusterJ->GetMipFraction() > m_goodFeaturesMinMipFraction))
                    nGoodFeatures++;
            }

            // Now have sufficient information to decide whether to join clusters
            if(isOutsideECal || (nGoodFeatures >= m_nGoodFeaturesForClusterMerge))
            {
                // TODO decide which to delete and which to enlarge
                // TODO decide whether to continue loop over daughter cluster candidates after merging
                clusterFitResultMap.erase(pClusterJ);
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pClusterI, pClusterJ));
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float LoopingTracksAlgorithm::GetClosestDistanceBetweenOuterLayerHits(const Cluster *const pClusterI, const Cluster *const pClusterJ) const
{
    float closestDistance(std::numeric_limits<float>::max());

    const PseudoLayer outerLayerI(pClusterI->GetOuterPseudoLayer());
    const PseudoLayer outerLayerJ(pClusterJ->GetOuterPseudoLayer());

    CaloHitList *pCaloHitListI = NULL;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pClusterI->GetOrderedCaloHitList().GetCaloHitsInPseudoLayer(outerLayerI, pCaloHitListI));

    CaloHitList *pCaloHitListJ = NULL;
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pClusterJ->GetOrderedCaloHitList().GetCaloHitsInPseudoLayer(outerLayerJ, pCaloHitListJ));

    for (CaloHitList::const_iterator iterI = pCaloHitListI->begin(), iterIEnd = pCaloHitListI->end(); iterI != iterIEnd; ++iterI)
    {
        CaloHit *pCaloHitI = *iterI;

        for (CaloHitList::const_iterator iterJ = pCaloHitListJ->begin(), iterIEnd = pCaloHitListJ->end(); iterJ != iterIEnd; ++iterJ)
        {
            CaloHit *pCaloHitJ = *iterJ;

            const float distance((pCaloHitI->GetPositionVector() - pCaloHitJ->GetPositionVector()).GetMagnitude());

            if (distance < closestDistance)
                closestDistance = distance;
        }
    }

    return closestDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LoopingTracksAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_nLayersToFit = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersToFit", m_nLayersToFit));

    m_fitChi2Cut = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitChi2Cut", m_fitChi2Cut));

    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_maxOuterLayerDifference = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxOuterLayerDifference", m_maxOuterLayerDifference));

    m_maxCentroidDifference = 2000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    m_fitDirectionDotProductCutECal = -0.1f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitDirectionDotProductCutECal", m_fitDirectionDotProductCutECal));

    m_fitDirectionDotProductCutHCal = 0.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitDirectionDotProductCutHCal", m_fitDirectionDotProductCutHCal));

    m_closestHitDistanceCutECal = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestHitDistanceCutECal", m_closestHitDistanceCutECal));

    m_closestHitDistanceCutHCal = 500.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestHitDistanceCutHCal", m_closestHitDistanceCutHCal));

    m_fitResultsClosestApproachCutECal = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitResultsClosestApproachCutECal", m_fitResultsClosestApproachCutECal));

    m_fitResultsClosestApproachCutHCal = 200.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitResultsClosestApproachCutHCal", m_fitResultsClosestApproachCutHCal));

    m_nGoodFeaturesForClusterMerge = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NGoodFeaturesForClusterMerge", m_nGoodFeaturesForClusterMerge));

    m_goodFeaturesMaxFitDotProduct = -0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NGoodFeaturesForClusterMerge", m_nGoodFeaturesForClusterMerge));

    m_goodFeaturesMaxFitApproach = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GoodFeaturesMaxFitApproach", m_goodFeaturesMaxFitApproach));

    m_goodFeaturesMaxLayerDifference = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GoodFeaturesMaxLayerDifference", m_goodFeaturesMaxLayerDifference));

    m_goodFeaturesMinMipFraction = 0.9f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GoodFeaturesMinMipFraction", m_goodFeaturesMinMipFraction));

    return STATUS_CODE_SUCCESS;
}
