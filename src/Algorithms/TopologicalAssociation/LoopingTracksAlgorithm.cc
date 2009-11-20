/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/LoopingTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the looping tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/LoopingTracksAlgorithm.h"

#include <cmath>

using namespace pandora;

StatusCode LoopingTracksAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    // Fit a straight line to the last n occupied pseudo layers in each cluster and store results
    typedef std::map<Cluster *, ClusterFitResult> ClusterFitResultMap;
    ClusterFitResultMap clusterFitResultMap;

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        ClusterFitResult clusterFitResult;

        if (STATUS_CODE_SUCCESS != ClusterHelper::FitEnd(*iter, m_nLayersToFit, clusterFitResult))
            continue;

        if (!clusterFitResultMap.insert(ClusterFitResultMap::value_type(*iter, clusterFitResult)).second)
            return STATUS_CODE_FAILURE;
    }

    // Loop over cluster combinations, comparing fit results to determine whether clusters should be merged
    for (ClusterFitResultMap::const_iterator iterI = clusterFitResultMap.begin(); iterI != clusterFitResultMap.end(); ++iterI)
    {
        Cluster *pClusterI = iterI->first;
        const ClusterFitResult &clusterFitResultI = iterI->second;
        const PseudoLayer outerLayerI(pClusterI->GetOuterPseudoLayer());

        if (!this->CanMergeCluster(pClusterI) || (clusterFitResultI.GetChi2() > m_fitChi2Cut))
            continue;

        ClusterFitResultMap::const_iterator iterJ = iterI;
        for (++iterJ ; iterJ != clusterFitResultMap.end(); ++iterJ)
        {
            Cluster *pClusterJ = iterJ->first;
            const ClusterFitResult &clusterFitResultJ = iterJ->second;
            const PseudoLayer outerLayerJ(pClusterJ->GetOuterPseudoLayer());

            const bool isHCal = false; // TODO set this bool correctly

            if (!this->CanMergeCluster(pClusterJ) || (clusterFitResultJ.GetChi2() > m_fitChi2Cut))
                continue;

            // Apply loose cuts to examine suitability of merging clusters before proceeding
            const PseudoLayer outerLayerDifference((outerLayerI > outerLayerJ) ? (outerLayerI - outerLayerJ) : (outerLayerJ - outerLayerI));
            if (outerLayerDifference > m_maxOuterLayerDifference)
                continue;

            const CartesianVector centroidDifference(pClusterI->GetCentroid(outerLayerI) - pClusterJ->GetCentroid(outerLayerJ));
            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            const float fitDirectionDotProduct(clusterFitResultI.GetDirection().GetDotProduct(clusterFitResultJ.GetDirection()));
            if (fitDirectionDotProduct > m_maxFitDirectionDotProduct)
                continue;

            // Skip if clusters are not generally pointing towards one another
            if (!(centroidDifference.GetDotProduct(clusterFitResultJ.GetDirection() - clusterFitResultI.GetDirection())) > 0.);
                continue;

            // Calculate the distance of closest approach between hits in last layers of clusters
            const float closestHitDistanceCut = (isHCal ? m_closestHitDistanceCutHCal : m_closestHitDistanceCutECal);

            if (this->GetClosestDistanceBetweenOuterLayerHits(pClusterI, pClusterJ) > closestHitDistanceCut)
                continue;

            // Calculate the distance of closest approach between fit extrapolations
            const float fitResultClosestApproachCut = (isHCal ? m_fitResultClosestApproachCutHCal : m_fitResultClosestApproachCutECal);

            if (this->GetFitResultsClosestApproach(clusterFitResultI, clusterFitResultJ) > fitResultClosestApproachCut)
                continue;

            // If clusters are matched and are within cut distance : join them
            if(isHCal || this->AreClustersCompatible())
            {
                // TODO decide which to delete and which to enlarge AND enable option to assimilate at end of algorithm
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
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pClusterI->GetOrderedCaloHitList().GetCaloHitsInPseudoLayer(outerLayerJ, pCaloHitListJ));

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

float LoopingTracksAlgorithm::GetFitResultsClosestApproach(const ClusterFitResult &clusterFitResultI, const ClusterFitResult &clusterFitResultJ) const
{
    const CartesianVector directionNormal(clusterFitResultI.GetDirection().GetCrossProduct(clusterFitResultJ.GetDirection()).GetUnitVector());
    const CartesianVector interceptDifference(clusterFitResultI.GetIntercept() - clusterFitResultJ.GetIntercept());

    return std::fabs(directionNormal.GetDotProduct(interceptDifference));
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LoopingTracksAlgorithm::AreClustersCompatible() const
{
    return true; // TODO implement ad hoc "good features" method
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool LoopingTracksAlgorithm::CanMergeCluster(Cluster *const pCluster)
{
    const bool canMerge(!pCluster->IsPhoton() ||
        (pCluster->GetMipFraction() > m_canMergeMinMipFraction) ||
        (pCluster->GetFitToAllHitsResult().GetRms() < m_canMergeMaxRms));

    return canMerge;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LoopingTracksAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    m_nLayersToFit = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersToFit", m_nLayersToFit));

    m_fitChi2Cut = 100.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitChi2Cut", m_fitChi2Cut));

    m_canMergeMinMipFraction = 0.7;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_maxOuterLayerDifference = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxOuterLayerDifference", m_maxOuterLayerDifference));

    m_maxCentroidDifference = 2000.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    m_maxFitDirectionDotProduct = 0.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitDirectionDotProduct", m_maxFitDirectionDotProduct));

    m_closestHitDistanceCutECal = 250.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestHitDistanceCutECal", m_closestHitDistanceCutECal));

    m_closestHitDistanceCutHCal = 500.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestHitDistanceCutHCal", m_closestHitDistanceCutHCal));

    m_fitResultClosestApproachCutECal = 50.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitResultClosestApproachCutECal", m_fitResultClosestApproachCutECal));

    m_fitResultClosestApproachCutHCal = 200.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitResultClosestApproachCutHCal", m_fitResultClosestApproachCutHCal));

    return STATUS_CODE_SUCCESS;
}
