/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ShowerMipMergingAlgorithm.h
 * 
 *  @brief  Header file for the shower mip merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef SHOWER_MIP_MERGING_ALGORITHM_H
#define SHOWER_MIP_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ShowerMipMergingAlgorithm class
 */
class ShowerMipMergingAlgorithm : public pandora::Algorithm
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

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The max all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_nPointsToFit;                     ///< The number of occupied pseudolayers to use in fit to the end of the cluster
    unsigned int    m_minCaloHitsPerCluster;            ///< The min number of calo hits a cluster must contain if it is to be merged

    float           m_mipFractionCut;                   ///< The min mip fraction required to identify a cluster as mip-like
    float           m_fitToAllHitsRmsCut;               ///< The max rms value (for the fit to all hits) to identify a cluster as mip-like

    float           m_maxCentroidDifference;            ///< The max difference (for merging) between cluster outer and inner centroids

    unsigned int    m_nFitProjectionLayers;             ///< The number of layers to project cluster fit for comparison with second cluster
    float           m_maxDistanceToClosestHit;          ///< The max distance between projected cluster fit and hits in second cluster
    float           m_maxDistanceToClosestCentroid;     ///< The max distance between projected cluster fit and second cluster centroid
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ShowerMipMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new ShowerMipMergingAlgorithm();
}

#endif // #ifndef SHOWER_MIP_MERGING_ALGORITHM_H
