/**
 *  @file   PandoraPFANew/include/Algorithms/Clustering/ClusteringAlgorithm.h
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
 *  @brief  CustomHitOrder class
 */
class CustomHitOrder
{
public:
    /**
     *  @brief  Default constructor
     */
    CustomHitOrder();

    /**
     *  @brief  Operator () for determining custom calo hit ordering. Operator returns true if lhs hit is to be
     *          placed at an earlier position than rhs hit.
     * 
     *  @param  lhs calo hit for comparison
     *  @param  rhs calo hit for comparison
     */
    bool operator()(const CaloHit *lhs, const CaloHit *rhs) const;

    /**
     *  @brief  Order calo hits by decreasing input energy
     * 
     *  @param  lhs calo hit for comparison
     *  @param  rhs calo hit for comparison
     */
    bool SortByEnergy(const CaloHit *lhs, const CaloHit *rhs) const;

    /**
     *  @brief  Order calo hits by decreasing density weight
     * 
     *  @param  lhs calo hit for comparison
     *  @param  rhs calo hit for comparison
     */
    bool SortByDensityWeight(const CaloHit *lhs, const CaloHit *rhs) const;

    /**
     *  @brief  Function pointer, specifying the function used to sort the calo hits
     */
    bool (CustomHitOrder::*pSortFunction)(const CaloHit *lhs, const CaloHit *rhs) const;

    static unsigned int     m_hitSortingStrategy;   ///< Strategy used to sort the calo hits: 0 - input energy, 1 - density weight, ...
};

typedef std::set<CaloHit *, CustomHitOrder> CustomSortedCaloHitList;

//------------------------------------------------------------------------------------------------------------------------------------------

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
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

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
     *  @param  pCustomSortedCaloHitList address of the custom sorted list of calo hits in the current pseudo layer
     *  @param  pOrderedCaloHitList address of the current ordered calo hit list
     *  @param  clusterVector vector containing addresses of current clusters
     */
    StatusCode FindHitsInPreviousLayers(PseudoLayer pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
        const OrderedCaloHitList *const pOrderedCaloHitList, ClusterVector &clusterVector) const;

    /**
     *  @brief  Match clusters to calo hits in current pseudo layer
     * 
     *  @param  pseudoLayer the current pseudo layer
     *  @param  pCustomSortedCaloHitList address of the custom sorted list of calo hits in the current pseudo layer
     *  @param  clusterVector vector containing addresses of current clusters
     */
    StatusCode FindHitsInSameLayer(PseudoLayer pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
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

    bool            m_shouldUseIsolatedHits;        ///< Whether to use isolated hits in the clustering algorithm

    unsigned int    m_layersToStepBackECal;         ///< Number of layers to step back when associating an ecal hit to clusters
    unsigned int    m_layersToStepBackHCal;         ///< Number of layers to step back when associating an hcal hit to clusters

    unsigned int    m_clusterFormationStrategy;     ///< Flag determining when to add "best" calo hit found to clusters
    float           m_genericDistanceCut;           ///< Generic distance cut to apply throughout algorithm

    bool            m_shouldUseTrackSeed;           ///< Whether to use track seed information in clustering
    unsigned int    m_trackSeedCutOffLayer;         ///< Pseudo layer beyond which track seed is no longer considered
    bool            m_shouldFollowInitialDirection; ///< Whether the cluster should grow preferentially in its initial direction

    float           m_sameLayerPadWidthsECal;       ///< ECal adjacent pad widths used to calculate generic distance to same layer hit
    float           m_sameLayerPadWidthsHCal;       ///< HCal adjacent pad widths used to calculate generic distance to same layer hit

    float           m_coneApproachMaxSeparation;    ///< Maximum separation between calo hit and specified cluster position
    float           m_tanConeAngleECal;             ///< ECal tan cone angle used to calculate cone approach distance
    float           m_tanConeAngleHCal;             ///< HCal tan cone angle used to calculate cone approach distance
    float           m_additionalPadWidthsECal;      ///< ECal adjacent pad widths used to calculate cone approach distance
    float           m_additionalPadWidthsHCal;      ///< HCal adjacent pad widths used to calculate cone approach distance
    float           m_maxClusterDirProjection;      ///< Max projection of cluster-hit separation in cluster dir to calculate cone distance
    float           m_minClusterDirProjection;      ///< Min projection of cluster-hit separation in cluster dir to calculate cone distance

    float           m_trackPathWidth;               ///< Track path width, used to determine whether hits are associated with seed track
    float           m_maxTrackSeedSeparation;       ///< Maximum distance between a calo hit and track seed

    unsigned int    m_maxLayersToTrackSeed;         ///< Max number of layers to the track seed for trackSeedDistance to be calculated
    unsigned int    m_maxLayersToTrackLikeHit;      ///< Max number of layers to a "track-like" hit for trackSeedDistance to be calculated

    unsigned int    m_nLayersSpannedForFit;         ///< Min number of layers spanned by cluster before current direction fit is performed
    unsigned int    m_nLayersSpannedForApproxFit;   ///< If current fit fails, but cluster spans enough layers, calculate an approximate fit
    unsigned int    m_nLayersToFit;                 ///< Number of layers (from outermost layer) to examine in current direction fit

    float           m_fitSuccessDotProductCut1;     ///< 1. Min value of dot product (current fit dir dot initial dir) for fit success
    float           m_fitSuccessChi2Cut1;           ///< 1. Max value of fit chi2 for fit success

    float           m_fitSuccessDotProductCut2;     ///< 2. Min value of dot product (current fit dir dot initial dir) for fit success
    float           m_fitSuccessChi2Cut2;           ///< 2. Max value of fit chi2 for fit success

    float           m_mipTrackChi2Cut;              ///< Max value of fit chi2 for track seeded cluster to retain its IsMipTrack status
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline CustomHitOrder::CustomHitOrder()
{
    if (1 == m_hitSortingStrategy)
    {
        pSortFunction = &CustomHitOrder::SortByDensityWeight;
    }
    else
    {
        pSortFunction = &CustomHitOrder::SortByEnergy;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CustomHitOrder::operator()(const CaloHit *lhs, const CaloHit *rhs) const
{
    return (this->*pSortFunction)(lhs, rhs);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CustomHitOrder::SortByEnergy(const CaloHit *lhs, const CaloHit *rhs) const
{
    return (!(lhs->GetInputEnergy() > rhs->GetInputEnergy()) && !(rhs->GetInputEnergy() > lhs->GetInputEnergy()) ?
        (lhs > rhs) :
        (lhs->GetInputEnergy() > rhs->GetInputEnergy()));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CustomHitOrder::SortByDensityWeight(const CaloHit *lhs, const CaloHit *rhs) const
{
    return (!(lhs->GetDensityWeight() > rhs->GetDensityWeight()) && !(rhs->GetDensityWeight() > lhs->GetDensityWeight()) ?
        (lhs > rhs) :
        (lhs->GetDensityWeight() > rhs->GetDensityWeight()));
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ClusteringAlgorithm();
}

#endif // #ifndef CLUSTERING_ALGORITHM_H
