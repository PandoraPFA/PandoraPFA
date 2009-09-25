/**
 *  @file   PandoraPFANew/include/Algorithms/FullCheatingClusteringAlgorithm.h
 * 
 *  @brief  Header file for the full cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef FULL_CHEATING_CLUSTERING_ALGORITHM_H
#define FULL_CHEATING_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief FullCheatingClusteringAlgorithm class
 */
class FullCheatingClusteringAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *FullCheatingClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new FullCheatingClusteringAlgorithm();
}

#endif // #ifndef FULL_CHEATING_CLUSTERING_ALGORITHM_H
