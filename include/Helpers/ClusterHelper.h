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
         *  @brief  Get the thickness of the cell in which the point was recorded
         * 
         *  @return the thickness of the cell in which the point was recorded
         */
        float GetCellThickness() const;

        /**
         *  @brief  Get the pseudolayer in which the point was recorded
         * 
         *  @return the pseudolayer in which the point was recorded
         */
        PseudoLayer GetPseudoLayer() const;

    private:
        CartesianVector         m_position;              ///< The position vector of the fit point
        float                   m_cellThickness;         ///< The thickness of the cell in which the point was recorded
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
     *  @brief  Perform linear regression of x vs d and y vs d and z vs d (assuming same error on all hits)
     * 
     *  @param  orderedCaloHitList the ordered list of calo hits to fit
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitPoints(const OrderedCaloHitList &orderedCaloHitList, ClusterFitResult &clusterFitResult);

    /**
     *  @brief  Perform linear regression of x vs d and y vs d and z vs d (assuming same error on all hits)
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult);

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

inline float ClusterHelper::ClusterFitPoint::GetCellThickness() const
{
    return m_cellThickness;
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
