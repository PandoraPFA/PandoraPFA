/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ShowerMipMergingAlgorithm.h
 * 
 *  @brief  Header file for the shower mip merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef SHOWER_MIP_MERGING_ALGORITHM_H
#define SHOWER_MIP_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ShowerMipMergingAlgorithm class
 */
class ShowerMipMergingAlgorithm : public pandora::Algorithm
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
    StatusCode ReadSettings(TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ShowerMipMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new ShowerMipMergingAlgorithm();
}

#endif // #ifndef SHOWER_MIP_MERGING_ALGORITHM_H
