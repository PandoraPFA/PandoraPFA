/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Utility/EventPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the event preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Utility/EventPreparationAlgorithm.h"

using namespace pandora;

StatusCode EventPreparationAlgorithm::Run()
{
    // Filter current track list to select tracks to be used during clustering
    const TrackList *pCurrentTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pCurrentTrackList));

    TrackList clusteringTrackList;

    for (TrackList::const_iterator iter = pCurrentTrackList->begin(), iterEnd = pCurrentTrackList->end(); iter != iterEnd; ++iter)
    {
        if ((*iter)->GetDaughterTrackList().empty())
            clusteringTrackList.insert(*iter);
    }

    // Save the filtered list and set it to be the current list for subsequent algorithms
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveTrackList(*this, clusteringTrackList, m_outputTrackListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentTrackList(*this, m_outputTrackListName));

    // Split input calo hit list into ecal/hcal and muon calo hits
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentCaloHitList(*this, pCaloHitList));

    CaloHitList caloHitList, muonCaloHitList;

    for (CaloHitList::const_iterator hitIter = pCaloHitList->begin(), hitIterEnd = pCaloHitList->end(); hitIter != hitIterEnd; ++hitIter)
    {
        if (MUON == (*hitIter)->GetHitType())
        {
            if (!muonCaloHitList.insert(*hitIter).second)
                return STATUS_CODE_ALREADY_PRESENT;
        }
        else
        {
            if (!caloHitList.insert(*hitIter).second)
                return STATUS_CODE_ALREADY_PRESENT;
        }
    }

    // Save the lists, setting the ecal/hcal list to be the current list for subsequent algorithms
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveCaloHitList(*this, muonCaloHitList, m_outputMuonCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveCaloHitList(*this, caloHitList, m_outputCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentCaloHitList(*this, m_outputCaloHitListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_outputTrackListName = "TracksForClustering";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputTrackListName", m_outputTrackListName));

    m_outputMuonCaloHitListName = "MuonYokeHits";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputMuonCaloHitListName", m_outputMuonCaloHitListName));

    m_outputCaloHitListName = "CalorimeterHits";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputCaloHitListName", m_outputCaloHitListName));

    return STATUS_CODE_SUCCESS;
}
