/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Clustering/ConeClusteringAlgorithm.h
 * 
 *  @brief  Header file for the clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef CONE_CLUSTERING_ALGORITHM_H
#define CONE_CLUSTERING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Objects/CaloHit.h"
#include "Objects/CartesianVector.h"

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
    bool operator()(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const;

    /**
     *  @brief  Order calo hits by decreasing input energy
     * 
     *  @param  lhs calo hit for comparison
     *  @param  rhs calo hit for comparison
     */
    bool SortByEnergy(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const;

    /**
     *  @brief  Order calo hits by decreasing density weight
     * 
     *  @param  lhs calo hit for comparison
     *  @param  rhs calo hit for comparison
     */
    bool SortByDensityWeight(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const;

    /**
     *  @brief  Function pointer, specifying the function used to sort the calo hits
     */
    bool (CustomHitOrder::*pSortFunction)(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const;

    static unsigned int     m_hitSortingStrategy;   ///< Strategy used to sort the calo hits: 0 - input energy, 1 - density weight, ...
};

typedef std::set<pandora::CaloHit *, CustomHitOrder> CustomSortedCaloHitList;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ConeClusteringAlgorithm class
 */
class ConeClusteringAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Use current track list to make seed clusters
     * 
     *  @param  clusterVector to receive the addresses of clusters created (which could also be accessed via current cluster list)
     */
    pandora::StatusCode SeedClustersWithTracks(pandora::ClusterVector &clusterVector) const;

    /**
     *  @brief  Match clusters to calo hits in previous pseudo layers
     * 
     *  @param  pseudoLayer the current pseudo layer
     *  @param  pCustomSortedCaloHitList address of the custom sorted list of calo hits in the current pseudo layer
     *  @param  pOrderedCaloHitList address of the current ordered calo hit list
     *  @param  clusterVector vector containing addresses of current clusters
     */
    pandora::StatusCode FindHitsInPreviousLayers(pandora::PseudoLayer pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
        pandora::ClusterVector &clusterVector) const;

    /**
     *  @brief  Match clusters to calo hits in current pseudo layer
     * 
     *  @param  pseudoLayer the current pseudo layer
     *  @param  pCustomSortedCaloHitList address of the custom sorted list of calo hits in the current pseudo layer
     *  @param  clusterVector vector containing addresses of current clusters
     */
    pandora::StatusCode FindHitsInSameLayer(pandora::PseudoLayer pseudoLayer, CustomSortedCaloHitList *const pCustomSortedCaloHitList,
        pandora::ClusterVector &clusterVector) const;

    /**
     *  @brief  Update the properties of the current clusters, calculating their current directions and identifying whether
     *          they are likely to be sections of mip tracks
     * 
     *  @param  clusterVector vector containing addresses of current clusters
     */
    pandora::StatusCode UpdateClusterProperties(pandora::ClusterVector &clusterVector) const;

    /**
     *  @brief  Get the "generic distance" between a calo hit and a cluster; the smaller the distance, the stronger the association
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  searchLayer the pseudolayer currently being examined
     *  @param  genericDistance to receive the generic distance
     */
    pandora::StatusCode GetGenericDistanceToHit(pandora::Cluster *const pCluster, pandora::CaloHit *const pCaloHit,
        const pandora::PseudoLayer searchLayer, float &genericDistance) const;

    /**
     *  @brief  Get the generic distance between a calo hit and a cluster in the same pseudo layer
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  pCaloHitList address of the cluster's constituent hit list
     *  @param  distance to receive the distance
     */
    pandora::StatusCode GetDistanceToHitInSameLayer(pandora::CaloHit *const pCaloHit, const pandora::CaloHitList *const pCaloHitList,
        float &distance) const;

    /**
     *  @brief  Get the smallest cone approach distance between a calo hit and all the hits in a cluster, using a specified
     *          measurement of the cluster direction
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  pCaloHitList
     *  @param  clusterDirection
     *  @param  distance to receive the generic distance
     */
    pandora::StatusCode GetConeApproachDistanceToHit(pandora::CaloHit *const pCaloHit, const pandora::CaloHitList *const pCaloHitList,
        const pandora::CartesianVector &clusterDirection, float &distance) const;

    /**
     *  @brief  Get the cone approach distance between a calo hit and a specified point in the cluster, using a specified
     *          measurement of the cluster direction
     * 
     *  @param  pCaloHit address of the calo hit
     *  @param  clusterPosition the cluster position vector
     *  @param  clusterDirection the cluster direction unit vector
     *  @param  distance to receive the distance
     */
    pandora::StatusCode GetConeApproachDistanceToHit(pandora::CaloHit *const pCaloHit, const pandora::CartesianVector &clusterPosition,
        const pandora::CartesianVector &clusterDirection, float &distance) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the calorimeter surface, ONLY calculated if:
     *          1) the calo hit is within the first m_maxLayersToTrackSeed OR
     *          2) cluster contains a hit consistent with the track seed within the last m_maxLayersToTrackLikeHit
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  the pseudolayer currently being examined
     *  @param  to receive the distance
     */
    pandora::StatusCode GetDistanceToTrackSeed(pandora::Cluster *const pCluster, pandora::CaloHit *const pCaloHit,
        pandora::PseudoLayer searchLayer, float &distance) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the calorimeter surface
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  distance to receive the distance
     */
    pandora::StatusCode GetDistanceToTrackSeed(pandora::Cluster *const pCluster, pandora::CaloHit *const pCaloHit, float &distance) const;

    /**
     *  @brief  Remove any empty clusters at the end of the algorithm
     * 
     *  @param  clusterVector containing the addresses of all clusters created
     */
    pandora::StatusCode RemoveEmptyClusters(pandora::ClusterVector &clusterVector) const;

    unsigned int    m_clusterSeedStrategy;          ///< Flag determining if and how clusters should be seeded with tracks

    bool            m_shouldUseOnlyECalHits;        ///< Whether to only use ecal hits in the clustering algorithm
    bool            m_shouldUseIsolatedHits;        ///< Whether to use isolated hits in the clustering algorithm

    unsigned int    m_layersToStepBackFine;         ///< Number of layers to step back when associating a fine granularity hit to clusters
    unsigned int    m_layersToStepBackCoarse;       ///< Number of layers to step back when associating a coarse granularity hit to clusters

    unsigned int    m_clusterFormationStrategy;     ///< Flag determining when to add "best" calo hit found to clusters
    float           m_genericDistanceCut;           ///< Generic distance cut to apply throughout algorithm

    float           m_minHitTrackCosAngle;          ///< Min cos(angle) between calo hit position and track projection
    float           m_minHitClusterCosAngle;        ///< Min cos(angle) between calo hit position and cluster searchlayer centroid

    bool            m_shouldUseTrackSeed;           ///< Whether to use track seed information in clustering
    unsigned int    m_trackSeedCutOffLayer;         ///< Pseudo layer beyond which track seed is no longer considered
    bool            m_shouldFollowInitialDirection; ///< Whether the cluster should grow preferentially in its initial direction

    float           m_sameLayerPadWidthsFine;       ///< Fine adjacent pad widths used to calculate generic distance to same layer hit
    float           m_sameLayerPadWidthsCoarse;     ///< Coarse adjacent pad widths used to calculate generic distance to same layer hit

    float           m_coneApproachMaxSeparation2;   ///< Maximum separation between calo hit and specified cluster position (squared)
    float           m_tanConeAngleFine;             ///< Fine tan cone angle used to calculate cone approach distance
    float           m_tanConeAngleCoarse;           ///< Coarse tan cone angle used to calculate cone approach distance
    float           m_additionalPadWidthsFine;      ///< Fine adjacent pad widths used to calculate cone approach distance
    float           m_additionalPadWidthsCoarse;    ///< Coarse adjacent pad widths used to calculate cone approach distance
    float           m_maxClusterDirProjection;      ///< Max projection of cluster-hit separation in cluster dir to calculate cone distance
    float           m_minClusterDirProjection;      ///< Min projection of cluster-hit separation in cluster dir to calculate cone distance

    float           m_trackPathWidth;               ///< Track path width, used to determine whether hits are associated with seed track
    float           m_maxTrackSeedSeparation2;      ///< Maximum distance between a calo hit and track seed (squared)

    unsigned int    m_maxLayersToTrackSeed;         ///< Max number of layers to the track seed for trackSeedDistance to be calculated
    unsigned int    m_maxLayersToTrackLikeHit;      ///< Max number of layers to a "track-like" hit for trackSeedDistance to be calculated

    unsigned int    m_nLayersSpannedForFit;         ///< Min number of layers spanned by cluster before current direction fit is performed
    unsigned int    m_nLayersSpannedForApproxFit;   ///< If current fit fails, but cluster spans enough layers, calculate an approximate fit
    unsigned int    m_nLayersToFit;                 ///< Number of layers (from outermost layer) to examine in current direction fit
    float           m_nLayersToFitLowMipCut;        ///< Low cluster mip fraction threshold, below which may need to fit more layers
    unsigned int    m_nLayersToFitLowMipMultiplier; ///< Number of layers to fit multiplier for low mip fraction clusters

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

inline bool CustomHitOrder::operator()(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const
{
    return (this->*pSortFunction)(lhs, rhs);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CustomHitOrder::SortByEnergy(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const
{
    return (!(lhs->GetInputEnergy() > rhs->GetInputEnergy()) && !(rhs->GetInputEnergy() > lhs->GetInputEnergy()) ?
        (lhs > rhs) :
        (lhs->GetInputEnergy() > rhs->GetInputEnergy()));
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CustomHitOrder::SortByDensityWeight(const pandora::CaloHit *lhs, const pandora::CaloHit *rhs) const
{
    return (!(lhs->GetDensityWeight() > rhs->GetDensityWeight()) && !(rhs->GetDensityWeight() > lhs->GetDensityWeight()) ?
        (lhs > rhs) :
        (lhs->GetDensityWeight() > rhs->GetDensityWeight()));
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ConeClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ConeClusteringAlgorithm();
}

#endif // #ifndef CONE_CLUSTERING_ALGORITHM_H
