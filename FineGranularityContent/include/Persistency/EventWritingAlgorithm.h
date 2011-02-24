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

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EventWritingAlgorithm::Factory::CreateAlgorithm() const
{
    return new EventWritingAlgorithm();
}

#endif // #ifndef EVENT_WRITING_ALGORITHM_H
