/**
 *	@file	PandoraPFANew/src/Algorithms/PhotonClusteringAlgorithm.cc
 * 
 *	@brief	Implementation of the photon clustering algorithm class.
 * 
 *	$Log: $
 */

#include "Algorithms/PhotonClusteringAlgorithm.h"

using namespace pandora;

StatusCode PhotonClusteringAlgorithm::Run()
{
	// Run initial clustering algorithm
	const ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, "Clustering", pClusterList));

	// Select some clusters (a subset of those in pClusterList) to save
	ClusterList clustersToSave;

	//Save the clusters and remove the hits - clustersToSave argument is optional
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, "newClusterListName",
		clustersToSave));

	return STATUS_CODE_SUCCESS;
}
