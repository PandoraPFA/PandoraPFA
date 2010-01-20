/**
 *  @file   PandoraPFANew/include/Algorithms/Reclustering/TrackDrivenMergingAlg.h
 * 
 *  @brief  Header file for the track driven merging algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_DRIVEN_MERGING_ALGORITHM_H
#define TRACK_DRIVEN_MERGING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TrackDrivenMergingAlg class
 */
class TrackDrivenMergingAlg : public pandora::Algorithm
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

    typedef std::multimap<float, pandora::Cluster *> ConeFractionToClusterMap;

    std::string             m_trackClusterAssociationAlgName;       ///< The name of the track-cluster association algorithm to run

    unsigned int            m_minTrackAssociations;                 ///< The min number of associated tracks required to start merging
    unsigned int            m_maxTrackAssociations;                 ///< The max number of associated tracks required to start merging
    float                   m_chiToAttemptMerging;                  ///< The max track/cluster chi value required to start merging

    float                   m_coarseDaughterChiCut;                 ///< Coarse chi cut for candidate daughter clusters

    float                   m_coneCosineHalfAngle;                  ///< Angle of cone used to identify parent-daughter relationships
    float                   m_minConeFractionSingle;                ///< For single merging: min fraction of daughter hits in parent cone
    float                   m_minConeFractionMultiple;              ///< For multiple merging: min fraction of daughter hits in parent cone

    unsigned int            m_maxLayerSeparationMultiple;           ///< Max layers between parent/daughter clusters for multiple merging
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackDrivenMergingAlg::Factory::CreateAlgorithm() const
{
    return new TrackDrivenMergingAlg();
}

#endif // #ifndef TRACK_DRIVEN_MERGING_ALGORITHM_H
