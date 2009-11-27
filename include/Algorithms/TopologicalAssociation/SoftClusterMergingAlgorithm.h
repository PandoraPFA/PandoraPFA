/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/SoftClusterMergingAlgorithm.h
 * 
 *  @brief  Header file for the soft cluster merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef SOFT_CLUSTER_MERGING_ALGORITHM_H
#define SOFT_CLUSTER_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  SoftClusterMergingAlgorithm class
 */
class SoftClusterMergingAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *SoftClusterMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new SoftClusterMergingAlgorithm();
}

#endif // #ifndef SOFT_CLUSTER_MERGING_ALGORITHM_H
