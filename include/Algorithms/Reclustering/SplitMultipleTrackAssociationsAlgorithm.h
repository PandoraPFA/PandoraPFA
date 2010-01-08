/**
 *  @file   PandoraPFANew/include/Algorithms/Reclustering/SplitMultipleTrackAssociationsAlgorithm.h
 * 
 *  @brief  Header file for the split multiple track associations algorithm class.
 * 
 *  $Log: $
 */
#ifndef SPLIT_MULTIPLE_TRACK_ASSOCIATIONS_ALGORITHM_H
#define SPLIT_MULTIPLE_TRACK_ASSOCIATIONS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  SplitMultipleTrackAssociationsAlgorithm class
 */
class SplitMultipleTrackAssociationsAlgorithm : public pandora::Algorithm
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

    /**
     *  @brief  Get chi value indicating how compatible a cluster is with its associated tracks. Reclustering can split up a
     *          cluster and produce more favourable track/cluster matches.
     *
     *  @param  pCluster address of the cluster
     *  @param  trackList address of the list of tracks associated with the cluster
     */
    float GetTrackClusterCompatibility(const pandora::Cluster *const pCluster, const pandora::TrackList &trackList) const;

    /**
     *  @brief  Get chi2 value indicating suitability of recluster candidates as a replacement for the original clusters
     * 
     *  @param  pReclusterCandidatesList address of the recluster candidates list
     */
    float GetReclusterFigureOfMerit(const pandora::ClusterList *const pReclusterCandidatesList) const;

    pandora::StringVector   m_clusteringAlgorithms;                 ///< The ordered list of clustering algorithms to be used
    std::string             m_associationAlgorithmName;             ///< The name of the topological association algorithm to run
    std::string             m_trackClusterAssociationAlgName;       ///< The name of the track-cluster association algorithm to run

    unsigned int            m_minTrackAssociationsToSplit;          ///< The min number of associated tracks required to start reclustering
    unsigned int            m_maxTrackAssociationsToSplit;          ///< The max number of associated tracks required to start reclustering
    float                   m_chiToAttemptReclustering;             ///< The min track/cluster chi value required to start reclustering

    float                   m_minClusterEnergyForTrackAssociation;  ///< Energy threshold for recluster candidates with track associations
    float                   m_chi2ForAutomaticClusterSelection;     ///< Chi2 below which recluster candidates are automatically selected
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *SplitMultipleTrackAssociationsAlgorithm::Factory::CreateAlgorithm() const
{
    return new SplitMultipleTrackAssociationsAlgorithm();
}

#endif // #ifndef SPLIT_MULTIPLE_TRACK_ASSOCIATIONS_ALGORITHM_H
