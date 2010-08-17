/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstruction/ClusterPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the cluster preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/ClusterPreparationAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode ClusterPreparationAlgorithm::Run()
{
    // Create pfo cluster list, containing all candidate clusters for use in final pfo creation
    for (StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *pClusterList = NULL;

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetClusterList(*this, *iter, pClusterList))
            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_INITIALIZED, !=, PandoraContentApi::SaveClusterList(*this, *iter, m_finalPfoListName));
    }

    // Save the filtered list and set it to be the current list for next algorithms
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentClusterList(*this, m_finalPfoListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    m_finalPfoListName = "PfoCreation";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FinalPfoListName", m_finalPfoListName));

    return STATUS_CODE_SUCCESS;
}
