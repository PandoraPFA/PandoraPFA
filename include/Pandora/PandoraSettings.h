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

    bool                        m_isInitialized;            ///< Whether the pandora settings have been initialized

    static bool                 m_instanceFlag;             ///< The geometry helper instance flag
    static PandoraSettings     *m_pPandoraSettings;         ///< The geometry helper instance

    bool                        m_isMonitoringEnabled;      ///< Whether monitoring is enabled

    friend class Pandora;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool PandoraSettings::IsMonitoringEnabled() const
{
    return m_isMonitoringEnabled;
}

} // namespace pandora

#endif // #ifndef PANDORA_SETTINGS_H
