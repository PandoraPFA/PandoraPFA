/**
 *  @file   PandoraPFANew/include/Algorithms/Cheating/CheatingclustermergingAlgorithm.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef CHEATING_CLUSTER_MERGING_ALGORITHM_H
#define CHEATING_CLUSTER_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief CheatingClusterMergingAlgorithm class
 */
class CheatingClusterMergingAlgorithm : public pandora::Algorithm
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
     *  @brief  Find the mc particle making the largest electromagnetic energy contribution to a specified cluster
     * 
     *  @param  pCluster address of the cluster to examine
     * 
     *  @return address of the main mc particle
     */
    const pandora::MCParticle *GetMainMCParticle(const pandora::Cluster *const pCluster) const;

    typedef std::map<const pandora::MCParticle*, pandora::Cluster*> MCParticleToClusterMap;
    typedef std::map<const pandora::MCParticle*, float> MCParticleToFloatMap;

    pandora::StringVector   m_clusterListNames;             ///< use the given cluster lists to search for clusters to merge

    bool                    m_debug;                        ///< turn on additional debugging output
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingClusterMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingClusterMergingAlgorithm();
}

#endif // #ifndef CHEATING_CLUSTER_MERGING_ALGORITHM_H
