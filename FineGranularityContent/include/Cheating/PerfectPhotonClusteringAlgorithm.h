/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Cheating/PerfectPhotonClusteringAlgorithm.h
 * 
 *  @brief  Header file for the cheating photon clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PERFECT_PHOTON_CLUSTERING_ALGORITHM_H
#define PERFECT_PHOTON_CLUSTERING_ALGORITHM_H 1

#include "Cheating/PerfectClusteringAlgorithm.h"

/**
 *  @brief PerfectPhotonClusteringAlgorithm class
 */
class PerfectPhotonClusteringAlgorithm : public PerfectClusteringAlgorithm
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

protected:
    virtual bool SelectMCParticlesForClustering(const pandora::MCParticle *pMCParticle) const;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectPhotonClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectPhotonClusteringAlgorithm();
}

#endif // #ifndef PERFECT_PHOTON_CLUSTERING_ALGORITHM_H
