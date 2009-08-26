/**
 *	@file	PandoraPFANew/src/Algorithms/PrimaryClusteringAlgorithm.cc
 * 
 *	@brief	Implementation of the primary clustering algorithm class.
 * 
 *	$Log: $
 */

#include "Algorithms/PrimaryClusteringAlgorithm.h"

using namespace pandora;

StatusCode PrimaryClusteringAlgorithm::Run()
{
	// Run initial clustering algorithm
	const ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, "Clustering", pClusterList));

	//Save the clusters and replace current list- clustersToSave argument is optional
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterListAndReplaceCurrent(*this, "newClusterListName"));

	return STATUS_CODE_SUCCESS;
}
