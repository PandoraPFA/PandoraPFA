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

using namespace pandora;

/**
 *  @brief  FragmentRemovalAlgorithm class
 */
class FragmentRemovalAlgorithm : public Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    pandora::StringVector   m_fragmentRemovalAlgorithms;    ///< The ordered list of fragment removal algorithms to be used
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *FragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new FragmentRemovalAlgorithm();
}

#endif // #ifndef FRAGMENT_REMOVAL_ALGORITHM_H
