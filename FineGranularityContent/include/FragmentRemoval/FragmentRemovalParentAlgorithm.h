/**
 *  @file   PandoraPFANew/FineGranularityContent/include/FragmentRemoval/FragmentRemovalParentAlgorithm.h
 * 
 *  @brief  Header file for the fragment removal parent algorithm class.
 * 
 *  $Log: $
 */
#ifndef FRAGMENT_REMOVAL_PARENT_ALGORITHM_H
#define FRAGMENT_REMOVAL_PARENT_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  FragmentRemovalParentAlgorithm class
 */
class FragmentRemovalParentAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    pandora::StringVector   m_FragmentRemovalParentAlgorithms;    ///< The ordered list of fragment removal algorithms to be used
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *FragmentRemovalParentAlgorithm::Factory::CreateAlgorithm() const
{
    return new FragmentRemovalParentAlgorithm();
}

#endif // #ifndef FRAGMENT_REMOVAL_PARENT_ALGORITHM_H
