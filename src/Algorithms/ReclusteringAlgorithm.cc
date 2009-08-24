/**
 *	@file	PandoraPFANew/src/Algorithms/ReclusteringAlgorithm.cc
 * 
 *	@brief	Implementation of the reclustering algorithm class.
 * 
 *	$Log: $
 */

#include "Algorithms/ReclusteringAlgorithm.h"

using namespace pandora;

StatusCode ReclusteringAlgorithm::Run()
{
	TrackList *pInputTrackList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pInputTrackList));

	ClusterList *pInputClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pInputClusterList));

	if ((NULL == pInputTrackList) || (NULL == pInputClusterList))
		return STATUS_CODE_FAILURE; // TODO remove Shouldn't need this later

	for (TrackList::const_iterator trackIter = pInputTrackList->begin(); trackIter != pInputTrackList->end(); ++trackIter)
	{

		for (ClusterList::const_iterator clusterIter = pInputClusterList->begin(); clusterIter != pInputClusterList->end(); ++clusterIter)
		{
			// Select clusters and tracks to use for reclustering
			TrackList reclusterTrackList;
			ClusterList reclusterClusterList;
			
			// Initialize reclustering with these local lists
			std::string originalClustersListName;
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeReclustering(*this, reclusterTrackList, 
				reclusterClusterList, originalClustersListName));
			
			// Repeat for many clustering algorithms, using pReclusterCandidates to calculate a figure of merit
			std::string reclusterCandidatesListName;
			ClusterList *pReclusterCandidatesList = NULL;
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, "Clustering", 
				pReclusterCandidatesList, reclusterCandidatesListName));
		
			// Choose best clusters, which may be the originals
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndReclustering(*this, reclusterCandidatesListName));
		}
	}

	return STATUS_CODE_SUCCESS;
}
