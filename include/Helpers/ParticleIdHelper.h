/**
 *  @file   PandoraPFANew/include/Helpers/ParticleIdHelper.h
 * 
 *  @brief  Header file for the particle id helper class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_ID_HELPER_H
#define PARTICLE_ID_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

#include "Xml/tinyxml.h"

#include "StatusCodes.h"

namespace pandora
{

/**
 *  @brief  ParticleIdHelper class
 */
class ParticleIdHelper
{
public:
    /**
     *  @brief  Provide fast identification of whether a cluster is a photon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsPhotonFast(const Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is a photon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsPhotonFull(const Cluster *const pCluster);

    /**
     *  @brief  Provide fast identification of whether a cluster is an electron
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsElectronFast(const Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is an electron
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsElectronFull(const Cluster *const pCluster);

    /**
     *  @brief  Provide fast identification of whether a cluster is a muon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsMuonFast(const Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is a muon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsMuonFull(const Cluster *const pCluster);

    /**
     *  @brief  Calculate shower profile for a cluster and compare it with the expected profile for a photon
     * 
     *  @param  pCluster address of the cluster to investigate
     *  @param  showerProfileStart to receive the shower profile start, in radiation lengths
     *  @param  showerProfileDiscrepancy to receive the shower profile discrepancy
     */
    static StatusCode CalculateShowerProfile(const Cluster *const pCluster, float &showerProfileStart, float &showerProfileDiscrepancy);

