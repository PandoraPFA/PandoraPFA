/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Persistency/EventReadingAlgorithm.h
 * 
 *  @brief  Header file for the event reading algorithm class.
 * 
 *  $Log: $
 */
#ifndef EVENT_READING_ALGORITHM_H
#define EVENT_READING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

namespace pandora {class FileReader;}

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  EventReadingAlgorithm class
 */
class EventReadingAlgorithm : public pandora::Algorithm
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
    EventReadingAlgorithm();

    /**
     *  @brief  Destructor
     */
    ~EventReadingAlgorithm();

private:
    pandora::StatusCode Initialize();
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    pandora::FileReader    *m_pFileReader;      ///< Address of the file reader
    std::string             m_fileName;         ///< Name of the output file
    unsigned int            m_skipToEvent;      ///< Index of first event to consider in input file
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EventReadingAlgorithm::Factory::CreateAlgorithm() const
{
    return new EventReadingAlgorithm();
}

#endif // #ifndef EVENT_READING_ALGORITHM_H
