/**
 *  @file   PandoraPFANew/src/Algorithms/PrimaryClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the primary clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PrimaryClusteringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode PrimaryClusteringAlgorithm::Run()
{
    // If specified, change the current calo hit list, i.e. the input to the clustering algorithm
    std::string originalCaloHitListName;

    if (!m_inputCaloHitListName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitListName(*this, originalCaloHitListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentOrderedCaloHitList(*this, m_inputCaloHitListName));
    }

    // Run the initial cluster formation algorithm
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));

    if (!pClusterList->empty())
    {
        // Run the topological association algorithms
        if (!m_associationAlgorithmName.empty())
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));

        // Save the new cluster list
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_clusterListName));

        // If specified, replace the current cluster list
        if (m_replaceCurrentClusterList)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentClusterList(*this, m_clusterListName));
    }

    // Unless specified, return current calo hit list to that when algorithm started
    if (m_restoreOriginalCaloHitList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentOrderedCaloHitList(*this, originalCaloHitListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PrimaryClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Daughter algorithm parameters
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterFormation", m_clusteringAlgorithmName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterAssociation", m_associationAlgorithmName));

    // Input parameters: name of input ordered calo hit list and whether it should persist as the current list after algorithm has finished
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InputCaloHitListName", m_inputCaloHitListName));

    m_restoreOriginalCaloHitList = !m_inputCaloHitListName.empty();
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "RestoreOriginalCaloHitList", m_restoreOriginalCaloHitList));

    // Output parameters: name of output cluster list and whether it should subsequently be used as the current list
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterListName", m_clusterListName));

    m_replaceCurrentClusterList = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ReplaceCurrentClusterList", m_replaceCurrentClusterList));

    return STATUS_CODE_SUCCESS;
}
