/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/TrackClusterAssociationAlgorithm.h
 * 
 *  @brief  Header file for the track-cluster association algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_CLUSTER_ASSOCIATION_ALGORITHM_H
#define TRACK_CLUSTER_ASSOCIATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TrackClusterAssociationAlgorithm class
 */
class TrackClusterAssociationAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *TrackClusterAssociationAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackClusterAssociationAlgorithm();
}

#endif // #ifndef TRACK_CLUSTER_ASSOCIATION_ALGORITHM_H
