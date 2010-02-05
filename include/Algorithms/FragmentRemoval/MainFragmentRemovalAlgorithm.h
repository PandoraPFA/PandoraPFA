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
     *  @param  isFirstPass
     *  @param  affectedClusters
     *  @param  clusterContactMap
     */
    StatusCode GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters, ClusterContactMap &clusterContactMap) const;

    /**
     *  @brief  
     * 
     *  @param  clusterContact
     */
    bool PassesClusterContactCuts(const ClusterContact &clusterContact) const;

    /**
     *  @brief  
     * 
     *  @param  clusterContactMap
     *  @param  pBestParentCluster
     *  @param  pBestDaughterCluster
     */
    StatusCode GetClusterMergingCandidates(const ClusterContactMap &clusterContactMap, Cluster *&pBestParentCluster,
        Cluster *&pBestDaughterCluster) const;

    /**
     *  @brief  
     * 
     *  @param  pDaughterCluster
     *  @param  clusterContactVector
     *  @param  globalDeltaChi2
     * 
     *  @return boolean
     */
    bool PassesPreselection(Cluster *const pDaughterCluster, const ClusterContactVector &clusterContactVector, float &globalDeltaChi2) const;

    /**
     *  @brief  
     * 
     *  @param  clusterContact
     * 
     *  @return
     */
    float GetTotalEvidenceForMerge(const ClusterContact &clusterContact) const;

    /**
     *  @brief  
     * 
     *  @param  pDaughterCluster
     *  @param  clusterContact
     *  @param  correctionLayer
     *  @param  globalDeltaChi2
     * 
     *  @return
     */
    float GetRequiredEvidenceForMerge(Cluster *const pDaughterCluster, const ClusterContact &clusterContact, const PseudoLayer correctionLayer,
        const float globalDeltaChi2) const;

    /**
     *  @brief  
     * 
     *  @param  pDaughterCluster
     * 
     *  @return
     */
    PseudoLayer GetClusterCorrectionLayer(const Cluster *const pDaughterCluster) const;

    /**
     *  @brief  
     * 
     *  @param  clusterEnergy
     *  @param  trackEnergy
     * 
     *  @return 
     */
    float GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy) const;

    /**
     *  @brief  
     * 
     *  @param  clusterContactMap
     *  @param  pBestParentCluster
     *  @param  pBestDaughterCluster
     *  @param  affectedClusters
     */
    StatusCode GetAffectedClusters(const ClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
        Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const;

    float           m_contactWeight;                ///< 
    float           m_coneWeight;                   ///< 
    float           m_distanceWeight;               ///< 
    float           m_trackExtrapolationWeight;     ///< 

    float           m_chi2Base;                     ///< 
    float           m_globalChi2Penalty;            ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline Algorithm *MainFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new MainFragmentRemovalAlgorithm();
}

#endif // #ifndef FRAGMENT_REMOVAL_ALGORITHM_H
