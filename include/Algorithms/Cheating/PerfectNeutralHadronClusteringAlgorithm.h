/**
 *  @file   PandoraPFANew/include/Algorithms/Cheating/PerfectNeutralHadronClusteringAlgorithm.h
 * 
 *  @brief  Header file for the cheating neutral hadron clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PERFECT_NEUTRAL_HADRON_CLUSTERING_ALGORITHM_H
#define PERFECT_NEUTRAL_HADRON_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Cheating/PerfectClusteringAlgorithm.h"

/**
 *  @brief PerfectNeutralHadronClusteringAlgorithm class
 */
class PerfectNeutralHadronClusteringAlgorithm : public PerfectClusteringAlgorithm
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
    virtual bool SelectCaloHitsOfMcParticleForClustering( const pandora::MCParticle* pMcParticle ) const;


private:

};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectNeutralHadronClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectNeutralHadronClusteringAlgorithm();
}

#endif // #ifndef PERFECT_NEUTRAL_HADRON_CLUSTERING_ALGORITHM_H
