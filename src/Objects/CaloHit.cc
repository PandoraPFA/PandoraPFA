/**
 *	@file PandoraPFANew/src/Objects/CaloHit.cc
 * 
 *	@brief Implementation of the calo hit class.
 * 
 *	$Log: $
 */

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

namespace pandora
{

bool CaloHit::operator< (const CaloHit &rhs) const
{
	return m_energy > rhs.m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetPfoTarget()
{
        MCParticle* pMCParticle = NULL;
	PANDORA_RETURN_RESULT_IF( STATUS_CODE_SUCCESS, !=, GetMCParticle( pMCParticle ) );

	if (NULL == pMCParticle)
		return STATUS_CODE_FAILURE;

	pMCParticle->GetPfoTarget( pMCParticle );
	
	PANDORA_RETURN_RESULT_IF( STATUS_CODE_SUCCESS, !=, SetMCParticle( pMCParticle ) );
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::CaloHit(const PandoraApi::CaloHitParameters &caloHitParameters) :
	m_isSortedIntoPseudoLayer(false),
	m_energy(caloHitParameters.m_energy),
	m_layer(caloHitParameters.m_layer),
	m_pMCParticle(NULL),
	m_pParentAddress(caloHitParameters.m_pParentAddress)
{
}

} // namespace pandora
