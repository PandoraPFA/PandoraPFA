/**
 *  @file   PandoraPFANew/src/Algorithms/TrackSelectionAlgorithm.cc
 * 
 *  @brief  Implementation of the track selection algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TrackSelectionAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode TrackSelectionAlgorithm::Run()
{
    // Read current track list
    const TrackList *pCurrentTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pCurrentTrackList));

    // Filter current track list to select tracks to be used during clustering
    TrackList clusteringTrackList;

    for (TrackList::const_iterator iter = pCurrentTrackList->begin(), iterEnd = pCurrentTrackList->end(); iter != iterEnd; ++iter)
    {
        if ((*iter)->GetDaughterTrackList().empty())
            clusteringTrackList.insert(*iter);
    }

    // Save the filtered list and set it to be the current list for next algorithms
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveTrackListAndReplaceCurrent(*this, clusteringTrackList,
        m_selectedTrackListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackSelectionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_selectedTrackListName = "Clustering";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SelectedTrackListName", m_selectedTrackListName));

    return STATUS_CODE_SUCCESS;
}
