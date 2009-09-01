/**
 *	@file	PandoraPFANew/src/Algorithms/PhotonClusteringAlgorithm.cc
 * 
 *	@brief	Implementation of the photon clustering algorithm class.
 * 
 *	$Log: $
 */

#include "Algorithms/PhotonClusteringAlgorithm.h"

using namespace pandora;

StatusCode PhotonClusteringAlgorithm::Run()
{
	// Run initial clustering algorithm
	const ClusterList *pClusterList = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));

	// Could select some clusters here (a subset of those in pClusterList) to save. Would then pass this list when calling SaveClusterList.
	// ClusterList clustersToSave;

	//Save the clusters and remove the hits - clustersToSave argument is optional
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_photonClusterListName));
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonClusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
	TiXmlElement *pXmlElement = xmlHandle.FirstChild("algorithm").Element();

	if (NULL == pXmlElement)
		return STATUS_CODE_NOT_FOUND;

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateDaughterAlgorithm(*this, pXmlElement, m_clusteringAlgorithmName));

	pXmlElement = xmlHandle.FirstChild("photonClusterListName").Element();

	if (NULL == pXmlElement)
		return STATUS_CODE_NOT_FOUND;

	m_photonClusterListName = pXmlElement->GetText();

	return STATUS_CODE_SUCCESS;
}
