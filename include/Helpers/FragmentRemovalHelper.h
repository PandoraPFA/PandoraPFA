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
     *  @param  closestDistanceToHit to receive the closest distance between the helix and a hit in the speicified range of the cluster
     *  @param  meanDistanceToHits to receive the mean distance between the helix and hits in the specified range of the cluster
     */
    static StatusCode GetClusterHelixDistance(const Cluster *const pCluster, const Helix *const pHelix, const PseudoLayer startLayer,
        const PseudoLayer endLayer, const unsigned int maxOccupiedLayers, float &closestDistanceToHit, float &meanDistanceToHits);

    /**
     *  @brief  Get the number of contact layers for two clusters and also the ratio of the number of contact layers to overlap layers
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     *  @param  distanceThreshold the distance, used alongside the calorimeter cell sizes, to determine whether layers are in contact
     *  @param  nContactLayers to receive the number of contact layers
     *  @param  contactFraction to receive the ratio of the number of contact layers to number of overlap layers
     */
    static StatusCode GetClusterContactDetails(const Cluster *const pClusterI, const Cluster *const pClusterJ, const float distanceThreshold,
        unsigned int &nContactLayers, float &contactFraction);
};

} // namespace pandora

#endif // #ifndef FRAGMENT_REMOVAL_HELPER_H
