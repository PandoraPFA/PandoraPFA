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
    // Construct a map from clusters to energy contributions from each mc particle
    typedef std::map<const MCParticle*, float> EnergyPerMCParticle;
    typedef std::map<Cluster*, EnergyPerMCParticle> EnergyPerMCParticleInClusters;
    EnergyPerMCParticleInClusters energyPerMCParticleInClusters;

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster)
    {
        Cluster *const pCluster = (*itCluster);

        std::pair<EnergyPerMCParticleInClusters::iterator, bool> pairMCInClusterBool;
        pairMCInClusterBool = energyPerMCParticleInClusters.insert(EnergyPerMCParticleInClusters::value_type(pCluster, EnergyPerMCParticle()));

        if (!pairMCInClusterBool.second)
            return STATUS_CODE_FAILURE;

        EnergyPerMCParticle &energyPerMCParticle(pairMCInClusterBool.first->second);

        const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

        for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
        {
            for (CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                CaloHit *pCaloHit = *hitIter;
                const float hitEnergy(pCaloHit->GetElectromagneticEnergy());

                const MCParticle *pMCParticle = NULL;
                pCaloHit->GetMCParticle(pMCParticle);

                // Some CalorimeterHits don't have a MCParticle (e.g. noise)
                if (NULL == pMCParticle)
                    continue;

                EnergyPerMCParticle::iterator itEPerMCParticle = energyPerMCParticle.find(pMCParticle);

                if (energyPerMCParticle.end() == itEPerMCParticle)
                {
                    if (!energyPerMCParticle.insert(EnergyPerMCParticle::value_type(pMCParticle, hitEnergy)).second)
                        return STATUS_CODE_FAILURE;
                }
                else
                {
                    itEPerMCParticle->second += hitEnergy;
                }
            }
        }
    }

    // Construct a map from mc particle to clusters
    typedef std::map<const MCParticle*, Cluster*> MCParticleToCluster;
    typedef std::map<const MCParticle*, float> MCParticleToEnergy;
    MCParticleToCluster mcParticleToCluster;
    MCParticleToEnergy mcParticleToEnergy;

    for (EnergyPerMCParticleInClusters::const_iterator itCluster = energyPerMCParticleInClusters.begin(), itClusterEnd = energyPerMCParticleInClusters.end();
        itCluster != itClusterEnd; ++itCluster)
    {
        Cluster *const pCluster = itCluster->first;
        const EnergyPerMCParticle &energyPerMCParticle = itCluster->second;

        for (EnergyPerMCParticle::const_iterator itEPerMC = energyPerMCParticle.begin(), itEPerMCEnd = energyPerMCParticle.end();
            itEPerMC != itEPerMCEnd; ++itEPerMC)
        {
            const MCParticle *pMCParticle = itEPerMC->first;
            const float energy(itEPerMC->second);

            MCParticleToEnergy::const_iterator itMCParticleToEnergy = mcParticleToEnergy.find(pMCParticle);

            if (mcParticleToEnergy.end() == itMCParticleToEnergy)
            {
                if (!mcParticleToEnergy.insert(MCParticleToEnergy::value_type(pMCParticle, energy)).second)
                    return STATUS_CODE_FAILURE;

                if (!mcParticleToCluster.insert(MCParticleToCluster::value_type(pMCParticle, pCluster)).second)
                    return STATUS_CODE_FAILURE;

                continue;
            }

            if (itMCParticleToEnergy->second > energy)
                continue;

            mcParticleToCluster.erase(pMCParticle);
            mcParticleToEnergy.erase(pMCParticle);

            if (!mcParticleToEnergy.insert(MCParticleToEnergy::value_type(pMCParticle, energy)).second)
                return STATUS_CODE_FAILURE;

            if (!mcParticleToCluster.insert(MCParticleToCluster::value_type(pMCParticle, pCluster)).second)
                return STATUS_CODE_FAILURE;
        }
    }

    // Construct a map from mc particle to tracks
    typedef std::map< const MCParticle*, TrackList*> TracksPerMCParticle;
    TracksPerMCParticle tracksPerMCParticle;

    const TrackList *pCurrentTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pCurrentTrackList));

    for (TrackList::const_iterator itTrack = pCurrentTrackList->begin(), itTrackEnd = pCurrentTrackList->end(); itTrack != itTrackEnd; ++itTrack)
    {
        const MCParticle *pMCParticle = NULL;
        (*itTrack)->GetMCParticle(pMCParticle);

        if (NULL == pMCParticle)
            continue;

        TracksPerMCParticle::iterator itTracksPerMCParticle(tracksPerMCParticle.find(pMCParticle));

        if (tracksPerMCParticle.end() == itTracksPerMCParticle)
        {
            TrackList *pTrackList = new TrackList();
            pTrackList->insert((*itTrack));
            tracksPerMCParticle.insert(std::make_pair(pMCParticle, pTrackList));
        }
        else
        {
            itTracksPerMCParticle->second->insert((*itTrack));
        }
    }

    // Make the track to cluster associations
    for (TracksPerMCParticle::iterator itMCToTracks = tracksPerMCParticle.begin(), itMCToTracksEnd = tracksPerMCParticle.end();
        itMCToTracks != itMCToTracksEnd; ++itMCToTracks)
    {
        const MCParticle *pMCParticle = itMCToTracks->first;
        TrackList *pTrackList = itMCToTracks->second;

        MCParticleToCluster::const_iterator itMCParticleToCluster(mcParticleToCluster.find(pMCParticle));

        if (mcParticleToCluster.end() == itMCParticleToCluster)
            continue;

        Cluster *pCluster = itMCParticleToCluster->second;

        for (TrackList::iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack)
        {
            Track *pTrack = (*itTrack);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pCluster));
        }
    }

    // Delete the created TrackLists
    for (TracksPerMCParticle::iterator itTrackList = tracksPerMCParticle.begin(), itTrackListEnd = tracksPerMCParticle.end();
         itTrackList != itTrackListEnd; ++itTrackList)
    {
        delete (*itTrackList).second;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingTrackToClusterMatching::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}
