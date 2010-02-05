/**
 *  @file   PandoraPFANew/include/Algorithms/FragmentRemoval/NeutralFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the neutral fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H
#define NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  NeutralFragmentRemovalAlgorithm class
 */
class NeutralFragmentRemovalAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *NeutralFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new NeutralFragmentRemovalAlgorithm();
}

#endif // #ifndef NEUTRAL_FRAGMENT_REMOVAL_ALGORITHM_H
