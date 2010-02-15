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

    std::string             m_inputTrackListName;           ///< The name of the input track list to prepare
    std::string             m_outputTrackListName;          ///< The name of the output track list, containing parent tracks of charged pfos
    pandora::StringVector   m_associationAlgorithms;        ///< The ordered list of track-cluster associations algorithm to run
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackPreparationAlgorithm();
}

#endif // #ifndef TRACK_PREPARATION_ALGORITHM_H
