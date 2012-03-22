/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Cheating/PerfectClusteringAlgorithm.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PERFECT_CLUSTERING_ALGORITHM_H
#define PERFECT_CLUSTERING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief PerfectClusteringAlgorithm class
 */
class PerfectClusteringAlgorithm : public pandora::Algorithm
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
    virtual bool SelectMCParticlesForClustering(const pandora::MCParticle *const pMCParticle) const;

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    pandora::IntVector  m_particleIdList;               ///< list of particle ids of MCPFOs to be selected
    bool                m_shouldUseOnlyECalHits;        ///< Whether to only use ecal hits in the clustering algorithm
    bool                m_shouldUseIsolatedHits;        ///< Whether to use isolated hits in the clustering algorithm
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectClusteringAlgorithm();
}

#endif // #ifndef PERFECT_CLUSTERING_ALGORITHM_H
