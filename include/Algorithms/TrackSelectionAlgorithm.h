/**
 *  @file   PandoraPFANew/include/Algorithms/TrackSelectionAlgorithm.h
 * 
 *  @brief  Header file for the track selection algorithm class.
 * 
 *  $Log: $
 */
#ifndef TRACK_SELECTION_ALGORITHM_H
#define TRACK_SELECTION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TemplateAlgorithm class
 */
class TrackSelectionAlgorithm : public pandora::Algorithm
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

    std::string     m_selectedTrackListName;        ///< The name under which to save the new list of selected tracks
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TrackSelectionAlgorithm::Factory::CreateAlgorithm() const
{
    return new TrackSelectionAlgorithm();
}

#endif // #ifndef TRACK_SELECTION_ALGORITHM_H
