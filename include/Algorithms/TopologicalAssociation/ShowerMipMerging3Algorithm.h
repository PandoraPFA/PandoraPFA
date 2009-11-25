/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ShowerMipMerging3Algorithm.h
 * 
 *  @brief  Header file for the shower mip merging 3 algorithm class.
 * 
 *  $Log: $
 */
#ifndef SHOWER_MIP_MERGING_3_ALGORITHM_H
#define SHOWER_MIP_MERGING_3_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ShowerMipMerging3Algorithm class
 */
class ShowerMipMerging3Algorithm : public pandora::Algorithm
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

inline pandora::Algorithm *ShowerMipMerging3Algorithm::Factory::CreateAlgorithm() const
{
    return new ShowerMipMerging3Algorithm();
}

#endif // #ifndef SHOWER_MIP_MERGING_3_ALGORITHM_H
