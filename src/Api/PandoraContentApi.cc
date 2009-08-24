/**
 *	@file	PandoraPFANew/src/Api/PandoraContentApi.cc
 * 
 *	@brief	Implementation of the pandora content api class.
 * 
 *	$Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

template <typename CLUSTER_PARAMETERS>
StatusCode PandoraContentApi::Cluster::Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters)
{
	return algorithm.GetPandoraContentApiImpl()->CreateCluster(pClusterParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::ParticleFlowObject::Create(const pandora::Algorithm &algorithm, const Parameters &parameters)
{
	return algorithm.GetPandoraContentApiImpl()->CreateParticleFlowObject(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::MatchCaloHitsToMCPfoTargets(const pandora::Pandora &pandora)
{
	return pandora.GetPandoraContentApiImpl()->MatchCaloHitsToMCPfoTargets();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RunAlgorithm(const pandora::Pandora &pandora, const std::string &algorithmName)
{
	return pandora.GetPandoraContentApiImpl()->RunAlgorithm(algorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RunAlgorithm(const pandora::Algorithm &algorithm, const std::string &algorithmName)
{
	return algorithm.GetPandoraContentApiImpl()->RunAlgorithm(algorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentClusterList(const pandora::Algorithm &algorithm, pandora::ClusterList *const pClusterList)
{
	std::string clusterListName;
	return PandoraContentApi::GetCurrentClusterList(algorithm, pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentClusterList(const pandora::Algorithm &algorithm, pandora::ClusterList *const pClusterList,
	std::string &clusterListName)
{
	return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm,
	pandora::OrderedCaloHitList *const pOrderedCaloHitList)
{
	std::string orderedCaloHitListName;
	return PandoraContentApi::GetCurrentOrderedCaloHitList(algorithm, pOrderedCaloHitList, orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm,
	pandora::OrderedCaloHitList *const pOrderedCaloHitList, std::string &orderedCaloHitListName)
{
	return algorithm.GetPandoraContentApiImpl()->GetCurrentOrderedCaloHitList(pOrderedCaloHitList, orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentTrackList(const pandora::Algorithm &algorithm, pandora::TrackList *const pTrackList)
{
	std::string trackListName;
	return PandoraContentApi::GetCurrentTrackList(algorithm, pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentTrackList(const pandora::Algorithm &algorithm, pandora::TrackList *const pTrackList,
	std::string &trackListName)
{
	return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::InitializeReclustering(const pandora::Algorithm &algorithm, const pandora::TrackList &inputTrackList,
	const pandora::ClusterList &inputClusterList, std::string &originalClustersListName)
{
	return algorithm.GetPandoraContentApiImpl()->InitializeReclustering(algorithm, inputTrackList, inputClusterList, originalClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName)
{
	return algorithm.m_pPandora->GetPandoraContentApiImpl()->EndReclustering(algorithm, selectedClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
	pandora::ClusterList *pNewClusterList)
{
	std::string newClusterListName;
	return PandoraContentApi::RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
	pandora::ClusterList *pNewClusterList, std::string &newClusterListName)
{
	return algorithm.GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterListAndRemoveCaloHits(const pandora::Algorithm &algorithm, const std::string newClusterListName)
{
	return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndRemoveCaloHits(algorithm, newClusterListName);
}
		
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterListAndRemoveCaloHits(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
	const pandora::ClusterList &clustersToSave)
{
	return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndRemoveCaloHits(algorithm, newClusterListName, &clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
	return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndReplaceCurrent(algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
	const pandora::ClusterList &clustersToSave)
{
	return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndReplaceCurrent(algorithm, newClusterListName, &clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit);
template StatusCode PandoraContentApi::Cluster::Create<pandora::InputCaloHitList>(const pandora::Algorithm &algorithm, pandora::InputCaloHitList *pInputCaloHitList);
template StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack);
