/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/CheatingTrackToClusterMatching.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/CheatingTrackToClusterMatching.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode CheatingTrackToClusterMatching::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList( *this, pClusterList ) );

    // Consider each cluster in the list and identify mc particle sources
    typedef std::map< const MCParticle*, float > EnergyPerMCParticle;
    typedef std::map< Cluster *const, EnergyPerMCParticle > EnergyPerMCParticleInClusters;

    EnergyPerMCParticleInClusters energyPerMCParticleInClusters;

    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster)
    {
        Cluster *const pCluster = (*itCluster);

        // Add cluster to the map
        std::pair<EnergyPerMCParticleInClusters::iterator, bool> pairMCInClusterBool;
        pairMCInClusterBool = energyPerMCParticleInClusters.insert(EnergyPerMCParticleInClusters::value_type(pCluster, EnergyPerMCParticle()));

        if (!pairMCInClusterBool.second)
            return STATUS_CODE_FAILURE;

        EnergyPerMCParticle &energyPerMCParticle(pairMCInClusterBool.first->second);

        const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

        for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
        {
            for(CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                CaloHit* pCaloHit = *hitIter;

                // TODO option to take hadronic energy as instead?
                float energy = pCaloHit->GetElectromagneticEnergy();

                const MCParticle *pMCParticle = NULL; 
                pCaloHit->GetMCParticle(pMCParticle);

                // Some CalorimeterHits don't have a MCParticle (e.g. noise)
                if (NULL == pMCParticle)
                    continue;

                EnergyPerMCParticle::iterator itEPerMCParticle = energyPerMCParticle.find(pMCParticle);

                if (itEPerMCParticle == energyPerMCParticle.end())
                {
                    std::pair< EnergyPerMCParticle::iterator, bool > pairEPerMCParticle_bool;
                    pairEPerMCParticle_bool = energyPerMCParticle.insert(EnergyPerMCParticle::value_type(pMCParticle, 0.f));

                    if (!pairEPerMCParticle_bool.second)
                        return STATUS_CODE_FAILURE;

                    itEPerMCParticle = pairEPerMCParticle_bool.first;
                }

                itEPerMCParticle->second += energy;
            }
        }

    }


    // Make the MCParticle to Cluster associations
    typedef std::map< const MCParticle*, Cluster *const > MCParticleToCluster;
    typedef std::map< const MCParticle*, float > MCParticleToEnergy;
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
            const float energy = itEPerMC->second;

            MCParticleToEnergy::const_iterator itMCParticleToEnergy = mcParticleToEnergy.find(pMCParticle);

            if (itMCParticleToEnergy == mcParticleToEnergy.end())
            {
                std::pair<MCParticleToEnergy::iterator, bool> mcPtclToEBool;
                mcPtclToEBool = mcParticleToEnergy.insert(MCParticleToEnergy::value_type(pMCParticle, energy));

                if(!mcPtclToEBool.second)
                    return STATUS_CODE_FAILURE;

                if(!mcParticleToCluster.insert(MCParticleToCluster::value_type(pMCParticle, pCluster)).second)
                    return STATUS_CODE_FAILURE;

                continue;
            }
        
            if (itMCParticleToEnergy->second > energy)
                continue;

            // If the energy deposited in the calohits of this MCParticle is larger than of the old one, replace the old by this.
            mcParticleToCluster.erase(pMCParticle);
            mcParticleToEnergy.erase(pMCParticle);

            if (!mcParticleToEnergy.insert(MCParticleToEnergy::value_type(pMCParticle, energy)).second)
                return STATUS_CODE_FAILURE;

            if (!mcParticleToCluster.insert( MCParticleToCluster::value_type(pMCParticle, pCluster)).second)
                return STATUS_CODE_FAILURE;
        }
    }


    // Make the MCParticle to track associations
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    typedef std::map< const MCParticle*, TrackList* > TracksPerMCParticle;
    TracksPerMCParticle tracksPerMCParticle;
    TracksPerMCParticle::iterator itTracksPerMCParticle;

    for (TrackList::const_iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack)
    {
        const MCParticle *pMCParticle = NULL;
        (*itTrack)->GetMCParticle(pMCParticle);

        // Maybe an error should be thrown here?
        if (NULL == pMCParticle)
            continue;

        TrackList *pCurrentTrackList = NULL;

        itTracksPerMCParticle = tracksPerMCParticle.find(pMCParticle);

        if (itTracksPerMCParticle == tracksPerMCParticle.end())
        {
            pCurrentTrackList = new TrackList();
            tracksPerMCParticle.insert(std::make_pair(pMCParticle, pCurrentTrackList));
        }
        else
        {
            pCurrentTrackList = itTracksPerMCParticle->second;
        }

        pCurrentTrackList->insert((*itTrack));
    }


    // Associate the tracks to the clusters on the basis of their MCParticles
    for(TracksPerMCParticle::iterator itMCToTracks = tracksPerMCParticle.begin(), itMCToTracksEnd = tracksPerMCParticle.end();
        itMCToTracks != itMCToTracksEnd; ++itMCToTracks)
    {
        const MCParticle *pMCParticle = itMCToTracks->first;
        TrackList *pTrackList = itMCToTracks->second;

        MCParticleToCluster::const_iterator itMCParticleToCluster = mcParticleToCluster.find(pMCParticle);

        if (itMCParticleToCluster == mcParticleToCluster.end())
            continue;

        Cluster *const pCluster = itMCParticleToCluster->second;

        for (TrackList::iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack)
        {
            Track* pTrack = (*itTrack);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pCluster));
        }
    }


    // Delete the created TrackLists
    for( TracksPerMCParticle::iterator itTrackList = tracksPerMCParticle.begin(), itTrackListEnd = tracksPerMCParticle.end(); 
         itTrackList != itTrackListEnd; ++itTrackList )
    {
        delete (*itTrackList).second;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingTrackToClusterMatching::ReadSettings(const TiXmlHandle xmlHandle)
{
    return STATUS_CODE_SUCCESS;
}
