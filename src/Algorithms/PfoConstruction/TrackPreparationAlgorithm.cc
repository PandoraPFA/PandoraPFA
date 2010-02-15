/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstruction/TrackPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the track preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/TrackPreparationAlgorithm.h"

using namespace pandora;

StatusCode TrackPreparationAlgorithm::Run()
{
    // Revert current track list back to a named-list, typically the full input list
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentTrackList(*this, m_inputTrackListName));

    const TrackList *pInputTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pInputTrackList));

    // Run track-cluster association daughter algorithms
    for (StringVector::const_iterator iter = m_associationAlgorithms.begin(), iterEnd = m_associationAlgorithms.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    // Filter input track list to identify parent tracks of charged pfos
    TrackList pfoTrackList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreatePfoTrackList(*pInputTrackList, pfoTrackList));

    // Save the filtered list and set it to be the current list for next algorithms
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveTrackListAndReplaceCurrent(*this, pfoTrackList,
        m_outputTrackListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackPreparationAlgorithm::CreatePfoTrackList(const TrackList &inputTrackList, TrackList &pfoTrackList) const
{
    // ATTN: Identify low pt tracks as those for which:
    //       ((zStart < 100.f) && (zEnd > mainTrackerZExtent - 100.f) && (pTrack->GetEnergyAtDca()< 1.5f))

    // TODO

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_inputTrackListName = "input";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InputTrackListName", m_inputTrackListName));

    m_outputTrackListName = "pfoCreation";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputTrackListName", m_outputTrackListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "trackClusterAssociationAlgorithms",
        m_associationAlgorithms));

    return STATUS_CODE_SUCCESS;
}
