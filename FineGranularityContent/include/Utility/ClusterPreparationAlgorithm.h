/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Utility/ClusterPreparationAlgorithm.h
 * 
 *  @brief  Header file for the cluster preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_PREPARATION_ALGORITHM_H
#define CLUSTER_PREPARATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  ClusterPreparationAlgorithm class
 */
class ClusterPreparationAlgorithm : public pandora::Algorithm
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

    pandora::StringVector   m_candidateListNames;           ///< The list of cluster list names to use
    std::string             m_mergedCandidateListName;      ///< The name of the merged candidate list name
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusterPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new ClusterPreparationAlgorithm();
}

#endif // #ifndef CLUSTER_PREPARATION_ALGORITHM_H
