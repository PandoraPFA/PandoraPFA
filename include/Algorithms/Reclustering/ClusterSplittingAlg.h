/**
 *  @file   PandoraPFANew/include/Algorithms/Reclustering/ClusterSplittingAlg.h
 * 
 *  @brief  Header file for the cluster splitting algorithm class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_SPLITTING_ALGORITHM_H
#define CLUSTER_SPLITTING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ClusterSplittingAlg class
 */
class ClusterSplittingAlg : public pandora::Algorithm
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

inline pandora::Algorithm *ClusterSplittingAlg::Factory::CreateAlgorithm() const
{
    return new ClusterSplittingAlg();
}

#endif // #ifndef CLUSTER_SPLITTING_ALGORITHM_H
