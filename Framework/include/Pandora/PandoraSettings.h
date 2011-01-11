/**
 *  @file   PandoraPFANew/Framework/include/Pandora/PandoraSettings.h
 * 
 *  @brief  Header file for the pandora settings class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_SETTINGS_H
#define PANDORA_SETTINGS_H 1

#include "Pandora/StatusCodes.h"

class TiXmlHandle;

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
     *  @brief  Whether to display algorithm information during processing
     * 
     *  @return boolean
     */
    bool ShouldDisplayAlgorithmInfo() const;

    /**
     *  @brief  Get the electromagnetic energy resolution as a fraction, X, such that sigmaE = ( X * E / sqrt(E) )
     * 
     *  @return The electromagnetic energy resolution
     */
    float GetElectromagneticEnergyResolution() const;

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
     *  @brief  Get the momentum magnitude used to select the pfo target from a mc particle decay chain, units GeV
     * 
     *  @return The pfo selection momentum magnitude
     */
    float GetMCPfoSelectionMomentum() const;

    /**
     *  @brief  Get the low energy cut-off for selection of protons/neutrons as mc pfos
     * 
     *  @return The low energy cut-off for selection of protons/neutrons as mc pfos
     */
    float GetMCPfoSelectionLowEnergyNeutronProtonCutOff() const;

    /**
     *  @brief  Whether to collapse mc particle decay chains down to just the pfo target
     * 
     *  @return boolean
     */
    bool ShouldCollapseMCParticlesToPfoTarget() const;

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
     *  @brief  Register a pandora settings function to e.g. read settings for a registered particle id or energy correction function
     * 
     *  @param  xmlTagName the name of the xml tag (within the <pandora></pandora> tags) containing the settings
     *  @param  pSettingsFunction pointer to the pandora settings function
     */
    StatusCode RegisterSettingsFunction(const std::string &xmlTagName, SettingsFunction *pSettingsFunction);

    /**
     *  @brief  Initialize pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode Initialize(const TiXmlHandle *const pXmlHandle);

    /**
     *  @brief  Run all registered settings functions
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode RunRegisteredSettingsFunctions(const TiXmlHandle *const pXmlHandle) const;

    /**
     *  @brief  Read global pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode ReadGlobalSettings(const TiXmlHandle *const pXmlHandle);

    typedef std::map<SettingsFunction *, std::string> SettingsFunctionToNameMap;

    static PandoraSettings *m_pPandoraSettings;             ///< The pandora settings instance
    bool            m_isInitialized;                        ///< Whether the pandora settings have been initialized
    static bool     m_instanceFlag;                         ///< The pandora settings instance flag

    bool            m_isMonitoringEnabled;                  ///< Whether monitoring is enabled
    bool            m_shouldDisplayAlgorithmInfo;           ///< Whether to display algorithm information during processing

    float           m_electromagneticEnergyResolution;      ///< Electromagnetic energy resolution, X, such that sigmaE = ( X * E / sqrt(E) )
    float           m_hadronicEnergyResolution;             ///< Hadronic energy resolution, X, such that sigmaE = ( X * E / sqrt(E) )

    float           m_mcPfoSelectionRadius;                 ///< Radius used to select pfo target from a mc decay chain, units mm
    float           m_mcPfoSelectionMomentum;               ///< Momentum magnitude used to select pfo target from a mc decay chain, units GeV/c
    float           m_mcPfoSelectionLowEnergyNPCutOff;      ///< Low energy cut-off for selection of protons/neutrons as MCPFOs
    bool            m_shouldCollapseMCParticlesToPfoTarget; ///< Whether to collapse mc particle decay chains down to just the pfo target

    SettingsFunctionToNameMap m_settingsFunctionToNameMap;  ///< The settings function to xml tag name map

    friend class Pandora;
    friend class PandoraApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::IsMonitoringEnabled() const
{
    return m_isMonitoringEnabled;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::ShouldDisplayAlgorithmInfo() const
{
    return m_shouldDisplayAlgorithmInfo;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetElectromagneticEnergyResolution() const
{
    return m_electromagneticEnergyResolution;
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

inline float PandoraSettings::GetMCPfoSelectionMomentum() const
{
    return m_mcPfoSelectionMomentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PandoraSettings::GetMCPfoSelectionLowEnergyNeutronProtonCutOff() const
{
    return m_mcPfoSelectionLowEnergyNPCutOff;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::ShouldCollapseMCParticlesToPfoTarget() const
{
    return m_shouldCollapseMCParticlesToPfoTarget;
}

} // namespace pandora

#endif // #ifndef PANDORA_SETTINGS_H
