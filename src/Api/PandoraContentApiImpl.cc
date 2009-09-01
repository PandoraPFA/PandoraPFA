/**
 *	@file	PandoraPFANew/src/Api/PandoraContentApiImpl.cc
 * 
 *	@brief	Implementation of the pandora content api class.
 * 
 *	$Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/TrackManager.h"
#include "Managers/ParticleFlowObjectManager.h"

#include "Pandora.h"
#include "PandoraSettings.h"

namespace pandora
{

template <typename CLUSTER_PARAMETERS>
StatusCode PandoraContentApiImpl::CreateCluster(CLUSTER_PARAMETERS *pClusterParameters) const
{
	return m_pPandora->m_pClusterManager->CreateCluster(pClusterParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------	
	
StatusCode PandoraContentApiImpl::CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters) const
{
	return m_pPandora->m_pParticleFlowObjectManager->CreateParticleFlowObject(particleFlowObjectParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::CreateDaughterAlgorithm(TiXmlElement *const pXmlElement, std::string &daughterAlgorithmName) const
{
	return m_pPandora->m_pAlgorithmManager->CreateAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::RunAlgorithm(const std::string &algorithmName) const
{
	AlgorithmManager::AlgorithmMap::const_iterator iter = m_pPandora->m_pAlgorithmManager->m_algorithmMap.find(algorithmName);
	
	if (m_pPandora->m_pAlgorithmManager->m_algorithmMap.end() == iter)
		return STATUS_CODE_NOT_FOUND;

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->RegisterAlgorithm(iter->second));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RegisterAlgorithm(iter->second));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RegisterAlgorithm(iter->second));
	
	try
	{
		std::cout << "Running Algorithm: " << iter->first << ", " << iter->second->GetAlgorithmType() << std::endl;
		PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->Run());
	}
	catch (StatusCodeException &statusCodeException)
	{
		std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetAlgorithmType() << ", " << statusCodeException.ToString() << std::endl;
	}
	catch (...)
	{
		std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetAlgorithmType() << ", unrecognized exception" << std::endl;
	}

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAfterAlgorithmCompletion(iter->second));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAfterAlgorithmCompletion(iter->second));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAfterAlgorithmCompletion(iter->second));
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::MatchCaloHitsToMCPfoTargets() const
{
	UidToMCParticleMap caloHitToPfoTargetMap;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->SelectPfoTargets());
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->CreateCaloHitToPfoTargetMap(caloHitToPfoTargetMap));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->MatchCaloHitsToMCPfoTargets(caloHitToPfoTargetMap));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->DeleteNonPfoTargets());

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::OrderInputCaloHits() const
{
	return m_pPandora->m_pCaloHitManager->OrderInputCaloHits();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReadPandoraSettings(const TiXmlHandle *const pXmlHandle) const
{
	return m_pPandora->m_pPandoraSettings->Read(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::InitializeAlgorithms(const TiXmlHandle *const pXmlHandle) const
{
	return m_pPandora->m_pAlgorithmManager->InitializeAlgorithms(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::GetCurrentClusterList(const ClusterList *&pClusterList, std::string &clusterListName) const
{
	return m_pPandora->m_pClusterManager->GetCurrentList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::GetCurrentClusterListName(std::string &clusterListName) const
{
	return m_pPandora->m_pClusterManager->GetCurrentListName(clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::GetCurrentOrderedCaloHitList(const OrderedCaloHitList *&pOrderedCaloHitList,
	std::string &orderedCaloHitListName) const
{
	return m_pPandora->m_pCaloHitManager->GetCurrentList(pOrderedCaloHitList, orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::GetCurrentOrderedCaloHitListName(std::string &orderedCaloHitListName) const
{
	return m_pPandora->m_pCaloHitManager->GetCurrentListName(orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::GetCurrentTrackList(const TrackList *&pTrackList, std::string &trackListName) const
{
	return m_pPandora->m_pTrackManager->GetCurrentList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::GetCurrentTrackListName(std::string &trackListName) const
{
	return m_pPandora->m_pTrackManager->GetCurrentListName(trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::InitializeReclustering(const Algorithm &algorithm, const TrackList &inputTrackList,
	const ClusterList &inputClusterList, std::string &originalClustersListName) const
{
	std::string temporaryListName;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->CreateTemporaryListAndSetCurrent(&algorithm, inputTrackList, temporaryListName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->CreateTemporaryListAndSetCurrent(&algorithm, inputClusterList, temporaryListName));

	std::string inputClusterListName;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MoveClustersToTemporaryListAndSetCurrent(&algorithm, inputClusterListName, originalClustersListName, &inputClusterList));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const
{
	std::string inputClusterListName;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveTemporaryClusters(&algorithm, inputClusterListName, selectedClusterListName));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::RunClusteringAlgorithm(const Algorithm &algorithm, const std::string &clusteringAlgorithmName,
	const ClusterList *&pNewClusterList, std::string &newClusterListName) const
{
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MakeTemporaryListAndSetCurrent(&algorithm,	newClusterListName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RunAlgorithm(clusteringAlgorithmName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentList(pNewClusterList, newClusterListName));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
	return m_pPandora->m_pClusterManager->AddCaloHitToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::DeleteCluster(Cluster *pCluster) const
{
	return m_pPandora->m_pClusterManager->DeleteCluster(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(Cluster *pClusterLhs, Cluster *pClusterRhs) const
{
	return m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterLhs, pClusterRhs);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::SaveClusterList(const Algorithm &algorithm, const std::string &newClusterListName,
	const ClusterList *const pClustersToSave) const
{
	std::string currentClusterListName;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentListName(currentClusterListName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveTemporaryClusters(&algorithm, newClusterListName, currentClusterListName, pClustersToSave));

	const ClusterList *pNewClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetList(newClusterListName, pNewClusterList));

	std::string inputOrderedCaloHitListName;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->GetAlgorithmInputListName(&algorithm, inputOrderedCaloHitListName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->RemoveCaloHitsFromList(inputOrderedCaloHitListName, *pNewClusterList));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::ReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName) const
{
	return m_pPandora->m_pClusterManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::SaveClusterListAndReplaceCurrent(const Algorithm &algorithm, const std::string &newClusterListName,
	const ClusterList *const pClustersToSave) const
{
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SaveClusterList(algorithm, newClusterListName, pClustersToSave));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newClusterListName));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::SaveOrderedCaloHitList(const Algorithm &algorithm, const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName) const
{
	return m_pPandora->m_pCaloHitManager->SaveList(orderedCaloHitList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::ReplaceCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &newListName) const
{
	return m_pPandora->m_pCaloHitManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::SaveOrderedCaloHitListAndReplaceCurrent(const Algorithm &algorithm, const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName) const
{
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SaveOrderedCaloHitList(algorithm, orderedCaloHitList, newListName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::SaveTrackList(const Algorithm &algorithm, const TrackList &trackList, const std::string &newListName) const
{
	return m_pPandora->m_pTrackManager->SaveList(trackList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::ReplaceCurrentTrackList(const pandora::Algorithm &algorithm, const std::string &newListName) const
{
	return m_pPandora->m_pTrackManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------	

StatusCode PandoraContentApiImpl::SaveTrackListAndReplaceCurrent(const Algorithm &algorithm, const TrackList &trackList, const std::string &newListName) const
{
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SaveTrackList(algorithm, trackList, newListName));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraContentApiImpl::PandoraContentApiImpl(Pandora *pPandora) :
	m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode PandoraContentApiImpl::CreateCluster<CaloHit>(CaloHit *pCaloHit) const;
template StatusCode PandoraContentApiImpl::CreateCluster<InputCaloHitList>(InputCaloHitList *pInputCaloHitList) const;
template StatusCode PandoraContentApiImpl::CreateCluster<Track>(pandora::Track *pTrack) const;

} // namespace pandora
