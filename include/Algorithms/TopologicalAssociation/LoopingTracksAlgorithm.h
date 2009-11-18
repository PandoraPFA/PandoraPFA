/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/LoopingTracksAlgorithm.h
 * 
 *  @brief  Header file for the looping tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef LOOPING_TRACKS_ALGORITHM_H
#define LOOPING_TRACKS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  LoopingTracksAlgorithm class
 */
class LoopingTracksAlgorithm : public pandora::Algorithm
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
    StatusCode ReadSettings(TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *LoopingTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new LoopingTracksAlgorithm();
}

#endif // #ifndef LOOPING_TRACKS_ALGORITHM_H
