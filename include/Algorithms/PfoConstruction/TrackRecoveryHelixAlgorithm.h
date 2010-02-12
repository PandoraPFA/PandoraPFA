/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/TrackRecoveryHelixAlgorithm.h
 * 
 *  @brief  Header file for the track recovery helix algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_RECOVERY_HELIX_ALGORITHM_H
#define TRACK_RECOVERY_HELIX_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

using namespace pandora;

/**
 *  @brief  TrackRecoveryHelixAlgorithm class
 */
class TrackRecoveryHelixAlgorithm : public Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    /**
     *  @brief  AssociationInfo class
     */
    class AssociationInfo
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pCluster address of cluster to which association could be made
         *  @param  closestApproach distance of closest approach between the cluster and the track under consideration
         */
        AssociationInfo(Cluster *const pCluster, const float closestApproach);

        /**
         *  @brief  Get the address of the cluster to which association could be made
         * 
         *  @return The address of the cluster
         */
        Cluster *GetCluster() const;

        /**
         *  @brief  Get the distance of closest approach between the cluster and the track under consideration
         * 
         *  @return The distance of closest approach
         */
        float GetClosestApproach() const;

    private:
        Cluster    *m_pCluster;                         ///< 
        float       m_closestApproach;                  ///< 
    };

    typedef std::vector<AssociationInfo> AssociationInfoVector;
    typedef std::map<Track *, AssociationInfoVector> TrackAssociationInfoMap;

    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  Get a map specifying cluster association information for every possible matching cluster
     * 
     *  @param  trackAssociationInfoMap the track association info map
     */
    StatusCode GetTrackAssociationInfoMap(TrackAssociationInfoMap &trackAssociationInfoMap) const;

    /**
     *  @brief  Use information in the track association info map to create track to cluster associations
     * 
     *  @param  trackAssociationInfoMap the track association info map
     */
    StatusCode MakeTrackClusterAssociations(TrackAssociationInfoMap &trackAssociationInfoMap) const;

    float           m_maxTrackClusterDeltaZ;            ///< Max z separation between track ecal projection and cluster to allow association
    float           m_maxAbsoluteTrackClusterChi;       ///< Max absolute track-cluster consistency chi value to allow association
    unsigned int    m_maxLayersCrossed;                 ///< Max number of layers crossed by track helix between ecal projection and cluster

    unsigned int    m_maxSearchLayer;                   ///< Max pseudo layer to examine when calculating track-cluster distance
    float           m_parallelDistanceCut;              ///< Max allowed projection of track-hit separation along track direction

    unsigned int    m_helixComparisonNLayers;           ///< Number of cluster layers used in cluster-helix comparison
    unsigned int    m_helixComparisonMaxOccupiedLayers; ///< Max number of occupied cluster layers used in cluster-helix comparison

    float           m_maxTrackClusterDistance;          ///< Max track-cluster separation to allow association
    float           m_maxClosestHelixClusterDistance;   ///< Max helix-cluster closest approach to allow association
    float           m_maxMeanHelixClusterDistance;      ///< Max helix-cluster mean approach to allow association
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackRecoveryHelixAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackRecoveryHelixAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline TrackRecoveryHelixAlgorithm::AssociationInfo::AssociationInfo(Cluster *const pCluster, const float closestApproach) :
    m_pCluster(pCluster),
    m_closestApproach(closestApproach)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *TrackRecoveryHelixAlgorithm::AssociationInfo::GetCluster() const
{
    return m_pCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TrackRecoveryHelixAlgorithm::AssociationInfo::GetClosestApproach() const
{
    return m_closestApproach;
}

#endif // #ifndef TRACK_RECOVERY_HELIX_ALGORITHM_H
