/**
 *  @file   PandoraPFANew/include/Helpers/FragmentRemovalHelper.h
 * 
 *  @brief  Header file for the fragment removal helper class.
 * 
 *  $Log: $
 */
#ifndef FRAGMENT_REMOVAL_HELPER_H
#define FRAGMENT_REMOVAL_HELPER_H 1

#include "Pandora/PandoraInternal.h"

#include "Objects/CartesianVector.h"

#include "StatusCodes.h"

namespace pandora
{

/**
 *  @brief  FragmentRemovalHelper class
 */
class FragmentRemovalHelper
{
public:
    /**
     *  @brief  Get the fraction of calo hits in cluster I that lie within a specified distance of a calo hit in cluster J
     * 
     *  @param  pClusterI address of the cluster for which the fraction is calculated
     *  @param  pClusterJ address of the cluster used in the comparison
     *  @param  distanceThreshold the specified distance threshold
     * 
     *  @return The fraction of close calo hits
     */
    static float GetFractionOfCloseHits(const Cluster *const pClusterI, const Cluster *const pClusterJ, const float distanceThreshold);

    /**
     *  @brief  Get the fraction of calo hits in cluster I that lie within a cone along the direction of cluster J.
     *          If cluster J has an associated track, it's projected position and direction at the ecal will be used to define cone,
     *          otherwise cone will be radial and its axis will pass through centroid of cluster J at its showermax layer.
     * 
     *  @param  pClusterI address of the cluster for which the fraction is calculated
     *  @param  pClusterJ address of the cluster used in the comparison
     *  @param  coneCosineHalfAngle the cone cosine half angle
     * 
     *  @return The fraction of calo hits in the cone
     */
    static float GetFractionOfHitsInCone(const Cluster *const pClusterI, Cluster *const pClusterJ, const float coneCosineHalfAngle);

    /**
     *  @brief  Get the fraction of calo hits in a cluster that lie within a cone along the direction of a specified track
     * 
     *  @param  pCluster address of the cluster for which the fraction is calculated
     *  @param  pTrack address of the cluster used in the comparison
     *  @param  coneCosineHalfAngle the cone cosine half angle
     * 
     *  @return The fraction of calo hits in the cone
     */
    static float GetFractionOfHitsInCone(const Cluster *const pCluster, const Track *const pTrack, const float coneCosineHalfAngle);

    /**
     *  @brief  Get the fraction of calo hits in a cluster that lie within a specified cone
     * 
     *  @param  pCluster address of the cluster for which the fraction is calculated
     *  @param  coneApex position vector specifying cone apex
     *  @param  coneDirection unit vector specifying cone direction
     *  @param  coneCosineHalfAngle the cone cosine half angle
     * 
     *  @return The fraction of calo hits in the cone
     */
    static float GetFractionOfHitsInCone(const Cluster *const pCluster, const CartesianVector &coneApex, const CartesianVector &coneDirection,
        const float coneCosineHalfAngle);

    /**
     *  @brief  Get the number of pseudo layers crossed by helix in specified range of z coordinates
     * 
     *  @param  pHelix address of the helix
     *  @param  zStart start z coordinate
     *  @param  zEnd end z coordinate
     *  @param  nSamplingPoints number of points at which to sample the helix in the z interval
     * 
     *  @return The number of pseudo layers crossed
     */
    static PseudoLayer GetNLayersCrossed(const Helix *const pHelix, const float zStart, const float zEnd, const unsigned int nSamplingPoints = 100);

    /**
     *  @brief  Get the distance between hits in a cluster and a helix, typically the result of a fit to a track
     * 
     *  @param  pCluster address of the cluster
     *  @param  pHelix address of the helix
     *  @param  startLayer the first pseudo layer of the cluster to examine
     *  @param  endLayer the last pseudo layer of the cluster to examine
     *  @param  maxOccupiedLayers the maximum number of occupied cluster pseudo layers to examine
     *  @param  closestDistanceToHit to receive the closest distance between the helix and a hit in the specified range of the cluster
     *  @param  meanDistanceToHits to receive the mean distance between the helix and hits in the specified range of the cluster
     */
    static StatusCode GetClusterHelixDistance(const Cluster *const pCluster, const Helix *const pHelix, const PseudoLayer startLayer,
        const PseudoLayer endLayer, const unsigned int maxOccupiedLayers, float &closestDistanceToHit, float &meanDistanceToHits);

    /**
     *  @brief  Get the number of contact layers for two clusters and also the ratio of the number of contact layers to overlap layers
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     *  @param  distanceThreshold number of calorimeter cell-widths used to determine whether layers are in contact
     *  @param  nContactLayers to receive the number of contact layers
     *  @param  contactFraction to receive the ratio of the number of contact layers to number of overlap layers
     */
    static StatusCode GetClusterContactDetails(const Cluster *const pClusterI, const Cluster *const pClusterJ, const float distanceThreshold,
        unsigned int &nContactLayers, float &contactFraction);
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ClusterContact class, describing the interactions and proximity between parent and daughter candidate clusters
 */
class ClusterContact
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  pParentCluster address of the parent candidate cluster
     */
    ClusterContact(Cluster *const pDaughterCluster, Cluster *const pParentCluster);

