/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstruction/TrackRecoveryInteractionsAlgorithm.cc
 * 
 *  @brief  Implementation of the track recovery interactions algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/TrackRecoveryInteractionsAlgorithm.h"

#include "Helpers/ReclusterHelper.h"

using namespace pandora;

StatusCode TrackRecoveryInteractionsAlgorithm::Run()
{
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    TrackVector trackVector(pTrackList->begin(), pTrackList->end());
    std::sort(trackVector.begin(), trackVector.end(), Track::SortByEnergy);

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    // Loop over all unassociated tracks in the current track list
    for (TrackVector::const_iterator iterT = trackVector.begin(), iterTEnd = trackVector.end(); iterT != iterTEnd; ++iterT)
    {
        Track *pTrack = *iterT;

        // Use only unassociated tracks that can be used to form a pfo
        if (pTrack->HasAssociatedCluster() || !pTrack->CanFormPfo())
            continue;

        if (!pTrack->GetDaughterTrackList().empty())
            continue;

        if ((std::fabs(pTrack->GetD0()) > m_maxAbsoluteTrackD0) || (std::fabs(pTrack->GetZ0()) > m_maxAbsoluteTrackZ0))
            continue;

        // Identify best cluster to be associated with this track, based on energy consistency and proximity
        Cluster *pBestCluster(NULL);
        float smallestTrackClusterDistance(std::numeric_limits<float>::max());

        for (ClusterList::const_iterator iterC = pClusterList->begin(), iterCEnd = pClusterList->end(); iterC != iterCEnd; ++iterC)
        {
            Cluster *pCluster = *iterC;

            if (!pCluster->GetAssociatedTrackList().empty() || (0 == pCluster->GetNCaloHits()) || pCluster->IsPhoton())
                continue;

            float trackClusterDistance(std::numeric_limits<float>::max());

            if (STATUS_CODE_SUCCESS != ClusterHelper::GetTrackClusterDistance(pTrack, pCluster, m_maxSearchLayer, m_parallelDistanceCut,
                trackClusterDistance))
            {
                continue;
            }

            if (trackClusterDistance < smallestTrackClusterDistance)
            {
                smallestTrackClusterDistance = trackClusterDistance;
                pBestCluster = pCluster;
            }
        }

        if ((NULL == pBestCluster) || (smallestTrackClusterDistance > m_maxTrackClusterDistance))
            continue;

        // Should track be associated with "best" cluster?
        const float clusterEnergy(pBestCluster->GetHadronicEnergy());

        if ((smallestTrackClusterDistance > m_trackClusterDistanceCut) && (clusterEnergy > m_clusterEnergyCut))
        {
            const CartesianVector &trackECalPosition(pTrack->GetTrackStateAtECal().GetPosition());
            const CartesianVector &trackerEndPosition(pTrack->GetTrackStateAtEnd().GetPosition());
            const CartesianVector innerLayerCentroid(pBestCluster->GetCentroid(pBestCluster->GetInnerPseudoLayer()));

            const CartesianVector trackerToTrackECalUnitVector((trackECalPosition - trackerEndPosition).GetUnitVector());
            const CartesianVector trackerToClusterUnitVector((innerLayerCentroid - trackerEndPosition).GetUnitVector());

            const float directionCosine(trackerToClusterUnitVector.GetDotProduct(trackerToTrackECalUnitVector));

            if (directionCosine < m_directionCosineCut)
                continue;
        }

        const float trackEnergy(pTrack->GetEnergyAtDca());
        const float chi(ReclusterHelper::GetTrackClusterCompatibility(clusterEnergy, trackEnergy));

        if (chi < m_maxTrackAssociationChi)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pBestCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackRecoveryInteractionsAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_maxAbsoluteTrackD0 = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxAbsoluteTrackD0", m_maxAbsoluteTrackD0));

    m_maxAbsoluteTrackZ0 = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxAbsoluteTrackZ0", m_maxAbsoluteTrackZ0));

    m_maxTrackClusterDistance = 200.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackClusterDistance", m_maxTrackClusterDistance));

    m_trackClusterDistanceCut = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackClusterDistanceCut", m_trackClusterDistanceCut));

    m_clusterEnergyCut = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterEnergyCut", m_clusterEnergyCut));

    m_directionCosineCut = 0.9f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DirectionCosineCut", m_directionCosineCut));

    m_maxTrackAssociationChi = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackAssociationChi", m_maxTrackAssociationChi));

    m_maxSearchLayer = 19;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxSearchLayer", m_maxSearchLayer));

    m_parallelDistanceCut = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ParallelDistanceCut", m_parallelDistanceCut));

    return STATUS_CODE_SUCCESS;
}
