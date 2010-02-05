/**
 *  @file   PandoraPFANew/include/Pandora/PandoraSettings.h
 * 
 *  @brief  Header file for the pandora settings class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_SETTINGS_H
#define PANDORA_SETTINGS_H 1

#include "StatusCodes.h"

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

namespace pandora
{

/**
 *  @brief  PandoraSettings class
 */
class PandoraSettings
{
public:
    /**
     *  @brief  Get the pandora settings singleton
     */
    static PandoraSettings *GetInstance();

    /**
     *  @brief  Whether monitoring is enabled
     * 
     *  @return boolean
     */
    bool IsMonitoringEnabled() const;

    /**
     *  @brief  Get the hadronic energy resolution as a fraction, X, such that sigmaE = ( X * E / sqrt(E) )
     * 
     *  @return The hadronic energy resolution
     */
    float GetHadronicEnergyResolution() const;

    /**
     *  @brief  Get the radius used to select the pfo target from a mc particle decay chain, units mm
     * 
     *  @return The pfo selection radius
     */
    float GetMCPfoSelectionRadius() const;

    /**
     *  @brief  Get the maximum separation for associations between hits to be considered, units mm
     * 
     *  @return The maximum separation
     */
    float GetCaloHitMaxSeparation() const;

    /**
     *  @brief  Get the density weighting power
     * 
     *  @return The density weighting power
     */
    unsigned int GetDensityWeightPower() const;

    /**
     *  @brief  Get the number of adjacent layers to use in density weight calculation
     * 
     *  @return The number of layers
     */
    unsigned int GetDensityWeightNLayers() const;

    /**
     *  @brief  Whether to use the simple (density weight cut) isolation scheme
     * 
     *  @return boolean
     */
    bool ShouldUseSimpleIsolationScheme() const;

    /**
     *  @brief  Get the ecal isolation density weight cut
     * 
     *  @return The ecal isolation density weight cut
     */
    float GetIsolationDensityWeightCutECal() const;

    /**
     *  @brief  Get the hcal isolation density weight cut
     * 
     *  @return The hcal isolation density weight cut
     */
    float GetIsolationDensityWeightCutHCal() const;

    /**
     *  @brief  Get the number of adjacent layers to use in isolation calculation
     * 
     *  @return The number of adjacent layers to use in isolation calculation
     */
    unsigned int GetIsolationNLayers() const;

    /**
     *  @brief  Get the ecal isolation cut distance, units mm
     * 
     *  @return The ecal isolation cut distance, units mm
     */
    float GetIsolationCutDistanceECal() const;

    /**
     *  @brief  Get the hcal isolation cut distance, units mm
     * 
     *  @return The hcal isolation cut distance, units mm
     */
    float GetIsolationCutDistanceHCal() const;

    /**
     *  @brief  Get the maximum number of "nearby" hits for a hit to be considered isolated
     * 
     *  @return The maximum number of "nearby" hits
     */
    unsigned int GetIsolationMaxNearbyHits() const;

    /**
     *  @brief  Get the mip equivalent energy cut for a hit to be flagged as a possible mip
     * 
     *  @return The mip equivalent energy cut
     */
    float GetMipLikeMipCut() const;

    /**
     *  @brief  Get the separation (in calorimeter cells) for hits to be declared "nearby"
     * 
     *  @return The number of calorimeter cells
     */
    unsigned int GetMipNCellsForNearbyHit() const;

    /**
     *  @brief  Get the maximum number of "nearby" hits for a hit to be flagged as a possible mip
     * 
     *  @return The maximum number of "nearby" hits
     */
    unsigned int GetMipMaxNearbyHits() const;

    /**
     *  @brief  Get the bin width used to construct shower profiles, units radiation lengths
     * 
     *  @return The bin width used to construct shower profiles
     */
    float GetShowerProfileBinWidth() const;

    /**
     *  @brief  Get the number of bins used to construct shower profiles
     * 
     *  @return The number of bins used to construct shower profiles
     */
    unsigned int GetShowerProfileNBins() const;

    /**
     *  @brief  Get the min angular correction used to adjust radiation length measures
     * 
     *  @return The min angular correction used to adjust radiation length measures
     */
    float GetShowerProfileMinCosAngle() const;

    /**
     *  @brief  Get critical energy, used to calculate argument for gamma function:
     *          a = (parameter0 + parameter1 * std::log(clusterEnergy / criticalEnergy)
     * 
     *  @return The critical energy
     */
    float GetShowerProfileCriticalEnergy() const;

    /**
     *  @brief  Get parameter0, used to calculate argument for gamma function:
     *          a = (parameter0 + parameter1 * std::log(clusterEnergy / criticalEnergy)
     * 
     *  @return The parameter0
     */
    float GetShowerProfileParameter0() const;

    /**
     *  @brief  Get parameter1, used to calculate argument for gamma function:
     *          a = (parameter0 + parameter1 * std::log(clusterEnergy / criticalEnergy)
     * 
     *  @return The parameter1
     */
    float GetShowerProfileParameter1() const;

    /**
     *  @brief  Get the max difference between current and best shower profile comparisons
     * 
     *  @return The max difference between current and best shower profile comparisons
     */
    float GetShowerProfileMaxDifference() const;

private:
    /**
     *  @brief  Constructor
     */
    PandoraSettings();

