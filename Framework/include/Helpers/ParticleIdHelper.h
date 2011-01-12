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

#include "Utilities/ShowerProfileCalculator.h"

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
     *  @param  profileStart to receive the shower profile start, in radiation lengths
     *  @param  profileDiscrepancy to receive the shower profile discrepancy
     */
    static void CalculateShowerProfile(const Cluster *const pCluster, float &profileStart, float &profileDiscrepancy);

private:
    /**
     *  @brief  Set the shower profile calculator
     * 
     *  @param  pPseudoLayerCalculator address of the shower profile calculator
     */
    static StatusCode SetShowerProfileCalculator(ShowerProfileCalculator *pShowerProfileCalculator);

    /**
     *  @brief  Read the particle id helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    static ParticleIdFunction      *m_pEmShowerFastFunction;        ///< The fast electromagnetic shower id function pointer
    static ParticleIdFunction      *m_pEmShowerFullFunction;        ///< The full electromagnetic shower id function pointer
    static ParticleIdFunction      *m_pPhotonFastFunction;          ///< The fast photon id function pointer
    static ParticleIdFunction      *m_pPhotonFullFunction;          ///< The full photon id function pointer
    static ParticleIdFunction      *m_pElectronFastFunction;        ///< The fast electron id function pointer
    static ParticleIdFunction      *m_pElectronFullFunction;        ///< The full electron id function pointer
    static ParticleIdFunction      *m_pMuonFastFunction;            ///< The fast muon id function pointer
    static ParticleIdFunction      *m_pMuonFullFunction;            ///< The full muon id function pointer

    static ShowerProfileCalculator *m_pShowerProfileCalculator;     ///< Address of the shower profile calculator 

    friend class PandoraApiImpl;
    friend class PandoraSettings;
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef PARTICLE_ID_HELPER_H
