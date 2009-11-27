/**
 *  @file   PandoraPFANew/include/Algorithms/FragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef FRAGMENT_REMOVAL_ALGORITHM_H
#define FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  FragmentRemovalAlgorithm class
 */
class FragmentRemovalAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *FragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new FragmentRemovalAlgorithm();
}

#endif // #ifndef FRAGMENT_REMOVAL_ALGORITHM_H
