/**
 *	@file	PandoraPFANew/src/Algorithms/ClusteringAlgorithm.cc
 * 
 *	@brief	Implementation of the clustering algorithm class.
 * 
 *	$Log: $
 */

#include "Algorithms/ClusteringAlgorithm.h"

using namespace pandora;

StatusCode ClusteringAlgorithm::Run()
{
	const TrackList *pTrackList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

	const OrderedCaloHitList *pOrderedCaloHitList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

	// Test - create a cluster from every calo hit
	for (OrderedCaloHitList::const_iterator iter = pOrderedCaloHitList->begin(), iterEnd = pOrderedCaloHitList->end(); iter != iterEnd; ++iter)
	{
	
		for (CaloHitList::const_iterator caloHitIter = iter->second->begin(), caloHitIterEnd = iter->second->end(); caloHitIter != caloHitIterEnd; ++caloHitIter)
		{
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, *caloHitIter));	
		}
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
	return STATUS_CODE_SUCCESS;
}
