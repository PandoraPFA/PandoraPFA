/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ShowerMipMerging2Algorithm.h
 * 
 *  @brief  Header file for the shower mip merging 2 algorithm class.
 * 
 *  $Log: $
 */
#ifndef SHOWER_MIP_MERGING_2_ALGORITHM_H
#define SHOWER_MIP_MERGING_2_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ShowerMipMerging2Algorithm class
 */
class ShowerMipMerging2Algorithm : public pandora::Algorithm
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

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The max all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minCaloHitsPerCluster;            ///< The min number of calo hits for cluster to be used as a parent cluster
    float           m_fitToAllHitsChi2Cut;              ///< The max all hit fit chi2 for cluster to be used as a parent cluster

    unsigned int    m_nPointsToFit;                     ///< The number of occupied pseudolayers to use in fit to the end of the cluster

    unsigned int    m_maxLayerDifference;               ///< The max difference (for merging) between cluster outer and inner pseudo layers
    float           m_maxCentroidDifference;            ///< The max difference (for merging) between cluster outer and inner centroids

    float           m_maxFitDirectionDotProduct;        ///< Max dot product between fit direction and centroid difference unit vectors

    float           m_perpendicularDistanceCutEcal;     ///< ECal cut on perp. distance between fit direction and centroid difference
    float           m_perpendicularDistanceCutHcal;     ///< HCal cut on perp. distance between fit direction and centroid difference
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ShowerMipMerging2Algorithm::Factory::CreateAlgorithm() const
{
    return new ShowerMipMerging2Algorithm();
}

#endif // #ifndef SHOWER_MIP_MERGING_2_ALGORITHM_H
