/**
 *  @file   PandoraPFANew/include/Algorithms/ClusteringAlgorithm.h
 * 
 *  @brief  Header file for the clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef CLUSTERING_ALGORITHM_H
#define CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

using namespace pandora;

/**
 *  @brief  ClusteringAlgorithm class
 */
class ClusteringAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(TiXmlHandle xmlHandle);

    /**
     *  @brief  Use current track list to make seed clusters
     * 
     *  @param  clusterVector to receive the addresses of clusters created (which could also be accessed via current cluster list)
     */
    StatusCode SeedClustersWithTracks(ClusterVector &clusterVector) const;

    /**
     *  @brief  Match clusters to calo hits in previous pseudo layers
     * 
     *  @param  pseudoLayer the current pseudo layer
     *  @param  pEnergySortedCaloHitList address of the energy sorted list of calo hits in the current pseudo layer
     *  @param  pOrderedCaloHitList address of the current ordered calo hit list
     *  @param  clusterVector vector containing addresses of current clusters
     */
    StatusCode FindHitsInPreviousLayers(PseudoLayer pseudoLayer, EnergySortedCaloHitList *const pEnergySortedCaloHitList,
        const OrderedCaloHitList *const pOrderedCaloHitList, ClusterVector &clusterVector) const;

    /**
     *  @brief  Match clusters to calo hits in current pseudo layer
     * 
     *  @param  pseudoLayer the current pseudo layer
     *  @param  pEnergySortedCaloHitList address of the energy sorted list of calo hits in the current pseudo layer
     *  @param  clusterVector vector containing addresses of current clusters
     */
    StatusCode FindHitsInSameLayer(PseudoLayer pseudoLayer, EnergySortedCaloHitList *const pEnergySortedCaloHitList,
        ClusterVector &clusterVector) const;

    /**
     *  @brief  Update the properties of the current clusters, calculating their current directions and identifying whether
     *          they are likely to be sections of mip tracks
     * 
     *  @param  pseudoLayer the current pseudo layer
     *  @param  clusterVector vector containing addresses of current clusters
     */
    StatusCode UpdateClusterProperties(PseudoLayer pseudoLayer, ClusterVector &clusterVector) const;

    /**
     *  @brief  Get the "generic distance" between a calo hit and a cluster; the smaller the distance, the stronger the association
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  searchLayer the pseudolayer currently being examined
     *  @param  genericDistance to receive the generic distance
     */
    StatusCode GetGenericDistanceToHit(Cluster *const pCluster, CaloHit *const pCaloHit, PseudoLayer searchLayer,
        float &genericDistance) const;

    /**
     *  @brief  Get the generic distance between a calo hit and a cluster in the same pseudo layer
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  pCaloHitList address of the cluster's constituent hit list
     *  @param  distance to receive the distance
     */
    StatusCode GetDistanceToHitInSameLayer(CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList, float &distance) const;

    /**
     *  @brief  Get the smallest cone approach distance between a calo hit and all the hits in a cluster, using a specified
     *          measurement of the cluster direction
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  pCaloHitList
     *  @param  clusterDirection
     *  @param  distance to receive the generic distance
     */
    StatusCode GetConeApproachDistanceToHit(CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList,
        const CartesianVector &clusterDirection, float &distance) const;

    /**
     *  @brief  Get the cone approach distance between a calo hit and a specified point in the cluster, using a specified
     *          measurement of the cluster direction
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  clusterPosition the cluster position vector
     *  @param  clusterDirection the cluster direction unit vector
     *  @param  distance to receive the distance
     */
    StatusCode GetConeApproachDistanceToHit(CaloHit *const pCaloHit, const CartesianVector &clusterPosition,
        const CartesianVector &clusterDirection, float &distance) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the ecal surface, ONLY calculated if:
     *          1) the calo hit is within the first m_maxLayersToTrackSeed OR
     *          2) cluster contains a hit consistent with the track seed within the last m_maxLayersToTrackLikeHit
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  the pseudolayer currently being examined
     *  @param  to receive the distance
     */
    StatusCode GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, PseudoLayer searchLayer,
        float &distance) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the ecal surface
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  distance to receive the distance
     */
    StatusCode GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, float &distance) const;

    static const float FLOAT_MAX;

    unsigned int    m_clusterSeedStrategy;          ///< Flag determining if and how clusters should be seeded with tracks
    float           m_trackSeedMaxCosTheta;         ///< Cut off for seeding clusters with tracks (unless all tracks requested)

    unsigned int    m_layersToStepBackECal;         ///< Number of layers to step back when associating an ecal hit to clusters
    unsigned int    m_layersToStepBackHCal;         ///< Number of layers to step back when associating an hcal hit to clusters

    unsigned int    m_clusterFormationStrategy;     ///< Flag determining when to add "best" calo hit found to clusters
    float           m_genericDistanceCut;           ///< Generic distance cut to apply throughout algorithm

    bool            m_shouldUseTrackSeed;           ///< Whether to use track seed information in clustering
    unsigned int    m_trackSeedCutOffLayer;         ///< Pseudo layer beyond which track seed is no longer considered

    float           m_sameLayerPadWidthsECal;       ///< ECal adjacent pad widths used to calculate generic distance to same layer hit
    float           m_sameLayerPadWidthsHCal;       ///< HCal adjacent pad widths used to calculate generic distance to same layer hit

    float           m_coneApproachMaxSeparation;    ///< Maximum separation between calo hit and specified cluster position
    float           m_tanConeAngleECal;             ///< ECal tan cone angle used to calculate cone approach distance
    float           m_tanConeAngleHCal;             ///< HCal tan cone angle used to calculate cone approach distance
    float           m_additionalPadWidthsECal;      ///< ECal adjacent pad widths used to calculate cone approach distance
    float           m_additionalPadWidthsHCal;      ///< HCal adjacent pad widths used to calculate cone approach distance

    float           m_trackPathWidth;               ///< Track path width, used to determine whether hits are associated with seed track
    float           m_maxTrackSeedSeparation;       ///< Maximum distance between a calo hit and track seed

    unsigned int    m_maxLayersToTrackSeed;         ///< Max number of layers to the track seed for trackSeedDistance to be calculated
    unsigned int    m_maxLayersToTrackLikeHit;      ///< Max number of layers to a "track-like" hit for trackSeedDistance to be calculated
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ClusteringAlgorithm();
}

#endif // #ifndef CLUSTERING_ALGORITHM_H
