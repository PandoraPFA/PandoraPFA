/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/BackscatteredTracksAlgorithm.h
 * 
 *  @brief  Header file for the backscattered tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef BACKSCATTERED_TRACKS_ALGORITHM_H
#define BACKSCATTERED_TRACKS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  BackscatteredTracksAlgorithm class
 */
class BackscatteredTracksAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *BackscatteredTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new BackscatteredTracksAlgorithm();
}

#endif // #ifndef BACKSCATTERED_TRACKS_ALGORITHM_H
