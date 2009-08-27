/**
 *	@file	PandoraPFANew/include/Algorithms/ClusteringAlgorithm.h
 * 
 *	@brief	Header file for the clustering algorithm class.
 * 
 *	$Log: $
 */
#ifndef CLUSTERING_ALGORITHM_H
#define CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *	@brief ClusteringAlgorithm class
 */
class ClusteringAlgorithm : public pandora::Algorithm
{
public:
	/**
	 *	@brief	Factory class for instantiating algorithm
	 */
	class Factory : public pandora::AlgorithmFactory
	{
	public:
		Algorithm *CreateAlgorithm() const;
	};	
	
private:
	StatusCode Run();
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusteringAlgorithm::Factory::CreateAlgorithm() const
{
	return new ClusteringAlgorithm();
}

#endif // #ifndef CLUSTERING_ALGORITHM_H
