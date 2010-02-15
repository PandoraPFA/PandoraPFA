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
    TrackList siblingTracks;

    for (TrackList::const_iterator iter = inputTrackList.begin(), iterEnd = inputTrackList.end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        if (!pTrack->GetParentTrackList().empty())
            continue;

        // Sibling tracks as first evidence of pfo target
        if (!pTrack->GetSiblingTrackList().empty())
        {
            if (siblingTracks.end() != siblingTracks.find(pTrack))
                continue;

            if (this->HasAssociatedClusters(pTrack))
            {
                pfoTrackList.insert(pTrack);
                siblingTracks.insert(pTrack);
            }
        }
        // Single parent track as pfo target
        else if (this->HasAssociatedClusters(pTrack) || this->IsLowPt(pTrack))
        {
            pfoTrackList.insert(pTrack);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool TrackPreparationAlgorithm::HasAssociatedClusters(const Track *const pTrack, const bool readSiblingInfo) const
{
    if (pTrack->HasAssociatedCluster())
        return true;

    // Consider any sibling tracks
    if (readSiblingInfo)
    {
        const TrackList &siblingTrackList(pTrack->GetSiblingTrackList());

        for (TrackList::const_iterator iter = siblingTrackList.begin(), iterEnd = siblingTrackList.end(); iter != iterEnd; ++iter)
        {
            if (this->HasAssociatedClusters(*iter, false))
                return true;
        }
    }

    // Consider any daughter tracks
    const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());

    for (TrackList::const_iterator iter = daughterTrackList.begin(), iterEnd = daughterTrackList.end(); iter != iterEnd; ++iter)
    {
        if (this->HasAssociatedClusters(*iter))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool TrackPreparationAlgorithm::IsLowPt(const Track *const pTrack) const
{
    if (!pTrack->GetParentTrackList().empty() || !pTrack->GetSiblingTrackList().empty())
        return false;

    static const float mainTrackerZExtent(GeometryHelper::GetInstance()->GetMainTrackerZExtent());

    const float zEnd(std::fabs(pTrack->GetTrackStateAtEnd().GetPosition().GetZ()));
    const float zStart(std::fabs(pTrack->GetTrackStateAtStart().GetPosition().GetZ()));

    return ((zStart < m_lowPtTrackStartDeltaZ) && ((mainTrackerZExtent - zEnd) < m_lowPtTrackEndDeltaZ) && (pTrack->GetEnergyAtDca() < m_lowPtEnergy));
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

    m_lowPtEnergy = 1.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowPtEnergy", m_lowPtEnergy));

    m_lowPtTrackStartDeltaZ = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowPtTrackStartDeltaZ", m_lowPtTrackStartDeltaZ));

    m_lowPtTrackEndDeltaZ = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowPtTrackEndDeltaZ", m_lowPtTrackEndDeltaZ));

    return STATUS_CODE_SUCCESS;
}
