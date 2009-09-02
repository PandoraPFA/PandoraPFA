/**
 *  @file   PandoraPFANew/include/PandoraSettings.h
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

private:
    /**
     *  @brief  Read pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode Read(const TiXmlHandle *const pXmlHandle);    

    friend class PandoraContentApiImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_SETTINGS_H
