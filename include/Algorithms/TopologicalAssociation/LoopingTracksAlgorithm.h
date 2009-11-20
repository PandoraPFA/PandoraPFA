/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/LoopingTracksAlgorithm.h
 * 
 *  @brief  Header file for the looping tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef LOOPING_TRACKS_ALGORITHM_H
#define LOOPING_TRACKS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  LoopingTracksAlgorithm class
 */
class LoopingTracksAlgorithm : public pandora::Algorithm
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
    typedef pandora::ClusterHelper::ClusterFitResult ClusterFitResult;

    StatusCode Run();
    StatusCode ReadSettings(TiXmlHandle xmlHandle);

    /**
     *  @brief  Get the closest distance between hits in the outermost pseudolayer of two clusters
     * 
     *  @param  pClusterI address of first cluster
     *  @param  pClusterJ address of second cluster
     * 
     *  @return the closest distance between outer layer hits
     */
    float GetClosestDistanceBetweenOuterLayerHits(const pandora::Cluster *const pClusterI, const pandora::Cluster *const pClusterJ) const;

    /**
     *  @brief  Get closest distance of approach between two cluster fit results
     * 
     *  @param  clusterFitResultI first cluster fit result
     *  @param  clusterFitResultJ second cluster fit result
     * 
     *  @return the distance of closest approach
     */
    float GetFitResultsClosestApproach(const ClusterFitResult &clusterFitResultI, const ClusterFitResult &clusterFitResultJ) const;

    /**
     *  @brief  Whether clusters are compatible with the looping track hypothesis (final criteria)
     * 
     *  @return boolean
     */
    bool AreClustersCompatible() const;

    /**
     *  @brief  Whether a cluster can be merged with another (simple criteria)
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    bool CanMergeCluster(pandora::Cluster *const pCluster);

    unsigned int    m_nLayersToFit;                     ///< The number of occupied pseudolayers to use in fit to the end of the cluster
    float           m_fitChi2Cut;                       ///< The chi2 cut to apply to fit results

    float           m_canMergeMinMipFraction;           ///< The minimum mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The maximum all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_maxOuterLayerDifference;          ///< The maximum difference (for merging) between cluster outer pseudo layers
    float           m_maxCentroidDifference;            ///< The maximum difference (for merging) between cluster outer layer centroids
    float           m_maxFitDirectionDotProduct;        ///< The maximum value (for merging) of dot product between cluster fit directions

    float           m_closestHitDistanceCutECal;        ///< ECal cut on distance between cluster hits in outermost pseudolayers
    float           m_closestHitDistanceCutHCal;        ///< HCal cut on distance between cluster hits in outermost pseudolayers

    float           m_fitResultClosestApproachCutECal;  ///< ECal cut on closest distance of approach between two cluster fit results
    float           m_fitResultClosestApproachCutHCal;  ///< HCal cut on closest distance of approach between two cluster fit results
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *LoopingTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new LoopingTracksAlgorithm();
}

#endif // #ifndef LOOPING_TRACKS_ALGORITHM_H
