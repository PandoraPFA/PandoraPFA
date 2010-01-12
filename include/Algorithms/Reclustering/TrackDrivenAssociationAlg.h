/**
 *  @file   PandoraPFANew/include/Algorithms/Reclustering/TrackDrivenAssociationAlg.h
 * 
 *  @brief  Header file for the track driven association algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_DRIVEN_ASSOCIATION_ALGORITHM_H
#define TRACK_DRIVEN_ASSOCIATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TrackDrivenAssociationAlg class
 */
class TrackDrivenAssociationAlg : public pandora::Algorithm
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

inline pandora::Algorithm *TrackDrivenAssociationAlg::Factory::CreateAlgorithm() const
{
    return new TrackDrivenAssociationAlg();
}

#endif // #ifndef TRACK_DRIVEN_ASSOCIATION_ALGORITHM_H
