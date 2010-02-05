/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/TrackRecoveryHelixAlgorithm.h
 * 
 *  @brief  Header file for the track recovery helix algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_RECOVERY_HELIX_ALGORITHM_H
#define TRACK_RECOVERY_HELIX_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TrackRecoveryHelixAlgorithm class
 */
class TrackRecoveryHelixAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *TrackRecoveryHelixAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackRecoveryHelixAlgorithm();
}

#endif // #ifndef TRACK_RECOVERY_HELIX_ALGORITHM_H
