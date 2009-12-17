/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/IsolatedHitMergingAlgorithm.h
 * 
 *  @brief  Header file for the isolated hit merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef ISOLATED_HIT_MERGING_ALGORITHM_H
#define ISOLATED_HIT_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  IsolatedHitMergingAlgorithm class
 */
class IsolatedHitMergingAlgorithm : public pandora::Algorithm
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
     *  @brief  Get closest distance between a specified calo hit and a non-isolated hit in a specified cluster
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     * 
     *  @return The closest distance between the calo hit and a non-isolated hit in the cluster
     */
    float GetDistanceToHit(const pandora::Cluster *const pCluster, const pandora::CaloHit *const pCaloHit) const;

    bool            m_shouldUsePhotonClusters;          ///< Whether to match soft cluster candidates to both input and photon clusters
    std::string     m_photonClusterListName;            ///< The name of the photon cluster list to use

    unsigned int    m_minHitsInCluster;                 ///< The min number of hits allowed in a cluster - smaller clusters will be split up
    float           m_maxRecombinationDistance;         ///< The max distance between calo hit and cluster to allow addition of hit
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *IsolatedHitMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new IsolatedHitMergingAlgorithm();
}

#endif // #ifndef ISOLATED_HIT_MERGING_ALGORITHM_H
