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
 *  @brief  The object identification enum
 */
enum ObjectId
{
    EVENT,
    CALO_HIT,
    TRACK,
    EVENT_END,
    NUMBER_OF_OBJECTS
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
