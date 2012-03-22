/**
 *  @file   PandoraPFANew/FineGranularityContent/include/ClusteringLineClusteringAlgorithm.h
 * 
 *  @brief  Header file for the line clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef LINE_CLUSTERING_ALGORITHM_H
#define LINE_CLUSTERING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  LineClusteringAlgorithm class
 */
class LineClusteringAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    /**
     *  @brief Two dimensional line class
     */
    class TwoDLine
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  xPointOnLine the xcoordinate of a reference point on the line
         *  @param  yPointOnLine the ycoordinate of a reference point on the line
         *  @param  rotationAngle the angle (measured anti-clockwise) between the 2d line and the y axis
         */
        TwoDLine(const float xPointOnLine, const float yPointOnLine, const float rotationAngle);

        /**
         *  @brief  Get the Hough space r coordinate of the straight line
         * 
         *  @return the Hough space r coordinate
         */
        float GetHoughSpaceR() const;

        /**
         *  @brief  Get the Hough space theta coordinate of the straight line
         * 
         *  @return the Hough space theta coordinate
         */
        float GetHoughSpaceTheta() const;

    private:
        const float m_houghSpaceR;                  ///< The Hough space r coordinate of the straight line
        const float m_houghSpaceTheta;              ///< The Hough space theta coordinate of the straight line
    };

    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  Get the list of calo hits that are available for clustering (a subset of the current calo hit list)
     * 
     *  @param  caloHitList to receive the list of available calo hits
     */
    pandora::StatusCode GetListOfAvailableCaloHits(pandora::CaloHitList &caloHitList) const;

    /**
     *  @brief  Fill Hough space histograms for the calo hits in a specified list
     * 
     *  @param  caloHitList the specified list of available calo hits
     *  @param  twoDHistogram_XY to receive the populated Hough space histogram for 2D lines in the x-y plane
     *  @param  twoDHistogram_YZ to receive the populated Hough space histogram for 2D lines in the y-z plane
     */
    void FillHoughSpaceHistograms(const pandora::CaloHitList &caloHitList, pandora::TwoDHistogram &twoDHistogram_XY,
        pandora::TwoDHistogram &twoDHistogram_YZ) const;

    /**
     *  @brief  Use the results of the Hough transforms in the x-y and y-z planes to identify straight-line clusters
     * 
     *  @param  caloHitList the specified list of available calo hits
     *  @param  useXY whether to use the 2D line in the x-y plane to identify calo hits
     *  @param  useYZ whether to use the 2D line in the y-z plane to identify calo hits
     *  @param  peakR_XY the peak Hough space R value for clustering in the x-y plane
     *  @param  peakTheta_XY the peak Hough space theta value for clustering in the x-y plane
     *  @param  peakR_YZ the peak Hough space R value for clustering in the y-z plane
     *  @param  peakTheta_YZ the peak Hough space theta value for clustering in the y-z plane
     *  @param  pCluster to receive the address of the cluster
     */
    pandora::StatusCode CreateCluster(const pandora::CaloHitList &caloHitList, const bool useXY, const bool useYZ, const float peakR_XY,
        const float peakTheta_XY, const float peakR_YZ, const float peakTheta_YZ, pandora::Cluster *&pCluster) const;

    bool            m_shouldUseIsolatedHits;        ///< Whether to use isolated hits in the clustering algorithm
    unsigned int    m_minCaloHitsForClustering;     ///< Minimum number of available calo hits to proceed with clustering

    bool            m_useXY;                        ///< Whether to perform a Hough transform in the x-y plane
    bool            m_useYZ;                        ///< Whether to perform a Hough transform in the y-z plane

    unsigned int    m_nLines;                       ///< The number of lines to examined through each point (uniformly spaced in angle)
    float           m_minAbsCosAngleToHitDir;       ///< Min abs cos angle between line and expected hit dir for contribution to Hough transform

    unsigned int    m_nHoughSpaceRBins;             ///< The number of histogram bins for the Hough space r coordinate
    float           m_minHoughSpaceR;               ///< The histogram minimum value for the Hough space r coordinate
    float           m_maxHoughSpaceR;               ///< The histogram maximum value for the Hough space r coordinate

    float           m_minHoughSpacePeakEntries;     ///< The minimum number of entries in the hough space peak to proceed with clustering
    float           m_hitToLineTolerance;           ///< The maximum distance between line and candidate calo hit, units: cell length scales

    bool            m_houghSpaceMonitoring;         ///< Whether to draw Hough space histograms, if monitoring is enabled
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *LineClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new LineClusteringAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline float LineClusteringAlgorithm::TwoDLine::GetHoughSpaceR() const
{
    return m_houghSpaceR;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float LineClusteringAlgorithm::TwoDLine::GetHoughSpaceTheta() const
{
    return m_houghSpaceTheta;
}

#endif // #ifndef LINE_CLUSTERING_ALGORITHM_H
