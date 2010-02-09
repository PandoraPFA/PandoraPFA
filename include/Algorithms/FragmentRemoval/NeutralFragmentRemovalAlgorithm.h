/**
 *  @file   PandoraPFANew/include/Algorithms/FragmentRemoval/NeutralFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the neutral fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H
#define NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

#include "Helpers/FragmentRemovalHelper.h"

using namespace pandora;

/**
 *  @brief  NeutralFragmentRemovalAlgorithm class
 */
class NeutralFragmentRemovalAlgorithm : public Algorithm
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
     *  @brief  Get cluster contact map, linking each daughter candidate cluster to a list of parent candidates and describing
     *          the proximity/contact between each pairing
     * 
     *  @param  isFirstPass whether this is the first call to GetClusterContactMap
     *  @param  affectedClusters list of those clusters affected by previous cluster merging, for which contact details must be updated
     *  @param  clusterContactMap to receive the populated cluster contact map
     */
    StatusCode GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters, ClusterContactMap &clusterContactMap) const;

    /**
     *  @brief  Whether candidate daughter cluster can be considered as photon-like
     * 
     *  @param  pDaughterCluster address of the candidate daughter cluster
     * 
     *  @return boolean
     */
    bool IsPhotonLike(Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Whether candidate parent and daughter clusters are sufficiently in contact to warrant further investigation
     * 
     *  @param  clusterContact
     * 
     *  @return boolean
     */
    bool PassesClusterContactCuts(const ClusterContact &clusterContact) const;

    /**
     *  @brief  Find the best candidate parent and daughter clusters for fragment removal merging
     * 
     *  @param  clusterContactMap the populated cluster contact map
     *  @param  pBestParentCluster to receive the address of the best parent cluster candidate
     *  @param  pBestDaughterCluster to receive the address of the best daughter cluster candidate
     */
    StatusCode GetClusterMergingCandidates(const ClusterContactMap &clusterContactMap, Cluster *&pBestParentCluster,
        Cluster *&pBestDaughterCluster) const;

    /**
     *  @brief  Get a measure of the evidence for merging the parent and daughter candidate clusters
     * 
     *  @param  clusterContact the cluster contact details for parent/daughter candidate merge
     * 
     *  @return the evidence
     */
    float GetEvidenceForMerge(const ClusterContact &clusterContact) const;

    /**
     *  @brief  Get the list of clusters for which cluster contact information will be affected by a specified cluster merge
     * 
     *  @param  clusterContactMap the cluster contact map
     *  @param  pBestParentCluster address of the parent cluster to be merged
     *  @param  pBestDaughterCluster address of the daughter cluster to be merged
     *  @param  affectedClusters to receive the list of affected clusters
     */
    StatusCode GetAffectedClusters(const ClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
        Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const;

    unsigned int        m_nMaxPasses;                               ///< 

    unsigned int        m_minDaughterCaloHits;                      ///< 
    float               m_minDaughterHadronicEnergy;                ///< 

    float               m_minEvidence;                              ///< 

    unsigned int        m_photonLikeMinInnerLayer;                  ///< 
    float               m_photonLikeMinDCosR;                       ///< 
    float               m_photonLikeMaxShowerStart;                 ///< 
    float               m_photonLikeMaxPhotonFraction;              ///< 

    float               m_contactCutMaxDistance;                    ///< 
    unsigned int        m_contactCutNLayers;                        ///< 
    float               m_contactCutConeFraction1;                  ///< 
    float               m_contactCutCloseHitFraction1;              ///< 
    float               m_contactCutCloseHitFraction2;              ///< 
    float               m_contactCutNearbyDistance;                 ///< 
    float               m_contactCutNearbyCloseHitFraction2;        ///< 

    unsigned int        m_contactEvidenceNLayers1;                  ///< 
    unsigned int        m_contactEvidenceNLayers2;                  ///< 
    unsigned int        m_contactEvidenceNLayers3;                  ///< 
    float               m_contactEvidence1;                         ///< 
    float               m_contactEvidence2;                         ///< 
    float               m_contactEvidence3;                         ///< 

    float               m_coneEvidenceFraction1;                    ///< 
    float               m_coneEvidenceECalMultiplier;               ///< 

    float               m_distanceEvidence1;                        ///< 
    float               m_distanceEvidence1d;                       ///< 
    float               m_distanceEvidenceCloseFraction1Multiplier; ///< 
    float               m_distanceEvidenceCloseFraction2Multiplier; ///< 

    float               m_contactWeight;                            ///< 
    float               m_coneWeight;                               ///< 
    float               m_distanceWeight;                           ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *NeutralFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new NeutralFragmentRemovalAlgorithm();
}

#endif // #ifndef NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H
