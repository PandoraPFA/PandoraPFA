/**
 *  @file   PandoraPFANew/Framework/include/Utilities/FileReader.h
 * 
 *  @brief  Header file for the file reader class.
 * 
 *  $Log: $
 */
#ifndef FILE_READER_H
#define FILE_READER_H 1

#include "Api/PandoraApi.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraIO.h"
#include "Pandora/StatusCodes.h"

#include "Objects/CartesianVector.h"
#include "Objects/TrackState.h"

#include <fstream>
#include <string>

namespace pandora
{

/**
 *  @brief  FileReader class
 */
class FileReader
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pandora the pandora instance to be used alongside the file reader
     *  @param  fileName the name of the file containing the pandora objects
     */
    FileReader(const pandora::Pandora &pandora, const std::string &fileName);

    /**
     *  @brief  Destructor
     */
    ~FileReader();

    /**
     *  @brief  Read the current geometry information from the file
     */
    StatusCode ReadGeometry();

    /**
     *  @brief  Read an entire pandora event from the file, recreating the stored objects
     */
    StatusCode ReadEvent();

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
     *  @brief  Read the container header from the current position in the file, checking for properly written container
     */
    StatusCode ReadHeader();

    /**
     *  @brief  Read the next pandora event component from the current position in the file, recreating the stored component
     */
    StatusCode ReadNextEventComponent();

    /**
     *  @brief  Read the geometry parameters from the current position in the file
     */
    StatusCode ReadGeometryParameters();

    /**
     *  @brief  Read a sub detector from the current position in the file
     * 
     *  @param  pSubDetectorParameters address of the sub detector parameters to populate
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadSubDetector(PandoraApi::GeometryParameters::SubDetectorParameters *pSubDetectorParameters, bool checkComponentId = true);

    /**
     *  @brief  Read a calo hit from the current position in the file, recreating the stored object
     * 
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadCaloHit(bool checkComponentId = true);

    /**
     *  @brief  Read a track from the current position in the file, recreating the stored object
     * 
     *  @param  checkComponentId whether to check the component id before deserializing
     */
    StatusCode ReadTrack(bool checkComponentId = true);

    /**
     *  @brief  Read a variable from the file
     */
    template<typename T>
    StatusCode ReadVariable(T &t);

    const pandora::Pandora *const   m_pPandora;             ///< Address of pandora instance to be used alongside the file reader
    ContainerId                     m_containerId;          ///< The type of container currently being read from file
    std::ifstream::pos_type         m_containerPosition;    ///< Position of start of the current event/geometry container object in file
    std::ifstream::pos_type         m_containerSize;        ///< Size of the current event/geometry container object in the file
    std::ifstream                   m_fileStream;           ///< The stream class to read from the file
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode FileReader::ReadVariable(T &t)
{
    char *pMemBlock = new char[sizeof(T)];
    m_fileStream.read(pMemBlock, sizeof(T));

    t = *(reinterpret_cast<T*>(pMemBlock));
    delete[] pMemBlock;

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileReader::ReadVariable(const std::string &t)
{
    // Not currently supported
    return STATUS_CODE_INVALID_PARAMETER;
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
