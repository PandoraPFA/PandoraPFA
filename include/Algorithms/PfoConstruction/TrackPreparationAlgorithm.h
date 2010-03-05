/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/TrackPreparationAlgorithm.h
 * 
 *  @brief  Header file for the track preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_PREPARATION_ALGORITHM_H
#define TRACK_PREPARATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TrackPreparationAlgorithm class
 */
class TrackPreparationAlgorithm : public pandora::Algorithm
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
     *  @brief  Create list of parent tracks to be used in construction of charged pfos
     * 
     *  @param  inputTrackList the input track list
     *  @param  pfoTrackList to receive the list of parent tracks to be used in construction of charged pfos
     */
    StatusCode CreatePfoTrackList(const pandora::TrackList &inputTrackList, pandora::TrackList &pfoTrackList) const;

    /**
     *  @brief  Whether a track, or any of its daughters or siblings has an associated cluster
     * 
     *  @param  pTrack address of the track
     *  @param  readSiblingInfo whether to read sibling track information (set to false to avoid multiple counting)
     *
     *  @return boolean
     */
    bool HasAssociatedClusters(const pandora::Track *const pTrack, const bool readSiblingInfo = true) const;

    /**
     *  @brief  Whether a track is low pt and the charged particle is therefore unlikely to have produced calorimeter clusters
     * 
     *  @param  pTrack address of the track
     *
     *  @return boolean
     */
    bool IsLowPt(const pandora::Track *const pTrack) const;

    pandora::StringVector   m_candidateListNames;           ///< The list of track list names to use
    std::string             m_mergedCandidateListName;      ///< The name under which to save the full, merged, list of candidate tracks
    pandora::StringVector   m_associationAlgorithms;        ///< The ordered list of track-cluster associations algorithm to run
    std::string             m_finalPfoListName;             ///< The name of the output track list, containing parent tracks of charged pfos

    float                   m_lowPtEnergy;                  ///< Low pt track energy threshold
    float                   m_lowPtTrackStartDeltaZ;        ///< Low pt max z separation between interaction point and track start
    float                   m_lowPtTrackEndDeltaZ;          ///< Low pt max z separation between ecal endcap and track end
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackPreparationAlgorithm();
}

#endif // #ifndef TRACK_PREPARATION_ALGORITHM_H
