/**
 *  @file   PandoraPFANew/include/Helpers/ParticleIdHelper.h
 * 
 *  @brief  Header file for the particle id helper class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_ID_HELPER_H
#define PARTICLE_ID_HELPER_H 1

#include "Pandora/PandoraInternal.h"

#include "StatusCodes.h"

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

namespace pandora
{

/**
 *  @brief  ParticleIdHelper class
 */
class ParticleIdHelper
{
public:
    /**
     *  @brief  Calculate shower profile for a cluster and compare it with the expected profile for a photon
     * 
     *  @param  pCluster address of the cluster to investigate
     *  @param  showerProfileStart to receive the shower profile start, in radiation lengths
     *  @param  showerProfileDiscrepancy to receive the shower profile discrepancy
     */
    static StatusCode CalculateShowerProfile(Cluster *const pCluster, float &showerProfileStart, float &showerProfileDiscrepancy);

    /**
     *  @brief  Provide fast identification of whether a cluster is a photon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsPhotonFast(Cluster *const pCluster);

    /**
     *  @brief  Provide a more detailed identification of whether a cluster is a photon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsPhotonFull(Cluster *const pCluster);

    /**
     *  @brief  Provide fast identification of whether a cluster is a electron
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsElectronFast(Cluster *const pCluster);

private:
    /**
     *  @brief  Read the particle id helper settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle *const pXmlHandle);

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

    friend class PandoraSettings;
};

} // namespace pandora

#endif // #ifndef PARTICLE_ID_HELPER_H
