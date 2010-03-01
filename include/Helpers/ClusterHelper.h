/**
 *  @file   PandoraPFANew/include/Helpers/ClusterHelper.h
 * 
 *  @brief  Header file for the cluster helper class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_HELPER_H
#define CLUSTER_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *  @brief  ClusterHelper class
 */
class ClusterHelper
{
public:
    /**
     *  @brief  ClusterFitPoint class
     */
    class ClusterFitPoint
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pCaloHit address of a calo hit on which to base the cluster fit point
         */
        ClusterFitPoint(const CaloHit *const pCaloHit);

        /**
         *  @brief  Constructor
         * 
         *  @param  position the position vector of the fit point
         *  @param  cellThickness the thickness of the cell in which the point was recorded
         *  @param  pseudoLayer the pseudolayer in which the point was recorded
         */
        ClusterFitPoint(const CartesianVector &position, float cellThickness, PseudoLayer pseudoLayer);

        /**
         *  @brief  Get the position vector of the fit point
         * 
         *  @return the position vector of the fit point
         */
        const CartesianVector &GetPosition() const;

        /**
         *  @brief  Get the size of the cell in which the point was recorded
         * 
         *  @return the size of the cell in which the point was recorded
         */
        float GetCellSize() const;

        /**
         *  @brief  Get the pseudolayer in which the point was recorded
         * 
         *  @return the pseudolayer in which the point was recorded
         */
        PseudoLayer GetPseudoLayer() const;

