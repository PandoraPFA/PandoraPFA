/**
 *  @file   PandoraPFANew/include/Algorithms/PerfectClusteringAlgorithm.h
 * 
 *  @brief  Header file for the perfect clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PERFECT_CLUSTERING_ALGORITHM_H
#define PERFECT_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief PerfectClusteringAlgorithm class
 */
class PerfectClusteringAlgorithm : public pandora::Algorithm
{
private:
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
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectClusteringAlgorithm();
}

#endif // #ifndef FULL_CHEATING_CLUSTERING_H
