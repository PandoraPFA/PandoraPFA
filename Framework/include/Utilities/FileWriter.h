/**
 *  @file   PandoraPFANew/Framework/include/Utilities/FileWriter.h
 * 
 *  @brief  Header file for the file writer class.
 * 
 *  $Log: $
 */
#ifndef FILE_WRITER_H
#define FILE_WRITER_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraIO.h"
#include "Pandora/StatusCodes.h"

#include <fstream>

namespace pandora
{

class Algorithm;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  FileWriter class
 */
class FileWriter
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  algorithm the algorithm creating the file writer
     *  @param  fileName the name of the output file
     *  @param  fileMode the mode for file writing
     */
    FileWriter(const pandora::Algorithm &algorithm, const std::string &fileName, const FileMode fileMode = APPEND);

    /**
     *  @brief  Destructor
     */
    ~FileWriter();

   /**
     *  @brief  Write the event header to the file
     */
    StatusCode WriteEventHeader();

   /**
     *  @brief  Write the event footer to the file
     */
    StatusCode WriteEventFooter();

    /**
     *  @brief  Write the current event to the file
     */
    StatusCode WriteEvent();

    /**
     *  @brief  Write a calo hit to the current position in the file
     * 
     *  @param  pCaloHit address of the calo hit
     */
    StatusCode WriteCaloHit(const CaloHit *const pCaloHit);

    /**
     *  @brief  Write a track to the current position in the file
     * 
     *  @param   pTrack address of the track
     */
    StatusCode WriteTrack(const Track *const pTrack);

    /**
     *  @brief  Write a calo hit list to the current position in the file
     * 
     *  @param  caloHitList the calo hit list
     */
    StatusCode WriteCaloHitList(const CaloHitList &caloHitList);

    /**
     *  @brief  Write a track list to the current position in the file
     * 
     *  @param  trackList the track list
     */
    StatusCode WriteTrackList(const TrackList &trackList);

    /**
     *  @brief  Write an ordered calo hit list to the current position in the file
     * 
     *  @param  orderedCaloHitList the ordered calo hit list
     */
    StatusCode WriteOrderedCaloHitList(const OrderedCaloHitList &orderedCaloHitList);

private:
    /**
     *  @brief  Read a variable from the file
     */
    template<typename T>
    StatusCode WriteVariable(const T &t);

    const pandora::Algorithm *const m_pAlgorithm;           ///< Address of the algorithm that created the file writer
    std::ofstream::pos_type         m_position;             ///< The current position in the file
    std::ofstream::pos_type         m_eventPosition;        ///< The position of the start of the current event in the file
    std::ofstream                   m_fileStream;           ///< The stream class to write to the file
};

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline StatusCode FileWriter::WriteVariable(const T &t)
{
    m_fileStream.write(reinterpret_cast<const char*>(&t), sizeof(T));
    m_position = m_fileStream.tellp();

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileWriter::WriteVariable(const CartesianVector &t)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetX()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetY()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetZ()));
    return STATUS_CODE_SUCCESS;
}

template<>
inline StatusCode FileWriter::WriteVariable(const TrackState &t)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetPosition()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->WriteVariable(t.GetMomentum()));
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef FILE_WRITER_H
