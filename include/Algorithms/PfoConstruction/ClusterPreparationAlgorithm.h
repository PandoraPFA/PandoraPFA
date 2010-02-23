/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/ClusterPreparationAlgorithm.h
 * 
 *  @brief  Header file for the cluster preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_PREPARATION_ALGORITHM_H
#define CLUSTER_PREPARATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ClusterPreparationAlgorithm class
 */
class ClusterPreparationAlgorithm : public pandora::Algorithm
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

    /**
     *  @brief  Correct cluster energies by searching for constituent calo hits with anomalously high energy. Corrections
     *          are made by examining the energy in adjacent layers of the cluster.
     */
    StatusCode CleanClusters() const;

    /**
     *  @brief  Get the sum of the hadronic energies of all calo hits in a specified layer of an ordered calo hit list
     * 
     *  @param  orderedCaloHitList the ordered calo hit list
     *  @param  pseudoLayer the specified pseudolayer
     */
    float GetHadronicEnergyInLayer(const pandora::OrderedCaloHitList &orderedCaloHitList, const pandora::PseudoLayer pseudoLayer) const;

    /**
     *  @brief  Correct cluster energies by searching for clusters with anomalously high mip energies per constituent calo hit.
     *          Corrections are made by scaling back the mean number of mips per calo hit.
     */
    StatusCode ScaleHotHadronEnergy() const;

    /**
     *  @brief  Perform last minute identification of photon clusters.
     */
    StatusCode IdentifyPhotons() const;

    float           m_minCleanHitEnergy;                ///< Min calo hit hadronic energy to consider cleaning hit/cluster
    float           m_minCleanHitEnergyFraction;        ///< Min fraction of cluster energy represented by hit to consider cleaning
    float           m_minCleanCorrectedHitEnergy;       ///< Min value of new hit hadronic energy estimate after cleaning

    unsigned int    m_minHitsForHotHadron;              ///< Min number of hits in a hot hadron candidate cluster
    unsigned int    m_maxHitsForHotHadron;              ///< Max number of hits in a hot hadron candidate cluster

    unsigned int    m_hotHadronInnerLayerCut;           ///< Cut 1 of 3 (must fail all for rejection): Min inner pseudo layer for hot hadron
    float           m_hotHadronMipFractionCut;          ///< Cut 2 of 3 (must fail all for rejection): Min mip fraction for hot hadron
    unsigned int    m_hotHadronNHitsCut;                ///< Cut 3 of 3 (must fail all for rejection): Max number of hits for hot hadron

    float           m_hotHadronMipsPerHit;              ///< Min number of mips per hit for a hot hadron cluster
    float           m_scaledHotHadronMipsPerHit;        ///< Scale factor (new mips per hit value) to correct hot hadron energies

    bool            m_shouldPerformPhotonId;            ///< Whether algorithm should perform cluster photon id

    float           m_photonIdMipCut_0;                 ///< Default cluster mip fraction cut for photon id
    float           m_photonIdMipCutEnergy_1;           ///< Energy above which mip fraction cut value 1 is applied
    float           m_photonIdMipCut_1;                 ///< Cluster mip fraction cut value 1
    float           m_photonIdMipCutEnergy_2;           ///< Energy above which mip fraction cut value 2 is applied
    float           m_photonIdMipCut_2;                 ///< Cluster mip fraction cut value 2
    float           m_photonIdMipCutEnergy_3;           ///< Energy above which mip fraction cut value 3 is applied
    float           m_photonIdMipCut_3;                 ///< Cluster mip fraction cut value 3
    float           m_photonIdMipCutEnergy_4;           ///< Energy above which mip fraction cut value 4 is applied
    float           m_photonIdMipCut_4;                 ///< Cluster mip fraction cut value 4

    float           m_photonIdDCosRCutEnergy;           ///< Energy at which photon id cut (on cluster fit result dCosR) changes
    float           m_photonIdDCosRLowECut;             ///< Low energy cut on cluster fit result dCosR
    float           m_photonIdDCosRHighECut;            ///< High energy cut on cluster fit result dCosR

    float           m_photonIdRmsCutEnergy;             ///< Energy at which photon id cut (on cluster fit result rms) changes
    float           m_photonIdRmsLowECut;               ///< Low energy cut on cluster fit result rms
    float           m_photonIdRmsHighECut;              ///< High energy cut on cluster fit result rms

    float           m_photonIdEndCapZSeparation;        ///< To determine whether cluster strikes endcap: max z distance to inner centroid
    float           m_photonIdRadiationLengthsCut;      ///< Max number of radiation lengths before cluster inner layer

    float           m_photonIdShowerStartCut1_0;        ///< Default value for shower start cut 1: min number of layers from inner layer
    float           m_photonIdShowerStartCut1Energy_1;  ///< Energy above which shower start cut 1 value 1 is applied
    float           m_photonIdShowerStartCut1_1;        ///< Shower start cut 1 value 1
    float           m_photonIdShowerStartCut1Energy_2;  ///< Energy above which shower start cut 1 value 2 is applied
    float           m_photonIdShowerStartCut1_2;        ///< Shower start cut 1 value 2

    float           m_photonIdShowerStartCut2;          ///< Shower start cut 2: max number of layers from inner layer

    float           m_photonIdLayer90Cut1;              ///< Cut on cluster layer 90: min number of layers from inner layer

    float           m_photonIdLayer90Cut2Energy;        ///< Energy at which photon id cut (on cluster layer 90) changes
    float           m_photonIdLayer90LowECut2;          ///< Low energy cut on max number of layers between layer 90 and inner layer
    float           m_photonIdLayer90HighECut2;         ///< High energy cut on max number of layers between layer 90 and inner layer

    int             m_photonIdLayer90MaxLayersFromECal; ///< Max number of layers between cluster layer 90 and last ecal layer
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusterPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new ClusterPreparationAlgorithm();
}

#endif // #ifndef CLUSTER_PREPARATION_ALGORITHM_H
