/**
 *	@file	PandoraPFANew/src/Algorithms/FullCheatingAlgorithm.cc
 * 
 *	@brief	Implementation of the clustering algorithm class.
 * 
 *	$Log: $
 */

#include "Algorithms/FullCheatingAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

#include <sstream>

using namespace pandora;

StatusCode FullCheatingAlgorithm::Run()
{
    // Run initial clustering algorithm
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));

    // Could select some clusters here (a subset of those in pClusterList) to save. Would then pass this list when calling SaveClusterList.
    // ClusterList clustersToSave;

    // create PFOs
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));


    for( ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; itCluster++ )
    {
        PandoraContentApi::ParticleFlowObject::Parameters pfo;
//      pfo.m_clusterList = pClusterList;
//      pfo.m_energy = (*itCluster)->GetEnergy();
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfo));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FullCheatingAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    // Daughter clustering algorithm
    TiXmlElement *pXmlElement = xmlHandle.FirstChild("algorithm").Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateDaughterAlgorithm(*this, pXmlElement, m_clusteringAlgorithmName));

    // Photon cluster list name
    pXmlElement = xmlHandle.FirstChild("fullCheatingListName").Element();

    if (NULL == pXmlElement)
        return STATUS_CODE_NOT_FOUND;

    m_fullCheatingListName = pXmlElement->GetText();

    return STATUS_CODE_SUCCESS;
}
