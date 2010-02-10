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
     *  @brief  Get the list of clusters for which cluster contact information will be affected by a specified cluster merge
     * 
     *  @param  clusterContactMap the cluster contact map
     *  @param  pBestParentCluster address of the parent cluster to be merged
     *  @param  pBestDaughterCluster address of the daughter cluster to be merged
     *  @param  affectedClusters to receive the list of affected clusters
     */
    StatusCode GetAffectedClusters(const ClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
        Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const;

    unsigned int        m_minDaughterCaloHits;                      ///< Min number of calo hits in daughter candidate clusters
    float               m_minDaughterHadronicEnergy;                ///< Min hadronic energy for daughter candidate clusters

    float               m_contactCutMaxDistance;                    ///< Max distance between closest hits to store cluster contact info
    unsigned int        m_contactCutNLayers;                        ///< Number of contact layers to store cluster contact info
    float               m_contactCutConeFraction1;                  ///< Cone fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction1;              ///< Close hit fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction2;              ///< Close hit fraction 2 value to store cluster contact info
    float               m_contactCutMeanDistanceToHelix;            ///< Mean distance to helix value to store cluster contact info
    float               m_contactCutClosestDistanceToHelix;         ///< Closest distance to helix value to store cluster contact info
    unsigned int        m_contactCutLayersFromECal;                 ///< Inner layer "n" layers below ecal-end marks cluster as "near ecal"
    float               m_contactCutNearECalDistance;               ///< Hit separation for "near ecal" clusters to store contact info

    float               m_maxChi2;                                  ///< Pre-selection: new chi2 value to allow cluster merging
    float               m_maxGlobalChi2;                            ///< Pre-selection: new global chi2 value to allow cluster merging
    float               m_chi2Base;                                 ///< Required evidence: min contribution from change in chi2
    float               m_globalChi2Penalty;                        ///< Required evidence penalty for using global chi2 measure

    unsigned int        m_correctionLayerNHitLayers;                ///< Number of hit-layers passed to identify correction layer
    float               m_correctionLayerHadronicEnergy;            ///< Total hadronic energy sum passed to identify correction layer

    unsigned int        m_contactEvidenceNLayers1;                  ///< Contact evidence n layers cut 1
    unsigned int        m_contactEvidenceNLayers2;                  ///< Contact evidence n layers cut 2
    unsigned int        m_contactEvidenceNLayers3;                  ///< Contact evidence n layers cut 3
    float               m_contactEvidence1;                         ///< Contact evidence contribution 1
    float               m_contactEvidence2;                         ///< Contact evidence contribution 2
    float               m_contactEvidence3;                         ///< Contact evidence contribution 3

    float               m_coneEvidenceFraction1;                    ///< Cone fraction 1 value required for cone evidence contribution
    float               m_coneEvidenceECalMultiplier;               ///< Cone evidence multiplier for daughter clusters starting in ecal

    float               m_closestTrackEvidence1;                    ///< Offset for closest distance to helix evidence contribution 1
    float               m_closestTrackEvidence1d;                   ///< Denominator for closest distance to helix evidence contribution 1
    float               m_closestTrackEvidence2;                    ///< Offset for closest distance to helix evidence contribution 2
    float               m_closestTrackEvidence2d;                   ///< Denominator for closest distance to helix evidence contribution 2
    float               m_meanTrackEvidence1;                       ///< Offset for mean distance to helix evidence contribution 1
    float               m_meanTrackEvidence1d;                      ///< Denominator for mean distance to helix evidence contribution 1
    float               m_meanTrackEvidence2;                       ///< Offset for mean distance to helix evidence contribution 2
    float               m_meanTrackEvidence2d;                      ///< Denominator for mean distance to helix evidence contribution 1

    float               m_distanceEvidence1;                        ///< Offset for distance evidence contribution 1
    float               m_distanceEvidence1d;                       ///< Denominator for distance evidence contribution 1
    float               m_distanceEvidenceCloseFraction1Multiplier; ///< Distance evidence multiplier for close hit fraction 1
    float               m_distanceEvidenceCloseFraction2Multiplier; ///< Distance evidence multiplier for close hit fraction 2

    float               m_contactWeight;                            ///< Weight for layers in contact evidence
    float               m_coneWeight;                               ///< Weight for cone extrapolation evidence
    float               m_distanceWeight;                           ///< Weight for distance of closest approach evidence
    float               m_trackExtrapolationWeight;                 ///< Weight for track extrapolation evidence

    float               m_layerCorrection1;                         ///< Layer correction contribution 1
    float               m_layerCorrection2;                         ///< Layer correction contribution 2
    float               m_layerCorrection3;                         ///< Layer correction contribution 3
    float               m_layerCorrection4;                         ///< Layer correction contribution 4
    float               m_layerCorrection5;                         ///< Layer correction contribution 5
    float               m_layerCorrection6;                         ///< Layer correction contribution 6
    float               m_layerCorrectionLayerSpan;                 ///< Daughter layer span for layer correction contribution 5
    float               m_layerCorrectionMinInnerLayer;             ///< Daughter min inner layer for layer correction contribution 5
    float               m_layerCorrectionLayersFromECal;            ///< Daughter layers from ecal for layer correction contribution 6

    float               m_leavingCorrection;                        ///< Correction for clusters leaving calorimeters

    float               m_energyCorrectionThreshold;                ///< Energy correction threshold

    float               m_lowEnergyCorrectionThreshold;             ///< Low energy correction threshold
    unsigned int        m_lowEnergyCorrectionNHitLayers1;           ///< Number of hit layers for low energy correction contribution 1
    unsigned int        m_lowEnergyCorrectionNHitLayers2;           ///< Number of hit layers for low energy correction contribution 2
    float               m_lowEnergyCorrection1;                     ///< Low energy correction contribution 1
    float               m_lowEnergyCorrection2;                     ///< Low energy correction contribution 2
    float               m_lowEnergyCorrection3;                     ///< Low energy correction contribution 3

    float               m_angularCorrectionOffset;                  ///< Offset value for angular correction
    float               m_angularCorrectionConstant;                ///< Constant value for angular correction
    float               m_angularCorrectionGradient;                ///< Gradient value for angular correction

    float               m_photonCorrectionEnergy1;                  ///< Photon correction energy value 1
    float               m_photonCorrectionEnergy2;                  ///< Photon correction energy value 2
    float               m_photonCorrectionEnergy3;                  ///< Photon correction energy value 3
    float               m_photonCorrectionShowerStart1;             ///< Photon correction profile shower start value 1
    float               m_photonCorrectionShowerStart2;             ///< Photon correction profile shower start value 1
    float               m_photonCorrectionPhotonFraction1;          ///< Photon correction profile photon fraction value 1
    float               m_photonCorrectionPhotonFraction2;          ///< Photon correction profile photon fraction value 2
    float               m_photonCorrection1;                        ///< Photon correction contribution 1
    float               m_photonCorrection2;                        ///< Photon correction contribution 2
    float               m_photonCorrection3;                        ///< Photon correction contribution 3
    float               m_photonCorrection4;                        ///< Photon correction contribution 4
    float               m_photonCorrection5;                        ///< Photon correction contribution 5
    float               m_photonCorrection6;                        ///< Photon correction contribution 6
    float               m_photonCorrection7;                        ///< Photon correction contribution 7

    float               m_minRequiredEvidence;                      ///< Minimum required evidence to merge parent/daughter clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline Algorithm *MainFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new MainFragmentRemovalAlgorithm();
}

#endif // #ifndef FRAGMENT_REMOVAL_ALGORITHM_H
