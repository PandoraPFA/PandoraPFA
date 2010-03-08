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
    // Create candidate track list, containing all tracks that could be associated to clusters and so used in final pfo creation
    TrackList candidateTrackList;

    for (StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        const TrackList *pTrackList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetTrackList(*this, *iter, pTrackList));
        candidateTrackList.insert(pTrackList->begin(), pTrackList->end());
    }

    // Set this list of candidate pfo tracks to be the current track list for a number of track-cluster association algorithms
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveAllTrackClusterAssociations(*this));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveTrackListAndReplaceCurrent(*this, candidateTrackList,
        m_mergedCandidateListName));

    for (StringVector::const_iterator iter = m_associationAlgorithms.begin(), iterEnd = m_associationAlgorithms.end();
        iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    // Filter the candidate track list to identify the parent tracks of charged pfos
    TrackList finalPfoTrackList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreatePfoTrackList(candidateTrackList, finalPfoTrackList));

    // Save the filtered list and set it to be the current list for next algorithms
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveTrackListAndReplaceCurrent(*this, finalPfoTrackList,
        m_finalPfoListName));

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

            if ((pTrack->CanFormPfo() && this->HasAssociatedClusters(pTrack)) || (pTrack->CanFormClusterlessPfo()))
            {
                pfoTrackList.insert(pTrack);
                siblingTracks.insert(pTrack);
            }
        }
        // Single parent track as pfo target
        else if ((pTrack->CanFormPfo() && this->HasAssociatedClusters(pTrack)) || (pTrack->CanFormClusterlessPfo()))
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

StatusCode TrackPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        m_candidateListNames.push_back("input");

    m_mergedCandidateListName = "pfoCandidates";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MergedCandidateListName", m_mergedCandidateListName));

    m_finalPfoListName = "pfoCreation";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FinalPfoListName", m_finalPfoListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "trackClusterAssociationAlgorithms",
        m_associationAlgorithms));

    return STATUS_CODE_SUCCESS;
}
