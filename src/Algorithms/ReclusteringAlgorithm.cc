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
	const TrackList *pInputTrackList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pInputTrackList));

	const ClusterList *pInputClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pInputClusterList));

	for (ClusterList::const_iterator clusterIter = pInputClusterList->begin(); clusterIter != pInputClusterList->end(); ++clusterIter)
	{
		// Select clusters and tracks to use for reclustering
		TrackList reclusterTrackList;
		ClusterList reclusterClusterList;
		
		// For now, by way of example, just pair the first track with each cluster
		reclusterTrackList.insert(*pInputTrackList->begin());
		reclusterClusterList.insert(*clusterIter);
Cluster *pTempCluster = *clusterIter; //HACK doesn't work unless restoring original clusters
	
		// Initialize reclustering with these local lists
		std::string originalClustersListName;
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeReclustering(*this, reclusterTrackList, 
			reclusterClusterList, originalClustersListName));

		// Repeat for many clustering algorithms, using pReclusterCandidates to calculate a figure of merit and
		// identify the best recluster candidates. For now, just put things back as they were.
		std::string bestReclusterCandidateListName = originalClustersListName;

		for (StringVector::const_iterator clusteringIter = m_clusteringAlgorithms.begin(),
			clusteringIterEnd = m_clusteringAlgorithms.end(); clusteringIter != clusteringIterEnd; ++clusteringIter)
		{
			std::string reclusterCandidatesListName;
			const ClusterList *pReclusterCandidatesList = NULL;
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, *clusteringIter, 
				pReclusterCandidatesList, reclusterCandidatesListName));
		}

		// Choose best clusters, which may be the originals
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndReclustering(*this, bestReclusterCandidateListName));

clusterIter = pInputClusterList->find(pTempCluster); //HACK doesn't work unless restoring original clusters
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
	// Daughter clustering algorithm
	TiXmlHandle algorithmListHandle = TiXmlHandle(xmlHandle.FirstChild("clusteringAlgorithms").Element());

	for (TiXmlElement *pXmlElement = algorithmListHandle.FirstChild("algorithm").Element(); NULL != pXmlElement;
		pXmlElement = pXmlElement->NextSiblingElement("algorithm"))
	{
		std::string algorithmName;
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateDaughterAlgorithm(*this, pXmlElement, algorithmName));
		m_clusteringAlgorithms.push_back(algorithmName);
	}	

	return STATUS_CODE_SUCCESS;
}