    private:
        CartesianVector         m_position;              ///< The position vector of the fit point
        float                   m_cellSize;              ///< The size of the cell in which the point was recorded
        PseudoLayer             m_pseudoLayer;           ///< The pseudolayer in which the point was recorded
    };

    typedef std::vector<ClusterFitPoint> ClusterFitPointList;

    /**
     *  @brief  ClusterFitResult class
     */
    class ClusterFitResult
    {
    public:
        /**
         *  @brief  Default constructor
         */
        ClusterFitResult();

        /**
         *  @brief  Query whether fit was successful
         * 
         *  @return boolean
         */
        bool IsFitSuccessful() const;

        /**
         *  @brief  Get the fit direction
         * 
         *  @return the fit direction
         */
        const CartesianVector &GetDirection() const;

        /**
         *  @brief  Get the fit intercept
         * 
         *  @return the fit intercept
         */
        const CartesianVector &GetIntercept() const;

        /**
         *  @brief  Get the fit ch2
         * 
         *  @return the fit chi2
         */
        float GetChi2() const;

        /**
         *  @brief  Get the fit rms
         * 
         *  @return the fit rms
         */
        float GetRms() const;

        /**
         *  @brief  Get the fit direction cosine w.r.t. the radial direction
         * 
         *  @return the fit direction cosine w.r.t. the radial direction
         */
        float GetRadialDirectionCosine() const;

        /**
         *  @brief  Set the fit success flag
         * 
         *  @param  successFlag the fit success flag
         */
        void SetSuccessFlag(bool successFlag);

        /**
         *  @brief  Set the fit direction
         * 
         *  @param  direction the fit direction
         */
        void SetDirection(const CartesianVector &direction);

        /**
         *  @brief  Set the fit intercept
         * 
         *  @param  intercept the fit intercept
         */
        void SetIntercept(const CartesianVector &intercept);

        /**
         *  @brief  Set the fit chi2
         * 
         *  @param  chi2 the fit chi2
         */
        void SetChi2(float chi2);

        /**
         *  @brief  Set the fit rms
         * 
         *  @param  rms the fit rms
         */
        void SetRms(float rms);

        /**
         *  @brief  Set the fit direction cosine w.r.t. the radial direction
         * 
         *  @param  radialDirectionCosine the fit direction cosine w.r.t. the radial direction
         */
        void SetRadialDirectionCosine(float radialDirectionCosine);

        /**
         *  @brief  Reset the cluster fit result
         */
        void Reset();

    private:
        bool                    m_isFitSuccessful;       ///< Whether the fit was successful
        CartesianVector         m_direction;             ///< The best fit direction
        CartesianVector         m_intercept;             ///< The best fit intercept
        InputFloat              m_chi2;                  ///< The chi2 value for the fit
        InputFloat              m_rms;                   ///< The rms of the fit
        InputFloat              m_dirCosR;               ///< The direction cosine wrt to the radial direction
    };

    /**
     *  @brief  Fit points in first n occupied pseudolayers of a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  maxOccupiedLayers the maximum number of occupied pseudo layers to consider
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitStart(const Cluster *const pCluster, unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit points in last n occupied pseudolayers of a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  maxOccupiedLayers the maximum number of occupied pseudo layers to consider
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitEnd(const Cluster *const pCluster, unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit all cluster points within the specified (inclusive) pseudolayer range
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  startLayer the start of the pseudolayer range
     *  @param  endLayer the end of the pseudolayer range
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitLayers(const Cluster *const pCluster, PseudoLayer startLayer, PseudoLayer endLayer, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit all points in a cluster
     * 
     *  @param  pCluster the cluster containing the ordered list of calo hits to fit
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitPoints(const Cluster *const pCluster, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Perform linear regression of x vs d and y vs d and z vs d (assuming same error on all hits)
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Get the closest distance of approach between two cluster fit results
     * 
     *  @param  lhs first cluster fit result
     *  @param  rhs second cluster fit result
     * 
     *  @return the distance of closest approach
     */
    static float GetFitResultsClosestApproach(const ClusterFitResult &lhs, const ClusterFitResult &rhs);

    /**
     *  @brief  Get closest distance of approach between projected cluster fit result and hits in a second cluster
     * 
     *  @param  clusterFitResult the fit result to the first cluster
     *  @param  pCluster address of the second cluster
     *  @param  startLayer first layer to examine
     *  @param  endLayer last layer to examine
     * 
     *  @return the closest distance of approach
     */
    static float GetDistanceToClosestHit(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
        PseudoLayer startLayer, PseudoLayer endLayer);

    /**
     *  @brief  Get smallest distance between pairs of hits in two clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     * 
     *  @return the smallest distance
     */
    static float GetDistanceToClosestHit(const Cluster *const pClusterI, const Cluster *const pClusterJ);

    /**
     *  @brief  Get closest distance of approach between projected cluster fit result and layer centroid position of a second cluster
     * 
     *  @param  clusterFitResult the fit result to the first cluster
     *  @param  pCluster address of the second cluster
     *  @param  startLayer first layer to examine
     *  @param  endLayer last layer to examine
     * 
     *  @return the closest distance of approach
     */
    static float GetDistanceToClosestCentroid(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
        PseudoLayer startLayer, pandora::PseudoLayer endLayer);

    /**
     *  @brief  Get the closest distance between layer centroid positions in two overlapping clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     *  @param  intraLayerDistance to receive the closest centroid distance
     */
    static StatusCode GetDistanceToClosestCentroid(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &centroidDistance);

    /**
     *  @brief  Get the closest distance between same layer centroid positions in two overlapping clusters
     * 
     *  @param  pClusterI address of the first cluster
     *  @param  pClusterJ address of the second cluster
     *  @param  intraLayerDistance to receive the closest intra layer distance
     */
    static StatusCode GetClosestIntraLayerDistance(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &intraLayerDistance);

    /**
     *  @brief  Get the distance of closest approach between the projected track direction at ecal the hits within a cluster.
     *          Note that only a specified number of layers are examined.
     * 
     *  @param  pTrack address of the track
     *  @param  pCluster address of the cluster
     *  @param  maxSearchLayer the maximum pseudolayer to examine
     *  @param  parallelDistanceCut maximum allowed projection of track-cluster separation along track direction
     *  @param  trackClusterDistance to receive the track cluster distance
     */
    static StatusCode GetTrackClusterDistance(const pandora::Track *const pTrack, const pandora::Cluster *const pCluster,
        const pandora::PseudoLayer maxSearchLayer, float parallelDistanceCut, float &trackClusterDistance);

    /**
     *  @brief  Get the distance of closest approach between a specified track state and the hits within a cluster.
     *          Note that only a specified number of layers are examined.
     * 
     *  @param  trackState the specified track state (position and momentum vectors)
     *  @param  pCluster address of the cluster
     *  @param  maxSearchLayer the maximum pseudolayer to examine
     *  @param  parallelDistanceCut maximum allowed projection of track-cluster separation along track direction
     *  @param  trackClusterDistance to receive the track cluster distance
     */
    static StatusCode GetTrackClusterDistance(const pandora::TrackState &trackState, const pandora::Cluster *const pCluster,
        const pandora::PseudoLayer maxSearchLayer, float parallelDistanceCut, float &trackClusterDistance);

    /**
     *  @brief  Whether a cluster can be merged with another. Uses simple suggested criteria, including cluster photon id flag
     *          and supplied cuts on cluster mip fraction and all hits fit rms.
     * 
     *  @param  pCluster address of the cluster
     *  @param  minMipFraction the minimum mip fraction for clusters (flagged as photons) to be merged
     *  @param  maxAllHitsFitRms the maximum all hit fit rms for clusters (flagged as photons) to be merged
     * 
     *  @return boolean
     */
    static bool CanMergeCluster(Cluster *const pCluster, float minMipFraction, float maxAllHitsFitRms);

    /**
     *  @brief  Whether a cluster should be considered as leaving the calorimeters, leading to leakage of its energy
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    static bool IsClusterLeavingDetector(Cluster *const pCluster);

    /**
     *  @brief  Get the layer at which shower can be considered to start; this function evaluates the the starting point of
     *          a series of "showerStartNonMipLayers" successive layers, each with mip fraction below "showerLayerMipFraction"
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return The shower start layer
     */
    static PseudoLayer GetShowerStartLayer(Cluster *const pCluster);

