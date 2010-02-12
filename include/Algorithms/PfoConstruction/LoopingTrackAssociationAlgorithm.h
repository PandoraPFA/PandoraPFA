/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/LoopingTrackAssociationAlgorithm.h
 * 
 *  @brief  Header file for the looping track association algorithm class.
 * 
 *  $Log: $
 */
#ifndef LOOPING_TRACK_ASSOCIATION_ALGORITHM_H
#define LOOPING_TRACK_ASSOCIATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  LoopingTrackAssociationAlgorithm class
 */
class LoopingTrackAssociationAlgorithm : public pandora::Algorithm
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

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     */
    float GetMeanDeltaR(pandora::Cluster *const pCluster, const float helixXCentre, const float helixYCentre, const float helixRadius) const;

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *LoopingTrackAssociationAlgorithm::Factory::CreateAlgorithm() const
{
    return new LoopingTrackAssociationAlgorithm();
}

#endif // #ifndef LOOPING_TRACK_ASSOCIATION_ALGORITHM_H
