/**
 *  @file   PandoraPFANew/include/Algorithms/FinalParticleIdAlgorithm.h
 * 
 *  @brief  Header file for the final particle id algorithm class.
 * 
 *  $Log: $
 */
#ifndef FINAL_PARTICLE_ID_ALGORITHM_H
#define FINAL_PARTICLE_ID_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  FinalParticleIdAlgorithm class
 */
class FinalParticleIdAlgorithm : public pandora::Algorithm
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
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *FinalParticleIdAlgorithm::Factory::CreateAlgorithm() const
{
    return new FinalParticleIdAlgorithm();
}

#endif // #ifndef FINAL_PARTICLE_ID_ALGORITHM_H
