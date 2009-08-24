/**
 *	@file	PandoraPFANew/include/Algorithms/ReclusteringAlgorithm.h
 * 
 *	@brief	Header file for the reclustering algorithm class.
 * 
 *	$Log: $
 */
#ifndef RECLUSTERING_ALGORITHM_H
#define RECLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *	@brief ReclusteringAlgorithm class
 */
class ReclusteringAlgorithm : public pandora::Algorithm
{
private:
	/**
	 *	@brief Run the reclustering algorithm
	 */
	StatusCode Run();
};

#endif // #ifndef RECLUSTERING_ALGORITHM_H
