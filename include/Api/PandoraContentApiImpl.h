/**
 *  @file   PandoraPFANew/include/Api/PandoraContentApiImpl.h
 *
 *  @brief  Header file for the pandora content api implementation class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CONTENT_API_IMPL_H
#define PANDORA_CONTENT_API_IMPL_H 1

#include "StatusCodes.h"

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
     *  @param  pClusterParameters address of either 1) a calo hit, 2) an input calo hit list or 3) a track
     */
    template <typename CLUSTER_PARAMETERS>
    StatusCode CreateCluster(CLUSTER_PARAMETERS *pClusterParameters) const;

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
     *  @brief  Match calo hits to their correct mc particles for particle flow
     */
    StatusCode MatchCaloHitsToMCPfoTargets() const;

    /**
     *  @brief  Order input calo hits by pseudo layer
     */
    StatusCode OrderInputCaloHits() const;

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode ReadPandoraSettings(const TiXmlHandle *const pXmlHandle) const;

    /**
     *  @brief  Initialize pandora algorithms
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializeAlgorithms(const TiXmlHandle *const pXmlHandle) const;

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
     *  @brief  Initialize reclustering operations
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  inputTrackList the input track list
     *  @param  inputClusterList the input cluster list
     *  @param  originalClustersListName to receive the name of the list in which the original clusters are stored
     */
    StatusCode InitializeReclustering(const pandora::Algorithm &algorithm, const TrackList &inputTrackList, 
        const ClusterList &inputClusterList, std::string &originalClustersListName) const;

    /**
     *  @brief  End reclustering operations
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pandora the pandora instance performing reclustering
     *  @param  selectedClusterListName the name of the list containing the chosen recluster candidates (or the original candidates)
     */
    StatusCode EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName) const;

    /**
     *  @brief  Run a clustering algorithm (an algorithm that will create new cluster objects)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusteringAlgorithmName the name of the clustering algorithm to run
     *  @param  pNewClusterList the address of the new cluster list populated
     *  @param  newClusterListName the name of the new cluster list populated
     */
     StatusCode RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName,
        const ClusterList *&pNewClusterList, std::string &newClusterListName) const;

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    StatusCode AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const;
    
    /**
     *  @brief  Delete a cluster and remove it from the current cluster list
     * 
     *  @param  pCluster address of the cluster to delete
     */
    StatusCode DeleteCluster(Cluster *pCluster) const;

    /**
     *  @brief  Merge two clusters, deleting the original clusters and removing them from the current cluster list
     * 
     *  @param  pClusterLhs address of the first cluster
     *  @param  pClusterRhs address of the second cluster
     */
    StatusCode MergeAndDeleteClusters(Cluster *pClusterLhs, Cluster *pClusterRhs) const;
        
    /**
     *  @brief  Save the current cluster list and remove the constituent hits from the current ordered calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     *  @param  pClustersToSave a subset of the algorithm input cluster list - only clusters in both this and the current
     *          cluster lists will be saved
     */
    StatusCode SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
        const ClusterList *const pClustersToSave = NULL) const;

    /**
     *  @brief  Replace the current cluster list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the name of the replacement cluster list
     */        
    StatusCode ReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName) const;
    
    /**
     *  @brief  Save the current cluster list under a new name; use this new list as a permanent replacement for the current
     *          list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     *  @param  pClustersToSave a subset of the current cluster list - only clusters in both this and the current
     *          cluster lists will be saved
     */
    StatusCode SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
        const ClusterList *const pClustersToSave = NULL) const;

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
    StatusCode ReplaceCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &newListName) const;
    
    /**
     *  @brief  Save the current ordered calo hit list under a new name; use this new list as a permanent replacement for the
     *          current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new ordered calo hit list name
     */
    StatusCode SaveOrderedCaloHitListAndReplaceCurrent(const Algorithm &algorithm, const OrderedCaloHitList &orderedCaloHitList,
        const std::string &newListName) const;

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
    StatusCode ReplaceCurrentTrackList(const pandora::Algorithm &algorithm, const std::string &newListName) const;
    
    /**
     *  @brief  Save the current track list under a new name; use this new list as a permanent replacement for the current
     *          list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new track list name
     */
    StatusCode SaveTrackListAndReplaceCurrent(const Algorithm &algorithm, const TrackList &trackList, const std::string &newListName) const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the pandora object to interface
     */
    PandoraContentApiImpl(Pandora *pPandora);

    Pandora    *m_pPandora;    ///< The pandora object to provide an interface to
    
    friend class Pandora;
};

} // namespace pandora

#endif // #ifndef PANDORA_CONTENT_API_IMPL_H