    /**
     *  @brief  Destructor
     */
    ~PandoraSettings();

    /**
     *  @brief  Initialize pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode Initialize(const TiXmlHandle *const pXmlHandle);

    bool                    m_isInitialized;                    ///< Whether the pandora settings have been initialized

    float                   m_mcPfoSelectionRadius;             ///< Radius used to select the pfo target from a mc decay chain, units mm

    static bool             m_instanceFlag;                     ///< The geometry helper instance flag
    static PandoraSettings *m_pPandoraSettings;                 ///< The geometry helper instance

    bool                    m_isMonitoringEnabled;              ///< Whether monitoring is enabled

    float                   m_hadronicEnergyResolution;         ///< Hadronic energy resolution, X, such that sigmaE = ( X * E / sqrt(E) )

    float                   m_caloHitMaxSeparation;             ///< Max separation for associations between hits to be considered, units mm

    unsigned int            m_densityWeightPower;               ///< The density weighting power
    unsigned int            m_densityWeightNLayers;             ///< Number of adjacent layers to use in density weight calculation

    bool                    m_shouldUseSimpleIsolationScheme;   ///< Whether to use the simple (density weight cut) isolation scheme
    float                   m_isolationDensityWeightCutECal;    ///< ECal isolation density weight cut
    float                   m_isolationDensityWeightCutHCal;    ///< HCal isolation density weight cut

    unsigned int            m_isolationNLayers;                 ///< Number of adjacent layers to use in isolation calculation
    float                   m_isolationCutDistanceECal;         ///< ECal isolation cut distance, units mm
    float                   m_isolationCutDistanceHCal;         ///< HCal isolation cut distance, units mm
    unsigned int            m_isolationMaxNearbyHits;           ///< Max number of "nearby" hits for a hit to be considered isolated

    float                   m_mipLikeMipCut;                    ///< Mip equivalent energy cut for a hit to be flagged as a possible mip
    unsigned int            m_mipNCellsForNearbyHit;            ///< Separation (in calorimeter cells) for hits to be declared "nearby"
    unsigned int            m_mipMaxNearbyHits;                 ///< Max number of "nearby" hits for a hit to be flagged as a possible mip

    float                   m_showerProfileBinWidth;            ///< Bin width used to construct shower profiles, units radiation lengths
    unsigned int            m_showerProfileNBins;               ///< Number of bins used to construct shower profiles
    float                   m_showerProfileMinCosAngle;         ///< Min angular correction used to adjust radiation length measures

    float                   m_showerProfileCriticalEnergy;      ///< Critical energy, used to calculate argument for gamma function
    float                   m_showerProfileParameter0;          ///< Parameter0, used to calculate argument for gamma function
    float                   m_showerProfileParameter1;          ///< Parameter1, used to calculate argument for gamma function

    float                   m_showerProfileMaxDifference;       ///< Max difference between current and best shower profile comparisons

    friend class Pandora;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::IsMonitoringEnabled() const
{
    return m_isMonitoringEnabled;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetHadronicEnergyResolution() const
{
    return m_hadronicEnergyResolution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMCPfoSelectionRadius() const
{
    return m_mcPfoSelectionRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetCaloHitMaxSeparation() const
{
    return m_caloHitMaxSeparation;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int PandoraSettings::GetDensityWeightPower() const
{
    return m_densityWeightPower;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int PandoraSettings::GetDensityWeightNLayers() const
{
    return m_densityWeightNLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::ShouldUseSimpleIsolationScheme() const
{
    return m_shouldUseSimpleIsolationScheme;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetIsolationDensityWeightCutECal() const
{
    return m_isolationDensityWeightCutECal;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetIsolationDensityWeightCutHCal() const
{
    return m_isolationDensityWeightCutHCal;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int PandoraSettings::GetIsolationNLayers() const
{
    return m_isolationNLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetIsolationCutDistanceECal() const
{
    return m_isolationCutDistanceECal;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetIsolationCutDistanceHCal() const
{
    return m_isolationCutDistanceHCal;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int PandoraSettings::GetIsolationMaxNearbyHits() const
{
    return m_isolationMaxNearbyHits;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMipLikeMipCut() const
{
    return m_mipLikeMipCut;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int PandoraSettings::GetMipNCellsForNearbyHit() const
{
    return m_mipNCellsForNearbyHit;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int PandoraSettings::GetMipMaxNearbyHits() const
{
    return m_mipMaxNearbyHits;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetShowerProfileBinWidth() const
{
    return m_showerProfileBinWidth;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int PandoraSettings::GetShowerProfileNBins() const
{
    return m_showerProfileNBins;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetShowerProfileMinCosAngle() const
{
    return m_showerProfileMinCosAngle;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetShowerProfileCriticalEnergy() const
{
    return m_showerProfileCriticalEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetShowerProfileParameter0() const
{
    return m_showerProfileParameter0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetShowerProfileParameter1() const
{
    return m_showerProfileParameter1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetShowerProfileMaxDifference() const
{
    return m_showerProfileMaxDifference;
}

} // namespace pandora

#endif // #ifndef PANDORA_SETTINGS_H
