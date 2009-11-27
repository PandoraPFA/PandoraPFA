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
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  Get the closest distance between hits in the outermost pseudolayer of two clusters
     * 
     *  @param  pClusterI address of first cluster
     *  @param  pClusterJ address of second cluster
     * 
     *  @return the closest distance between outer layer hits
     */
    float GetClosestDistanceBetweenOuterLayerHits(const pandora::Cluster *const pClusterI, const pandora::Cluster *const pClusterJ) const;

    unsigned int    m_nLayersToFit;                     ///< The number of occupied pseudolayers to use in fit to the end of the cluster
    float           m_fitChi2Cut;                       ///< The chi2 cut to apply to fit results

    float           m_canMergeMinMipFraction;           ///< The minimum mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The maximum all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_maxOuterLayerDifference;          ///< The maximum difference (for merging) between cluster outer pseudo layers
    float           m_maxCentroidDifference;            ///< The maximum difference (for merging) between cluster outer layer centroids

    float           m_fitDirectionDotProductCutECal;    ///< ECal cut on max value of dot product between cluster fit directions
    float           m_fitDirectionDotProductCutHCal;    ///< HCal cut on max value of dot product between cluster fit directions

    float           m_closestHitDistanceCutECal;        ///< ECal cut on distance between cluster hits in outermost pseudolayers
    float           m_closestHitDistanceCutHCal;        ///< HCal cut on distance between cluster hits in outermost pseudolayers

    float           m_fitResultsClosestApproachCutECal; ///< ECal cut on closest distance of approach between two cluster fit results
    float           m_fitResultsClosestApproachCutHCal; ///< HCal cut on closest distance of approach between two cluster fit results

    unsigned int    m_nGoodFeaturesForClusterMerge;     ///< Number of identified "good features" required to merge ecal clusters

    float           m_goodFeaturesMaxFitDotProduct;     ///< Max dot product between cluster fit directions for good feature
    float           m_goodFeaturesMaxFitApproach;       ///< Max distance of closest approach between cluster fit results for good feature
    unsigned int    m_goodFeaturesMaxLayerDifference;   ///< Max difference between cluster outer pseudo layers for good feature
    float           m_goodFeaturesMinMipFraction;       ///< Min cluster mip fraction for good feature
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *LoopingTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new LoopingTracksAlgorithm();
}

#endif // #ifndef LOOPING_TRACKS_ALGORITHM_H
