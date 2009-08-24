/**
 *	@file	PandoraPFANew/include/Algorithms/PrimaryClusteringAlgorithm.h
 * 
 *	@brief	Header file for the primary clustering algorithm class.
 * 
 *	$Log: $
 */
#ifndef PRIMARY_CLUSTERING_ALGORITHM_H
#define PRIMARY_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *	@brief PrimaryClusteringAlgorithm class
 */
class PrimaryClusteringAlgorithm : public pandora::Algorithm
{
private:
	/**
	 *	@brief Run the initial clustering algorithm
	 */
	StatusCode Run();
};

#endif // #ifndef PRIMARY_CLUSTERING_ALGORITHM_H