private:
    /**
     *  @brief  Fit points in barrel region
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  cosTheta cosine of coordinate rotation angle
     *  @param  sinTheta sine of coordinate rotation angle
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitBarrelPoints(const ClusterFitPointList &clusterFitPointList, float cosTheta, float sinTheta,
        ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Fit points in endcap region
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  isPositiveZ whether fit is to endcap in region of positive or negative z coordinate
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitEndCapPoints(const ClusterFitPointList &clusterFitPointList, bool isPositiveZ, ClusterFitResult &clusterFitResult);
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterHelper::ClusterFitPoint::GetPosition() const
{
    return m_position;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitPoint::GetCellSize() const
{
    return m_cellSize;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayer ClusterHelper::ClusterFitPoint::GetPseudoLayer() const
{
    return m_pseudoLayer;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline ClusterHelper::ClusterFitResult::ClusterFitResult() :
    m_isFitSuccessful(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool ClusterHelper::ClusterFitResult::IsFitSuccessful() const
{
    return m_isFitSuccessful;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterHelper::ClusterFitResult::GetDirection() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_direction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ClusterHelper::ClusterFitResult::GetIntercept() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_intercept;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitResult::GetChi2() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_chi2.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitResult::GetRms() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_rms.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterHelper::ClusterFitResult::GetRadialDirectionCosine() const
{
    if (!m_isFitSuccessful)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_dirCosR.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetSuccessFlag(bool successFlag)
{
    m_isFitSuccessful = successFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetDirection(const CartesianVector &direction)
{
    m_direction = direction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetIntercept(const CartesianVector &intercept)
{
    m_intercept = intercept;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetChi2(float chi2)
{
    if (!(m_chi2 = chi2))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetRms(float rms)
{
    if (!(m_rms = rms))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::SetRadialDirectionCosine(float radialDirectionCosine)
{
    if (!(m_dirCosR = radialDirectionCosine))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ClusterHelper::ClusterFitResult::Reset()
{
    m_isFitSuccessful = false;
    m_direction.Reset();
    m_intercept.Reset();
    m_chi2.Reset();
    m_rms.Reset();
    m_dirCosR.Reset();
}

} // namespace pandora

#endif // #ifndef CLUSTER_HELPER_H
