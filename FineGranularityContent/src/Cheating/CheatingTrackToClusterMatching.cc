/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Cheating/CheatingTrackToClusterMatching.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Cheating/CheatingTrackToClusterMatching.h"

using namespace pandora;

StatusCode CheatingTrackToClusterMatching::Run()
{
    // Read current lists
    const TrackList *pCurrentTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pCurrentTrackList));

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    // Clear any existing track - cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCurrentTrackClusterAssociations(*this));

    // Construct a map from mc particle to tracks
    typedef std::map<const MCParticle*, TrackList*> TracksPerMCParticle;
    TracksPerMCParticle tracksPerMCParticle;

    for (TrackList::const_iterator iter = pCurrentTrackList->begin(), iterEnd = pCurrentTrackList->end(); iter != iterEnd; ++iter)
    {
        Track *const pTrack = *iter;

        const MCParticle *pMCParticle = NULL;
        (void) pTrack->GetMCParticle(pMCParticle);

        if (NULL == pMCParticle)
            continue;

        TracksPerMCParticle::iterator itTracksPerMCParticle(tracksPerMCParticle.find(pMCParticle));

        if (tracksPerMCParticle.end() == itTracksPerMCParticle)
        {
            TrackList *pTrackList = new TrackList();
            pTrackList->insert(pTrack);

            if (!tracksPerMCParticle.insert(TracksPerMCParticle::value_type(pMCParticle, pTrackList)).second)
                throw StatusCodeException(STATUS_CODE_FAILURE);
        }
        else
        {
            itTracksPerMCParticle->second->insert(pTrack);
        }
    }

    // Construct a map from mc particle to clusters
    typedef std::map<const MCParticle*, ClusterList*> ClustersPerMCParticle;
    ClustersPerMCParticle clustersPerMCParticle;

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *const pCluster = *iter;

        const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

        if (NULL == pMCParticle)
            continue;

        ClustersPerMCParticle::iterator itClustersPerMCParticle(clustersPerMCParticle.find(pMCParticle));

        if (clustersPerMCParticle.end() == itClustersPerMCParticle)
        {
            ClusterList *pClusterList = new ClusterList();
            pClusterList->insert(pCluster);

            if (!clustersPerMCParticle.insert(ClustersPerMCParticle::value_type(pMCParticle, pClusterList)).second)
                throw StatusCodeException(STATUS_CODE_FAILURE);
        }
        else
        {
            itClustersPerMCParticle->second->insert(pCluster);
        }
    }

    // Make the track to cluster associations
    for (TracksPerMCParticle::const_iterator iter = tracksPerMCParticle.begin(), iterEnd = tracksPerMCParticle.end(); iter != iterEnd; ++iter)
    {
        const MCParticle *pMCParticle = iter->first;
        TrackList *pTrackList = iter->second;

        ClustersPerMCParticle::const_iterator itClustersPerMCParticle(clustersPerMCParticle.find(pMCParticle));

        if (clustersPerMCParticle.end() == itClustersPerMCParticle)
            continue;

        ClusterList *pClusterList = itClustersPerMCParticle->second;

        if (pTrackList->empty() || pClusterList->empty())
            continue;

        for (TrackList::const_iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack)
        {
            // If the mc particle is associated with multiple clusters, can only associate to highest energy cluster (clusters should be merged)
            Cluster *pHighestEnergyCluster = NULL;
            float highestEnergy(-std::numeric_limits<float>::max());

            for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster)
            {
                const float clusterEnergy((*itCluster)->GetHadronicEnergy());

                if (clusterEnergy > highestEnergy)
                {
                    highestEnergy = clusterEnergy;
                    pHighestEnergyCluster = *itCluster;
                }
            }

            if (NULL == pHighestEnergyCluster)
                throw StatusCodeException(STATUS_CODE_FAILURE);

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, *itTrack, pHighestEnergyCluster));
        }
    }

    // Delete created lists
    for (TracksPerMCParticle::iterator iter = tracksPerMCParticle.begin(), iterEnd = tracksPerMCParticle.end(); iter != iterEnd; ++iter)
        delete iter->second;

    for (ClustersPerMCParticle::iterator iter = clustersPerMCParticle.begin(), iterEnd = clustersPerMCParticle.end(); iter != iterEnd; ++iter)
        delete iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingTrackToClusterMatching::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}
