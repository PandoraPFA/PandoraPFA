/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/TrackRecoveryInteractionsAlgorithm.h
 * 
 *  @brief  Header file for the track recovery interactions algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_RECOVERY_INTERACTIONS_ALGORITHM_H
#define TRACK_RECOVERY_INTERACTIONS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TrackRecoveryInteractionsAlgorithm class
 */
class TrackRecoveryInteractionsAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *TrackRecoveryInteractionsAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackRecoveryInteractionsAlgorithm();
}

#endif // #ifndef TRACK_RECOVERY_INTERACTIONS_ALGORITHM_H
