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

	pClusterList = NULL;
	std::string clusterListName;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList, clusterListName));
	
	for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
	{
		std::cout << "Cluster! " << *iter << std::endl;
	}

	return STATUS_CODE_SUCCESS;
}
