/**
 *  @file   PandoraPFANew/include/Algorithms/Clustering/ForcedClusteringAlgorithm.h
 * 
 *  @brief  Header file for the forced clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef FORCED_CLUSTERING_ALGORITHM_H
#define FORCED_CLUSTERING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ForcedClusteringAlgorithm class
 */
class ForcedClusteringAlgorithm : public pandora::Algorithm
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

    typedef std::pair<pandora::CaloHit *, float> CaloHitDistancePair;
    typedef std::vector<CaloHitDistancePair> CaloHitDistanceVector;

    /**
     *  @brief  Sort calo hit distance pairs by increasing distance from track seed
     * 
     *  @param  lhs the first calo hit distance pair
     *  @param  rhs the second calo hit distance pair
     */
    static bool SortByDistanceToTrackSeed(const CaloHitDistancePair &lhs, const CaloHitDistancePair &rhs);

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the ecal surface
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  distance to receive the distance
     */
    StatusCode GetDistanceToTrackSeed(pandora::Cluster *const pCluster, pandora::CaloHit *const pCaloHit, float &distance) const;

    float           m_trackPathWidth;                       ///< Track path width, to determine whether hits are associated with seed track
    float           m_maxTrackSeedSeparation;               ///< Maximum distance between a calo hit and track seed
    float           m_additionalPadWidthsECal;              ///< ECal adjacent pad widths used to calculate cone approach distance
    float           m_additionalPadWidthsHCal;              ///< HCal adjacent pad widths used to calculate cone approach distance

    bool            m_shouldRunStandardClusteringAlgorithm; ///< Whether to run a standard clustering algorithm to deal with remnants
    std::string     m_standardClusteringAlgorithmName;      ///< The name of the standard clustering algorithm to run

    bool            m_shouldClusterIsolatedHits;            ///< Whether to directly include isolated hits in newly formed clusters
    bool            m_shouldAssociateIsolatedHits;          ///< Whether to associate isolated hits to newly formed clusters
    std::string     m_isolatedHitAssociationAlgorithmName;  ///< The name of isolated hit association algorithm
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ForcedClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ForcedClusteringAlgorithm();
}

#endif // #ifndef FORCED_CLUSTERING_ALGORITHM_H
