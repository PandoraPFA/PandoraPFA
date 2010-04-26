/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/ClusterPreparationAlgorithm.h
 * 
 *  @brief  Header file for the cluster preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_PREPARATION_ALGORITHM_H
#define CLUSTER_PREPARATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

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
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    pandora::StringVector   m_candidateListNames;           ///< The list of cluster list names to use
    std::string             m_finalPfoListName;             ///< The name of the output cluster list
    bool                    m_shouldPerformPhotonId;        ///< Whether to run full photon id on the newly formed cluster list
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusterPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new ClusterPreparationAlgorithm();
}

#endif // #ifndef CLUSTER_PREPARATION_ALGORITHM_H
