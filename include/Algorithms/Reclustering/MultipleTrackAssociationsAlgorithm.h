/**
 *  @file   PandoraPFANew/include/Algorithms/Reclustering/MultipleTrackAssociationsAlgorithm.h
 * 
 *  @brief  Header file for the multiple track associations algorithm class.
 * 
 *  $Log: $
 */
#ifndef MULTIPLE_TRACK_ASSOCIATIONS_ALGORITHM_H
#define MULTIPLE_TRACK_ASSOCIATIONS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  MultipleTrackAssociationsAlgorithm class
 */
class MultipleTrackAssociationsAlgorithm : public pandora::Algorithm
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

    pandora::StringVector   m_clusteringAlgorithms;         ///< The ordered list of clustering algorithms to be used
    std::string             m_associationAlgorithmName;     ///< The name of the topological association algorithm to run
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MultipleTrackAssociationsAlgorithm::Factory::CreateAlgorithm() const
{
    return new MultipleTrackAssociationsAlgorithm();
}

#endif // #ifndef MULTIPLE_TRACK_ASSOCIATIONS_ALGORITHM_H
