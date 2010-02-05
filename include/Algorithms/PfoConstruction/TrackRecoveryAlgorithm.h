/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/TrackRecoveryAlgorithm.h
 * 
 *  @brief  Header file for the track recovery algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_RECOVERY_ALGORITHM_H
#define TRACK_RECOVERY_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TrackRecoveryAlgorithm class
 */
class TrackRecoveryAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *TrackRecoveryAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackRecoveryAlgorithm();
}

#endif // #ifndef TRACK_RECOVERY_ALGORITHM_H
