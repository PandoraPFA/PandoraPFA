/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.h
 * 
 *  @brief  Header file for the broken tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef BROKEN_TRACKS_ALGORITHM_H
#define BROKEN_TRACKS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  BrokenTracksAlgorithm class
 */
class BrokenTracksAlgorithm : public pandora::Algorithm
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
     *  @brief  Whether a cluster can be merged with another (simple criteria)
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    bool CanMergeCluster(pandora::Cluster *const pCluster) const;

    /**
     *  @brief  Whether a specified cluster position is outside of the ecal region
     * 
     *  @param  clusterPosition the specified cluster position
     * 
     *  @return boolean
     */
    bool IsOutsideECal(const pandora::CartesianVector &clusterPosition) const;

    unsigned int    m_nStartLayersToFit;                ///< The number of occupied pseudolayers to use in fit to the start of the cluster
    unsigned int    m_nEndLayersToFit;                  ///< The number of occupied pseudolayers to use in fit to the end of the cluster
    float           m_maxFitRms;                        ///< The max value of the start/end fit rms for cluster to be considered

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The max all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_maxLayerDifference;               ///< The max difference (for merging) between cluster outer and inner pseudo layers
    float           m_maxCentroidDifference;            ///< The max difference (for merging) between cluster outer and inner centroids
    float           m_fitDirectionDotProductCut;        ///< Cut on max value of dot product between cluster fit directions

    float           m_trackMergeCutEcal;                ///< ECal cut on closest distance of approach between two cluster fit results
    float           m_trackMergeCutHcal;                ///< HCal cut on closest distance of approach between two cluster fit results

    float           m_trackMergePerpCutEcal;            ///< ECal cut on perp. distance between fit directions and centroid difference
    float           m_trackMergePerpCutHcal;            ///< HCal cut on perp. distance between fit directions and centroid difference
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *BrokenTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new BrokenTracksAlgorithm();
}

#endif // #ifndef BROKEN_TRACKS_ALGORITHM_H
