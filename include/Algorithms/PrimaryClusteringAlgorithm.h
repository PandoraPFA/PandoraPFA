/**
 *  @file   PandoraPFANew/include/Algorithms/PrimaryClusteringAlgorithm.h
 * 
 *  @brief  Header file for the primary clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PRIMARY_CLUSTERING_ALGORITHM_H
#define PRIMARY_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  PrimaryClusteringAlgorithm class
 */
class PrimaryClusteringAlgorithm : public pandora::Algorithm
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

    std::string     m_clusteringAlgorithmName;      ///< The name of the clustering algorithm to run
    std::string     m_clusterListName;              ///< The name under which to save the new cluster list
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PrimaryClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new PrimaryClusteringAlgorithm();
}

#endif // #ifndef PRIMARY_CLUSTERING_ALGORITHM_H
