/**
 *  @file   PandoraPFANew/src/Algorithms/PrimaryClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the primary clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PrimaryClusteringAlgorithm.h"

using namespace pandora;

StatusCode PrimaryClusteringAlgorithm::Run()
{
    // Run initial clustering algorithm
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));

    if (pClusterList->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    // Run topological association algorithm
    if (!m_associationAlgorithmName.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));

    // Select all clusters containing calo hits - i.e. reject the clusters corresponding only to track projections
    ClusterList clustersToSave;
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        if ((*iter)->GetNCaloHits() > 0)
            clustersToSave.insert(*iter);
    }

    //Save the clusters and replace current list
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterListAndReplaceCurrent(*this, m_clusterListName, clustersToSave));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PrimaryClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterFormation", m_clusteringAlgorithmName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "ClusterAssociation", m_associationAlgorithmName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "clusterListName", m_clusterListName));

    return STATUS_CODE_SUCCESS;
}
