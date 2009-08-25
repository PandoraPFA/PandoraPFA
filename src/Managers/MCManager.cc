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

StatusCode MCManager::RetrieveExistingOrCreateEmptyMCParticle(const Uid mcParticleUid, MCParticle*& pMCParticle)
{
	pMCParticle = NULL;
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
// 		if ((NULL == pMCParticle) || (!m_uidToMCParticleMap.insert(UidToMCParticleMap::value_type(mcParticleUid, pMCParticle)).second))
// 			return STATUS_CODE_FAILURE;

		if ((NULL == pMCParticle)  )
			return STATUS_CODE_FAILURE;
		else 
    		        m_uidToMCParticleMap.insert(UidToMCParticleMap::value_type(mcParticleUid, pMCParticle));
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
        // this is the method which is (indirectly via the API) called by the user

        UidToMCParticleUidMapWeighted::iterator it = m_caloHitUidToMCParticleUidMap.find( caloHitUid );
	MCParticle* pMCParticle = m_uidToMCParticleMap.find( mcParticleUid )->second;

	// if no entry exists
 	if( it == m_caloHitUidToMCParticleUidMap.end() )
	{
		m_caloHitUidToMCParticleUidMap.insert( UidToMCParticleUidMapWeighted::value_type(caloHitUid, std::make_pair(mcParticleWeight,pMCParticle)) );
		return STATUS_CODE_SUCCESS;
        }
	else
	{
		// if an entry exists already
		// check if the weight of the old one is smaller than the weight of the new MCParticle 
		if( it->second.first < mcParticleWeight )
		{
			// remove it and replace it by the new one
			m_caloHitUidToMCParticleUidMap.erase( it );
			m_caloHitUidToMCParticleUidMap.insert( UidToMCParticleUidMapWeighted::value_type(caloHitUid, std::make_pair(mcParticleWeight,pMCParticle)) );
		}
	}
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::CreateCaloHitToMCParticleRelationships()
{
        ///< this method is internal not accessible for the user
	///< MCParticles are created from the known relationships

	for( UidToMCParticleUidMapWeighted::const_iterator itUid2MC = m_caloHitUidToMCParticleUidMap.begin(), itUid2MCEnd = m_caloHitUidToMCParticleUidMap.end(); 
	     itUid2MC != itUid2MCEnd; itUid2MC++ )
	{
		Uid mcParticleUid = itUid2MC->second.second;
		Uid caloHitUid    = itUid2MC->first;
		
		
		MCParticle* pMCParticle = NULL;
		PANDORA_RETURN_RESULT_IF( STATUS_CODE_SUCCESS, !=, RetrieveExistingOrCreateEmptyMCParticle(mcParticleUid, pMCParticle) ); ///< get the MCParticle for the uid (it's created if it's not existing)

		// check if I really got something 
		if (NULL == pMCParticle)
		{	
			return STATUS_CODE_FAILURE;
		}

		// check if the mcparticle has been initialized
		if (!pMCParticle->IsInitialized())
		{
			return STATUS_CODE_NOT_INITIALIZED;
		}

		m_caloHitUidToMCParticleMap.insert( std::make_pair(caloHitUid,pMCParticle) ); ///< store in map
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

StatusCode MCManager::CreateCaloHitToPfoTargetMap(UidToMCParticleMap &caloHitToPfoTargetMap)
{
	CreateCaloHitToMCParticleRelationships();

	for( UidToMCParticleMap::const_iterator itCaloUid2MC = m_caloHitUidToMCParticleMap.begin(), itCaloUid2MCEnd = m_caloHitUidToMCParticleMap.end(); 
	     itCaloUid2MC != itCaloUid2MCEnd; itCaloUid2MC++ )
	{
		MCParticle* pfoTarget = NULL;
		PANDORA_RETURN_RESULT_IF( STATUS_CODE_SUCCESS, != , itCaloUid2MC->second->GetPfoTarget( pfoTarget ));
		caloHitToPfoTargetMap.insert( std::make_pair(itCaloUid2MC->first, pfoTarget) );
	}

	return STATUS_CODE_SUCCESS;

//         for (UidToMCParticleMapWeighted::const_iterator relationIter = m_caloHitUidToMCParticleMap.begin(), relationIterEnd = m_caloHitUidToMCParticleMap.end();
// 	     relationIter != relationIterEnd; ++relationIter)
// 	{
// 	        UidToMCParticleMapWeighted::const_iterator mcParticleIter = m_caloHitUidToMCParticleMap.equal_range(relationIter->second.m_uid);

// 		if ((m_uidToMCParticleMap.end() == mcParticleIter) || (!mcParticleIter->second->IsInitialized()))
// 		         continue;

// 		MCParticle *pMCParticle = NULL;
// 		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mcParticleIter->second->GetPfoTarget(pMCParticle));

// 		if (!caloHitToPfoTargetMap.insert(UidToMCParticleMap::value_type(relationIter->first, pMCParticle)).second)
// 		         return STATUS_CODE_FAILURE;
// 	}

// 	return STATUS_CODE_SUCCESS;


//         // long version of MatchCaloHitsToMCPfoTargets
//         for (InputCaloHitList::iterator iter = m_inputCaloHitList.begin(), iterEnd = m_inputCaloHitList.end(); iter != iterEnd; ++iter)
// 	{
//   	        std::pair<UidToMCParticleMapWeighted::const_iterator,UidToMCParticleMapWeighted::const_iterator> pfoTargetMultiMapIterators;

// 	        pfoTargetMultiMapIterators = caloHitToPfoTargetMap.equal_range((*iter)->GetParentCaloHitAddress());
// 		if (pfoTargetMultiMapIterators.first == pfoTargetMultiMapIterators.second)
//  		         return STATUS_CODE_FAILURE;

// 		UidToMCParticleMapWeighted::const_iterator pfoTargetIter = pfoTargetMultiMapIterators.first;

// 		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetMCParticle( (*pfoTargetIter).second.second ));
// 	}

// 	return STATUS_CODE_SUCCESS;
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
	m_caloHitUidToMCParticleMap.clear();

	if (!m_uidToMCParticleMap.empty() || !m_caloHitUidToMCParticleMap.empty())
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
