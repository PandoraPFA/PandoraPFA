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
	StatusCode ReadSettings(TiXmlHandle xmlHandle);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonClusteringAlgorithm::Factory::CreateAlgorithm() const
{
	return new PhotonClusteringAlgorithm();
}

#endif // #ifndef PHOTON_CLUSTERING_ALGORITHM_H
