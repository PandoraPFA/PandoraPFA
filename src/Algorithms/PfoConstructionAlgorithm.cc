/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstructionAlgorithm.cc
 * 
 *  @brief  Implementation of the pfo construction algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstructionAlgorithm.h"

using namespace pandora;

StatusCode PfoConstructionAlgorithm::Run()
{
    // Prepare tracks for pfo construction, finalizing track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackPreparationAlgorithmName));

    // Perform cluster fragment removal operations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_fragmentRemovalAlgorithmName));

    // Prepare clusters for pfo construction
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_clusterPreparationAlgorithmName));

    // Create the particle flow objects
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_pfoCreationAlgorithmName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PfoConstructionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "TrackPreparation", m_trackPreparationAlgorithmName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "FragmentRemoval", m_fragmentRemovalAlgorithmName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ClusterPreparation", m_clusterPreparationAlgorithmName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "PfoCreation", m_pfoCreationAlgorithmName));

    return STATUS_CODE_SUCCESS;
}
