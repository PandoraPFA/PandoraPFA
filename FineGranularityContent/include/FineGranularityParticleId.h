/**
 *  @file   PandoraPFANew/FineGranularityContent/include/FineGranularityParticleId.h
 * 
 *  @brief  Header file for the fine granularity particle id class.
 * 
 *  $Log: $
 */
#ifndef FINE_GRANULARITY_PARTICLE_ID_H
#define FINE_GRANULARITY_PARTICLE_ID_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/StatusCodes.h"

#include "Xml/tinyxml.h"

/**
 *  @brief  FineGranularityParticleId class
 */
class FineGranularityParticleId
{
public:
    /**
     *  @brief  Whether a cluster is a candidate electromagnetic shower
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool FineGranularityEmShowerId(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Photon identification for use with fine granularity particle flow detectors
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool FineGranularityPhotonId(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Electron identification for use with fine granularity particle flow detectors
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool FineGranularityElectronId(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Muon identification for use with fine granularity particle flow detectors
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool FineGranularityMuonId(const pandora::Cluster *const pCluster);

    /**
     *  @brief  Read the fine granularity particle id settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

private:
    typedef std::pair<float, float> HitEnergyDistance;
    typedef std::vector<HitEnergyDistance> HitEnergyDistanceVector;

    /**
     *  @brief  Sort HitEnergyDistance objects by increasing distance
     * 
     *  @param  lhs the first hit energy distance pair
     *  @param  rhs the second hit energy distance pair
     */
    static bool SortHitsByDistance(const HitEnergyDistance &lhs, const HitEnergyDistance &rhs);

    static float        m_emShowerIdMipCut_0;               ///< Default cluster mip fraction cut for emshw id
    static float        m_emShowerIdMipCutEnergy_1;         ///< Energy above which mip fraction cut value 1 is applied
    static float        m_emShowerIdMipCut_1;               ///< Cluster mip fraction cut value 1
    static float        m_emShowerIdMipCutEnergy_2;         ///< Energy above which mip fraction cut value 2 is applied
    static float        m_emShowerIdMipCut_2;               ///< Cluster mip fraction cut value 2
    static float        m_emShowerIdMipCutEnergy_3;         ///< Energy above which mip fraction cut value 3 is applied
    static float        m_emShowerIdMipCut_3;               ///< Cluster mip fraction cut value 3
    static float        m_emShowerIdMipCutEnergy_4;         ///< Energy above which mip fraction cut value 4 is applied
    static float        m_emShowerIdMipCut_4;               ///< Cluster mip fraction cut value 4
    static float        m_emShowerIdDCosRCutEnergy;         ///< Energy at which emshw id cut (on cluster fit result dCosR) changes
    static float        m_emShowerIdDCosRLowECut;           ///< Low energy cut on cluster fit result dCosR
    static float        m_emShowerIdDCosRHighECut;          ///< High energy cut on cluster fit result dCosR
    static float        m_emShowerIdRmsCutEnergy;           ///< Energy at which emshw id cut (on cluster fit result rms) changes
    static float        m_emShowerIdRmsLowECut;             ///< Low energy cut on cluster fit result rms
    static float        m_emShowerIdRmsHighECut;            ///< High energy cut on cluster fit result rms
    static float        m_emShowerIdMinCosAngle;            ///< Min angular correction used to adjust radiation length measures
    static float        m_emShowerIdMaxInnerLayerRadLengths;///< Max number of radiation lengths before cluster inner layer
    static float        m_emShowerIdMinLayer90RadLengths;   ///< Min number of radiation lengths before cluster layer90
    static float        m_emShowerIdMaxLayer90RadLengths;   ///< Max number of radiation lengths before cluster layer90
    static float        m_emShowerIdMinShowerMaxRadLengths; ///< Min number of radiation lengths before cluster shower max layer
    static float        m_emShowerIdMaxShowerMaxRadLengths; ///< Max number of radiation lengths before cluster shower max layer
    static float        m_emShowerIdHighRadLengths;         ///< Max number of radiation lengths expected to be spanned by em shower
    static float        m_emShowerIdMaxHighRadLengthEnergyFraction;   ///< Max fraction of cluster energy above max expected radiation lengths
    static float        m_emShowerIdMaxRadial90;            ///< Max value of transverse profile radial90

    static unsigned int m_electronIdMaxInnerLayer;          ///< Max inner psuedo layer for fast electron id
    static float        m_electronIdMaxEnergy;              ///< Max electromagnetic energy for fast electron id
    static float        m_electronIdMaxProfileStart;        ///< Max shower profile start for fast electron id
    static float        m_electronIdMaxProfileDiscrepancy;  ///< Max shower profile discrepancy for fast electron id
    static float        m_electronIdProfileDiscrepancyForAutoId;    ///< Shower profile discrepancy for automatic fast electron selection
    static float        m_electronIdMaxResidualEOverP;      ///< Max absolute difference between unity and ratio em energy / track momentum

