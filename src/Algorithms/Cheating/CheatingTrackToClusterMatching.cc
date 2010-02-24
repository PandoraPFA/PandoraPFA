/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/CheatingTrackToClusterMatching.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/CheatingTrackToClusterMatching.h"

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

#include <sstream>

using namespace pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingTrackToClusterMatching::Run()
{
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));


    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList( *this, pClusterList ) );


    // get the energy of the CaloHits classified by their creating MCParticle for each of the clusters
    typedef std::map< const MCParticle*, float > EnergyPerMcParticle;
    typedef std::map< Cluster *const, EnergyPerMcParticle > EnergyPerMcParticleInClusters;

    EnergyPerMcParticleInClusters energyPerMcParticleInClusters;

    for( ClusterList::iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
    {
        Cluster *const pCluster = (*itCluster);

        // add the new cluster to the map
        std::pair<EnergyPerMcParticleInClusters::iterator,bool> pairMcInClusterBool = energyPerMcParticleInClusters.insert( EnergyPerMcParticleInClusters::value_type(pCluster, EnergyPerMcParticle()) );
        if( !pairMcInClusterBool.second )
            return STATUS_CODE_FAILURE;
        EnergyPerMcParticleInClusters::iterator itMcInCluster = pairMcInClusterBool.first;


        const OrderedCaloHitList pOrderedCaloHitList = pCluster->GetOrderedCaloHitList();
        for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList.begin(), itLyrEnd = pOrderedCaloHitList.end(); itLyr != itLyrEnd; itLyr++ )
        {
            // int pseudoLayer = itLyr->first;
            CaloHitList::const_iterator itCaloHit    = itLyr->second->begin();
            CaloHitList::const_iterator itCaloHitEnd = itLyr->second->end();

            for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
            {
                CaloHit* pCaloHit = (*itCaloHit);
                
                // get the energy of the hit
                float energy = pCaloHit->GetElectromagneticEnergy(); // ??? option to take hadronic energy as instead?

                // fetch the MCParticle
                const MCParticle* mc = NULL; 
                pCaloHit->GetMCParticle( mc );
                if( mc == NULL ) continue; // has to be continue, since sometimes some CalorimeterHits don't have a MCParticle (e.g. noise)

                EnergyPerMcParticle::iterator itEPerMcParticle = itMcInCluster->second.find( mc );
                if( itEPerMcParticle == itMcInCluster->second.end() )
                {
                    std::pair< EnergyPerMcParticle::iterator, bool > pairEPerMcParticle_bool = itMcInCluster->second.insert( EnergyPerMcParticle::value_type(mc, 0.f) );
                    if( !pairEPerMcParticle_bool.second )
                        return STATUS_CODE_FAILURE;
                    itEPerMcParticle = pairEPerMcParticle_bool.first;
                }
                
                itEPerMcParticle->second += energy;
            }
        }

    }


    // do the MCParticle to Cluster association
    typedef std::map< const MCParticle*, Cluster *const > McParticleToCluster;
    typedef std::map< const MCParticle*, float >          McParticleToEnergy;
    
    McParticleToCluster mcParticleToCluster;
    McParticleToEnergy  mcParticleToEnergy;

    for( EnergyPerMcParticleInClusters::iterator itCluster = energyPerMcParticleInClusters.begin(), itClusterEnd = energyPerMcParticleInClusters.end(); itCluster != itClusterEnd; ++itCluster )
    {
        Cluster *const pCluster = itCluster->first;
        EnergyPerMcParticle& energyPerMcParticle = itCluster->second;

        for( EnergyPerMcParticle::iterator itEPerMc = energyPerMcParticle.begin(), itEPerMcEnd = energyPerMcParticle.end(); itEPerMc != itEPerMcEnd; ++itEPerMc )
        {
            const MCParticle* mc = itEPerMc->first;
            float energy = itEPerMc->second;

            McParticleToEnergy::const_iterator itMcParticleToEnergy = mcParticleToEnergy.find( mc ); 
            if( itMcParticleToEnergy == mcParticleToEnergy.end() )
            {
                std::pair< McParticleToEnergy::iterator, bool > mcPtclToEBool = mcParticleToEnergy.insert( McParticleToEnergy::value_type(mc, energy) );
                if( !mcPtclToEBool.second )
                    return STATUS_CODE_FAILURE;

                if( !mcParticleToCluster.insert( McParticleToCluster::value_type(mc, pCluster) ).second )
                    return STATUS_CODE_FAILURE;
            
                continue;
            }
        
            if( itMcParticleToEnergy->second > energy )
                continue;

            // if the energy deposited in the calohits of this MCParticle is larger than of the old one, replace the old by this.
            mcParticleToCluster.erase( mc );
            mcParticleToEnergy.erase(  mc );

            if( !mcParticleToEnergy.insert ( McParticleToEnergy::value_type ( mc, energy   ) ).second )
                return STATUS_CODE_FAILURE;
            if( !mcParticleToCluster.insert( McParticleToCluster::value_type( mc, pCluster ) ).second )
                return STATUS_CODE_FAILURE;
        }
    }





    // do the MCParticle to track association
    typedef std::map< const MCParticle*, TrackList* > TracksPerMcParticle;

    TracksPerMcParticle tracksPerMcParticle;
    TracksPerMcParticle::iterator itTracksPerMcParticle;

    for( TrackList::const_iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack )
    {
        const MCParticle* mc = NULL;
        (*itTrack)->GetMCParticle( mc );
        if( mc == NULL ) continue; // maybe an error should be thrown here?
        
        TrackList* currentTracks = NULL;
        itTracksPerMcParticle = tracksPerMcParticle.find( mc );
        if( itTracksPerMcParticle == tracksPerMcParticle.end() )
        {
            currentTracks = new TrackList();
            tracksPerMcParticle.insert( std::make_pair( mc, currentTracks ) );
        }
        else
        {
            currentTracks = itTracksPerMcParticle->second;
        }
        currentTracks->insert( (*itTrack) );
    }



    // associate the tracks to the clusters on the base of their MCParticles
    for( TracksPerMcParticle::iterator itMcToTracks = tracksPerMcParticle.begin(), itMcToTracksEnd = tracksPerMcParticle.end(); itMcToTracks != itMcToTracksEnd; ++itMcToTracks )
    {
        const MCParticle *pMc = itMcToTracks->first;
        TrackList *pTrackList = itMcToTracks->second;

        McParticleToCluster::iterator itMcParticleToCluster = mcParticleToCluster.find( pMc );
        if( itMcParticleToCluster == mcParticleToCluster.end() )
            continue;

        Cluster *const pCluster = itMcParticleToCluster->second;

        for( TrackList::iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack )
        {
            Track* pTrack = (*itTrack);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation( *this, pTrack, pCluster ) );
        }
        
    }


    // delete the created TrackLists
    for( TracksPerMcParticle::iterator itTrackList = tracksPerMcParticle.begin(), itTrackListEnd = tracksPerMcParticle.end(); 
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
