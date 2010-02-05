/**
 *  @file   PandoraPFANew/include/Algorithms/Cheating/CheatingClusteringAlgorithm.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef CHEATING_CLUSTERING_ALGORITHM_H
#define CHEATING_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief CheatingClusteringAlgorithm class
 */
class CheatingClusteringAlgorithm : public pandora::Algorithm
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
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingClusteringAlgorithm();
}

#endif // #ifndef CHEATING_CLUSTERING_ALGORITHM_H
