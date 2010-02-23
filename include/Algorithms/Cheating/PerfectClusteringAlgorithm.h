/**
 *  @file   PandoraPFANew/include/Algorithms/Cheating/PerfectClusteringAlgorithm.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PERFECT_CLUSTERING_ALGORITHM_H
#define PERFECT_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief PerfectClusteringAlgorithm class
 */
class PerfectClusteringAlgorithm : public pandora::Algorithm
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
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    std::string     m_clusterListName;                  ///< if clusterListName is set, cluster-list are stored with this name
    std::string     m_orderedCaloHitListName;           ///< if orderedCaloHitListName is set, the orderedCaloHitList containing the remaining hits are stored and set current
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectClusteringAlgorithm();
}

#endif // #ifndef PERFECT_CLUSTERING_ALGORITHM_H
