/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/ShowerMipMerging4Algorithm.h
 * 
 *  @brief  Header file for the shower mip merging 4 algorithm class.
 * 
 *  $Log: $
 */
#ifndef SHOWER_MIP_MERGING_4_ALGORITHM_H
#define SHOWER_MIP_MERGING_4_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ShowerMipMerging4Algorithm class
 */
class ShowerMipMerging4Algorithm : public pandora::Algorithm
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

inline pandora::Algorithm *ShowerMipMerging4Algorithm::Factory::CreateAlgorithm() const
{
    return new ShowerMipMerging4Algorithm();
}

#endif // #ifndef SHOWER_MIP_MERGING_4_ALGORITHM_H
