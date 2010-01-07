/**
 *  @file   PandoraPFANew/include/Api/PandoraContentApi.h
 *
 *  @brief  Header file for the pandora content api class.
 *
 *  $Log: $
 */
#ifndef PANDORA_CONTENT_API_H
#define PANDORA_CONTENT_API_H 1

#include "Pandora/Pandora.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

class TiXmlElement;
class TiXmlHandle;

namespace pandora { class Algorithm; }

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraContentApi class
 */
class PandoraContentApi
{
public:
    /**
     *  @brief  Cluster creation class
     */
    class Cluster
    {
    public:
        /**
         *  @brief  Create a cluster
         *
         *  @param  algorithm the algorithm creating the cluster
         *  @param  pClusterParameters address of either 1) a single calo hit, 2) a calo hit vector, or 3) a track
         */
        template <typename CLUSTER_PARAMETERS>
        static StatusCode Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters);

        /**
         *  @brief  Create a cluster
         *
         *  @param  algorithm the algorithm creating the cluster
         *  @param  pClusterParameters address of either 1) a single calo hit, 2) a calo hit vector, or 3) a track
         *  @param  pCluster to receive the address of the cluster created
         */
        template <typename CLUSTER_PARAMETERS>
        static StatusCode Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters, pandora::Cluster *&pCluster);
    };

    /**
     *  @brief  Particle flow object creation class
     */
    class ParticleFlowObject
    {
    public:
        /**
         *  @brief  Parameters class
         */
        class Parameters
        {
        public:
            pandora::InputInt               m_particleId;       ///< The particle flow object id (PDG code)
            pandora::InputInt               m_chargeSign;       ///< The particle flow object charge sign
            pandora::InputFloat             m_mass;             ///< The particle flow object mass
            pandora::InputFloat             m_energy;           ///< The particle flow object energy
            pandora::InputCartesianVector   m_momentum;         ///< The particle flow object momentum
            pandora::ClusterList            m_clusterList;      ///< The clusters in the particle flow object
            pandora::TrackList              m_trackList;        ///< The tracks in the particle flow object
        };

        /**
         *  @brief  Create a particle flow object
         * 
         *  @param  algorithm the algorithm creating the particle flow object
         *  @param  particleFlowObjectParameters the particle flow object parameters
         */
        static StatusCode Create(const pandora::Algorithm &algorithm, const Parameters &parameters);
    };

    typedef ParticleFlowObject::Parameters ParticleFlowObjectParameters;

    /**
     *  @brief  Create an algorithm instance, via one of the algorithm factories registered with pandora.
     *          This function is expected to be called whilst reading the settings for a parent algorithm.
     * 
     *  @param  parentAlgorithm address of the parent algorithm, which will later run this daughter algorithm
     *  @param  pXmlElement address of the xml element describing the daughter algorithm type and settings
     *  @param  daughterAlgorithmName to receive the name of the daughter algorithm instance
     */
    static StatusCode CreateDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, TiXmlElement *const pXmlElement,
        std::string &daughterAlgorithmName);

    /**
     *  @brief  Run an algorithm registered with pandora, from within a parent algorithm
     * 
     *  @param  parentAlgorithm address of the parent algorithm, now attempting to run a daughter algorithm
     *  @param  daughterAlgorithmName the name of the daughter algorithm instance to run
     */
    static StatusCode RunDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, const std::string &daughterAlgorithmName);

    /**
     *  @brief  Get the current cluster list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterList to receive the address of the current cluster list
     */
    static StatusCode GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList);

    /**
     *  @brief  Get the current cluster list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterList to receive the address of the current cluster list
     *  @param  clusterListName to receive the current cluster list name
     */
    static StatusCode GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList,
        std::string &clusterListName);

    /**
     *  @brief  Get the current cluster list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterListName to receive the current cluster list name
     */
    static StatusCode GetCurrentClusterListName(const pandora::Algorithm &algorithm, std::string &clusterListName);

    /**
     *  @brief  Get a named cluster list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusterListName the name of the cluster list
     *  @param  pClusterList to receive the address of the cluster list
     */
    static StatusCode GetClusterList(const pandora::Algorithm &algorithm, const std::string &clusterListName,
        const pandora::ClusterList *&pClusterList);

    /**
     *  @brief  Get the current ordered calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pOrderedCaloHitList to receive the address of the current ordered calo hit list
     */
    static StatusCode GetCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm, const pandora::OrderedCaloHitList *&pOrderedCaloHitList);

    /**
     *  @brief  Get the current ordered calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pOrderedCaloHitList to receive the address of the current ordered calo hit list
     *  @param  orderedCaloHitListName to receive the current ordered calo hit list name
     */
    static StatusCode GetCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm, const pandora::OrderedCaloHitList *&pOrderedCaloHitList,
        std::string &orderedCaloHitListName);

    /**
     *  @brief  Get the current ordered calo hit list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  orderedCaloHitListName to receive the current ordered calo hit list name
     */
    static StatusCode GetCurrentOrderedCaloHitListName(const pandora::Algorithm &algorithm, std::string &orderedCaloHitListName);

    /**
     *  @brief  Get a named ordered calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  orderedCaloHitListName the name of the ordered calo hit list
     *  @param  pOrderedCaloHitList to receive the address of the ordered calo hit list
     */
    static StatusCode GetOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &orderedCaloHitListName,
        const pandora::OrderedCaloHitList *&pOrderedCaloHitList);

    /**
     *  @brief  Get the current track list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pTrackList to receive the address of the current track list
     */
    static StatusCode GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList);

    /**
     *  @brief  Get the current track list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pTrackList to receive the address of the current track list
     *  @param  trackListName to receive the current track list name
     */
    static StatusCode GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList,
        std::string &trackListName);

    /**
     *  @brief  Get the current track list name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  trackListName to receive the current track list name
     */
    static StatusCode GetCurrentTrackListName(const pandora::Algorithm &algorithm, std::string &trackListName);

    /**
     *  @brief  Get a named track list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  trackListName the name of the track list
     *  @param  pTrackList to receive the address of the track list
     */
    static StatusCode GetTrackList(const pandora::Algorithm &algorithm, const std::string &trackListName, const pandora::TrackList *&pTrackList);

    /**
     *  @brief  Initialize reclustering operations
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  inputTrackList the input track list
     *  @param  inputClusterList the input cluster list
     *  @param  originalClustersListName to receive the name of the list in which the original clusters are stored
     */
    static StatusCode InitializeReclustering(const pandora::Algorithm &algorithm, const pandora::TrackList &inputTrackList,
        const pandora::ClusterList &inputClusterList, std::string &originalClustersListName);

    /**
     *  @brief  End reclustering operations
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  selectedClusterListName the name of the list containing the chosen recluster candidates (or the original candidates)
     */
    static StatusCode EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName);

    /**
     *  @brief  Run a clustering algorithm (an algorithm that will create new cluster objects)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusteringAlgorithmName the name of the clustering algorithm to run
     *  @param  pNewClusterList to receive the address of the new cluster list populated
     */
    static StatusCode RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
        const pandora::ClusterList *&pNewClusterList);

    /**
     *  @brief  Run a clustering algorithm (an algorithm that will create new cluster objects)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  clusteringAlgorithmName the name of the clustering algorithm to run
     *  @param  pNewClusterList to receive the address of the new cluster list populated
     *  @param  newClusterListName to receive the name of the new cluster list populated
     */
    static StatusCode RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
        const pandora::ClusterList *&pNewClusterList, std::string &newClusterListName);

    /**
     *  @brief  Add a calo hit to a cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to modify
     *  @param  pCaloHit address of the hit to add
     */
    static StatusCode AddCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit);

    /**
     *  @brief  Delete a cluster in the current list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pCluster address of the cluster to delete
     */
    static StatusCode DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster);

    /**
     *  @brief  Merge two clusters in the current list, enlarging one cluster and deleting the second
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     */
    static StatusCode MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
        pandora::Cluster *pClusterToDelete);

    /**
     *  @brief  Merge two clusters from two specified lists, enlarging one cluster and deleting the second
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pClusterToEnlarge address of the cluster to enlarge
     *  @param  pClusterToDelete address of the cluster to delete
     *  @param  enlargeListName name of the list containing the cluster to enlarge
     *  @param  deleteListName name of the list containing the cluster to delete
     */
    static StatusCode MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
        pandora::Cluster *pClusterToDelete, const std::string &enlargeListName, const std::string &deleteListName);

    /**
     *  @brief  Add an association between a track and a cluster
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     */
    static StatusCode AddTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
        pandora::Cluster *const pCluster);

    /**
     *  @brief  Remove an association between a track and a cluster
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     */
    static StatusCode RemoveTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
        pandora::Cluster *const pCluster);

    /**
     *  @brief  Remove all track-cluster associations from objects in the current track and cluster lists
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static StatusCode RemoveCurrentTrackClusterAssociations(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Remove all associations between tracks and clusters
     * 
     *  @param  algorithm the algorithm calling this function
     */
    static StatusCode RemoveAllTrackClusterAssociations(const pandora::Algorithm &algorithm);

    /**
     *  @brief  Save the current cluster list and remove the constituent hits from the current ordered calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     */
    static StatusCode SaveClusterList(const pandora::Algorithm &algorithm, const std::string newClusterListName);

    /**
     *  @brief  Save the current cluster list and remove the constituent hits from the current ordered calo hit list
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     *  @param  clustersToSave a subset of the current cluster list - only clusters in both this and the current
     *          cluster list will be saved
     */
    static StatusCode SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
        const pandora::ClusterList &clustersToSave);

    /**
     *  @brief  Replace the current cluster list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the name of the replacement cluster list
     */
    static StatusCode ReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName);

    /**
     *  @brief  Save the current cluster list under a new name; use this new list as a permanent replacement for the current
     *          list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     */
    static StatusCode SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName);

    /**
     *  @brief  Save the current cluster list under a new name; use this new list as a permanent replacement for the current
     *          list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newClusterListName the new cluster list name
     *  @param  clustersToSave a subset of the current cluster list - only clusters in both this and the current
     *          cluster list will be saved
     */
    static StatusCode SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
        const pandora::ClusterList &clustersToSave);

    /**
     *  @brief  Save the current ordered calo hit list under a new name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new ordered calo hit list name
     */
    static StatusCode SaveOrderedCaloHitList(const pandora::Algorithm &algorithm, const pandora::OrderedCaloHitList &orderedCaloHitList,
        const std::string &newListName);

    /**
     *  @brief  Replace the current ordered calo hit list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement ordered calo hit list
     */
    static StatusCode ReplaceCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &newListName);

    /**
     *  @brief  Save the current ordered calo hit list under a new name; use this new list as a permanent replacement for the
     *          current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new ordered calo hit list name
     */
    static StatusCode SaveOrderedCaloHitListAndReplaceCurrent(const pandora::Algorithm &algorithm, 
        const pandora::OrderedCaloHitList &orderedCaloHitList, const std::string &newListName);

    /**
     *  @brief  Save the current track list under a new name
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new track list name
     */
    static StatusCode SaveTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList &trackList, const std::string &newListName);

    /**
     *  @brief  Replace the current track list with a pre-saved list; use this new list as a permanent replacement
     *          for the current list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the name of the replacement track list
     */
    static StatusCode ReplaceCurrentTrackList(const pandora::Algorithm &algorithm, const std::string &newListName);

    /**
     *  @brief  Save the current track list under a new name; use this new list as a permanent replacement for the current
     *          list (will persist outside the current algorithm)
     * 
     *  @param  algorithm the algorithm calling this function
     *  @param  newListName the new track list name
     */
    static StatusCode SaveTrackListAndReplaceCurrent(const pandora::Algorithm &algorithm, const pandora::TrackList &trackList,
        const std::string &newListName);
};

#endif // #ifndef PANDORA_CONTENT_API_H
