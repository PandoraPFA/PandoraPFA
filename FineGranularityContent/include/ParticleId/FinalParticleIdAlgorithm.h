/**
 *  @file   PandoraPFANew/FineGranularityContent/include/ParticleId/FinalParticleIdAlgorithm.h
 * 
 *  @brief  Header file for the final particle id algorithm class.
 * 
 *  $Log: $
 */
#ifndef FINAL_PARTICLE_ID_ALGORITHM_H
#define FINAL_PARTICLE_ID_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

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
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Determine whether any track in a specified tracklist has a sibling
     * 
     *  @param  trackList the tracklist
     * 
     *  @return boolean
     */
    bool ContainsSiblingTrack(const pandora::TrackList &trackList) const;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *FinalParticleIdAlgorithm::Factory::CreateAlgorithm() const
{
    return new FinalParticleIdAlgorithm();
}

#endif // #ifndef FINAL_PARTICLE_ID_ALGORITHM_H
