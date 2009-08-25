/**
 *	@file PandoraPFANew/src/Managers/ClusterManager.cc
 * 
 *	@brief Implementation of the cluster manager class.
 * 
 *	$Log: $
 */

#include "Managers/ClusterManager.h"

#include "Objects/Cluster.h"

#include <sstream>

namespace pandora
{

ClusterManager::ClusterManager() :
	m_canMakeNewClusters(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterManager::~ClusterManager()
{
	(void) this->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename CLUSTER_PARAMETERS>
StatusCode ClusterManager::CreateCluster(CLUSTER_PARAMETERS *pClusterParameters)
{
	if (!m_canMakeNewClusters)
		return STATUS_CODE_NOT_ALLOWED;

	NameToClusterListMap::iterator iter = m_nameToClusterListMap.find(m_currentListName);

	if (m_nameToClusterListMap.end() == iter)
		return STATUS_CODE_NOT_INITIALIZED;

	Cluster *pCluster = NULL;
	pCluster = new Cluster(pClusterParameters);
	
	if (NULL == pCluster)
		return STATUS_CODE_FAILURE;

	iter->second->insert(pCluster);

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::GetList(const std::string &listName, const ClusterList *&pClusterList) const
{
	NameToClusterListMap::const_iterator iter = m_nameToClusterListMap.find(listName);
	
	if (m_nameToClusterListMap.end() == iter)
		return STATUS_CODE_NOT_INITIALIZED;
	
	pClusterList = iter->second;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &clusterListName)
{
	if (m_nameToClusterListMap.end() == m_nameToClusterListMap.find(clusterListName))
		return STATUS_CODE_NOT_FOUND;

	if ((m_algorithmInfoMap.size() > 1) || (m_savedLists.end() == m_savedLists.find(clusterListName)))
		return STATUS_CODE_NOT_ALLOWED;

	m_canMakeNewClusters = false;
	m_currentListName = clusterListName;

	AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);	

	if (m_algorithmInfoMap.end() == iter)
		return STATUS_CODE_NOT_FOUND;
	
	iter->second.m_parentListName = clusterListName;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MakeTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName)
{
	AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);	

	if (m_algorithmInfoMap.end() == iter)
		return STATUS_CODE_NOT_FOUND;

	std::ostringstream temporaryListNameStream;
	temporaryListNameStream << pAlgorithm << "_" << iter->second.m_temporaryListNames.size();

	temporaryListName = temporaryListNameStream.str();
	iter->second.m_temporaryListNames.insert(temporaryListName);

	m_nameToClusterListMap[temporaryListName] = new ClusterList;
	m_currentListName = temporaryListName;
	m_canMakeNewClusters = true;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MoveClustersToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
	std::string &temporaryListName, const ClusterList *const pClusterList)
{
	if ((NULL != pClusterList) && pClusterList->empty())
		return STATUS_CODE_NOT_INITIALIZED;

	NameToClusterListMap::iterator originalClusterListIter = m_nameToClusterListMap.find(originalListName);

	if (m_nameToClusterListMap.end() == originalClusterListIter)
		return STATUS_CODE_NOT_FOUND;

	if (originalClusterListIter->second->empty())
		return STATUS_CODE_NOT_INITIALIZED;	

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, MakeTemporaryListAndSetCurrent(pAlgorithm, temporaryListName));

	if (m_nameToClusterListMap.end() == m_nameToClusterListMap.find(temporaryListName))
		return STATUS_CODE_FAILURE;

	for (ClusterList::iterator clusterIter = originalClusterListIter->second->begin(),
		clusterIterEnd = originalClusterListIter->second->end(); clusterIter != clusterIterEnd; ++clusterIter)
	{
		if ((NULL == pClusterList) || (pClusterList->end() != pClusterList->find(*clusterIter)))
		{
			m_nameToClusterListMap[m_currentListName]->insert(*clusterIter);			
			originalClusterListIter->second->erase(clusterIter);
		}
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::SaveTemporaryClusters(const Algorithm *const pAlgorithm, const std::string &newListName,
	const std::string &temporaryListName, const ClusterList *const pClusterList)
{
	if ((NULL != pClusterList) && pClusterList->empty())
		return STATUS_CODE_NOT_INITIALIZED;
	
	NameToClusterListMap::iterator temporaryClusterListIter = m_nameToClusterListMap.find(temporaryListName);

	if (m_nameToClusterListMap.end() == temporaryClusterListIter)
		return STATUS_CODE_NOT_FOUND;

	if (temporaryClusterListIter->second->empty())
		return STATUS_CODE_NOT_INITIALIZED;

	NameToClusterListMap::iterator newClusterListIter = m_nameToClusterListMap.find(newListName);

	if (m_nameToClusterListMap.end() == newClusterListIter)
	{
		m_nameToClusterListMap[newListName] = new ClusterList;
		newClusterListIter = m_nameToClusterListMap.find(newListName);

		if (m_nameToClusterListMap.end() == newClusterListIter)
			return STATUS_CODE_FAILURE;
	}

	for (ClusterList::iterator clusterIter = temporaryClusterListIter->second->begin(),
		clusterIterEnd = temporaryClusterListIter->second->end(); clusterIter != clusterIterEnd; ++clusterIter)
	{
		if ((NULL == pClusterList) || (pClusterList->end() != pClusterList->find(*clusterIter)))
		{
			newClusterListIter->second->insert(*clusterIter);
			temporaryClusterListIter->second->erase(clusterIter);
		}
	}

	m_savedLists.insert(newListName);

	if (temporaryClusterListIter->second->empty())
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, RemoveTemporaryList(pAlgorithm, temporaryListName));
	
	return STATUS_CODE_SUCCESS;
}
		
//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::AddCaloHitToCluster(const Cluster *pCluster, const CaloHit *pCaloHit)
{
	// Only in the manager modifier functions are these const_casts allowed!
	return const_cast<Cluster*>(pCluster)->AddCaloHit(const_cast<CaloHit*>(pCaloHit));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::DeleteCluster(const Cluster *pCluster)
{
	NameToClusterListMap::iterator listIter = m_nameToClusterListMap.find(m_currentListName);

	if (m_nameToClusterListMap.end() == listIter)
		return STATUS_CODE_NOT_INITIALIZED;

	// Only in the manager modifier functions are these const_casts allowed!
	ClusterList::iterator clusterIter = listIter->second->find(const_cast<Cluster*>(pCluster));

	if (listIter->second->end() == clusterIter)
		return STATUS_CODE_NOT_FOUND;

	delete *clusterIter;
	listIter->second->erase(clusterIter);

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MergeAndDeleteClusters(const Cluster *pClusterLhs, const Cluster *pClusterRhs)
{
	// Only in the manager modifier functions are these const_casts allowed!
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, 
		const_cast<OrderedCaloHitList*>(pClusterLhs->GetOrderedCaloHitList())->Add( *(pClusterRhs->GetOrderedCaloHitList()) ));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DeleteCluster(pClusterRhs));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RegisterAlgorithm(const Algorithm *const pAlgorithm)
{
	if (m_algorithmInfoMap.end() != m_algorithmInfoMap.find(pAlgorithm))
		return STATUS_CODE_ALREADY_INITIALIZED;
	
	AlgorithmInfo algorithmInfo;
	algorithmInfo.m_parentListName = m_currentListName;

	if (!m_algorithmInfoMap.insert(AlgorithmInfoMap::value_type(pAlgorithm, algorithmInfo)).second)
		return STATUS_CODE_FAILURE;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::ResetAfterAlgorithmCompletion(const Algorithm *const pAlgorithm)
{
	m_canMakeNewClusters = false;
	
	AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);
	
	if (m_algorithmInfoMap.end() == algorithmListIter)
		return STATUS_CODE_NOT_FOUND;

	for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
		listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
	{
		NameToClusterListMap::iterator clusterListIter = m_nameToClusterListMap.find(*listNameIter);
		
		if (m_nameToClusterListMap.end() == clusterListIter)
			return STATUS_CODE_FAILURE;
		
		for (ClusterList::iterator clusterIter = clusterListIter->second->begin(), clusterIterEnd = clusterListIter->second->end();
			clusterIter != clusterIterEnd; ++clusterIter)
		{
			delete (*clusterIter);
		}
	
		m_nameToClusterListMap.erase(clusterListIter);
	}

	m_currentListName = algorithmListIter->second.m_parentListName;
	m_algorithmInfoMap.erase(algorithmListIter);

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::ResetForNextEvent()
{
	m_canMakeNewClusters = false;
	m_currentListName.clear();

	for (NameToClusterListMap::iterator iter = m_nameToClusterListMap.begin(), iterEnd = m_nameToClusterListMap.end(); iter != iterEnd; ++iter)
	{
		for (ClusterList::iterator clusterIter = iter->second->begin(), clusterIterEnd = iter->second->end(); 
			clusterIter != clusterIterEnd; ++clusterIter)
		{
			delete (*clusterIter);
		}

		m_nameToClusterListMap.erase(iter);
	}
	
	m_nameToClusterListMap.clear();
	m_algorithmInfoMap.clear();
	m_savedLists.clear();

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveTemporaryList(const Algorithm *const pAlgorithm, const std::string &temporaryListName)
{
	m_nameToClusterListMap.erase(m_nameToClusterListMap.find(temporaryListName));

	AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);	

	if (m_algorithmInfoMap.end() == algorithmListIter)
		return STATUS_CODE_NOT_FOUND;

	StringSet *pStringSet = &(algorithmListIter->second.m_temporaryListNames);
	pStringSet->erase(pStringSet->find(temporaryListName));

	if (temporaryListName == m_currentListName)
	{
		m_canMakeNewClusters = false;
		m_currentListName = algorithmListIter->second.m_parentListName;
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode ClusterManager::CreateCluster<CaloHit>(CaloHit *pCaloHit);
template StatusCode ClusterManager::CreateCluster<InputCaloHitList>(InputCaloHitList *pCaloHitList);
template StatusCode ClusterManager::CreateCluster<Track>(Track *pTrack);

} // namespace pandora
