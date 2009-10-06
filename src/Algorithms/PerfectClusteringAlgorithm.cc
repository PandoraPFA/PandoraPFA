/**
 *  @file   PandoraPFANew/src/Algorithms/PerfectClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PerfectClusteringAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

#include <sstream>

using namespace pandora;

StatusCode PerfectClusteringAlgorithm::Run()
{
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    // match calohitvectors to their MCParticles
    std::map< MCParticle*, CaloHitVector* > hitsPerMCParticle;
    std::map< MCParticle*, CaloHitVector* >::iterator itHitsPerMCParticle;

    for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList->begin(), itLyrEnd = pOrderedCaloHitList->end(); itLyr != itLyrEnd; itLyr++ )
    {
        // int pseudoLayer = itLyr->first;
        CaloHitList::iterator itCaloHit    = itLyr->second->begin();
        CaloHitList::iterator itCaloHitEnd = itLyr->second->end();

        CaloHitVector* currentHits = NULL;
        for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
        {
            MCParticle* mc = NULL; 
            (*itCaloHit)->GetMCParticle( mc );
            if( mc == NULL ) continue; // has to be continue, since sometimes some CalorimeterHits don't have a MCParticle (e.g. noise)

            itHitsPerMCParticle = hitsPerMCParticle.find( mc );
            if( itHitsPerMCParticle == hitsPerMCParticle.end() )
            {
                currentHits = new CaloHitVector();
                hitsPerMCParticle.insert( std::make_pair( mc, currentHits ) );
            }
            else
            {
                currentHits = itHitsPerMCParticle->second;
            }

            currentHits->push_back( (*itCaloHit) );
        }
    }

    // do the same for the tracks (match the tracks to the mcparticles)
    std::map< MCParticle*, TrackList* > tracksPerMCParticle;
    std::map< MCParticle*, TrackList* >::iterator itTracksPerMCParticle;

    for( TrackList::const_iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack )
    {
        MCParticle* mc = NULL;
        (*itTrack)->GetMCParticle( mc );
        if( mc == NULL ) continue; // maybe an error should be thrown here?
        
        TrackList* currentTracks = NULL;
        itTracksPerMCParticle = tracksPerMCParticle.find( mc );
        if( itTracksPerMCParticle == tracksPerMCParticle.end() )
        {
            currentTracks = new TrackList();
            tracksPerMCParticle.insert( std::make_pair( mc, currentTracks ) );
        }
        else
        {
            currentTracks = itTracksPerMCParticle->second;
        }
        currentTracks->insert( (*itTrack) );
    }


    // create the clusters
    pandora::Cluster *pCluster;

    for( std::map< MCParticle*, CaloHitVector* >::iterator itCHList = hitsPerMCParticle.begin(), itCHListEnd = hitsPerMCParticle.end(); 
         itCHList != itCHListEnd; itCHList++ )
    {
        if( itCHList->first == NULL ) continue;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, itCHList->second, pCluster ));
        float energy = itCHList->first->GetEnergy();
        pCluster->SetBestEnergyEstimate( energy );

        // add the track associations to the clusters
        itTracksPerMCParticle = tracksPerMCParticle.find( itCHList->first );
        if( itTracksPerMCParticle != tracksPerMCParticle.end() )
        {
            for( TrackList::iterator itTrack = itTracksPerMCParticle->second->begin(), itTrackEnd = itTracksPerMCParticle->second->end(); itTrack != itTrackEnd; ++itTrack )
            {
                MCParticle *mc = NULL;
                (*itTrack)->GetMCParticle( mc );
//                std::cout << "add track " << (*itTrack) << " energy " << mc->GetEnergy() << "  mc " << mc << std::endl;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation( *this, (*itTrack), pCluster ) )
            }
        }
        
        // delete the created CaloHitVectors
        delete itCHList->second;
    }

    // delete the created TrackLists
    for( std::map< MCParticle*, TrackList* >::iterator itTrackList = tracksPerMCParticle.begin(), itTrackListEnd = tracksPerMCParticle.end(); 
         itTrackList != itTrackListEnd; ++itTrackList )
    {
        delete (*itTrackList).second;
    }

    return STATUS_CODE_SUCCESS;
}



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectClusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    return STATUS_CODE_SUCCESS;
}
