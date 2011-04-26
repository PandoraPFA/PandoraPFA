/**
 *  @file   PandoraPFANew/FineGranularityContent/src/TopologicalAssociation/MipPhotonSeparationAlgorithm.cc
 * 
 *  @brief  Implementation of the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "TopologicalAssociation/MipPhotonSeparationAlgorithm.h"

using namespace pandora;

const float MipPhotonSeparationAlgorithm::FLOAT_MAX = std::numeric_limits<float>::max();
const unsigned int MipPhotonSeparationAlgorithm::LAYER_MAX = std::numeric_limits<unsigned int>::max();

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    // Create ordered vector of current clusters
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), Cluster::SortByInnerLayer);

    // Examine fragmentation possibilities for each cluster
    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        *iter = NULL;

        const TrackList trackList(pCluster->GetAssociatedTrackList());

        if (trackList.empty() || (trackList.size() > 1))
            continue;

        // Decide whether to fragment cluster, simultaneously determining cluster shower start/end layers
        Track *pTrack = *(trackList.begin());
        PseudoLayer showerStartLayer(LAYER_MAX), showerEndLayer(LAYER_MAX);

        if (!this->ShouldFragmentCluster(pCluster, pTrack, showerStartLayer, showerEndLayer))
            continue;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PerformFragmentation(pCluster, pTrack, showerStartLayer, showerEndLayer));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MipPhotonSeparationAlgorithm::ShouldFragmentCluster(Cluster *const pCluster, Track *const pTrack, PseudoLayer &showerStartLayer,
    PseudoLayer &showerEndLayer) const
{
    static const PseudoLayer firstPseudoLayer(GeometryHelper::GetPseudoLayerAtIp());

    PseudoLayer mipRegion1StartLayer(LAYER_MAX), mipRegion1EndLayer(LAYER_MAX);
    PseudoLayer mipRegion2StartLayer(LAYER_MAX), mipRegion2EndLayer(LAYER_MAX);

    const PseudoLayer lastPseudoLayer(pCluster->GetOuterPseudoLayer());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    bool shouldContinue(true);
    unsigned int layersMissed(0), mipCount(0), showerCount(0);
    bool mipRegion1(true), mipRegion2(false), showerRegion(false);

    // Loop over pseudo layers, looking for hits consistent with track projection
    for (PseudoLayer iLayer = firstPseudoLayer; (iLayer <= lastPseudoLayer) && shouldContinue; ++iLayer)
    {
        // Find hits consistent with track projection
        bool trackHitFound(false), mipTrackHitFound(false), showerTrackHitFound(false);
        OrderedCaloHitList::const_iterator hitListIter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() != hitListIter)
        {
            for (CaloHitList::const_iterator iter = hitListIter->second->begin(), iterEnd = hitListIter->second->end(); iter != iterEnd; ++iter)
            {
                CaloHit *pCaloHit = *iter;
                float distance(FLOAT_MAX);

                if (STATUS_CODE_SUCCESS != this->GetDistanceToTrack(pCluster, pTrack, pCaloHit, distance))
                    continue;

                if (distance < m_genericDistanceCut)
                {
                    trackHitFound = true;
                    pCaloHit->IsPossibleMip() ? mipTrackHitFound = true : showerTrackHitFound = true;
                }
            }
        }

        // Use results to identify start and end layers for mip and shower regions
        if (trackHitFound)
        {
            layersMissed = 0;
        }
        else
        {
            ++layersMissed;
        }

        if (mipTrackHitFound)
        {
            if (mipRegion1)
                mipRegion1EndLayer = iLayer;

            if (mipRegion2)
                mipRegion2EndLayer = iLayer;
        }

        if (showerTrackHitFound && showerRegion)
        {
            showerEndLayer = iLayer;
        }

        if (mipTrackHitFound && !showerTrackHitFound)
        {
            if (mipRegion1 && (iLayer < mipRegion1StartLayer))
                mipRegion1StartLayer = iLayer;

            if (mipRegion1 || mipRegion2)
                showerCount = 0;

            if (showerRegion)
            {
                if (++mipCount == m_nLayersForMipRegion)
                {
                    mipRegion2 = true;
                    showerRegion = false;
                    showerCount = 0;
                }
                else
                {
                    mipRegion2StartLayer = iLayer;
                }
            }
        }

        if (!mipTrackHitFound && showerTrackHitFound)
        {
            if (showerRegion)
                mipCount = 0;

            if (mipRegion1 || mipRegion2)
            {
                if (++showerCount == m_nLayersForShowerRegion)
                {
                    if (mipRegion1)
                    {
                        showerRegion = true;
                        mipRegion1 = false;
                        showerCount = 0;
                    }

                    if (mipRegion2)
                        shouldContinue = false;
                }
                else if (mipRegion1)
                {
                    showerStartLayer = iLayer;
                }
            }
        }

        if (layersMissed > m_maxLayersMissed)
            shouldContinue = false;
    }

    // Use above findings to determine whether to fragment cluster
    if (mipRegion2EndLayer == LAYER_MAX)
        return false;

    if ((showerEndLayer != LAYER_MAX) && (showerStartLayer == LAYER_MAX))
        return true;

    if (((mipRegion2EndLayer != LAYER_MAX) && (mipRegion2EndLayer - mipRegion2StartLayer > m_minMipRegion2Span)) &&
        ((showerStartLayer < m_maxShowerStartLayer) && (((showerEndLayer != LAYER_MAX) && (showerEndLayer - showerStartLayer > m_minShowerRegionSpan)))) )
    {
        return true;
    }

    if (((showerStartLayer < m_maxShowerStartLayer2) && (((showerEndLayer != LAYER_MAX) && (showerEndLayer - showerStartLayer > m_minShowerRegionSpan2)))) )
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::PerformFragmentation(Cluster *const pOriginalCluster, Track *const pTrack, PseudoLayer showerStartLayer,
    PseudoLayer showerEndLayer) const
{
    ClusterList clusterList;
    clusterList.insert(pOriginalCluster);
    std::string originalClustersListName, fragmentClustersListName;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList, originalClustersListName,
        fragmentClustersListName));

    // Make the cluster fragments
    Cluster *pMipCluster = NULL, *pPhotonCluster = NULL;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MakeClusterFragments(showerStartLayer, showerEndLayer, pOriginalCluster,
        pMipCluster, pPhotonCluster));

    // Decide whether to keep original cluster or the fragments
    std::string clusterListToSaveName(originalClustersListName), clusterListToDeleteName(fragmentClustersListName);

    if ((NULL != pMipCluster) && (NULL != pPhotonCluster) && (pPhotonCluster->GetNCaloHits() >= m_minHitsInPhotonCluster))
    {
        const float trackEnergy(pTrack->GetEnergyAtDca());
        const float originalChi(ReclusterHelper::GetTrackClusterCompatibility(pOriginalCluster->GetTrackComparisonEnergy(), trackEnergy));
        const float newChi(ReclusterHelper::GetTrackClusterCompatibility(pMipCluster->GetTrackComparisonEnergy(), trackEnergy));
        const float dChi2(newChi * newChi - originalChi * originalChi);

        const bool passChi2Cuts((dChi2 < m_nonPhotonDeltaChi2Cut) || (pPhotonCluster->IsPhotonFast() && (dChi2 < m_photonDeltaChi2Cut)));

        if (passChi2Cuts)
        {
            clusterListToSaveName = fragmentClustersListName;
            clusterListToDeleteName = originalClustersListName;
        }
    }

    // End cluster fragmentation operations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,
        clusterListToDeleteName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::MakeClusterFragments(const PseudoLayer showerStartLayer, const PseudoLayer showerEndLayer,
    Cluster *const pOriginalCluster, Cluster *&pMipCluster, Cluster *&pPhotonCluster) const
{
    Track *pTrack = *(pOriginalCluster->GetAssociatedTrackList().begin());
    OrderedCaloHitList orderedCaloHitList(pOriginalCluster->GetOrderedCaloHitList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(pOriginalCluster->GetIsolatedCaloHitList()));

    for (OrderedCaloHitList::const_iterator iter =  orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer iLayer = iter->first;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;
            float distance(0.f);

            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_UNCHANGED, !=, this->GetDistanceToTrack(pOriginalCluster, 
                pTrack, pCaloHit, distance));

            if ((distance < m_genericDistanceCut) || (iLayer < showerStartLayer) || (iLayer > showerEndLayer))
            {
                if (NULL == pMipCluster)
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pTrack, pMipCluster));
                }

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pMipCluster, pCaloHit));
            }
            else
            {
                if (NULL == pPhotonCluster)
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHit, pPhotonCluster));
                }
                else
                {
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pPhotonCluster, pCaloHit));
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::GetDistanceToTrack(Cluster *const pCluster, Track *const pTrack, CaloHit *const pCaloHit,
    float &distance) const
{
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const CartesianVector &trackSeedPosition(pTrack->GetTrackStateAtCalorimeter().GetPosition());

    const CartesianVector positionDifference(hitPosition - trackSeedPosition);
    const float separationSquared(positionDifference.GetMagnitudeSquared());

    if (separationSquared < m_maxTrackSeparation2)
    {
        const float flexibility(1.f + (m_trackPathWidth * std::sqrt(separationSquared / m_maxTrackSeparation2)));

        const float dCut ((GeometryHelper::GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            flexibility * (m_additionalPadWidthsFine * pCaloHit->GetCellLengthScale()) :
            flexibility * (m_additionalPadWidthsCoarse * pCaloHit->GetCellLengthScale()) );

        if (0.f == dCut)
            return STATUS_CODE_FAILURE;

        const float dPerp((pCluster->GetInitialDirection().GetCrossProduct(positionDifference)).GetMagnitude());

        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MipPhotonSeparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_trackClusterAssociationAlgName));

    // Parameters aiding decision whether to proceed with fragmentation
    m_nLayersForMipRegion = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersForMipRegion", m_nLayersForMipRegion));

    m_nLayersForShowerRegion = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLayersForShowerRegion", m_nLayersForShowerRegion));

    m_maxLayersMissed = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersMissed", m_maxLayersMissed));

    m_minMipRegion2Span = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinMipRegion2Span", m_minMipRegion2Span));

    m_maxShowerStartLayer = 20;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxShowerStartLayer", m_maxShowerStartLayer));

    m_minShowerRegionSpan = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinShowerRegionSpan", m_minShowerRegionSpan));

    m_maxShowerStartLayer2 = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxShowerStartLayer2", m_maxShowerStartLayer2));

    m_minShowerRegionSpan2 = 200;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinShowerRegionSpan2", m_minShowerRegionSpan2));

    // Parameters aiding selection of original clusters or new fragments
    m_nonPhotonDeltaChi2Cut = 0.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NonPhotonDeltaChi2Cut", m_nonPhotonDeltaChi2Cut));

    m_photonDeltaChi2Cut = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonDeltaChi2Cut", m_photonDeltaChi2Cut));

    m_minHitsInPhotonCluster = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInPhotonCluster", m_minHitsInPhotonCluster));

    // Generic distance to track parameters
    m_genericDistanceCut = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GenericDistanceCut", m_genericDistanceCut));

    m_trackPathWidth = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackPathWidth", m_trackPathWidth));

    float maxTrackSeparation = 1000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackSeparation", maxTrackSeparation));

    m_maxTrackSeparation2 = maxTrackSeparation * maxTrackSeparation;

    if (0.f == m_maxTrackSeparation2)
        return STATUS_CODE_INVALID_PARAMETER;

    m_additionalPadWidthsFine = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsFine", m_additionalPadWidthsFine));

    m_additionalPadWidthsCoarse = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsCoarse", m_additionalPadWidthsCoarse));

    return STATUS_CODE_SUCCESS;
}
