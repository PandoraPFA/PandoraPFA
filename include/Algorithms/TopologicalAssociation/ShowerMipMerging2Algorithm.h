/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ShowerMipMerging2Algorithm.h
 * 
 *  @brief  Header file for the shower mip merging 2 algorithm class.
 * 
 *  $Log: $
 */
#ifndef SHOWER_MIP_MERGING_2_ALGORITHM_H
#define SHOWER_MIP_MERGING_2_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ShowerMipMerging2Algorithm class
 */
class ShowerMipMerging2Algorithm : public pandora::Algorithm
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

inline pandora::Algorithm *ShowerMipMerging2Algorithm::Factory::CreateAlgorithm() const
{
    return new ShowerMipMerging2Algorithm();
}

#endif // #ifndef SHOWER_MIP_MERGING_2_ALGORITHM_H
