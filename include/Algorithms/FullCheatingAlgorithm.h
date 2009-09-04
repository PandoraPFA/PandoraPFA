/**
 *	@file	PandoraPFANew/include/Algorithms/FullCheatingAlgorithm.h
 * 
 *	@brief	Header file for the clustering algorithm class.
 * 
 *	$Log: $
 */
#ifndef FULL_CHEATING_ALGORITHM_H
#define FULL_CHEATING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *	@brief FullCheatingAlgorithm class
 */
class FullCheatingAlgorithm : public pandora::Algorithm
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

    std::string     m_clusteringAlgorithmName;      ///< The name of the clustering algorithm to run
    std::string     m_fullCheatingListName;         ///< The name under which to save the new fully cheated cluster list
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *FullCheatingAlgorithm::Factory::CreateAlgorithm() const
{
    return new FullCheatingAlgorithm();
}



#endif // #ifndef CLUSTERING_ALGORITHM_H
