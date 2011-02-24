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

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EventReadingAlgorithm::Factory::CreateAlgorithm() const
{
    return new EventReadingAlgorithm();
}

#endif // #ifndef EVENT_READING_ALGORITHM_H
