/**
 *  @file   PandoraPFANew/Framework/include/Utilities/FileReader.h
 * 
 *  @brief  Header file for the file reader class.
 * 
 *  $Log: $
 */
#ifndef FILE_READER_H
#define FILE_READER_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraIO.h"
#include "Pandora/StatusCodes.h"

#include <fstream>

namespace pandora
{

class Algorithm;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  FileReader class
 */
class FileReader
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  algorithm the algorithm creating the file reader
     *  @param  fileName the name of the file containing the pandora objects
     */
    FileReader(const pandora::Algorithm &algorithm, const std::string &fileName);

    /**
     *  @brief  Destructor
     */
    ~FileReader();

    /**
     *  @brief  Read the event header from the current position in the file, checking for properly written event
     */
    StatusCode ReadEventHeader();

    /**
     *  @brief  Read an entire pandora event from the file, recreating the stored objects
     */
    StatusCode ReadEvent();

    /**
     *  @brief  Read the next pandora object from the current position in the file, recreating the stored object
     */
    StatusCode ReadNextObject();

    /**
     *  @brief  Read a calo hit from the current position in the file, recreating the stored object
     * 
     *  @param  checkObjectId whether to check the object id before deserializing
     */
    StatusCode ReadCaloHit(bool checkObjectId = true);

    /**
     *  @brief  Read a track from the current position in the file, recreating the stored object
     * 
     *  @param  checkObjectId whether to check the object id before deserializing
     */
    StatusCode ReadTrack(bool checkObjectId = true);

    /**
     *  @brief  Read all calo hits in the current event in the file, recreating the stored objects
     */
    StatusCode ReadAllCaloHitsInEvent();

    /**
     *  @brief  Read all tracks in the current event in the file, recreating the stored objects
     */
    StatusCode ReadAllTracksInEvent();

    /**
     *  @brief  Skip to next event in the file
     */
    StatusCode GoToNextEvent();

    /**
     *  @brief  Skip to a specified event number in the file
     * 
     *  @param  eventNumber the event number
     */
    StatusCode GoToEvent(const unsigned int eventNumber);

private:
    /**
     *  @brief  Read a variable from the file
     */
    template<typename T>
    StatusCode ReadVariable(T &t);

    const pandora::Algorithm *const m_pAlgorithm;       ///< Address of the algorithm that created the file writer
    std::ifstream::pos_type         m_position;         ///< The current position in the file
    std::ifstream::pos_type         m_eventPosition;    ///< The position of the start of the current event in the file
    std::ifstream                   m_fileStream;       ///< The stream class to read from the file
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode FileReader::ReadVariable(T &t)
{
    char *pMemBlock = new char[sizeof(T)];
    m_fileStream.read(pMemBlock, sizeof(T));
    m_position = m_fileStream.tellg();

    t = *(reinterpret_cast<T*>(pMemBlock));
    delete[] pMemBlock;

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileReader::ReadVariable(CartesianVector &t)
{
    float x, y, z;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(x));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(y));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(z));
    t = CartesianVector(x, y, z);

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileReader::ReadVariable(TrackState &t)
{
    CartesianVector position, momentum;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(position));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(momentum));
    t = TrackState(position, momentum);

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef FILE_READER_H
