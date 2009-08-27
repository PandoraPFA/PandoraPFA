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

inline pandora::Algorithm *ReclusteringAlgorithm::Factory::CreateAlgorithm() const
{
	return new ReclusteringAlgorithm();
}

#endif // #ifndef RECLUSTERING_ALGORITHM_H
