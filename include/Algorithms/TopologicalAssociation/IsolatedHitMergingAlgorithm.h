/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/IsolatedHitMergingAlgorithm.h
 * 
 *  @brief  Header file for the isolated hit merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef ISOLATED_HIT_MERGING_ALGORITHM_H
#define ISOLATED_HIT_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  IsolatedHitMergingAlgorithm class
 */
class IsolatedHitMergingAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *IsolatedHitMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new IsolatedHitMergingAlgorithm();
}

#endif // #ifndef ISOLATED_HIT_MERGING_ALGORITHM_H
