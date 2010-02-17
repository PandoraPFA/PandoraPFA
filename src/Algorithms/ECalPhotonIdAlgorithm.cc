/**
 *  @file   PandoraPFANew/src/Algorithms/ECalPhotonIdAlgorithm.cc
 * 
 *  @brief  Implementation of the ecal photon id algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/ECalPhotonIdAlgorithm.h"

using namespace pandora;


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonIdAlgorithm::Initialize()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonIdAlgorithm::Run()
{
    // Run initial clustering algorithm
    const ClusterList* pInitialClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pInitialClusterList ));

    if( !pInitialClusterList->empty() )
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_clusterListName));

    const ClusterList* pPhotonClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_photonExtractionAlgorithmName, pPhotonClusterList));

    if( !pPhotonClusterList->empty() )
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_clusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonIdAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ClusterFormation", m_clusteringAlgorithmName   ));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "PhotonExtraction", m_photonExtractionAlgorithmName ));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "clusterListName", m_clusterListName));

    return STATUS_CODE_SUCCESS;
}

