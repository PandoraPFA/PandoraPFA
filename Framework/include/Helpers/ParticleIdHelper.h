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
     *  @brief  Provide fast identification of whether a cluster is an electromagnetic shower
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsEmShowerFast(const Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is an electromagnetic shower
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsEmShowerFull(const Cluster *const pCluster);

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

private:
    /**
     *  @brief  Read the particle id helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    static ParticleIdFunction *m_pEmShowerFastFunction;     ///< The fast electromagnetic shower id function pointer
    static ParticleIdFunction *m_pEmShowerFullFunction;     ///< The full electromagnetic shower id function pointer
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

    friend class PandoraSettings;
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef PARTICLE_ID_HELPER_H
