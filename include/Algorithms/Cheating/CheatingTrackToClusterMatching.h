/**
 *  @file   PandoraPFANew/include/Algorithms/Cheating/CheatingTrackToClusterMatching.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef CHEATING_TRACK_TO_CLUSTER_MATCHING_ALGORITHM_H
#define CHEATING_TRACK_TO_CLUSTER_MATCHING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief CheatingTrackToClusterMatching class
 */
class CheatingTrackToClusterMatching : public pandora::Algorithm
{
private:
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
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingTrackToClusterMatching::Factory::CreateAlgorithm() const
{
    return new CheatingTrackToClusterMatching();
}

#endif // #ifndef CHEATING_TRACK_TO_CLUSTER_MATCHING_ALGORITHM_H
