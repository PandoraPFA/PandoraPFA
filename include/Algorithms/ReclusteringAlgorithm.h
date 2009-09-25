/**
 *  @file   PandoraPFANew/include/Algorithms/ReclusteringAlgorithm.h
 * 
 *  @brief  Header file for the reclustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef RECLUSTERING_ALGORITHM_H
#define RECLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ReclusteringAlgorithm class
 */
class ReclusteringAlgorithm : public pandora::Algorithm
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

    pandora::StringVector   m_clusteringAlgorithms;         ///< The ordered list of clustering algorithms to be used
    std::string             m_associationAlgorithmName;     ///< The name of the topological association algorithm to run
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ReclusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ReclusteringAlgorithm();
}

#endif // #ifndef RECLUSTERING_ALGORITHM_H
