/**
 *  @file   PandoraPFANew/src/Algorithms/Clustering/ClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/Clustering/ClusteringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

unsigned int CustomHitOrder::m_hitSortingStrategy = 0;
const float ClusteringAlgorithm::FLOAT_MAX = std::numeric_limits<float>::max();

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::Run()
{
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    if (pOrderedCaloHitList->empty())
        return STATUS_CODE_SUCCESS;

    ClusterVector clusterVector;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SeedClustersWithTracks(clusterVector));

    for (OrderedCaloHitList::const_iterator iter = pOrderedCaloHitList->begin(), iterEnd = pOrderedCaloHitList->end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer pseudoLayer(iter->first);
        CustomSortedCaloHitList customSortedCaloHitList;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            if (CaloHitHelper::IsCaloHitAvailable(pCaloHit) && (m_shouldUseIsolatedHits || !pCaloHit->IsIsolated()) &&
               (!m_shouldUseOnlyECalHits || (ECAL == pCaloHit->GetHitType())) )
            {
                customSortedCaloHitList.insert(pCaloHit);
            }
        }

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindHitsInPreviousLayers(pseudoLayer, &customSortedCaloHitList, clusterVector));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindHitsInSameLayer(pseudoLayer, &customSortedCaloHitList, clusterVector));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->UpdateClusterProperties(clusterVector));
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveEmptyClusters(clusterVector));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::SeedClustersWithTracks(ClusterVector &clusterVector) const
{
    if (0 == m_clusterSeedStrategy)
        return STATUS_CODE_SUCCESS;

    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        if (!pTrack->CanFormPfo())
            continue;

        bool useTrack(false);

        if (2 == m_clusterSeedStrategy)
        {
            useTrack = true;
        }
        else if ((1 == m_clusterSeedStrategy) && pTrack->IsProjectedToEndCap())
        {
            useTrack = true;
        }

        if (useTrack)
        {
            Cluster *pCluster = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pTrack, pCluster));
            clusterVector.push_back(pCluster);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::FindHitsInPreviousLayers(PseudoLayer pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
    ClusterVector &clusterVector) const
{
    for (CustomSortedCaloHitList::iterator iter = pCustomSortedCaloHitList->begin(), iterEnd = pCustomSortedCaloHitList->end();
        iter != iterEnd;)
    {
        CaloHit *pCaloHit = *iter;

        Cluster *pBestCluster = NULL;
        float bestClusterEnergy(0.f);
        float smallestGenericDistance(m_genericDistanceCut);
        const PseudoLayer layersToStepBack((GeometryHelper::GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            m_layersToStepBackFine : m_layersToStepBackCoarse);

        // Associate with existing clusters in stepBack layers. If stepBackLayer == pseudoLayer, will examine TRACK_PROJECTION_LAYER.
        for (PseudoLayer stepBackLayer = 1; (stepBackLayer <= layersToStepBack) && (stepBackLayer <= pseudoLayer); ++stepBackLayer)
        {
            const PseudoLayer searchLayer(pseudoLayer - stepBackLayer);

            // See if hit should be associated with any existing clusters
            for (ClusterVector::iterator clusterIter = clusterVector.begin(), clusterIterEnd = clusterVector.end();
                clusterIter != clusterIterEnd; ++clusterIter)
            {
                Cluster *pCluster = *clusterIter;
                float genericDistance(FLOAT_MAX);
                const float clusterEnergy(pCluster->GetHadronicEnergy());

                PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetGenericDistanceToHit(pCluster,
                    pCaloHit, searchLayer, genericDistance));

                if ((genericDistance < smallestGenericDistance) || ((genericDistance == smallestGenericDistance) && (clusterEnergy > bestClusterEnergy)))
                {
                    pBestCluster = pCluster;
                    bestClusterEnergy = clusterEnergy;
                    smallestGenericDistance = genericDistance;
                }
            }

            // Add best hit found after completing examination of a stepback layer
            if ((0 == m_clusterFormationStrategy) && (NULL != pBestCluster))
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pBestCluster, pCaloHit));
                break;
            }
        }

        // Add best hit found after examining all stepback layers
        if ((1 == m_clusterFormationStrategy) && (NULL != pBestCluster))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pBestCluster, pCaloHit));
        }

        // Tidy the energy sorted calo hit list
        if (!CaloHitHelper::IsCaloHitAvailable(pCaloHit))
        {
            pCustomSortedCaloHitList->erase(iter++);
        }
        else
        {
            iter++;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::FindHitsInSameLayer(PseudoLayer pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
    ClusterVector &clusterVector) const
{
    while (!pCustomSortedCaloHitList->empty())
    {
        bool clustersModified = true;

        while (clustersModified)
        {
            clustersModified = false;

            for (CustomSortedCaloHitList::iterator iter = pCustomSortedCaloHitList->begin(), iterEnd = pCustomSortedCaloHitList->end();
                iter != iterEnd;)
            {
                CaloHit *pCaloHit = *iter;

                Cluster *pBestCluster = NULL;
                float bestClusterEnergy(0.f);
                float smallestGenericDistance(m_genericDistanceCut);

                // See if hit should be associated with any existing clusters
                for (ClusterVector::iterator clusterIter = clusterVector.begin(), clusterIterEnd = clusterVector.end();
                    clusterIter != clusterIterEnd; ++clusterIter)
                {
                    Cluster *pCluster = *clusterIter;
                    float genericDistance(FLOAT_MAX);
                    const float clusterEnergy(pCluster->GetHadronicEnergy());

                    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetGenericDistanceToHit(pCluster,
                        pCaloHit, pseudoLayer, genericDistance));

                    if ((genericDistance < smallestGenericDistance) || ((genericDistance == smallestGenericDistance) && (clusterEnergy > bestClusterEnergy)))
                    {
                        pBestCluster = pCluster;
                        bestClusterEnergy = clusterEnergy;
                        smallestGenericDistance = genericDistance;
                    }
                }

                if (NULL != pBestCluster)
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pBestCluster, pCaloHit));
                    pCustomSortedCaloHitList->erase(iter++);
                    clustersModified = true;
                }
                else
                {
                    iter++;
                }
            }
        }

        // Seed a new cluster
        if (!pCustomSortedCaloHitList->empty())
        {
            CaloHit *pCaloHit = *(pCustomSortedCaloHitList->begin());
            pCustomSortedCaloHitList->erase(pCaloHit);

            Cluster *pCluster = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHit, pCluster));
            clusterVector.push_back(pCluster);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::UpdateClusterProperties(ClusterVector &clusterVector) const
{
    // TODO replace this eventually - it remains only to reproduce old pandora results
    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (pCluster->GetNCaloHits() < 2)
            continue;

        ClusterHelper::ClusterFitPointList clusterFitPointList;
        ClusterHelper::ClusterFitResult clusterFitResult;

        const PseudoLayer innerLayer(pCluster->GetInnerPseudoLayer());
        const PseudoLayer outerLayer(pCluster->GetOuterPseudoLayer());
        const PseudoLayer nLayersSpanned(outerLayer - innerLayer);

        if (nLayersSpanned > m_nLayersSpannedForFit)
        {
            PseudoLayer nLayersToFit(m_nLayersToFit);

            if (pCluster->GetMipFraction() - m_nLayersToFitLowMipCut < std::numeric_limits<float>::epsilon())
                nLayersToFit *= m_nLayersToFitLowMipMultiplier;

            const PseudoLayer startLayer( (nLayersSpanned > nLayersToFit) ? (outerLayer - nLayersToFit) : innerLayer);
            (void) ClusterHelper::FitLayerCentroids(pCluster, startLayer, outerLayer, clusterFitResult);

            if (clusterFitResult.IsFitSuccessful())
            {
                const float dotProduct(clusterFitResult.GetDirection().GetDotProduct(pCluster->GetInitialDirection()));
                const float chi2(clusterFitResult.GetChi2());

                if (((dotProduct < m_fitSuccessDotProductCut1) && (chi2 > m_fitSuccessChi2Cut1)) ||
                    ((dotProduct < m_fitSuccessDotProductCut2) && (chi2 > m_fitSuccessChi2Cut2)) )
                {
                    clusterFitResult.SetSuccessFlag(false);
                }

                if ((chi2 > m_mipTrackChi2Cut) && pCluster->IsMipTrack())
                    pCluster->SetIsMipTrackFlag(false);
            }
        }
        else if (nLayersSpanned > m_nLayersSpannedForApproxFit)
        {
            const CartesianVector centroidChange(pCluster->GetCentroid(outerLayer) - pCluster->GetCentroid(innerLayer));
            clusterFitResult.Reset();
            clusterFitResult.SetDirection(centroidChange.GetUnitVector());
            clusterFitResult.SetSuccessFlag(true);
        }

        pCluster->SetCurrentFitResult(clusterFitResult);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::GetGenericDistanceToHit(Cluster *const pCluster, CaloHit *const pCaloHit, PseudoLayer searchLayer,
    float &genericDistance) const
{
    // Cone approach measurement to track projections
    if ((searchLayer == TRACK_PROJECTION_LAYER) && (pCluster->IsTrackSeeded()))
    {
        return this->GetConeApproachDistanceToHit(pCaloHit, pCluster->GetTrackSeed()->GetTrackStateAtCalorimeter().GetPosition(),
            pCluster->GetInitialDirection(), genericDistance);
    }

    OrderedCaloHitList::const_iterator clusterHitListIter = pCluster->GetOrderedCaloHitList().find(searchLayer);
    if (pCluster->GetOrderedCaloHitList().end() == clusterHitListIter)
        return STATUS_CODE_UNCHANGED;

    const CaloHitList *pClusterCaloHitList = clusterHitListIter->second;

    const bool useTrackSeed(m_shouldUseTrackSeed && pCluster->IsTrackSeeded());
    const bool followInitialDirection(m_shouldFollowInitialDirection && pCluster->IsTrackSeeded() && (searchLayer > m_trackSeedCutOffLayer));
    float initialDirectionDistance(FLOAT_MAX), currentDirectionDistance(FLOAT_MAX), trackSeedDistance(FLOAT_MAX);

    // Cone approach measurements
    if (!useTrackSeed || (searchLayer > m_trackSeedCutOffLayer))
    {
        if (searchLayer == pCaloHit->GetPseudoLayer())
        {
            return this->GetDistanceToHitInSameLayer(pCaloHit, pClusterCaloHitList, genericDistance);
        }

        // Measurement using initial cluster direction
        StatusCode statusCode = this->GetConeApproachDistanceToHit(pCaloHit, pClusterCaloHitList, pCluster->GetInitialDirection(),
            initialDirectionDistance);

        if (STATUS_CODE_SUCCESS == statusCode)
        {
            if (followInitialDirection)
                initialDirectionDistance /= 5.;
        }
        else if (STATUS_CODE_UNCHANGED != statusCode)
        {
            return statusCode;
        }

        // Measurement using current cluster direction
        if (pCluster->GetCurrentFitResult().IsFitSuccessful())
        {
            StatusCode statusCode = this->GetConeApproachDistanceToHit(pCaloHit, pClusterCaloHitList,
                pCluster->GetCurrentFitResult().GetDirection(), currentDirectionDistance);

            if (STATUS_CODE_SUCCESS == statusCode)
            {
                if ((currentDirectionDistance < m_genericDistanceCut) && pCluster->IsMipTrack())
                    currentDirectionDistance /= 5.;
            }
            else if (STATUS_CODE_UNCHANGED != statusCode)
            {
                return statusCode;
            }
        }
    }

    // Seed track distance measurements
    if (useTrackSeed && !followInitialDirection)
    {
        StatusCode statusCode = this->GetDistanceToTrackSeed(pCluster, pCaloHit, searchLayer, trackSeedDistance);

        if (STATUS_CODE_SUCCESS == statusCode)
        {
            if (trackSeedDistance < m_genericDistanceCut)
                trackSeedDistance /= 5.;
        }
        else if (STATUS_CODE_UNCHANGED != statusCode)
        {
            return statusCode;
        }
    }

    // Identify best measurement of generic distance
    const float smallestDistance(std::min(trackSeedDistance, std::min(initialDirectionDistance, currentDirectionDistance)));
    if (smallestDistance < genericDistance)
    {
        genericDistance = smallestDistance;

        if (FLOAT_MAX != genericDistance)
            return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::GetDistanceToHitInSameLayer(CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList,
    float &distance) const
{
    const float dCut ((GeometryHelper::GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
        (m_sameLayerPadWidthsFine * pCaloHit->GetCellLengthScale()) :
        (m_sameLayerPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

    if (0 == dCut)
        return STATUS_CODE_FAILURE;

    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

    bool hitFound(false);
    float smallestDistance(FLOAT_MAX);

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        CaloHit *pHitInCluster = *iter;
        const CartesianVector &hitInClusterPosition(pHitInCluster->GetPositionVector());
        const float separation((hitPosition - hitInClusterPosition).GetMagnitude());
        const float hitDistance(separation / dCut);

        if (hitDistance < smallestDistance)
        {
            smallestDistance = hitDistance;
            hitFound = true;
        }
    }

    if (!hitFound)
        return STATUS_CODE_UNCHANGED;

    distance = smallestDistance;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::GetConeApproachDistanceToHit(CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList,
    const CartesianVector &clusterDirection, float &distance) const
{
    bool hitFound(false);
    float smallestDistance(FLOAT_MAX);

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        CaloHit *pHitInCluster = *iter;
        float hitDistance(FLOAT_MAX);

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetConeApproachDistanceToHit(pCaloHit,
            pHitInCluster->GetPositionVector(), clusterDirection, hitDistance));

        if (hitDistance < smallestDistance)
        {
            smallestDistance = hitDistance;
            hitFound = true;
        }
    }

    if (!hitFound)
        return STATUS_CODE_UNCHANGED;

    distance = smallestDistance;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::GetConeApproachDistanceToHit(CaloHit *const pCaloHit, const CartesianVector &clusterPosition,
    const CartesianVector &clusterDirection, float &distance) const
{
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const CartesianVector positionDifference(hitPosition - clusterPosition);

    if (positionDifference.GetMagnitude() > m_coneApproachMaxSeparation)
        return STATUS_CODE_UNCHANGED;

    const float dPerp (clusterDirection.GetCrossProduct(positionDifference).GetMagnitude());
    const float dAlong(clusterDirection.GetDotProduct(positionDifference));

    const float dCut ((GeometryHelper::GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
        (std::fabs(dAlong) * m_tanConeAngleFine) + (m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale()) :
        (std::fabs(dAlong) * m_tanConeAngleCoarse) + (m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

    if (0 == dCut)
        return STATUS_CODE_FAILURE;

    if ((dAlong < m_maxClusterDirProjection) && (dAlong > m_minClusterDirProjection))
    {
        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, PseudoLayer searchLayer,
    float &distance) const
{
    if (searchLayer < m_maxLayersToTrackSeed)
        return this->GetDistanceToTrackSeed(pCluster, pCaloHit, distance);

    const int searchLayerInt(static_cast<int>(searchLayer));
    const int startLayer(std::max(0, searchLayerInt - static_cast<int>(m_maxLayersToTrackLikeHit)));

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();

    for (int iLayer = startLayer; iLayer < searchLayerInt; ++iLayer)
    {
        OrderedCaloHitList::const_iterator listIter = orderedCaloHitList.find(iLayer);
        if (orderedCaloHitList.end() == listIter)
            continue;

        for (CaloHitList::const_iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
        {
            float tempDistance(FLOAT_MAX);
            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetDistanceToTrackSeed(pCluster, *iter,
                tempDistance));

            if (tempDistance < m_genericDistanceCut)
                return this->GetDistanceToTrackSeed(pCluster, pCaloHit, distance);
        }
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, float &distance) const
{
    if (0 == m_maxTrackSeedSeparation)
        return STATUS_CODE_FAILURE;

    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

    const CartesianVector &trackSeedPosition(pCluster->GetTrackSeed()->GetTrackStateAtCalorimeter().GetPosition());
    const CartesianVector positionDifference(hitPosition - trackSeedPosition);
    const float separation(positionDifference.GetMagnitude());

    if (separation < m_maxTrackSeedSeparation)
    {
        const float dPerp((pCluster->GetInitialDirection().GetCrossProduct(positionDifference)).GetMagnitude());
        const float flexibility(1.f + (m_trackPathWidth * (separation / m_maxTrackSeedSeparation)));

        const float dCut ((GeometryHelper::GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            flexibility * (m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale()) :
            flexibility * (m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

        if (0 == dCut)
            return STATUS_CODE_FAILURE;

        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::RemoveEmptyClusters(ClusterVector &clusterVector) const
{
    ClusterList clusterDeletionList;

    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        if (0 == (*iter)->GetNCaloHits())
        {
            clusterDeletionList.insert(*iter);
            (*iter) = NULL;
        }
    }

    if (!clusterDeletionList.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteClusters(*this, clusterDeletionList));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Track seeding parameters
    m_clusterSeedStrategy = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterSeedStrategy", m_clusterSeedStrategy));

    m_trackSeedMaxCosTheta = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackSeedMaxCosTheta", m_trackSeedMaxCosTheta));

    // High level clustering parameters
    CustomHitOrder::m_hitSortingStrategy = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HitSortingStrategy", CustomHitOrder::m_hitSortingStrategy));

    m_shouldUseOnlyECalHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseOnlyECalHits", m_shouldUseOnlyECalHits));

    m_shouldUseIsolatedHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseIsolatedHits", m_shouldUseIsolatedHits));

    m_layersToStepBackFine = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayersToStepBackFine", m_layersToStepBackFine));

    m_layersToStepBackCoarse = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayersToStepBackCoarse", m_layersToStepBackCoarse));

    m_clusterFormationStrategy = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterFormationStrategy", m_clusterFormationStrategy));

    m_genericDistanceCut = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GenericDistanceCut", m_genericDistanceCut));

    m_shouldUseTrackSeed = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseTrackSeed", m_shouldUseTrackSeed));

    m_trackSeedCutOffLayer = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackSeedCutOffLayer", m_trackSeedCutOffLayer));

    m_shouldFollowInitialDirection = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldFollowInitialDirection", m_shouldFollowInitialDirection));

    // Same layer distance parameters
    m_sameLayerPadWidthsFine = 2.8f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SameLayerPadWidthsFine", m_sameLayerPadWidthsFine));

    m_sameLayerPadWidthsCoarse = 1.8f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SameLayerPadWidthsCoarse", m_sameLayerPadWidthsCoarse));

    // Cone approach distance parameters
    m_coneApproachMaxSeparation = 1000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeApproachMaxSeparation", m_coneApproachMaxSeparation));

    m_tanConeAngleFine = 0.3f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TanConeAngleFine", m_tanConeAngleFine));

    m_tanConeAngleCoarse = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TanConeAngleCoarse", m_tanConeAngleCoarse));

    m_additionalPadWidthsFine = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsFine", m_additionalPadWidthsFine));

    m_additionalPadWidthsCoarse = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsCoarse", m_additionalPadWidthsCoarse));

    m_maxClusterDirProjection = 200.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterDirProjection", m_maxClusterDirProjection));

    m_minClusterDirProjection = -10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterDirProjection", m_minClusterDirProjection));

    // Track seed distance parameters
    m_trackPathWidth = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackPathWidth", m_trackPathWidth));

    m_maxTrackSeedSeparation = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackSeedSeparation", m_maxTrackSeedSeparation));

    m_maxLayersToTrackSeed = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersToTrackSeed", m_maxLayersToTrackSeed));

    m_maxLayersToTrackLikeHit = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersToTrackLikeHit", m_maxLayersToTrackLikeHit));

    // Cluster current direction and mip track parameters
    m_nLayersSpannedForFit = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersSpannedForFit", m_nLayersSpannedForFit));

    m_nLayersSpannedForApproxFit = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersSpannedForApproxFit", m_nLayersSpannedForApproxFit));

    m_nLayersToFit = 8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersToFit", m_nLayersToFit));

    m_nLayersToFitLowMipCut = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersToFitLowMipCut", m_nLayersToFitLowMipCut));

    m_nLayersToFitLowMipMultiplier = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersToFitLowMipMultiplier", m_nLayersToFitLowMipMultiplier));

    m_fitSuccessDotProductCut1 = 0.75f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitSuccessDotProductCut1", m_fitSuccessDotProductCut1));

    m_fitSuccessChi2Cut1 = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitSuccessChi2Cut1", m_fitSuccessChi2Cut1));

    m_fitSuccessDotProductCut2 = 0.50f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitSuccessDotProductCut2", m_fitSuccessDotProductCut2));

    m_fitSuccessChi2Cut2 = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitSuccessChi2Cut2", m_fitSuccessChi2Cut2));

    m_mipTrackChi2Cut = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipTrackChi2Cut", m_mipTrackChi2Cut));

    return STATUS_CODE_SUCCESS;
}
