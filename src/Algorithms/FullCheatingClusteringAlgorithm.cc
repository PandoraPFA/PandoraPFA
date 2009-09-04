/**
 *	@file	PandoraPFANew/src/Algorithms/FullCheatingClusteringAlgorithm.cc
 * 
 *	@brief	Implementation of the clustering algorithm class.
 * 
 *	$Log: $
 */

#include "Algorithms/FullCheatingClusteringAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

#include <sstream>

using namespace pandora;

StatusCode FullCheatingClusteringAlgorithm::Run()
{
	const TrackList *pTrackList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

	const OrderedCaloHitList *pOrderedCaloHitList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

	
	std::map< MCParticle*, InputCaloHitList* > hitsPerMCParticle;
	std::map< MCParticle*, InputCaloHitList* >::iterator itHitsPerMCParticle;

	for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList->begin(), itLyrEnd = pOrderedCaloHitList->end(); itLyr != itLyrEnd; itLyr++ )
	{
		// int pseudoLayer = itLyr->first;
		CaloHitList::iterator itCaloHit    = itLyr->second->begin();
		CaloHitList::iterator itCaloHitEnd = itLyr->second->end();
		
		InputCaloHitList* currentHits = NULL;
		for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
		{
			MCParticle* mc = NULL; 
			(*itCaloHit)->GetMCParticle( mc );
			itHitsPerMCParticle = hitsPerMCParticle.find( mc );
			if( itHitsPerMCParticle == hitsPerMCParticle.end() )
			{
				currentHits = new InputCaloHitList();
				hitsPerMCParticle.insert( std::make_pair( mc, currentHits ) );
			}
			else
			{
				currentHits = itHitsPerMCParticle->second;
			}
			
			currentHits->push_back( (*itCaloHit) );
		}
	}


	pandora::Cluster *pCluster;

	// write out clusters
	for( std::map< MCParticle*, InputCaloHitList* >::iterator itCHList = hitsPerMCParticle.begin(), itCHListEnd = hitsPerMCParticle.end(); 
	     itCHList != itCHListEnd; itCHList++ )
	{
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, itCHList->second, pCluster ));
	   
		float energy = itCHList->first->GetEnergy();

//		pCluster->SetEnergy( energy );
	}

				 

	return STATUS_CODE_SUCCESS;
}



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FullCheatingClusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    // Daughter clustering algorithm
    TiXmlElement *pXmlElement = xmlHandle.FirstChild("algorithm").Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateDaughterAlgorithm(*this, pXmlElement, m_clusteringAlgorithmName));

    // Photon cluster list name
    pXmlElement = xmlHandle.FirstChild("fullCheatingClusteringListName").Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    m_fullCheatingListName = pXmlElement->GetText();

    return STATUS_CODE_SUCCESS;
}
