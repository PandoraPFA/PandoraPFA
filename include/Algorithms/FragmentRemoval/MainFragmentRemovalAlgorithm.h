/**
 *  @file   PandoraPFANew/include/Algorithms/FragmentRemoval/MainFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the main fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef MAIN_FRAGMENT_REMOVAL_ALGORITHM_H
#define MAIN_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"
#include "Algorithms/FragmentRemovalAlgorithm.h"

using namespace pandora;

/**
 *  @brief  MainFragmentRemovalAlgorithm class
 */
class MainFragmentRemovalAlgorithm : public Algorithm
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

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     */
    StatusCode GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters, ClusterContactMap &clusterContactMap,
        const Cluster *const pBestParentCluster, const Cluster *const pBestDaughterCluster) const;

    /**
     *  @brief  
     * 
     *  @param  
     */
    bool PassesClusterContactCuts(const ClusterContact &clusterContact) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     */
    StatusCode GetClusterMergingCandidates(const ClusterContactMap &clusterContactMap, Cluster *&pBestParentCluster,
        Cluster *&pBestDaughterCluster) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     * 
     *  @return boolean
     */
    bool PassesPreselection(Cluster *const pDaughterCluster, const ClusterContactVector &clusterContactVector) const;

    /**
     *  @brief  
     * 
     *  @param  
     * 
     *  @return
     */
    float GetTotalEvidenceForMerge(const ClusterContact &clusterContact) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     * 
     *  @return
     */
    float GetRequiredEvidenceForMerge(const float daughterClusterEnergy, const PseudoLayer correctionLayer, const ClusterContact &clusterContact) const;

    /**
     *  @brief  
     * 
     *  @param  
     * 
     *  @return
     */
    PseudoLayer GetClusterCorrectionLayer(const Cluster *const pDaughterCluster) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     * 
     *  @return 
     */
    float GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     */
    StatusCode GetAffectedClusters(const ClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
        Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const;

    float           m_contactWeight;                ///< 
    float           m_coneWeight;                   ///< 
    float           m_distanceWeight;               ///< 
    float           m_trackExtrapolationWeight;     ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline Algorithm *MainFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new MainFragmentRemovalAlgorithm();
}

#endif // #ifndef FRAGMENT_REMOVAL_ALGORITHM_H
