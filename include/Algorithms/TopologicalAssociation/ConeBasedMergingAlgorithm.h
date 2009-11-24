/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ConeBasedMergingAlgorithm.h
 * 
 *  @brief  Header file for the cone based merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef CONE_BASED_MERGING_ALGORITHM_H
#define CONE_BASED_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ConeBasedMergingAlgorithm class
 */
class ConeBasedMergingAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *ConeBasedMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new ConeBasedMergingAlgorithm();
}

#endif // #ifndef CONE_BASED_MERGING_ALGORITHM_H
