/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Cheating/PerfectFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H
#define PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Pandora/PandoraInternal.h"

/**
 *  @brief PerfectFragmentRemovalAlgorithm class
 */
class PerfectFragmentRemovalAlgorithm : public pandora::Algorithm
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

    std::string     m_inputClusterListName;         ///< The input cluster list name; if not specified, use current list
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectFragmentRemovalAlgorithm();
}

#endif // #ifndef PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H
