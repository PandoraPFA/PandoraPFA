/**
 *  @file   PandoraPFANew/src/Algorithms/ClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/ClusteringAlgorithm.h"

#include <cmath>

using namespace pandora;

const float ClusteringAlgorithm::FLOAT_MAX = std::numeric_limits<float>::max();

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::Run()
{
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    ClusterVector clusterVector;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SeedClustersWithTracks(clusterVector));

    for (OrderedCaloHitList::const_iterator iter = pOrderedCaloHitList->begin(), iterEnd = pOrderedCaloHitList->end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer pseudoLayer(iter->first);
        EnergySortedCaloHitList energySortedCaloHitList;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if (CaloHitHelper::IsCaloHitAvailable(*hitIter))
                energySortedCaloHitList.insert(*hitIter);
        }
        
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindHitsInPreviousLayers(pseudoLayer, &energySortedCaloHitList,
            pOrderedCaloHitList, clusterVector));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindHitsInSameLayer(pseudoLayer, &energySortedCaloHitList, clusterVector));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::SeedClustersWithTracks(ClusterVector &clusterVector) const
{
    if(0 == m_clusterSeedStrategy)
        return STATUS_CODE_SUCCESS;

    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    for(TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;
        bool useTrack = (3 == m_clusterSeedStrategy); // TODO implement enum

        const CartesianVector &seedPosition(pTrack->GetTrackStateAtECal().GetPosition());
        const CartesianVector &seedMomentum(pTrack->GetTrackStateAtECal().GetMomentum());
        const float magnitudesSquared(seedPosition.GetMagnitudeSquared() * seedMomentum.GetMagnitudeSquared());

        if (0 >= magnitudesSquared)
            continue;

        const float cosTheta(seedPosition.GetDotProduct(seedMomentum) / std::sqrt(magnitudesSquared));

        if (cosTheta < m_trackSeedMaxCosTheta)
        {
            static const float eCalEndCapZCoordinate(GeometryHelper::GetInstance()->GetECalEndCapParameters().GetInnerZCoordinate());

            if ((2 == m_clusterSeedStrategy) || (std::fabs(seedPosition.GetZ()) < eCalEndCapZCoordinate))
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

StatusCode ClusteringAlgorithm::FindHitsInPreviousLayers(PseudoLayer pseudoLayer, EnergySortedCaloHitList *const pEnergySortedCaloHitList,
    const OrderedCaloHitList *const pOrderedCaloHitList, ClusterVector &clusterVector) const
{
    for (EnergySortedCaloHitList::const_iterator iter = pEnergySortedCaloHitList->begin(), iterEnd = pEnergySortedCaloHitList->end();
        iter != iterEnd;)
    {
        CaloHit *pCaloHit = *iter;

        Cluster *pBestCluster = NULL;
        float smallestGenericDistance(FLOAT_MAX);
        const PseudoLayer layersToStepBack((ECAL == pCaloHit->GetHitType()) ? m_layersToStepBackECal : m_layersToStepBackHCal);

        // Associate with existing clusters in stepBack layers. If stepBackLayer == pseudoLayer + 1, will examine TRACK_PROJECTION_LAYER.
        for(PseudoLayer stepBackLayer = 1; (stepBackLayer <= layersToStepBack) && (stepBackLayer <= (pseudoLayer + 1)); ++stepBackLayer)
        {
            const PseudoLayer searchLayer(pseudoLayer - stepBackLayer);

            // See if hit should be associated with any existing clusters
            for (ClusterVector::iterator clusterIter = clusterVector.begin(), clusterIterEnd = clusterVector.end();
                clusterIter != clusterIterEnd; ++clusterIter)
            {
                Cluster *pCluster = *clusterIter;
                float genericDistance(FLOAT_MAX);

                PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetGenericDistanceToHit(pCluster,
                    pCaloHit, searchLayer, genericDistance));

                if((genericDistance < m_genericDistanceCut) && (genericDistance < smallestGenericDistance))
                {
                    pBestCluster = pCluster;
                    smallestGenericDistance = genericDistance; 
                }
            }

            // Add best hit found after completing examination of a stepback layer
            if ((0 == m_clusterFormationStrategy) && (NULL != pBestCluster)) // TODO turn clusterFormationStrategy into a bool
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pBestCluster, pCaloHit));
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
            pEnergySortedCaloHitList->erase(iter++);
        }
        else
        {
            iter++;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::FindHitsInSameLayer(PseudoLayer pseudoLayer, EnergySortedCaloHitList *const pEnergySortedCaloHitList,
    ClusterVector &clusterVector) const
{
    while (!pEnergySortedCaloHitList->empty())
    {
        for (EnergySortedCaloHitList::const_iterator iter = pEnergySortedCaloHitList->begin(), iterEnd = pEnergySortedCaloHitList->end();
            iter != iterEnd;)
        {
            CaloHit *pCaloHit = *iter;
            Cluster *pBestCluster = NULL;
            float smallestGenericDistance = FLOAT_MAX;

            // See if hit should be associated with any existing clusters
            for (ClusterVector::iterator clusterIter = clusterVector.begin(), clusterIterEnd = clusterVector.end();
                clusterIter != clusterIterEnd; ++clusterIter)
            {
                Cluster *pCluster = *clusterIter;
                float genericDistance(FLOAT_MAX);

                PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetGenericDistanceToHit(pCluster,
                    pCaloHit, pseudoLayer, genericDistance));

                if((genericDistance < m_genericDistanceCut) && (genericDistance < smallestGenericDistance))
                {
                    pBestCluster = pCluster;
                    smallestGenericDistance = genericDistance; 
                }
            }

            if (NULL != pBestCluster)
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pBestCluster, pCaloHit));
                pEnergySortedCaloHitList->erase(iter++);
            }
            else
            {
                iter++;
            }
        }

        // Seed a new cluster
        if (!pEnergySortedCaloHitList->empty())
        {
            CaloHit *pCaloHit = *(pEnergySortedCaloHitList->begin());
            pEnergySortedCaloHitList->erase(pCaloHit);

            Cluster *pCluster = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHit, pCluster));
            clusterVector.push_back(pCluster);
        }
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
        return this->GetConeApproachDistanceToHit(pCaloHit, pCluster->GetCentroid(searchLayer), pCluster->GetInitialDirection(),
            genericDistance);
    }

    OrderedCaloHitList::const_iterator clusterHitListIter = pCluster->GetOrderedCaloHitList().find(searchLayer);
    if (pCluster->GetOrderedCaloHitList().end() == clusterHitListIter)
        return STATUS_CODE_UNCHANGED;

    const CaloHitList *pClusterCaloHitList = clusterHitListIter->second;

    const bool useTrackSeed(m_shouldUseTrackSeed && pCluster->IsTrackSeeded());
    float initialDirectionDistance(FLOAT_MAX), currentDirectionDistance(FLOAT_MAX), trackSeedDistance(FLOAT_MAX);

    // Cone approach measurements
    if (!useTrackSeed || (searchLayer > m_trackSeedCutOffLayer))
    {
        if (searchLayer == pCaloHit->GetPseudoLayer())
        {
            return this->GetDistanceToHitInSameLayer(pCaloHit, pClusterCaloHitList, genericDistance);
        }

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetConeApproachDistanceToHit(pCaloHit,
            pClusterCaloHitList, pCluster->GetInitialDirection(), initialDirectionDistance));

        if (useTrackSeed)
            initialDirectionDistance /= 5.;

        if (pCluster->GetCurrentFitResult().IsFitSuccessful())
        {
            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetConeApproachDistanceToHit(pCaloHit,
                pClusterCaloHitList, pCluster->GetCurrentFitResult().GetDirection(), currentDirectionDistance));

            if ((currentDirectionDistance < m_genericDistanceCut) && true)// TODO pCluster->IsMipTrack())
                currentDirectionDistance /= 5.;
        }
    }

    // Seed track distance measurements
    if (useTrackSeed && (searchLayer <= m_trackSeedCutOffLayer))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetDistanceToTrackSeed(pCluster, pCaloHit, searchLayer,
            trackSeedDistance));
    }

    // Identify best measurement of generic distance
    const float smallestDistance(std::min(trackSeedDistance, std::min(initialDirectionDistance, currentDirectionDistance)));
    if (smallestDistance < genericDistance)
    {
        genericDistance = smallestDistance;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::GetDistanceToHitInSameLayer(CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList,
    float &distance) const
{
    const float dCut ((ECAL == pCaloHit->GetHitType()) ?
        (m_sameLayerPadWidthsECal * pCaloHit->GetCellSizeU()) :
        (m_sameLayerPadWidthsHCal * pCaloHit->GetCellSizeU()) );

    if (0 == dCut)
        return STATUS_CODE_FAILURE;

    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());

    bool hitFound(false);
    float smallestDistance(FLOAT_MAX);

    for(CaloHitList::iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        CaloHit *pHitInCluster = *iter;
        const CartesianVector &hitInClusterPosition(pHitInCluster->GetPositionVector());
        const float separation((hitPosition - hitInClusterPosition).GetMagnitude());
        const float hitDistance(separation / dCut);

        if(hitDistance < smallestDistance)
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

    for (CaloHitList::iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        CaloHit *pHitInCluster = *iter;
        float hitDistance(FLOAT_MAX);

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetConeApproachDistanceToHit(pCaloHit,
            pHitInCluster->GetPositionVector(), clusterDirection, hitDistance));

        if(hitDistance < smallestDistance)
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

    const float dCut ((ECAL == pCaloHit->GetHitType()) ?
        (std::fabs(dAlong) * m_tanConeAngleECal) + (m_additionalPadWidthsECal * pCaloHit->GetCellSizeU()) :
        (std::fabs(dAlong) * m_tanConeAngleHCal) + (m_additionalPadWidthsHCal * pCaloHit->GetCellSizeU()) );

    if (0 == dCut)
        return STATUS_CODE_FAILURE;

    if ((dAlong < 200.) && (dAlong > -10.)) // TODO make parameters
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

    for(int iLayer = startLayer; iLayer < searchLayerInt; ++iLayer)
    {
        OrderedCaloHitList::const_iterator listIter = orderedCaloHitList.find(iLayer);
        if (orderedCaloHitList.end() == listIter)
            continue;

        for(CaloHitList::const_iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
        {
            float tempDistance(FLOAT_MAX);
            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetDistanceToTrackSeed(pCluster, *iter,
                tempDistance));

            if(tempDistance < m_genericDistanceCut)
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

    const CartesianVector hitPosition(pCaloHit->GetPositionVector());

    const CartesianVector &trackSeedPosition(pCluster->GetTrackSeed()->GetTrackStateAtECal().GetPosition());
    const CartesianVector positionDifference(hitPosition - trackSeedPosition);
    const float separation(positionDifference.GetMagnitude());

    if(separation < m_maxTrackSeedSeparation)
    {
        const float dPerp((pCluster->GetInitialDirection().GetCrossProduct(positionDifference)).GetMagnitude());
        const float flexibility(1. + (m_trackPathWidth * (separation / m_maxTrackSeedSeparation)));

        const float dCut ((ECAL == pCaloHit->GetHitType()) ?
            flexibility * (m_additionalPadWidthsECal * pCaloHit->GetCellSizeU()) :
            flexibility * (m_additionalPadWidthsHCal * pCaloHit->GetCellSizeU()) );

        if (0 == dCut)
            return STATUS_CODE_FAILURE;

        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    // Track seeding parameters
    m_clusterSeedStrategy = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterSeedStrategy", m_clusterSeedStrategy));

    m_trackSeedMaxCosTheta = 0.7;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackSeedMaxCosTheta", m_trackSeedMaxCosTheta));

    // High level clustering parameters
    m_layersToStepBackECal = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayersToStepBackECal", m_layersToStepBackECal));

    m_layersToStepBackHCal = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayersToStepBackHCal", m_layersToStepBackHCal));

    m_clusterFormationStrategy = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterFormationStrategy", m_clusterFormationStrategy));

    m_genericDistanceCut = 1.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GenericDistanceCut", m_genericDistanceCut));

    m_shouldUseTrackSeed = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseTrackSeed", m_shouldUseTrackSeed));

    m_trackSeedCutOffLayer = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackSeedCutOffLayer", m_trackSeedCutOffLayer));

    // Same layer distance parameters
    m_sameLayerPadWidthsECal = 1.8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SameLayerPadWidthsECal", m_sameLayerPadWidthsECal));

    m_sameLayerPadWidthsHCal = 1.8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SameLayerPadWidthsHCal", m_sameLayerPadWidthsHCal));

    // Cone approach distance parameters
    m_coneApproachMaxSeparation = 100.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeApproachMaxSeparation", m_coneApproachMaxSeparation));

    m_tanConeAngleECal = 0.36;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TanConeAngleECal", m_tanConeAngleECal));

    m_tanConeAngleHCal = 1.00;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TanConeAngleHCal", m_tanConeAngleHCal));

    m_additionalPadWidthsECal = 1.5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsECal", m_additionalPadWidthsECal));

    m_additionalPadWidthsHCal = 2.5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsHCal", m_additionalPadWidthsHCal));

    // Track seed distance parameters
    m_trackPathWidth = 2.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackPathWidth", m_trackPathWidth));

    m_maxTrackSeedSeparation = 250.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackSeedSeparation", m_maxTrackSeedSeparation));

    m_maxLayersToTrackSeed = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersToTrackSeed", m_maxLayersToTrackSeed));

    m_maxLayersToTrackLikeHit = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersToTrackLikeHit", m_maxLayersToTrackLikeHit));

    return STATUS_CODE_SUCCESS;
}
