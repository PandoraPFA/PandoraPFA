/**
 *  @file   PandoraPFANew/Framework/include/Api/PandoraContentApiImpl.h
 *
 *  @brief  Header file for the pandora content api implementation class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CONTENT_API_IMPL_H
#define PANDORA_CONTENT_API_IMPL_H 1

#include "Api/PandoraContentApi.h"

#include "Pandora/StatusCodes.h"

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief PandoraContentApiImpl class
 */
class PandoraContentApiImpl
{
public:
    /**
     *  @brief  Create a cluster
     *
     *  @param  pClusterParameters address of either 1) a calo hit, 2) a calo hit vector or 3) a track
     *  @param  pCluster to receive the address of the cluster created
     */
    template <typename CLUSTER_PARAMETERS>
    StatusCode CreateCluster(CLUSTER_PARAMETERS *pClusterParameters, Cluster *&pCluster) const;

    /**
     *  @brief  Create a particle flow object
     * 
     *  @param  particleFlowObjectParameters the particle flow object parameters
     */
    StatusCode CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters) const;

    /**
     *  @brief  Create an algorithm instance, via one of the algorithm factories registered with pandora.
     *          This function is expected to be called whilst reading the settings for a parent algorithm.
     * 
     *  @param  pXmlElement address of the xml element describing the daughter algorithm type and settings
     *  @param  daughterAlgorithmName to receive the name of the daughter algorithm instance
     */
    StatusCode CreateDaughterAlgorithm(TiXmlElement *const pXmlElement, std::string &daughterAlgorithmName) const;

    /**
     *  @brief  Run an algorithm registered with pandora
     * 
     *  @param  algorithmName the algorithm name
     */
    StatusCode RunAlgorithm(const std::string &algorithmName) const;

    /**
     *  @brief  Repeat the event preparation stages, which are used to calculate properties of input objects for later use in algorithms
     */
    StatusCode RepeatEventPreparation() const;

    /**
     *  @brief  Get the current cluster list
     * 
     *  @param  pClusterList to receive the address of the current cluster list
     *  @param  clusterListName to receive the current cluster list name
     */
    StatusCode GetCurrentClusterList(const ClusterList *&pClusterList, std::string &clusterListName) const;

    /**
     *  @brief  Get the current cluster list name
     * 
     *  @param  clusterListName to receive the current cluster list name
     */
    StatusCode GetCurrentClusterListName(std::string &clusterListName) const;

    /**
     *  @brief  Get a named cluster list
     * 
     *  @param  clusterListName the name of the cluster list
     *  @param  pClusterList to receive the address of the cluster list
     */
    StatusCode GetClusterList(const std::string &clusterListName, const ClusterList *&pClusterList) const;

    /**
     *  @brief  Drop the current cluster list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentClusterList() const;

    /**
     *  @brief  Get the current ordered calo hit list
     * 
     *  @param  pOrderedCaloHitList to receive the address of the current ordered calo hit list
     *  @param  orderedCaloHitListName to receive the current ordered calo hit list name
     */
    StatusCode GetCurrentOrderedCaloHitList(const OrderedCaloHitList *&pOrderedCaloHitList, std::string &orderedCaloHitListName) const;

    /**
     *  @brief  Get the current ordered calo hit list name
     * 
     *  @param  orderedCaloHitListName to receive the current ordered calo hit list name
     */
    StatusCode GetCurrentOrderedCaloHitListName(std::string &orderedCaloHitListName) const;

    /**
     *  @brief  Get a named ordered calo hit list
     * 
     *  @param  orderedCaloHitListName the name of the ordered calo hit list
     *  @param  pOrderedCaloHitList to receive the address of the ordered calo hit list
     */
    StatusCode GetOrderedCaloHitList(const std::string &orderedCaloHitListName, const OrderedCaloHitList *&pOrderedCaloHitList) const;

    /**
     *  @brief  Drop the current ordered calo hit list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentOrderedCaloHitList() const;

    /**
     *  @brief  Get the current track list
     * 
     *  @param  pTrackList to receive the address of the current track list
     *  @param  trackListName to receive the current track list name
     */
    StatusCode GetCurrentTrackList(const TrackList *&pTrackList, std::string &trackListName) const;

    /**
     *  @brief  Get the current track list name
     * 
     *  @param  trackListName to receive the current track list name
     */
    StatusCode GetCurrentTrackListName(std::string &trackListName) const;

    /**
     *  @brief  Get a named track list
     * 
     *  @param  trackListName the name of the track list
     *  @param  pTrackList to receive the address of the track list
     */
    StatusCode GetTrackList(const std::string &trackListName, const TrackList *&pTrackList) const;

    /**
     *  @brief  Drop the current track list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentTrackList() const;

    /**
     *  @brief  Get the current particle flow object list
     * 
     *  @param  pParticleFlowObjectList to receive the address of the current particle flow object list
     */
    StatusCode GetCurrentPfoList(const ParticleFlowObjectList *&pParticleFlowObjectList) const;

    /**
     *  @brief  Get the list of mc pfo targets
     *
     *  @param  mcParticleList to receive the mc particle list
     */
    StatusCode GetMCParticleList(MCParticleList &mcParticleList) const;

    /**
     *  @brief  Initialize cluster fragmentation operations on clusters in the algorithm input list. This allows hits in a list
     *          of clusters (a subset of the algorithm input list) to be redistributed.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  inputClusterList the input cluster list
     *  @param  originalClustersListName to receive the name of the list in which the original clusters are stored
     *  @param  fragmentClustersListName to receive the name of the list in which the fragment clusters are stored
     */
    StatusCode InitializeFragmentation(const Algorithm &algorithm, const ClusterList &inputClusterList,
        std::string &originalClustersListName, std::string &fragmentClustersListName) const;

    /**
     *  @brief  End cluster fragmentation operations on clusters in the algorithm input list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterListToSaveName the name of the list containing the clusters chosen to be saved (original or fragments)
     *  @param  clusterListToDeleteName the name of the list containing the clusters chosen to be deleted (original or fragments)
     */
    StatusCode EndFragmentation(const Algorithm &algorithm, const std::string &clusterListToSaveName,
        const std::string &clusterListToDeleteName) const;

    /**
     *  @brief  Initialize reclustering operations on clusters in the algorithm input list. This allows hits in a list
     *          of clusters (a subset of the algorithm input list) to be redistributed.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  inputTrackList the input track list
     *  @param  inputClusterList the input cluster list
     *  @param  originalClustersListName to receive the name of the list in which the original clusters are stored
     */
    StatusCode InitializeReclustering(const Algorithm &algorithm, const TrackList &inputTrackList, const ClusterList &inputClusterList,
        std::string &originalClustersListName) const;

    /**
     *  @brief  End reclustering operations on clusters in the algorithm input list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pandora the pandora instance performing reclustering
     *  @param  selectedClusterListName the name of the list containing the chosen recluster candidates (or the original candidates)
     */
    StatusCode EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const;

    /**
     *  @brief  Run a clustering algorithm (an algorithm that will create new cluster objects)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusteringAlgorithmName the name of the clustering algorithm to run
     *  @param  pNewClusterList the address of the new cluster list populated
     *  @param  newClusterListName the name of the new cluster list populated
     */
     StatusCode RunClusteringAlgorithm(const Algorithm &algorithm, const std::string &clusteringAlgorithmName,
        const ClusterList *&pNewClusterList, std::string &newClusterListName) const;

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Remove a calo hit from a cluster. Note this function will not remove the final calo hit from a cluster, and
     *          will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to remove
     */
    StatusCode RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Add an isolated calo hit to a cluster. This is not counted as a regular calo hit: it contributes only
     *          towards the cluster energy and does not affect any other cluster properties.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the isolated hit to add
     */
    StatusCode AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Remove an isolated calo hit from a cluster.
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the isolated hit to remove
     */
    StatusCode RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const;

    /**
     *  @brief  Delete a cluster in the current list
     * 
     *  @param  pCluster address of the cluster to delete
     */
    StatusCode DeleteCluster(Cluster *pCluster) const;

    /**
     *  @brief  Delete a cluster from a specified list
     * 
     *  @param  pCluster address of the cluster to delete
     *  @param  clusterListName name of the list containing the cluster
     */
    StatusCode DeleteCluster(Cluster *pCluster, const std::string &clusterListName) const;

    /**
     *  @brief  Delete a list of clusters from the current list
     * 
     *  @param  clusterList the list of clusters to delete
     */
    StatusCode DeleteClusters(const ClusterList &clusterList) const;

    /**
     *  @brief  Delete a list of clusters from a specified list
     * 
     *  @param  clusterList the list of clusters to delete
     *  @param  clusterListName name of the list containing the clusters
     */
     StatusCode DeleteClusters(const ClusterList &clusterList, const std::string &clusterListName) const;

    /**
     *  @brief  Merge two clusters in the current list, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete) const;

    /**
     *  @brief  Merge two clusters from two specified lists, enlarging one cluster and deleting the second
     * 
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     *  @param  enlargeListName name of the list containing the cluster to enlarge
     *  @param  deleteListName name of the list containing the cluster to delete
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
        const std::string &deleteListName) const;

    /**
     *  @brief  Add a cluster to a particle flow object
     *
     *  @param  pParticleFlowObject address of the particle flow object to modify
     *  @param  pCluster address of the cluster to add
     */
    StatusCode AddClusterToPfo(ParticleFlowObject *pParticleFlowObject, Cluster *pCluster) const;

    /**
     *  @brief  Add a track to a particle flow object
     *
     *  @param  pParticleFlowObject address of the particle flow object to modify
     *  @param  pTrack address of the track to add
     */
    StatusCode AddTrackToPfo(ParticleFlowObject *pParticleFlowObject, Track *pTrack) const;

    /**
     *  @brief  Remove a cluster from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pParticleFlowObject address of the particle flow object to modify
     *  @param  pCluster address of the cluster to remove
     */
    StatusCode RemoveClusterFromPfo(ParticleFlowObject *pParticleFlowObject, Cluster *pCluster) const;

    /**
     *  @brief  Remove a track from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pParticleFlowObject address of the particle flow object to modify
     *  @param  pTrack address of the track to remove
     */
    StatusCode RemoveTrackFromPfo(ParticleFlowObject *pParticleFlowObject, Track *pTrack) const;

    /**
     *  @brief  Delete a particle flow object from the current list
     * 
     *  @param  pParticleFlowObject address of the particle flow object to delete
     */
    StatusCode DeletePfo(ParticleFlowObject *pParticleFlowObject) const;

    /**
     *  @brief  Add an association between a track and a cluster
     * 
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     */
    StatusCode AddTrackClusterAssociation(Track *const pTrack, Cluster *const pCluster) const;

    /**
     *  @brief  Remove an association between a track and a cluster
     * 
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     */
    StatusCode RemoveTrackClusterAssociation(Track *const pTrack, Cluster *const pCluster) const;

    /**
     *  @brief  Remove all track-cluster associations from objects in the current track and cluster lists
     */
    StatusCode RemoveCurrentTrackClusterAssociations() const;

    /**
     *  @brief  Remove all associations between tracks and clusters
     */
    StatusCode RemoveAllTrackClusterAssociations() const;

    /**
     *  @brief  Save the current cluster list in a list with the specified new name. Note that this will empty the current
     *          cluster list; the clusters will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     */
    StatusCode SaveClusterList(const Algorithm &algorithm, const std::string &newClusterListName) const;

    /**
     *  @brief  Save elements of the current cluster list in a list with the specified new name. If all the clusters in the
     *          current list are saved, this will empty the current list; the clusters will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     *  @param  clustersToSave a subset of the current cluster list - only clusters in both this and the current list will be saved
     */
    StatusCode SaveClusterList(const Algorithm &algorithm, const std::string &newClusterListName, const ClusterList &clustersToSave) const;

    /**
     *  @brief  Save a named cluster list in a list with the specified new name. Note that this will empty the old cluster list;
     *          the clusters will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  oldClusterListName the old cluster list name
     *  @param  newClusterListName the new cluster list name
     */
    StatusCode SaveClusterList(const Algorithm &algorithm, const std::string &oldClusterListName,  const std::string &newClusterListName) const;

    /**
     *  @brief  Save elements of a named cluster list in a list with the specified new name. If all the clusters in the old
     *          list are saved, this will empty the old cluster list; the clusters will all be moved to the new named list.
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  oldClusterListName the old cluster list name
     *  @param  newClusterListName the new cluster list name
     *  @param  clustersToSave a subset of the old cluster list - only clusters in both this and the old cluster list will be saved
     */
    StatusCode SaveClusterList(const Algorithm &algorithm, const std::string &oldClusterListName, const std::string &newClusterListName,
        const ClusterList &clustersToSave) const;

    /**
     *  @brief  Replace the current cluster list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the name of the replacement cluster list
     */
    StatusCode ReplaceCurrentClusterList(const Algorithm &algorithm, const std::string &newClusterListName) const;

    /**
     *  @brief  Temporarily replace the current cluster list with another list, which may only be a temporary list. This switch
     *          will persist only for the duration of the algorithm and its daughters; unless otherwise specified, the current list
     *          will revert to the algorithm input list upon algorithm completion.
     * 
     *  @param  newClusterListName the name of the replacement cluster list
     */
    StatusCode TemporarilyReplaceCurrentClusterList(const std::string &newClusterListName) const;

    /**
     *  @brief  Save the current ordered calo hit list under a new name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new ordered calo hit list name
     */
    StatusCode SaveOrderedCaloHitList(const Algorithm &algorithm, const OrderedCaloHitList &orderedCaloHitList,
        const std::string &newListName) const;

    /**
     *  @brief  Replace the current ordered calo hit list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement ordered calo hit list
     */
    StatusCode ReplaceCurrentOrderedCaloHitList(const Algorithm &algorithm, const std::string &newListName) const;

    /**
     *  @brief  Save the current track list under a new name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new ordered calo hit list name
     */
    StatusCode SaveTrackList(const Algorithm &algorithm, const TrackList &trackList, const std::string &newListName) const;

    /**
     *  @brief  Replace the current track list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement track list
     */
    StatusCode ReplaceCurrentTrackList(const Algorithm &algorithm, const std::string &newListName) const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the pandora object to interface
     */
    PandoraContentApiImpl(Pandora *pPandora);

    /**
     *  @brief  Prepare a cluster for deletion, flagging constituent calo hits as available and removing track associations
     * 
     *  @param  pCluster address of the cluster to prepare for deletion
     */
    StatusCode PrepareClusterForDeletion(const Cluster *const pCluster) const;

    /**
     *  @brief  Prepare a list of clusters for deletion, flagging constituent calo hits as available and removing track associations
     * 
     *  @param  clusterList the list of clusters to prepare for deletion
     */
    StatusCode PrepareClustersForDeletion(const ClusterList &clusterList) const;

    /**
     *  @brief  Prepare a list of clusters (formed as recluster candidates) for deletion, removing any track associations.
     * 
     *  @param  clusterList the list of clusters to prepare for deletion
     */
    StatusCode PrepareReclusterCandidatesForDeletion(const ClusterList &clusterList) const;

    Pandora    *m_pPandora;    ///< The pandora object to provide an interface to

    friend class Pandora;
};

} // namespace pandora

#endif // #ifndef PANDORA_CONTENT_API_IMPL_H
