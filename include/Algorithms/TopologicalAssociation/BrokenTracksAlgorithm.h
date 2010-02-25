/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.h
 * 
 *  @brief  Header file for the broken tracks algorithm class.
 * 
 *  $Log: $
 */
#ifndef BROKEN_TRACKS_ALGORITHM_H
#define BROKEN_TRACKS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

using namespace pandora;

/**
 *  @brief  BrokenTracksAlgorithm class
 */
class BrokenTracksAlgorithm : public Algorithm
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
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    typedef ClusterHelper::ClusterFitResult ClusterFitResult;

    /**
     *  @brief  ClusterFitRelation class
     */
    class ClusterFitRelation
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pCluster the address of the cluster
         *  @param  startFitResult the cluster start fit result
         *  @param  endFitResult the cluster end fit result
         */
        ClusterFitRelation(Cluster *const pCluster, const ClusterFitResult &startFitResult, const ClusterFitResult &endFitResult);

        /**
         *  @brief  Get the address of the cluster
         * 
         *  @return The address of the cluster
         */
        Cluster *GetCluster() const;

        /**
         *  @brief  Get the cluster start fit result
         * 
         *  @return The cluster start fit result
         */
        const ClusterFitResult &GetStartFitResult() const;

        /**
         *  @brief  Get the cluster end fit result
         * 
         *  @return The cluster end fit result
         */
        const ClusterFitResult &GetEndFitResult() const;

        /**
         *  @brief  Whether the cluster fit relation is defunct (the cluster has changed or been deleted and the
         *          fit result is no longer valid).
         * 
         *  @return boolean
         */
        bool IsDefunct() const;

        /**
         *  @brief  Set the cluster fit relation as defunct. This should be called when the cluster has changed,
         *          or if it has been deleted or merged with another cluster.
         */
        void SetAsDefunct();

    private:
        bool                        m_isDefunct;            ///< Whether the cluster fit relation is defunct
        Cluster                    *m_pCluster;             ///< Address of the cluster
        const ClusterFitResult      m_startFitResult;       ///< The cluster start fit result
        const ClusterFitResult      m_endFitResult;         ///< The cluster end fit result
    };

    typedef std::vector<ClusterFitRelation *> ClusterFitRelationList;

    unsigned int    m_nStartLayersToFit;                ///< The number of occupied pseudolayers to use in fit to the start of the cluster
    unsigned int    m_nEndLayersToFit;                  ///< The number of occupied pseudolayers to use in fit to the end of the cluster
    float           m_maxFitRms;                        ///< The max value of the start/end fit rms for cluster to be considered

    float           m_canMergeMinMipFraction;           ///< The min mip fraction for clusters (flagged as photons) to be merged
    float           m_canMergeMaxRms;                   ///< The max all hit fit rms for clusters (flagged as photons) to be merged

    unsigned int    m_minHitsInCluster;                 ///< Min number of calo hits in cluster
    unsigned int    m_minOccupiedLayersInCluster;       ///< Min number of occupied layers in cluster

    unsigned int    m_maxLayerDifference;               ///< The max difference (for merging) between cluster outer and inner pseudo layers
    float           m_maxCentroidDifference;            ///< The max difference (for merging) between cluster outer and inner centroids
    float           m_fitDirectionDotProductCut;        ///< Cut on max value of dot product between cluster fit directions

    float           m_trackMergeCutEcal;                ///< ECal cut on closest distance of approach between two cluster fit results
    float           m_trackMergeCutHcal;                ///< HCal cut on closest distance of approach between two cluster fit results

    float           m_trackMergePerpCutEcal;            ///< ECal cut on perp. distance between fit directions and centroid difference
    float           m_trackMergePerpCutHcal;            ///< HCal cut on perp. distance between fit directions and centroid difference
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline Algorithm *BrokenTracksAlgorithm::Factory::CreateAlgorithm() const
{
    return new BrokenTracksAlgorithm();
}


//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline BrokenTracksAlgorithm::ClusterFitRelation::ClusterFitRelation(Cluster *const pCluster, const ClusterFitResult &startFitResult,
        const ClusterFitResult &endFitResult) :
    m_isDefunct(false),
    m_pCluster(pCluster),
    m_startFitResult(startFitResult),
    m_endFitResult(endFitResult)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *BrokenTracksAlgorithm::ClusterFitRelation::GetCluster() const
{
    if (m_isDefunct)
        throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

    return m_pCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ClusterHelper::ClusterFitResult &BrokenTracksAlgorithm::ClusterFitRelation::GetStartFitResult() const
{
    if (m_isDefunct)
        throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

    return m_startFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ClusterHelper::ClusterFitResult &BrokenTracksAlgorithm::ClusterFitRelation::GetEndFitResult() const
{
    if (m_isDefunct)
        throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);

    return m_endFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool BrokenTracksAlgorithm::ClusterFitRelation::IsDefunct() const
{
    return m_isDefunct;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void BrokenTracksAlgorithm::ClusterFitRelation::SetAsDefunct()
{
    m_isDefunct = true;
}

#endif // #ifndef BROKEN_TRACKS_ALGORITHM_H