    static unsigned int m_muonIdMaxInnerLayer;              ///< Max inner psuedo layer for fast muon id
    static float        m_muonIdMinTrackEnergy;             ///< Min energy of associated track for fast muon id
    static float        m_muonIdMaxHCalHitEnergy;           ///< Max hadronic energy for an individual hcal hit (suppress fluctuations)
    static unsigned int m_muonIdMinECalLayers;              ///< Min number of ecal layers for fast muon id
    static unsigned int m_muonIdMinHCalLayers;              ///< Min number of hcal layers for fast muon id
    static bool         m_muonIdShouldPerformGapCheck;      ///< Whether to perform muon recovery gap checks
    static unsigned int m_muonIdMinHCalLayersForGapCheck;   ///< Min number of hcal layers to perform muon recovery gap check
    static unsigned int m_muonIdMinMuonHitsForGapCheck;     ///< Min number of muon hits to perform muon recovery gap check
    static float        m_muonIdECalEnergyCut0;             ///< Parameter 0 for ecal energy cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdECalEnergyCut1;             ///< Parameter 1 for ecal energy cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdHCalEnergyCut0;             ///< Parameter 0 for hcal energy cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdHCalEnergyCut1;             ///< Parameter 1 for hcal energy cut: cut = par0 + (par1 * trackEnergy)
    static unsigned int m_muonIdMinECalLayersForFit;        ///< Min number of ecal layers to perform fit to ecal section of cluster
    static unsigned int m_muonIdMinHCalLayersForFit;        ///< Min number of hcal layers to perform fit to hcal section of cluster
    static unsigned int m_muonIdMinMuonLayersForFit;        ///< Min number of muon layers to perform fit to muon section of cluster
    static unsigned int m_muonIdECalFitInnerLayer;          ///< Inner layer used for fit to ecal section of cluster
    static unsigned int m_muonIdECalFitOuterLayer;          ///< Outer layer used for fit to ecal section of cluster
    static unsigned int m_muonIdHCalFitInnerLayer;          ///< Inner layer used for fit to hcal section of cluster
    static unsigned int m_muonIdHCalFitOuterLayer;          ///< Outer layer used for fit to hcal section of cluster
    static float        m_muonIdECalRmsCut0;                ///< Parameter 0 for ecal rms cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdECalRmsCut1;                ///< Parameter 1 for ecal rms cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdECalMaxRmsCut;              ///< Max value of ecal rms cut
    static float        m_muonIdHCalRmsCut0;                ///< Parameter 0 for hcal rms cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdHCalRmsCut1;                ///< Parameter 0 for hcal rms cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdHCalMaxRmsCut;              ///< Max value of hcal rms cut
    static float        m_muonIdECalMipFractionCut0;        ///< Parameter 0 for ecal mip fraction cut: cut = par0 - (par1 * trackEnergy)
    static float        m_muonIdECalMipFractionCut1;        ///< Parameter 1 for ecal mip fraction cut: cut = par0 - (par1 * trackEnergy)
    static float        m_muonIdECalMaxMipFractionCut;      ///< Max value of ecal mip fraction cut
    static float        m_muonIdHCalMipFractionCut0;        ///< Parameter 0 for hcal mip fraction cut: cut = par0 - (par1 * trackEnergy)
    static float        m_muonIdHCalMipFractionCut1;        ///< Parameter 1 for hcal mip fraction cut: cut = par0 - (par1 * trackEnergy)
    static float        m_muonIdHCalMaxMipFractionCut;      ///< Max value of hcal mip fraction cut
    static float        m_muonIdECalHitsPerLayerCut0;       ///< Parameter 0 for ecal hits per layer cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdECalHitsPerLayerCut1;       ///< Parameter 1 for ecal hits per layer cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdECalMaxHitsPerLayerCut;     ///< Max value of ecal hits per layer cut
    static float        m_muonIdHCalHitsPerLayerCut0;       ///< Parameter 0 for hcal hits per layer cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdHCalHitsPerLayerCut1;       ///< Parameter 1 for hcal hits per layer cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdHCalMaxHitsPerLayerCut;     ///< Max value of hcal hits per layer cut
    static float        m_muonIdCurlingTrackEnergy;         ///< Max energy for associated track to be considered as curling
    static float        m_muonIdInBarrelHitFraction;        ///< Min fraction of hcal hits in barrel region to identify "barrel cluster"
    static float        m_muonIdTightMipFractionCut;        ///< Tight mip fraction cut
    static float        m_muonIdTightMipFractionECalCut;    ///< Tight ecal mip fraction cut
    static float        m_muonIdTightMipFractionHCalCut;    ///< Tight hcal mip fraction cut
    static unsigned int m_muonIdMinMuonHitsCut;             ///< Min number of hits in muon region
    static unsigned int m_muonIdMinMuonTrackSegmentHitsCut; ///< Min number of muon track segment hits
    static float        m_muonIdMuonRmsCut;                 ///< Muon rms cut
    static float        m_muonIdMaxMuonHitsCut0;            ///< Parameter 0 for max muon hits cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdMaxMuonHitsCut1;            ///< Parameter 1 for max muon hits cut: cut = par0 + (par1 * trackEnergy)
    static float        m_muonIdMaxMuonHitsCutMinValue;     ///< Min value of max muon hits cut
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool FineGranularityParticleId::SortHitsByDistance(const HitEnergyDistance &lhs, const HitEnergyDistance &rhs)
{
    return (lhs.second < rhs.second);
}

#endif // #ifndef FINE_GRANULARITY_PARTICLE_ID_H
