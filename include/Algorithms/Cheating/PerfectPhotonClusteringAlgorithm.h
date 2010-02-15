/**
 *  @file   PandoraPFANew/include/Algorithms/Cheating/PerfectPhotonClusteringAlgorithm.h
 * 
 *  @brief  Header file for the cheating photon clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PERFECT_PHOTON_CLUSTERING_ALGORITHM_H
#define PERFECT_PHOTON_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Cheating/PerfectClusteringAlgorithm.h"

/**
 *  @brief PerfectPhotonClusteringAlgorithm class
 */
class PerfectPhotonClusteringAlgorithm : public PerfectClusteringAlgorithm
{
private:
public:


    class pandora::MCParticle;


    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

protected:
    virtual bool SelectCaloHitsOfMcParticleForClustering( pandora::MCParticle* pMcParticle ) const;


private:

};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectPhotonClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectPhotonClusteringAlgorithm();
}

#endif // #ifndef PERFECT_PHOTON_CLUSTERING_ALGORITHM_H
