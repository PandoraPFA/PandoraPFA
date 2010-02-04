/**
 *  @file   PandoraPFANew/src/Algorithms/ECalPhotonFindingAlgorithm.cc
 * 
 *  @brief  Implementation of the photon clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/ECalPhotonFindingAlgorithm.h"

using namespace pandora;


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonFindingAlgorithm::Initialize()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonFindingAlgorithm::Run()
{
    // Run initial clustering algorithm
    const ClusterList* pInitialClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pInitialClusterList ));

    std::cout << "save clusterlist A  size " << pInitialClusterList->size() << std::endl;
    if( !pInitialClusterList->empty() )
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_clusterListName));

    std::cout << "run photonextraction" << std::endl;
    const ClusterList* pPhotonClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_photonExtractionAlgorithmName, pPhotonClusterList));

    std::cout << "save clusterlist B  size " << pPhotonClusterList->size() << std::endl;
    if( !pPhotonClusterList->empty() )
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_clusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonFindingAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ClusterFormation", m_clusteringAlgorithmName   ));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "PhotonExtraction", m_photonExtractionAlgorithmName ));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "clusterListName", m_clusterListName));

    return STATUS_CODE_SUCCESS;
}

