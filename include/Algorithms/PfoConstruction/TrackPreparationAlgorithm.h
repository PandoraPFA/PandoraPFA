/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/TrackPreparationAlgorithm.h
 * 
 *  @brief  Header file for the track preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_PREPARATION_ALGORITHM_H
#define TRACK_PREPARATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TrackPreparationAlgorithm class
 */
class TrackPreparationAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackPreparationAlgorithm();
}

#endif // #ifndef TRACK_PREPARATION_ALGORITHM_H
