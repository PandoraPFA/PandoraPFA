/**
 *	@file	PandoraPFANew/src/Algorithms/PrimaryClusteringAlgorithm.cc
 * 
 *	@brief	Implementation of the primary clustering algorithm class.
 * 
 *	$Log: $
 */

#include "Algorithms/PrimaryClusteringAlgorithm.h"

using namespace pandora;

StatusCode PrimaryClusteringAlgorithm::Run()
{
	// Run initial clustering algorithm
	const ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));

	//Save the clusters and replace current list
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterListAndReplaceCurrent(*this, m_clusterListName));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PrimaryClusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
	// Daughter clustering algorithm
	TiXmlElement *pXmlElement = xmlHandle.FirstChild("algorithm").Element();

	if (NULL == pXmlElement)
		return STATUS_CODE_NOT_FOUND;

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateDaughterAlgorithm(*this, pXmlElement, m_clusteringAlgorithmName));

	// Cluster list name
	pXmlElement = xmlHandle.FirstChild("clusterListName").Element();

	if (NULL == pXmlElement)
		return STATUS_CODE_NOT_FOUND;

	m_clusterListName = pXmlElement->GetText();

	return STATUS_CODE_SUCCESS;
}
