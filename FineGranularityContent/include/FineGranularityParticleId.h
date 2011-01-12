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
    static pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

private:
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

#endif // #ifndef FINE_GRANULARITY_PARTICLE_ID_H
