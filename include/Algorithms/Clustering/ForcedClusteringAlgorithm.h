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

#include "Objects/CartesianVector.h"

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
     *  @brief  Sort calo hit distance pairs by increasing distance from track projection
     * 
     *  @param  lhs the first calo hit distance pair
     *  @param  rhs the second calo hit distance pair
     */
    static bool SortByDistanceToTrack(const CaloHitDistancePair &lhs, const CaloHitDistancePair &rhs);

    bool            m_shouldRunStandardClusteringAlgorithm; ///< Whether to run a standard clustering algorithm to deal with remnants
    std::string     m_standardClusteringAlgorithmName;      ///< The name of the standard clustering algorithm to run

    bool            m_shouldClusterIsolatedHits;            ///< Whether to directly include isolated hits in newly formed clusters
    bool            m_shouldAssociateIsolatedHits;          ///< Whether to associate isolated hits to newly formed clusters
    std::string     m_isolatedHitAssociationAlgorithmName;  ///< The name of isolated hit association algorithm
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ForcedClusteringAlgorithm::SortByDistanceToTrack(const CaloHitDistancePair &lhs, const CaloHitDistancePair &rhs)
{
    return (lhs.second < rhs.second);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ForcedClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ForcedClusteringAlgorithm();
}

#endif // #ifndef FORCED_CLUSTERING_ALGORITHM_H
