/**
 *	@file	PandoraPFANew/src/Managers/MCManager.cc
 * 
 * 	@brief	Implementation of the mc manager class.
 * 
 *	$Log: $
 */

#include "Managers/MCManager.h"

#include "Objects/MCParticle.h"

namespace pandora
{

MCManager::MCManager() :
	m_pMCPfoSelection(new MCPfoSelection)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCManager::MCManager(const MCManager::MCPfoSelection* mcPfoSelection) :
	m_pMCPfoSelection(mcPfoSelection)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCManager::~MCManager()
{
	delete m_pMCPfoSelection;
	(void) this->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::CreateMCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters)
{
	UidToMCParticleMap::iterator iter = m_uidToMCParticleMap.find(mcParticleParameters.m_pParentAddress);

	if (m_uidToMCParticleMap.end() != iter)
	{
		iter->second->SetProperties(mcParticleParameters);
	}
	else
	{
		MCParticle *pMCParticle = new MCParticle(mcParticleParameters);

		if (!m_uidToMCParticleMap.insert(UidToMCParticleMap::value_type(mcParticleParameters.m_pParentAddress, pMCParticle)).second)
			return STATUS_CODE_FAILURE;
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::RetrieveExistingOrCreateEmptyMCParticle(const Uid mcParticleUid, MCParticle *&pMCParticle)
{
	UidToMCParticleMap::const_iterator iter = m_uidToMCParticleMap.find(mcParticleUid);

	if(m_uidToMCParticleMap.end() != iter) 
	{
		pMCParticle = iter->second;

		if (NULL == pMCParticle)
			return STATUS_CODE_FAILURE;
	}
	else
	{
		pMCParticle = new MCParticle(mcParticleUid);

		if ((NULL == pMCParticle) || (!m_uidToMCParticleMap.insert(UidToMCParticleMap::value_type(mcParticleUid, pMCParticle)).second))
			return STATUS_CODE_FAILURE;
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SetMCParentDaughterRelationship(const Uid parentUid, const Uid daughterUid)
{
	MCParticle* pParent = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, RetrieveExistingOrCreateEmptyMCParticle(parentUid, pParent));

	MCParticle* pDaughter = NULL;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, RetrieveExistingOrCreateEmptyMCParticle(daughterUid, pDaughter));	

	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pParent->AddDaughter(pDaughter));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pDaughter->AddParent(pParent));
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SetCaloHitToMCParticleRelationship(const Uid caloHitUid, const Uid mcParticleUid, const float mcParticleWeight)
{
	UidRelationMap::iterator iter = m_caloHitToMCParticleMap.find(caloHitUid);
	
	if (m_caloHitToMCParticleMap.end() != iter)
	{
		if (mcParticleWeight > iter->second.m_weight)
			iter->second = UidAndWeight(mcParticleUid, mcParticleWeight);
	}
	else
	{
		if (!m_caloHitToMCParticleMap.insert(UidRelationMap::value_type(caloHitUid, UidAndWeight(mcParticleUid, mcParticleWeight))).second)
			return STATUS_CODE_FAILURE;
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SelectPfoTargets()
{
	for (UidToMCParticleMap::iterator iter = m_uidToMCParticleMap.begin(), iterEnd = m_uidToMCParticleMap.end(); iter != iterEnd; ++iter)
	{
		if (iter->second->IsRootParticle())
		{
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pMCPfoSelection->ApplySelectionRules(iter->second));
		}
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::CreateCaloHitToPfoTargetMap(UidToMCParticleMap &caloHitToPfoTargetMap) const
{
	for (UidRelationMap::const_iterator relationIter = m_caloHitToMCParticleMap.begin(), relationIterEnd = m_caloHitToMCParticleMap.end();
		relationIter != relationIterEnd; ++relationIter)
	{
		UidToMCParticleMap::const_iterator mcParticleIter = m_uidToMCParticleMap.find(relationIter->second.m_uid);
	
		if ((m_uidToMCParticleMap.end() == mcParticleIter) || (!mcParticleIter->second->IsInitialized()))
			continue;

		MCParticle *pMCParticle = NULL;
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mcParticleIter->second->GetPfoTarget(pMCParticle));

		if (!caloHitToPfoTargetMap.insert(UidToMCParticleMap::value_type(relationIter->first, pMCParticle)).second)
			return STATUS_CODE_FAILURE;
	}
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::DeleteNonPfoTargets()
{
	for (UidToMCParticleMap::iterator iter = m_uidToMCParticleMap.begin(), iterEnd = m_uidToMCParticleMap.end(); iter != iterEnd; ++iter)
	{
		if (!iter->second->IsPfoTarget())
		{
			delete iter->second;
			m_uidToMCParticleMap.erase(iter);
		}
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::ResetForNextEvent()
{
	for (UidToMCParticleMap::iterator iter = m_uidToMCParticleMap.begin(), iterEnd = m_uidToMCParticleMap.end(); iter != iterEnd; ++iter)
		delete iter->second;

	m_uidToMCParticleMap.clear();
	m_caloHitToMCParticleMap.clear();

	if (!m_uidToMCParticleMap.empty() || !m_caloHitToMCParticleMap.empty())
		return STATUS_CODE_FAILURE;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::MCPfoSelection::ApplySelectionRules(MCParticle *const mcRootParticle) const
{
	// TODO : write the default selection rules here. Care with non-initialized mc particles. Must check whether initialized.

	return STATUS_CODE_SUCCESS;
}

} // namespace pandora