    /**
     *  @brief  Whether a cluster is a candidate electromagnetic shower
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsElectromagneticShower(const Cluster *const pCluster);

private:
    /**
     *  @brief  Default pandora fast photon identification function
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsPhotonFastDefault(const Cluster *const pCluster);

    /**
     *  @brief  Default pandora fast electron identification function
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsElectronFastDefault(const Cluster *const pCluster);

    /**
     *  @brief  Default pandora fast muon identification function
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsMuonFastDefault(const Cluster *const pCluster);

    /**
     *  @brief  Read the particle id helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    static ParticleIdFunction *m_pPhotonFastFunction;       ///< The fast photon id function pointer
    static ParticleIdFunction *m_pPhotonFullFunction;       ///< The full photon id function pointer
    static ParticleIdFunction *m_pElectronFastFunction;     ///< The fast electron id function pointer
    static ParticleIdFunction *m_pElectronFullFunction;     ///< The full electron id function pointer
    static ParticleIdFunction *m_pMuonFastFunction;         ///< The fast muon id function pointer
    static ParticleIdFunction *m_pMuonFullFunction;         ///< The full muon id function pointer

    static float        m_showerProfileBinWidth;            ///< Bin width used to construct shower profiles, units radiation lengths
    static unsigned int m_showerProfileNBins;               ///< Number of bins used to construct shower profiles
    static float        m_showerProfileMinCosAngle;         ///< Min angular correction used to adjust radiation length measures
    static float        m_showerProfileCriticalEnergy;      ///< Critical energy, used to calculate argument for gamma function
    static float        m_showerProfileParameter0;          ///< Parameter0, used to calculate argument for gamma function
    static float        m_showerProfileParameter1;          ///< Parameter1, used to calculate argument for gamma function
    static float        m_showerProfileMaxDifference;       ///< Max difference between current and best shower profile comparisons

    static float        m_photonIdMipCut_0;                 ///< Default cluster mip fraction cut for photon id
    static float        m_photonIdMipCutEnergy_1;           ///< Energy above which mip fraction cut value 1 is applied
    static float        m_photonIdMipCut_1;                 ///< Cluster mip fraction cut value 1
    static float        m_photonIdMipCutEnergy_2;           ///< Energy above which mip fraction cut value 2 is applied
    static float        m_photonIdMipCut_2;                 ///< Cluster mip fraction cut value 2
    static float        m_photonIdMipCutEnergy_3;           ///< Energy above which mip fraction cut value 3 is applied
    static float        m_photonIdMipCut_3;                 ///< Cluster mip fraction cut value 3
    static float        m_photonIdMipCutEnergy_4;           ///< Energy above which mip fraction cut value 4 is applied
    static float        m_photonIdMipCut_4;                 ///< Cluster mip fraction cut value 4
    static float        m_photonIdDCosRCutEnergy;           ///< Energy at which photon id cut (on cluster fit result dCosR) changes
    static float        m_photonIdDCosRLowECut;             ///< Low energy cut on cluster fit result dCosR
    static float        m_photonIdDCosRHighECut;            ///< High energy cut on cluster fit result dCosR
    static float        m_photonIdRmsCutEnergy;             ///< Energy at which photon id cut (on cluster fit result rms) changes
    static float        m_photonIdRmsLowECut;               ///< Low energy cut on cluster fit result rms
    static float        m_photonIdRmsHighECut;              ///< High energy cut on cluster fit result rms
    static float        m_photonIdEndCapZSeparation;        ///< To determine whether cluster strikes endcap: max z distance to inner centroid
    static float        m_photonIdRadiationLengthsCut;      ///< Max number of radiation lengths before cluster inner layer
    static float        m_photonIdShowerMaxCut1_0;          ///< Default value for shower max cut 1: min number of layers from inner layer
    static float        m_photonIdShowerMaxCut1Energy_1;    ///< Energy above which shower max cut 1 value 1 is applied
    static float        m_photonIdShowerMaxCut1_1;          ///< Shower max cut 1 value 1
    static float        m_photonIdShowerMaxCut1Energy_2;    ///< Energy above which shower max cut 1 value 2 is applied
    static float        m_photonIdShowerMaxCut1_2;          ///< Shower max cut 1 value 2
    static float        m_photonIdShowerMaxCut2;            ///< Shower max cut 2: max number of layers from inner layer
    static float        m_photonIdLayer90Cut1;              ///< Cut on cluster layer 90: min number of layers from inner layer
    static float        m_photonIdLayer90Cut2Energy;        ///< Energy at which photon id cut (on cluster layer 90) changes
    static float        m_photonIdLayer90LowECut2;          ///< Low energy cut on max number of layers between layer 90 and inner layer
    static float        m_photonIdLayer90HighECut2;         ///< High energy cut on max number of layers between layer 90 and inner layer
    static int          m_photonIdLayer90MaxLayersFromECal; ///< Max number of layers between cluster layer 90 and last ecal layer

    static unsigned int m_electronIdMaxInnerLayer;          ///< Max inner psuedo layer for fast electron id
    static float        m_electronIdMaxEnergy;              ///< Max electromagnetic energy for fast electron id
    static float        m_electronIdMaxProfileStart;        ///< Max shower profile start for fast electron id
    static float        m_electronIdMaxProfileDiscrepancy;  ///< Max shower profile discrepancy for fast electron id
    static float        m_electronIdProfileDiscrepancyForAutoId;    ///< Shower profile discrepancy for automatic fast electron selection
    static float        m_electronIdMaxResidualEOverP;      ///< Max absolute difference between unity and ratio em energy / track momentum

    static unsigned int m_muonIdMaxInnerLayer;              ///< Max inner psuedo layer for fast muon id
    static float        m_muonIdMinTrackEnergy;             ///< Min energy of associated track for fast muon id
    static unsigned int m_muonIdMinECalLayers;              ///< Min number of ecal layers for fast muon id
    static unsigned int m_muonIdMinHCalLayers;              ///< Min number of hcal layers for fast muon id
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

    friend class PandoraSettings;
    friend class PluginManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ParticleIdHelper::IsPhotonFast(const Cluster *const pCluster)
{
    if (NULL == m_pPhotonFastFunction)
        return false;

    return (*m_pPhotonFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ParticleIdHelper::IsPhotonFull(const Cluster *const pCluster)
{
    if (NULL == m_pPhotonFullFunction)
        return false;

    return (*m_pPhotonFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ParticleIdHelper::IsElectronFast(const Cluster *const pCluster)
{
    if (NULL == m_pElectronFastFunction)
        return false;

    return (*m_pElectronFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ParticleIdHelper::IsElectronFull(const Cluster *const pCluster)
{
    if (NULL == m_pElectronFullFunction)
        return false;

    return (*m_pElectronFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ParticleIdHelper::IsMuonFast(const Cluster *const pCluster)
{
    if (NULL == m_pMuonFastFunction)
        return false;

    return (*m_pMuonFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ParticleIdHelper::IsMuonFull(const Cluster *const pCluster)
{
    if (NULL == m_pMuonFullFunction)
        return false;

    return (*m_pMuonFullFunction)(pCluster);
}

} // namespace pandora

#endif // #ifndef PARTICLE_ID_HELPER_H
