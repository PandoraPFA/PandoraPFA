/**
 *  @file   PandoraPFANew/Algorithms/include/ReclusteringAlgorithm.h
 * 
 *  @brief  Header file for the reclustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef RECLUSTERING_ALGORITHM_H
#define RECLUSTERING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

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
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    pandora::StringVector   m_reclusteringAlgorithms;       ///< The ordered list of reclustering algorithms to be used
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ReclusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ReclusteringAlgorithm();
}

#endif // #ifndef RECLUSTERING_ALGORITHM_H
