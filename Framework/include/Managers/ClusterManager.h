/**
 *  @file   PandoraPFANew/Framework/include/Managers/ClusterManager.h
 * 
 *  @brief  Header file for the cluster manager class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_MANAGER_H
#define CLUSTER_MANAGER_H 1

#include "Api/PandoraContentApi.h"

#include "Managers/Manager.h"

#include "Objects/Cluster.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class Algorithm;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief ClusterManager class
 */
class ClusterManager : public Manager<Cluster*>
{
public:
    /**
     *  @brief  Default constructor
     */
     ClusterManager();

    /**
     *  @brief  Destructor
     */
     ~ClusterManager();

private:
    /**
     *  @brief  Create cluster
     * 
     *  @param  clusterParameters the cluster parameters
     *  @param  pCluster to receive the address of the cluster created
     */
    template <typename CLUSTER_PARAMETERS>
    StatusCode CreateCluster(CLUSTER_PARAMETERS *pClusterParameters, Cluster *&pCluster);

    /**
     *  @brief  Temporarily replace the current cluster list with another list, which may only be a temporary list.
     *          This switch will persist only for the duration of the algorithm and its daughters; unless otherwise
     *          specified, the current list will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  clusterListName the name of the new current (and algorithm input) cluster list
     */
    StatusCode TemporarilyReplaceCurrentList(const std::string &clusterListName);

    /**
     *  @brief  Make a temporary cluster list and set it to be the current cluster list
     * 
     *  @param  pAlgorithm address of the algorithm requesting a temporary list
     *  @param  temporaryListName to receive the name of the temporary list
     */
    StatusCode MakeTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName);

    /**
     *  @brief  Move clusters to a new temporary cluster list and set it to be the current cluster list
     * 
     *  @param  pAlgorithm address of the algorithm requesting a temporary list
     *  @param  originalListName the list in which the clusters currently exist
     *  @param  temporaryListName to receive the name of the temporary list
     *  @param  clustersToMove only clusters in both this and the current list will be moved
     *          - other clusters in the current list will remain in original list
     *          - an empty cluster vector will be rejected
     */
    StatusCode MoveClustersToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
        std::string &temporaryListName, const ClusterList &clustersToMove);

    /**
     *  @brief  Save a list of clusters
     * 
     *  @param  pAlgorithm the algorithm associated with the temporary clusters
     *  @param  targetListName the name of the target cluster list, which will be created if it doesn't currently exist
     *  @param  sourceListName the name of the (typically temporary) cluster list to save
     */
    StatusCode SaveClusters(const Algorithm *const pAlgorithm, const std::string &targetListName, const std::string &sourceListName);

    /**
     *  @brief  Save a list of clusters
     * 
     *  @param  pAlgorithm the algorithm associated with the temporary clusters
     *  @param  targetListName the name of the target cluster list, which will be created if it doesn't currently exist
     *  @param  sourceListName the name of the (typically temporary) cluster list containing clusters to save
     *  @param  clustersToSave only clusters in both this and the temporary list will be stored
     *          - other clusters will remain in the temporary list and will be deleted when the parent algorithm exits
     *          - an empty cluster vector will be rejected
     */
    StatusCode SaveClusters(const Algorithm *const pAlgorithm, const std::string &targetListName, const std::string &sourceListName,
        const ClusterList &clustersToSave);

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Remove a calo hit from a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Add an isolated calo hit to a cluster. This is not counted as a regular calo hit: it contributes only
     *          towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Remove an isolated calo hit from a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit);

    /**
     *  @brief  Delete a cluster from the current list
     * 
     *  @param  pCluster address of the cluster to delete
     */
    StatusCode DeleteCluster(Cluster *pCluster);

    /**
     *  @brief  Delete a cluster from a specified list
     * 
     *  @param  pCluster address of the cluster to delete
     *  @param  listName the name of the list containing the cluster
     */
    StatusCode DeleteCluster(Cluster *pCluster, const std::string &listName);

    /**
     *  @brief  Delete a list of clusters from the current list
     * 
     *  @param  clusterList the list of addresses of clusters to delete
     */
    StatusCode DeleteClusters(const ClusterList &clusterList);

    /**
     *  @brief  Delete a list of clusters from a specified list
     * 
     *  @param  clusterList the list of addresses of clusters to delete
     *  @param  listName the name of the list containing the clusters
     */
    StatusCode DeleteClusters(const ClusterList &clusterList, const std::string &listName);

    /**
     *  @brief  Delete the contents of a temporary cluster list and remove the list from all relevant containers
     * 
     *  @param  pAlgorithm address of the algorithm calling this function
     *  @param  clusterListName the name of the temporary cluster list
     */
    StatusCode DeleteTemporaryClusterList(const Algorithm *const pAlgorithm, const std::string &clusterListName);

    /**
     *  @brief  Merge two clusters in the current list, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete);

    /**
     *  @brief  Merge two clusters from two specified lists, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     *  @param  enlargeListName name of the list containing the cluster to enlarge
     *  @param  deleteListName name of the list containing the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
        const std::string &deleteListName);

    /**
     *  @brief  Get the list of clusters that will be deleted when the algorithm info is reset
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  clusterList to receive the list of clusters that will be deleted when the algorithm info is reset
     */
    StatusCode GetClustersToBeDeleted(const Algorithm *const pAlgorithm, ClusterList &clusterList) const;

    /**
     *  @brief  Remove temporary lists and reset the current cluster list to that when algorithm was initialized
     * 
     *  @param  pAlgorithm the algorithm associated with the temporary clusters
     *  @param  isAlgorithmFinished whether the algorithm has completely finished and the algorithm info should be entirely removed
     */
    StatusCode ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished);

    /**
     *  @brief  Erase all cluster manager content
     */
    StatusCode EraseAllContent();

    /**
     *  @brief  Remove an empty cluster list
     * 
     *  @param  pAlgorithm the algorithm manipulating the cluster lists
     *  @param  clusterListName the name of the empty cluster list to be removed
     */
    StatusCode RemoveEmptyClusterList(const Algorithm *const pAlgorithm, const std::string &clusterListName);

    /**
     *  @brief  Remove all cluster to track associations
     */
    StatusCode RemoveAllTrackAssociations() const;

    /**
     *  @brief  Remove cluster to track associations from all clusters in the current list
     * 
     *  @param  danglingTracks to receive the list of "dangling" associations
     */
    StatusCode RemoveCurrentTrackAssociations(TrackList &danglingTracks) const;

    /**
     *  @brief  Remove a specified list of cluster to track associations
     * 
     *  @param  trackToClusterList the specified track to cluster list
     */
    StatusCode RemoveTrackAssociations(const TrackToClusterMap &trackToClusterList) const;

    bool                            m_canMakeNewClusters;               ///< Whether the manager is allowed to make new clusters

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->RemoveCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->AddIsolatedCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->RemoveIsolatedCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::DeleteCluster(Cluster *pCluster)
{
    return this->DeleteCluster(pCluster, m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode ClusterManager::DeleteClusters(const ClusterList &clusterList)
{
    return this->DeleteClusters(clusterList, m_currentListName);
}

} // namespace pandora

#endif // #ifndef CLUSTER_MANAGER_H
