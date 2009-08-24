/**
 *	@file	PandoraPFANew/include/Algorithms/PhotonClusteringAlgorithm.h
 * 
 *	@brief	Header file for the photon clustering algorithm class.
 * 
 *	$Log: $
 */
#ifndef PHOTON_CLUSTERING_ALGORITHM_H
#define PHOTON_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *	@brief PhotonClusteringAlgorithm class
 */
class PhotonClusteringAlgorithm : public pandora::Algorithm
{
private:
	/**
	 *	@brief Run the photon clustering algorithm
	 */
	StatusCode Run();
};

#endif // #ifndef PHOTON_CLUSTERING_ALGORITHM_H
