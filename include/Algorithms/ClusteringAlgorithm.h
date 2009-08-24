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
private:
	/**
	 *	@brief Run the clustering algorithm
	 */
	StatusCode Run();
};

#endif // #ifndef CLUSTERING_ALGORITHM_H
