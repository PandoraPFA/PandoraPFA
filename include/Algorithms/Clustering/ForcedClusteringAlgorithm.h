/**
 *  @file   PandoraPFANew/include/Algorithms/Clustering/ForcedClusteringAlgorithm.h
 * 
 *  @brief  Header file for the forced clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef FORCED_CLUSTERING_ALGORITHM_H
#define FORCED_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ForcedClusteringAlgorithm class
 */
class ForcedClusteringAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *ForcedClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ForcedClusteringAlgorithm();
}

#endif // #ifndef FORCED_CLUSTERING_ALGORITHM_H
