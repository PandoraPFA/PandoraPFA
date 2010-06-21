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
	
    class pandora::MCParticle;
    class pandora::Cluster;

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

    const pandora::MCParticle* GetMainMCParticle(const pandora::Cluster* pCluster);

    pandora::StringVector   m_clusterListNames;             ///< use the given cluster lists to search for clusters to merge
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingClusterMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingClusterMergingAlgorithm();
}

#endif // #ifndef CHEATING_CLUSTER_MERGING_ALGORITHM_H
