/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ProximityBasedMergingAlgorithm.h
 * 
 *  @brief  Header file for the proximity based merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef PROXIMITY_BASED_MERGING_ALGORITHM_H
#define PROXIMITY_BASED_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ProximityBasedMergingAlgorithm class
 */
class ProximityBasedMergingAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *ProximityBasedMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new ProximityBasedMergingAlgorithm();
}

#endif // #ifndef PROXIMITY_BASED_MERGING_ALGORITHM_H
