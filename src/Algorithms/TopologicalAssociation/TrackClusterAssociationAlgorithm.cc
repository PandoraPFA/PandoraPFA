/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/TrackClusterAssociationAlgorithm.cc
 * 
 *  @brief  Implementation of the track-cluster association algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/TrackClusterAssociationAlgorithm.h"

#include <cmath>
#include <limits>

using namespace pandora;

StatusCode TrackClusterAssociationAlgorithm::Run()
{
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    // Clear any existing track - cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCurrentTrackClusterAssociations(*this));

    // Look to make new associations
    for (TrackList::const_iterator trackIter = pTrackList->begin(), trackIterEnd = pTrackList->end();
        trackIter != trackIterEnd; ++trackIter)
    {
        Track *pTrack = *trackIter;

        // Use only tracks that are flagged as reaching ECal
        if (!pTrack->ReachesECal())
            continue;

        if (!pTrack->GetDaughterTrackList().empty())
            continue;

        if ((std::fabs(pTrack->GetD0()) > m_maxAbsoluteTrackD0) || (std::fabs(pTrack->GetZ0()) > m_maxAbsoluteTrackZ0))
            continue;

        Cluster *pBestCluster = NULL;
        Cluster *pBestLowEnergyCluster = NULL;

        float minDistance(std::numeric_limits<float>::max());
        float minLowEnergyDistance(std::numeric_limits<float>::max());

        // Identify the closest cluster and also the closest cluster below a specified hadronic energy threshold
        for (ClusterList::const_iterator clusterIter = pClusterList->begin(), clusterIterEnd = pClusterList->end();
            clusterIter != clusterIterEnd; ++clusterIter)
        {
            Cluster *pCluster = *clusterIter;

            if (0 == pCluster->GetNCaloHits() || pCluster->IsPhoton())
                continue;

            float trackClusterDistance(std::numeric_limits<float>::max());
            if (STATUS_CODE_SUCCESS != ClusterHelper::GetTrackClusterDistance(pTrack, pCluster, m_maxSearchLayer, m_parallelDistanceCut, trackClusterDistance))
                continue;

            if (pCluster->GetHadronicEnergy() > m_lowEnergyCut)
            {
                if (trackClusterDistance < minDistance)
                {
                    minDistance = trackClusterDistance;
                    pBestCluster = pCluster;
                }
            }
            else
            {
                if (trackClusterDistance < minLowEnergyDistance)
                {
                    minLowEnergyDistance = trackClusterDistance;
                    pBestLowEnergyCluster = pCluster;
                }
            }
        }

        // Apply a final track-cluster association distance cut
        Cluster *pMatchedCluster = NULL;

        if ((minDistance < m_maxTrackClusterDistance) && (NULL != pBestCluster))
        {
            pMatchedCluster = pBestCluster;
        }
        else if ((minLowEnergyDistance < m_maxTrackClusterDistance) && (NULL != pBestLowEnergyCluster))
        {
            pMatchedCluster = pBestLowEnergyCluster;
        }

        // Now make the association
        if (NULL != pMatchedCluster)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pMatchedCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackClusterAssociationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_maxAbsoluteTrackD0 = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxAbsoluteTrackD0", m_maxAbsoluteTrackD0));

    m_maxAbsoluteTrackZ0 = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxAbsoluteTrackZ0", m_maxAbsoluteTrackZ0));

    m_lowEnergyCut = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCut", m_lowEnergyCut));

    m_maxTrackClusterDistance = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackClusterDistance", m_maxTrackClusterDistance));

    m_maxSearchLayer = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxSearchLayer", m_maxSearchLayer));

    m_parallelDistanceCut = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ParallelDistanceCut", m_parallelDistanceCut));

    return STATUS_CODE_SUCCESS;
}
