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
	TrackList *pTrackList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

	OrderedCaloHitList *pOrderedCaloHitList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

	// Use tracks and ordered calo hits to create n clusters
	CaloHit *pCaloHit = NULL; // dummy
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHit));

	return STATUS_CODE_SUCCESS;
}