    /**
     *  @brief  Get the address of the daughter candidate cluster
     * 
     *  @return The address of the daughter candidate cluster
     */
    Cluster *GetDaughterCluster() const;

    /**
     *  @brief  Get the address of the parent candidate cluster
     * 
     *  @return The address of the parent candidate cluster
     */
    Cluster *GetParentCluster() const;

    /**
     *  @brief  Get the parent cluster hadronic energy
     * 
     *  @return The parent cluster hadronic energy
     */
    float GetParentClusterEnergy() const;

    /**
     *  @brief  Get the sum of energies of tracks associated with parent cluster
     * 
     *  @return The sum of energies of tracks associated with parent cluster
     */
    float GetParentTrackEnergy() const;

    /**
     *  @brief  Get the number of contact layers for parent and daughter clusters two clusters
     * 
     *  @return The number of contact layers
     */
    unsigned int GetNContactLayers() const;

    /**
     *  @brief  Get the ratio of the number of contact layers to the number of overlap layers
     * 
     *  @return The ratio of contact layers to overlap layers
     */
    float GetContactFraction() const;

    /**
     *  @brief  Distance between closest hits in parent and daughter clusters, units mm
     * 
     *  @return The distance between closest hits
     */
    float GetDistanceToClosestHit() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within sepcified distance 1 of parent cluster
     * 
     *  @return The daughter close hit fraction
     */
    float GetCloseHitFraction1() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within sepcified distance 2 of parent cluster
     * 
     *  @return The daughter close hit fraction
     */
    float GetCloseHitFraction2() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within specified cone 1 along parent direction
     * 
     *  @return The daughter cone fraction
     */
    float GetConeFraction1() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within specified cone 2 along parent direction
     * 
     *  @return The daughter cone fraction
     */
    float GetConeFraction2() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within specified cone 3 along parent direction
     * 
     *  @return The daughter cone fraction
     */
    float GetConeFraction3() const;

    /**
     *  @brief  Get the mean distance of daughter cluster from closest helix fix to parent associated tracks
     * 
     *  @return The mean daughter distance to the closest helix
     */
    float GetMeanDistanceToHelix() const;

    /**
     *  @brief  Get the closest distance between daughter cluster and helix fits to parent associated tracks
     * 
     *  @return The closest daughter distance to helix
     */
    float GetClosestDistanceToHelix() const;

private:
    /**
     *  @brief  Compare daughter cluster with helix fits to parent associated tracks
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  pParentCluster address of the parent candidate cluster
     */
    void ClusterHelixComparison(Cluster *const pDaughterCluster, Cluster *const pParentCluster);

    Cluster        *m_pDaughterCluster;             ///< Address of the daughter candidate cluster
    Cluster        *m_pParentCluster;               ///< Address of the parent candidate cluster

    float           m_parentClusterEnergy;          ///< The parent cluster hadronic energy
    float           m_parentTrackEnergy;            ///< Sum of energies of tracks associated with parent cluster

    unsigned int    m_nContactLayers;               ///< The number of contact layers for parent and daughter clusters two clusters
    float           m_contactFraction;              ///< The ratio of the number of contact layers to the number of overlap layers

    float           m_distanceToClosestHit;         ///< Distance between closest hits in parent and daughter clusters, units mm
    float           m_closeHitFraction1;            ///< Fraction of daughter hits that lie within sepcified distance 1 of parent cluster
    float           m_closeHitFraction2;            ///< Fraction of daughter hits that lie within sepcified distance 2 of parent cluster

    float           m_coneFraction1;                ///< Fraction of daughter hits that lie within specified cone 1 along parent direction
    float           m_coneFraction2;                ///< Fraction of daughter hits that lie within specified cone 2 along parent direction
    float           m_coneFraction3;                ///< Fraction of daughter hits that lie within specified cone 3 along parent direction

    float           m_meanDistanceToHelix;          ///< Mean distance of daughter cluster from closest helix fix to parent associated tracks
    float           m_closestDistanceToHelix;       ///< Closest distance between daughter cluster and helix fits to parent associated tracks
};

typedef std::vector<ClusterContact> ClusterContactVector;
typedef std::map<Cluster *, ClusterContactVector> ClusterContactMap;

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *ClusterContact::GetDaughterCluster() const
{
    return m_pDaughterCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *ClusterContact::GetParentCluster() const
{
    return m_pParentCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetParentClusterEnergy() const
{
    return m_parentClusterEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetParentTrackEnergy() const
{
    return m_parentTrackEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ClusterContact::GetNContactLayers() const
{
    return m_nContactLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetContactFraction() const
{
    return m_contactFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetDistanceToClosestHit() const
{
    return m_distanceToClosestHit;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetCloseHitFraction1() const
{
    return m_closeHitFraction1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetCloseHitFraction2() const
{
    return m_closeHitFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetConeFraction1() const
{
    return m_coneFraction1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetConeFraction2() const
{
    return m_coneFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetConeFraction3() const
{
    return m_coneFraction3;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetMeanDistanceToHelix() const
{
    return m_meanDistanceToHelix;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetClosestDistanceToHelix() const
{
    return m_closestDistanceToHelix;
}

} // namespace pandora

#endif // #ifndef FRAGMENT_REMOVAL_HELPER_H
