/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Persistency/EventWritingAlgorithm.h
 * 
 *  @brief  Header file for the event writing algorithm class.
 * 
 *  $Log: $
 */
#ifndef EVENT_WRITING_ALGORITHM_H
#define EVENT_WRITING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace pandora {class FileWriter;}

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  EventWritingAlgorithm class
 */
class EventWritingAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief  Default constructor
     */
    EventWritingAlgorithm();

    /**
     *  @brief  Destructor
     */
    ~EventWritingAlgorithm();

private:
    pandora::StatusCode Initialize();
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    pandora::FileWriter    *m_pFileWriter;      ///< Address of the file writer
    std::string             m_fileName;         ///< Name of the output file
    bool                    m_shouldOverwrite;  ///< Whether to overwrite an existing file with the specified name, or append
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EventWritingAlgorithm::Factory::CreateAlgorithm() const
{
    return new EventWritingAlgorithm();
}

#endif // #ifndef EVENT_WRITING_ALGORITHM_H
