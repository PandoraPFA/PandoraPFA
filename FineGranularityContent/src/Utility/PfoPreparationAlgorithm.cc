/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Utility/PfoPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the pfo preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Utility/PfoPreparationAlgorithm.h"

using namespace pandora;

StatusCode PfoPreparationAlgorithm::Run()
{
    // Merge all candidate pfos in specified input lists, to create e.g. final output pfo list
    for (StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        const PfoList *pPfoList = NULL;

        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetPfoList(*this, *iter, pPfoList))
            continue;

        PfoList pfosToSave;
        pfosToSave.insert(pPfoList->begin(), pPfoList->end());

        PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_INITIALIZED, !=, PandoraContentApi::SavePfoList(*this,
            *iter, m_mergedCandidateListName, pfosToSave));
    }

    // Save the merged list and set it to be the current list for future algorithms
    if (STATUS_CODE_SUCCESS != PandoraContentApi::ReplaceCurrentPfoList(*this, m_mergedCandidateListName))
    {
        std::cout << "PfoPreparationAlgorithm: empty pfo list for use by subsequent algorithms and client application." << std::endl;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DropCurrentPfoList(*this));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PfoPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    m_mergedCandidateListName = "OutputPfoList";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MergedCandidateListName", m_mergedCandidateListName));

    return STATUS_CODE_SUCCESS;
}
