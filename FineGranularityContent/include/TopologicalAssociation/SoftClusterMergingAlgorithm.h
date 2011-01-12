/**
 *  @file   PandoraPFANew/Algorithms/include/TopologicalAssociation/SoftClusterMergingAlgorithm.h
 * 
 *  @brief  Header file for the soft cluster merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef SOFT_CLUSTER_MERGING_ALGORITHM_H
#define SOFT_CLUSTER_MERGING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  SoftClusterMergingAlgorithm class
 */
class SoftClusterMergingAlgorithm : public pandora::Algorithm
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
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  Whether a daughter candidate cluster is a "soft" cluster
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     * 
     *  @return boolean
     */
    bool IsSoftCluster(pandora::Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Whether a soft daughter candidate cluster can be merged with a parent a specified distance away
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  closestDistance the closest distance between a pair of hits in the daughter and parent candidate clusters
     * 
     *  @return boolean
     */
    bool CanMergeSoftCluster(const pandora::Cluster *const pDaughterCluster, const float closestDistance) const;

    std::string     m_trackClusterAssociationAlgName;       ///< The name of the track-cluster association algorithm to run

    unsigned int    m_maxHitsInSoftCluster;                 ///< The maximum number of hits in a soft cluster
    unsigned int    m_maxLayersSpannedBySoftCluster;        ///< The maximum number of pseudo layers spanned by a soft cluster
    float           m_maxHadEnergyForSoftClusterNoTrack;    ///< The maximum hadronic energy for a soft cluster with no track associations

    float           m_minClusterHadEnergy;                  ///< The minimum hadronic energy in an "acceptable" cluster
    float           m_minClusterEMEnergy;                   ///< The minimum electromagnetic energy in an "acceptable" cluster

    float           m_minCosOpeningAngle;                   ///< Min cosine of angle between cluster directions to consider merging clusters
    unsigned int    m_minHitsInCluster;                     ///< The minimum number of hits in an "acceptable" cluster

    float           m_closestDistanceCut0;                  ///< Single cut: max distance at which can automatically merge parent/daughter

    float           m_closestDistanceCut1;                  ///< 1st pair of cuts: max distance between daughter and parent clusters
    unsigned int    m_innerLayerCut1;                       ///< 1st pair of cuts: max daughter inner pseudo layer
    float           m_closestDistanceCut2;                  ///< 2nd pair of cuts: max distance between daughter and parent clusters
    unsigned int    m_innerLayerCut2;                       ///< 2nd pair of cuts: max daughter inner pseudo layer

    float           m_maxClusterDistanceFine;               ///< Fine granularity max distance between parent and daughter clusters
    float           m_maxClusterDistanceCoarse;             ///< Coarse granularity max distance between parent and daughter clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *SoftClusterMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new SoftClusterMergingAlgorithm();
}

#endif // #ifndef SOFT_CLUSTER_MERGING_ALGORITHM_H
