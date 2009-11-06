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
        InputCartesianVector    m_position;                 ///< The position vector of the fit point
        InputFloat              m_cellThickness;            ///< The thickness of the cell in which the point was recorded
        InputPseudoLayer        m_pseudoLayer;              ///< The pseudolayer in which the point was recorded
    };

    typedef std::vector<ClusterFitPoint> ClusterFitPointList;

    /**
     *  @brief  ClusterFitResult class
     */
    class ClusterFitResult
    {
    public:
        bool                    m_isFitSuccessful;          ///< Whether the fit was successful
        CartesianVector         m_direction;                ///< The best fit direction
        CartesianVector         m_intercept;                ///< The best fit intercept
        float                   m_chi2;                     ///< The chi2 value for the fit
        float                   m_rms;                      ///< The rms of the fit
        float                   m_radialDirectionCosine;    ///< The direction cosine wrt to the radial direction
    };

    /**
     *  @brief  Perform linear regression of x vs d and y vs d and z vs d (assuming same error on all hits)
     * 
     *  @param  clusterFitPointList list of cluster fit points
     *  @param  clusterFitResult to receive the cluster fit result
     */
    static StatusCode FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult);
};

} // namespace pandora

#endif // #ifndef CLUSTER_HELPER_H
