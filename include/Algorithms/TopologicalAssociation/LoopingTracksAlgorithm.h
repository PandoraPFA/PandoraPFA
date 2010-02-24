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

using namespace pandora;

/**
 *  @brief  LoopingTracksAlgorithm class
 */
class LoopingTracksAlgorithm : public Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    typedef ClusterHelper::ClusterFitResult ClusterFitResult;

    /**
     *  @brief  ClusterAndFitResultPair class
     */
    class ClusterAndFitResultPair
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pCluster the address of the cluster
         *  @param  clusterFitResult the cluster fit result
         */
        ClusterAndFitResultPair(Cluster *const pCluster, const ClusterFitResult &clusterFitResult);

        /**
         *  @brief  Get the address of the cluster
         * 
         *  @return The address of the cluster
         */
        Cluster *GetCluster() const;

        /**
         *  @brief  Get the cluster fit result
         * 
         *  @return The cluster fit result
         */
        const ClusterFitResult &GetClusterFitResult() const;
    private:
        Cluster                    *m_pCluster;             ///< Address of the cluster
        const ClusterFitResult      m_clusterFitResult;     ///< The cluster fit result
    };

    typedef std::vector<ClusterAndFitResultPair*> ClusterFitResultVector;

    /**
     *  @brief  Get the closest distance between hits in the outermost pseudolayer of two clusters
     * 
     *  @param  pClusterI address of first cluster
     *  @param  pClusterJ address of second cluster
     * 
     *  @return the closest distance between outer layer hits
     */
    float GetClosestDistanceBetweenOuterLayerHits(const Cluster *const pClusterI, const Cluster *const pClusterJ) const;

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

inline Algorithm *LoopingTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new LoopingTracksAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline LoopingTracksAlgorithm::ClusterAndFitResultPair::ClusterAndFitResultPair(Cluster *const pCluster, const ClusterFitResult &clusterFitResult) :
    m_pCluster(pCluster),
    m_clusterFitResult(clusterFitResult)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *LoopingTracksAlgorithm::ClusterAndFitResultPair::GetCluster() const
{
    return m_pCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ClusterHelper::ClusterFitResult &LoopingTracksAlgorithm::ClusterAndFitResultPair::GetClusterFitResult() const
{
    return m_clusterFitResult;
}

#endif // #ifndef LOOPING_TRACKS_ALGORITHM_H
