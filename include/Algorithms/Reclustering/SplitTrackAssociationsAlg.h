/**
 *  @file   PandoraPFANew/include/Algorithms/Reclustering/SplitTrackAssociationsAlg.h
 * 
 *  @brief  Header file for the split track associations algorithm class.
 * 
 *  $Log: $
 */
#ifndef SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H
#define SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  SplitTrackAssociationsAlg class
 */
class SplitTrackAssociationsAlg : public pandora::Algorithm
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

inline pandora::Algorithm *SplitTrackAssociationsAlg::Factory::CreateAlgorithm() const
{
    return new SplitTrackAssociationsAlg();
}

#endif // #ifndef SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H
