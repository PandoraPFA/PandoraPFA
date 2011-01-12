/**
 *  @file   PandoraPFANew/Framework/include/Helpers/ParticleIdHelper.h
 * 
 *  @brief  Header file for the particle id helper class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_ID_HELPER_H
#define PARTICLE_ID_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

#include "Xml/tinyxml.h"

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
    typedef std::pair<float, float> HitEnergyDistance;
    typedef std::vector<HitEnergyDistance> HitEnergyDistanceVector;

    /**
     *  @brief  Sort HitEnergyDistance objects by increasing distance
     * 
     *  @param  lhs the first hit energy distance pair
     *  @param  rhs the second hit energy distance pair
     */
    static bool SortHitsByDistance(const HitEnergyDistance &lhs, const HitEnergyDistance &rhs);

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
    static float        m_photonIdMinCosAngle;              ///< Min angular correction used to adjust radiation length measures
    static float        m_photonIdMaxInnerLayerRadLengths;  ///< Max number of radiation lengths before cluster inner layer
    static float        m_photonIdMinLayer90RadLengths;     ///< Min number of radiation lengths before cluster layer90
    static float        m_photonIdMaxLayer90RadLengths;     ///< Max number of radiation lengths before cluster layer90
    static float        m_photonIdMinShowerMaxRadLengths;   ///< Min number of radiation lengths before cluster shower max layer
    static float        m_photonIdMaxShowerMaxRadLengths;   ///< Max number of radiation lengths before cluster shower max layer
    static float        m_photonIdHighRadLengths;           ///< Max number of radiation lengths expected to be spanned by em shower
    static float        m_photonIdMaxHighRadLengthEnergyFraction;   ///< Max fraction of cluster energy above max expected radiation lengths
    static float        m_photonIdMaxRadial90;              ///< Max value of transverse profile radial90

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

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ParticleIdHelper::SortHitsByDistance(const HitEnergyDistance &lhs, const HitEnergyDistance &rhs)
{
    return (lhs.second < rhs.second);
}

} // namespace pandora

#endif // #ifndef PARTICLE_ID_HELPER_H
