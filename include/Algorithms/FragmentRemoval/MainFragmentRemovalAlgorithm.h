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

#include "Helpers/FragmentRemovalHelper.h"

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
     *  @brief  Get cluster contact map, linking each daughter candidate cluster to a list of parent candidates and describing
     *          the proximity/contact between each pairing
     * 
     *  @param  isFirstPass whether this is the first call to GetClusterContactMap
     *  @param  affectedClusters list of those clusters affected by previous cluster merging, for which contact details must be updated
     *  @param  clusterContactMap to receive the populated cluster contact map
     */
    StatusCode GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters, ClusterContactMap &clusterContactMap) const;

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
     *  @brief  Whether the candidate parent and daughter clusters pass quick preselection for fragment removal merging
     * 
     *  @param  pDaughterCluster address of the daughter cluster candidate
     *  @param  clusterContactVector list cluster contact details for the given daughter cluster
     *  @param  globalDeltaChi2 to receive global delta chi2, indicating whether daughter cluster would be better merged
     *          with group of all nearby clusters
     * 
     *  @return boolean
     */
    bool PassesPreselection(Cluster *const pDaughterCluster, const ClusterContactVector &clusterContactVector, float &globalDeltaChi2) const;

    /**
     *  @brief  Get a measure of the total evidence for merging the parent and daughter candidate clusters
     * 
     *  @param  clusterContact the cluster contact details for parent/daughter candidate merge
     * 
     *  @return the total evidence
     */
    float GetTotalEvidenceForMerge(const ClusterContact &clusterContact) const;

    /**
     *  @brief  Get the required evidence for merging the parent and daughter candidate clusters
     * 
     *  @param  pDaughterCluster address of the daughter cluster candidate
     *  @param  clusterContact the cluster contact details for parent/daughter candidate merge
     *  @param  correctionLayer the daughter cluster correction layer
     *  @param  globalDeltaChi2 global delta chi2, indicating whether daughter cluster would be better merged
     *          with group of all nearby clusters
     * 
     *  @return the required evidence
     */
    float GetRequiredEvidenceForMerge(Cluster *const pDaughterCluster, const ClusterContact &clusterContact, const PseudoLayer correctionLayer,
        const float globalDeltaChi2) const;

    /**
     *  @brief  Get the cluster correction layer. Working from innermost to outermost layer, the correction layer is that in which:
     *          i)  the number of hit-layers passes m_correctionLayerNHitLayers, or
     *          ii) the total calo hit hadronic energy passes m_correctionLayerHadronicEnergy
     * 
     *  @param  pDaughterCluster address of the daughter cluster candidate
     * 
     *  @return the cluster correction layer
     */
    PseudoLayer GetClusterCorrectionLayer(const Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Get measure of the compatibility between a cluster and its associated tracks
     * 
     *  @param  clusterEnergy the cluster energy
     *  @param  trackEnergy the sum of the energies of the associated tracks
     * 
     *  @return the track-cluster compatibility
     */
    float GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy) const;

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

    unsigned int        m_minDaughterCaloHits;                      ///< 
    float               m_minDaughterHadronicEnergy;                ///< 

    float               m_contactCutMaxDistance;                    ///< 

    unsigned int        m_contactCutNLayers;                        ///< 
    float               m_contactCutConeFraction1;                  ///< 
    float               m_contactCutCloseHitFraction1;              ///< 
    float               m_contactCutCloseHitFraction2;              ///< 
    float               m_contactCutMeanDistanceToHelix;            ///< 
    float               m_contactCutClosestDistanceToHelix;         ///< 
    float               m_contactCutNearECalDistance;               ///< 
    unsigned int        m_contactCutLayersFromECal;                 ///< 

    float               m_maxChi2;                                  ///< 
    float               m_maxGlobalChi2;                            ///< 
    float               m_chi2Base;                                 ///< 
    float               m_globalChi2Penalty;                        ///< 

    unsigned int        m_correctionLayerNHitLayers;                ///< 
    float               m_correctionLayerHadronicEnergy;            ///< 

    unsigned int        m_contactEvidenceNLayers1;                  ///< 
    unsigned int        m_contactEvidenceNLayers2;                  ///< 
    unsigned int        m_contactEvidenceNLayers3;                  ///< 
    float               m_contactEvidence1;                         ///< 
    float               m_contactEvidence2;                         ///< 
    float               m_contactEvidence3;                         ///< 

    float               m_coneEvidenceFraction1;                    ///< 
    float               m_coneEvidenceECalMultiplier;               ///< 

    float               m_closestTrackEvidence1;                    ///< 
    float               m_closestTrackEvidence1d;                   ///< 
    float               m_closestTrackEvidence2;                    ///< 
    float               m_closestTrackEvidence2d;                   ///< 
    float               m_meanTrackEvidence1;                       ///< 
    float               m_meanTrackEvidence1d;                      ///< 
    float               m_meanTrackEvidence2;                       ///< 
    float               m_meanTrackEvidence2d;                      ///< 
    float               m_distanceEvidence1;                        ///< 
    float               m_distanceEvidence1d;                       ///< 
    float               m_distanceEvidenceCloseFraction1Multiplier; ///< 
    float               m_distanceEvidenceCloseFraction2Multiplier; ///< 

    float               m_contactWeight;                            ///< 
    float               m_coneWeight;                               ///< 
    float               m_distanceWeight;                           ///< 
    float               m_trackExtrapolationWeight;                 ///< 

    float               m_layerCorrection1;                         ///< 
    float               m_layerCorrection2;                         ///< 
    float               m_layerCorrection3;                         ///< 
    float               m_layerCorrection4;                         ///< 
    float               m_layerCorrection5;                         ///< 
    float               m_layerCorrection6;                         ///< 
    float               m_layerCorrectionLayerSpan;                 ///< 
    float               m_layerCorrectionMinInnerLayer;             ///< 
    float               m_layerCorrectionLayersFromECal;            ///< 

    float               m_energyCorrectionThreshold;                ///< 

    float               m_lowEnergyCorrectionThreshold;             ///< 
    unsigned int        m_lowEnergyCorrectionNHitLayers1;           ///< 
    unsigned int        m_lowEnergyCorrectionNHitLayers2;           ///< 
    float               m_lowEnergyCorrection1;                     ///< 
    float               m_lowEnergyCorrection2;                     ///< 
    float               m_lowEnergyCorrection3;                     ///< 

    float               m_angularCorrectionOffset;                  ///< 
    float               m_angularCorrectionConstant;                ///< 
    float               m_angularCorrectionGradient;                ///< 

    float               m_photonCorrectionEnergy1;                  ///< 
    float               m_photonCorrectionEnergy2;                  ///< 
    float               m_photonCorrectionEnergy3;                  ///< 
    float               m_photonCorrectionShowerStart1;             ///< 
    float               m_photonCorrectionShowerStart2;             ///< 
    float               m_photonCorrectionPhotonFraction1;          ///< 
    float               m_photonCorrectionPhotonFraction2;          ///< 
    float               m_photonCorrection1;                        ///< 
    float               m_photonCorrection2;                        ///< 
    float               m_photonCorrection3;                        ///< 
    float               m_photonCorrection4;                        ///< 
    float               m_photonCorrection5;                        ///< 
    float               m_photonCorrection6;                        ///< 
    float               m_photonCorrection7;                        ///< 

    float               m_minRequiredEvidence;                      ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline Algorithm *MainFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new MainFragmentRemovalAlgorithm();
}

#endif // #ifndef FRAGMENT_REMOVAL_ALGORITHM_H
