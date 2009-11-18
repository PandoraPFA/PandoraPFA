/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.h
 * 
 *  @brief  Header file for the broekn tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef BROKEN_TRACKS_ALGORITHM_H
#define BROKEN_TRACKS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  BrokenTracksAlgorithm class
 */
class BrokenTracksAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *BrokenTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new BrokenTracksAlgorithm();
}

#endif // #ifndef BROKEN_TRACKS_ALGORITHM_H
