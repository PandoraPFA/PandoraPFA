/**
 *  @file   PandoraPFANew/Framework/include/Pandora/PandoraIO.h
 * 
 *  @brief  Header file for pandora io structures.
 * 
 *  $Log: $
 */
#ifndef PANDORA_IO_H
#define PANDORA_IO_H 1

namespace pandora
{

const unsigned int pandoraFileHash(01110000);   ///< Look for hash ('p' in binary) each event to check integrity

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The container identification enum
 */
enum ContainerId
{
    EVENT,
    GEOMETRY,
    UNKNOWN_CONTAINER
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The component identification enum
 */
enum ComponentId
{
    CALO_HIT,
    TRACK,
    EVENT_END,
    SUB_DETECTOR,
    DETECTOR_GAP,
    GEOMETRY_END,
    UNKNOWN_COMPONENT
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  The file mode enum
 */
enum FileMode
{
    APPEND,
    OVERWRITE,
    NUMBER_OF_MODES
};

} // namespace pandora

#endif // #ifndef PANDORA_IO_H
