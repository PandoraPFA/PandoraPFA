/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ProximityBasedMergingAlgorithm.h
 * 
 *  @brief  Header file for the proximity based merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef PROXIMITY_BASED_MERGING_ALGORITHM_H
#define PROXIMITY_BASED_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ProximityBasedMergingAlgorithm class
 */
class ProximityBasedMergingAlgorithm : public pandora::Algorithm
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
    /**
     *  @brief  CustomClusterOrder class
     */
    class CustomClusterOrder
    {
    public:
        /**
         *  @brief  Operator () for determining custom cluster ordering. Operator returns true if lhs cluster is to be
         *          placed at an earlier position than rhs cluster.
         * 
         *  @param  lhs cluster for comparison
         *  @param  rhs cluster for comparison
         */
        bool operator()(const pandora::Cluster *lhs, const pandora::Cluster *rhs) const;
    };

    typedef std::set<pandora::Cluster *, CustomClusterOrder> CustomSortedClusterList;

    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  Get generic distance between hits in parent cluster candidate and those in nearby (typically overlapping) daughter
     *          cluster candidate. For each layer in parent cluster evaluate distance to hits in adjacent layers in daughter cluster.
     * 
     *  @param  pParentCluster address of the parent candidate cluster
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  startLayer first layer to examine
     *  @param  endLayer last layer to examine
     *  @param  distance to receive the generic distance
     */
    StatusCode GetGenericDistanceBetweenClusters(const pandora::Cluster *const pParentCluster, const pandora::Cluster *const pDaughterCluster,
        const pandora::PseudoLayer startLayer, const pandora::PseudoLayer endLayer, float &distance) const;

    /**
     *  @brief  Whether a daughter candidate cluster is consistent with being a fragment of a parent candidate cluster
     * 
     *  @param  pParentCluster address of the parent candidate cluster
     *  @param  pDaughterCluster address of the daughter candidate cluster
     * 
     *  @return boolean
     */
    bool IsClusterFragment(const pandora::Cluster *const pParentCluster, const pandora::Cluster *const pDaughterCluster) const;

    std::string     m_trackClusterAssociationAlgName;   ///< The name of the track-cluster association algorithm to run

    float           m_canMergeMinMipFraction;           ///< The minimum mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The maximum all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minClusterInnerLayer;             ///< Min inner layer number for merging (for either parent or daughter cluster)
    int             m_minLayerSpan;                     ///< Min span between parent and daughter outer and inner layers
    int             m_minShowerLayerSpan;               ///< Min span between daughter inner layer and parent shower max layer

    float           m_maxTrackClusterChi;               ///< Max no. standard deviations between cluster and associated track energies
    float           m_maxTrackClusterChi0;              ///< Max no. standard deviations between parent cluster energy and track energies

    unsigned int    m_nGenericDistanceLayers;           ///< Number of layers to examine when calculating generic distance between clusters
    float           m_maxGenericDistance;               ///< Max value of generic distance between two clusters for clusters to be merged
    unsigned int    m_nAdjacentLayersToExamine;         ///< Number of adjacent layers to examine when calculating cluster generic distance
    float           m_maxParallelDistance;              ///< Max component of hit position difference in (radial) direction of parent hit

    float           m_maxInnerLayerSeparation;          ///< Maximum distance between parent and daughter inner layer centroids

    float           m_clusterContactThreshold;          ///< Threshold distance for labelling layers in parent and daughter as "in contact"
    float           m_minContactFraction;               ///< Min fraction of overlapping parent/daughter layers that must be in contact

    float           m_closeHitThreshold;                ///< Threshold distance for labelling hits in parent and daughter as "close"
    float           m_minCloseHitFraction;              ///< Max fraction of daughter cluster hits that must be close to parent hits

    float           m_maxHelixPathlengthToDaughter;     ///< Max path length from helix ecal intersection to daughter cluster z coordinate
    unsigned int    m_helixDistanceNLayers;             ///< Layers to examine to calculate distance from helix to hits in daughter cluster
    unsigned int    m_helixDistanceMaxOccupiedLayers;   ///< Max number of occupied layers to consider in helix-cluster distance calculation
    float           m_maxClusterHelixDistance;          ///< Max distance between parent cluster associated helix projections and daughter
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ProximityBasedMergingAlgorithm::CustomClusterOrder::operator()(const pandora::Cluster *lhs, const pandora::Cluster *rhs) const
{
    const pandora::PseudoLayer innerLayerLhs(lhs->GetInnerPseudoLayer()), innerLayerRhs(rhs->GetInnerPseudoLayer());

    if (innerLayerLhs < innerLayerRhs)
        return true;

    if (innerLayerLhs > innerLayerRhs)
        return false;

    const float hadronicEnergyLhs(lhs->GetHadronicEnergy()), hadronicEnergyRhs(rhs->GetHadronicEnergy());

    if (hadronicEnergyLhs > hadronicEnergyRhs)
        return true;

    if (hadronicEnergyLhs < hadronicEnergyRhs)
        return false;

    return lhs > rhs;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ProximityBasedMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new ProximityBasedMergingAlgorithm();
}

#endif // #ifndef PROXIMITY_BASED_MERGING_ALGORITHM_H
