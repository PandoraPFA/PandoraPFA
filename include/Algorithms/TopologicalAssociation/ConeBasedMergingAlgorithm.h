/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ConeBasedMergingAlgorithm.h
 * 
 *  @brief  Header file for the cone based merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef CONE_BASED_MERGING_ALGORITHM_H
#define CONE_BASED_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ConeBasedMergingAlgorithm class
 */
class ConeBasedMergingAlgorithm : public pandora::Algorithm
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

    typedef pandora::ClusterHelper::ClusterFitResult ClusterFitResult;
    typedef std::map<pandora::Cluster *, ClusterFitResult> ClusterFitResultMap;

    /**
     *  @brief  Prepare clusters for the cone based merging algorithm, applying pre-selection cuts and performing a mip fit
     *          to candidate parent clusters.
     * 
     *  @param  daughterVector to receive the daughter cluster vector
     *  @param  parentFitResultMap to receive the parent cluster fit result map
     */
    StatusCode PrepareClusters(pandora::ClusterVector &daughterVector, ClusterFitResultMap &parentFitResultMap) const;

    /**
     *  @brief  Get the fraction of hits in a daughter candidate cluster that are contained in a cluster defined by a mip fit
     *          to the parent candidate cluster
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  parentMipFitResult the mip fit result for the parent candidate cluster
     *  @param  coneCosineHalfAngle the cone cosine half angle
     * 
     *  @return the fraction of the daughter cluster hits contained in the cone
     */
    float GetFractionInCone(const pandora::Cluster *const pDaughterCluster, const ClusterFitResult &parentMipFitResult,
        const float coneCosineHalfAngle) const;

    /**
     *  @brief  Sort clusters by ascending inner layer, and by descending number of calo hits within a layer
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortClustersByInnerLayer(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    std::string     m_trackClusterAssociationAlgName;   ///< The name of the track-cluster association algorithm to run

    float           m_canMergeMinMipFraction;           ///< The minimum mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The maximum all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minCaloHitsPerCluster;            ///< // TODO under construction
    unsigned int    m_minLayersToShowerMax;             ///< 

    float           m_minConeFraction;                  ///< 
    float           m_maxInnerLayerSeparation;          ///< 
    float           m_maxInnerLayerSeparationNoTrack;   ///< 
    float           m_coneCosineHalfAngle;              ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ConeBasedMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new ConeBasedMergingAlgorithm();
}

#endif // #ifndef CONE_BASED_MERGING_ALGORITHM_H
