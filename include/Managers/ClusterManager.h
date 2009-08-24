/**
 *	@file	PandoraPFANew/include/Managers/ClusterManager.h
 * 
 *	@brief	Header file for the cluster manager class.
 * 
 *	$Log: $
 */
#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H 1

#include "Api/PandoraContentApi.h"

namespace pandora
{

class Algorithm;

//------------------------------------------------------------------------------------------------------------------------------------------
	
/**
 *	@brief ClusterManager class
 */
class ClusterManager
{
public:
	/**
	 *	@brief	Default constructor
	 */
	 ClusterManager();

	/**
	 *	@brief	Destructor
	 */
	 ~ClusterManager();

private:
	/**
	 *	@brief	Create cluster
	 * 
	 *	@param	clusterParameters the cluster parameters
	 */
	template <typename CLUSTER_PARAMETERS>
	StatusCode CreateCluster(CLUSTER_PARAMETERS *pClusterParameters);

	/**
	 *	@brief	Get the current cluster list
	 * 
	 *	@param	pClusterList to receive the current cluster list
	 *	@param	clusterListName to receive the name of the current cluster list
	 */
	StatusCode GetCurrentList(const ClusterList *pClusterList, std::string &clusterListName) const;

	/**
	 *	@brief	Get a cluster list
	 * 
 	 *	@param	listName the name of the list
	 *	@param	pClusterList to receive the cluster list
	 */
	StatusCode GetList(const std::string &listName, const ClusterList *pClusterList) const;

	/**
	 *	@brief	Change the current cluster list to a pre-existing list
	 *
	 *	@param	pAlgorithm address of the algorithm changing the current cluster list
	 *	@param	clusterListName the name of the new current cluster list
	 */	
	StatusCode SetCurrentList(const Algorithm *const pAlgorithm, const std::string &clusterListName);
	
	/**
	 *	@brief	Make a temporary cluster list and set it to be the current cluster list
	 * 
	 *	@param	pAlgorithm address of the algorithm requesting a temporary list
	 *	@param	temporaryListName to receive the name of the temporary list
	 */
	StatusCode MakeTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName);

	/**
	 *	@brief	Move clusters to a new temporary cluster list and set it to be the current cluster list
	 * 
	 *	@param	pAlgorithm address of the algorithm requesting a temporary list
	 *	@param	originalListName the list in which the clusters currently exist
	 *	@param	temporaryListName to receive the name of the temporary list
	 *	@param	pClusterList optional: only clusters in both this and the current list will be moved
	 * 			- other clusters in the current list will remain in original list
	 * 			- an empty cluster vector will be rejected
	 */
	StatusCode MoveClustersToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
		std::string &temporaryListName,	const ClusterList *const pClusterList = NULL);

	/**
	 *	@brief	Save a list of temporary clusters
	 * 
	 *	@param	pAlgorithm the algorithm associated with the temporary clusters
	 *	@param	newListName the name of the new cluster list to be created
	 *	@param	temporaryListName the name of the temporary cluster list to save
	 *	@param	pClusterList optional: only clusters in both this and the temporary list will be stored
	 * 			- other clusters in the temporary list will be deleted
	 * 			- an empty cluster vector will be rejected
	 */
	StatusCode SaveTemporaryClusters(const Algorithm *const pAlgorithm, const std::string &newListName,
		const std::string &temporaryListName, const ClusterList *const pClusterList = NULL);

	/**
	 *	@brief	Remove temporary lists and reset the current cluster list to that when algorithm was initialized
	 * 
	 *	@param	pAlgorithm the algorithm associated with the temporary clusters
	 */
	StatusCode ResetAfterAlgorithmCompletion(const Algorithm *const pAlgorithm);

	/**
	 *	@brief	Reset the cluster manager
	 */	
	StatusCode ResetForNextEvent();

	/**
	 *	@brief	Remove a temporary cluster list
	 * 
	 *	@param	pAlgorithm the algorithm associated with the temporary clusters
	 *	@param	temporaryListName the name of the temporary cluster list
	 */
	StatusCode RemoveTemporaryList(const Algorithm *const pAlgorithm, const std::string &temporaryListName);

	/**
	 *	@brief	Get (and store) the name of the parent cluster list undergoing reclustering
	 *
	 *	@param	parentClusterListName to receive the parent cluster list name
	 */	
	StatusCode GetReclusterListName(std::string &parentClusterListName);

	/**
	 *	@brief	Get (and reset) the name of the parent cluster list undergoing reclustering
	 *
	 *	@param	parentClusterListName to receive the parent cluster list name
	 */		
	StatusCode GetAndResetReclusterListName(std::string &parentClusterListName);

	/**
	 *	@brief	AlgorithmInfo class
	 */	
	class AlgorithmInfo
	{
	public:
		std::string					m_parentClusterListName;		///< The current cluster list when algorithm was initialized
		StringSet					m_temporaryClusterListNames;	///< The temporary cluster list names
	};

	typedef std::map<std::string, ClusterList *> NameToClusterListMap;
	typedef std::map<const Algorithm *, AlgorithmInfo> AlgorithmInfoMap;

	NameToClusterListMap			m_nameToClusterListMap;			///< The name to cluster list map
	AlgorithmInfoMap				m_algorithmInfoMap;				///< The algorithm info map

	bool							m_canMakeNewClusters;			///< Whether the manager is allowed to make new clusters
	std::string						m_currentListName;				///< The name of the current cluster list
	std::string						m_reclusterListName;			///< The name of the parent cluster list undergoing reclustering
	StringSet						m_savedLists;					///< The set of saved cluster lists

	friend class PandoraApiImpl;
	friend class PandoraContentApiImpl;	
};

} // namespace pandora

#endif // #ifndef CLUSTER_MANAGER_H
