/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Utility/PfoPreparationAlgorithm.h
 * 
 *  @brief  Header file for the pfo preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef PFO_PREPARATION_ALGORITHM_H
#define PFO_PREPARATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  PfoPreparationAlgorithm class
 */
class PfoPreparationAlgorithm : public pandora::Algorithm
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

    pandora::StringVector   m_candidateListNames;           ///< The list of pfo list names to use
    std::string             m_mergedCandidateListName;      ///< The name of the merged candidate list name
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PfoPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new PfoPreparationAlgorithm();
}

#endif // #ifndef PFO_PREPARATION_ALGORITHM_H
