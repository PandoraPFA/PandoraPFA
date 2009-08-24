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

StatusCode CaloHit::GetPseudoLayer(PseudoLayer &pseudoLayer) const
{
	if (!m_isSortedIntoPseudoLayer)
		return STATUS_CODE_NOT_INITIALIZED;

	pseudoLayer = m_pseudoLayer;
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::GetMCParticle(MCParticle *pMCParticle) const
{
	if (NULL == m_pMCParticle)
		return STATUS_CODE_NOT_INITIALIZED;

	pMCParticle = m_pMCParticle;
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::CaloHit(const PandoraApi::CaloHitParameters &caloHitParameters) :
	m_isSortedIntoPseudoLayer(false),
	m_energy(caloHitParameters.m_energy),
	m_pMCParticle(NULL),
	m_pParentAddress(caloHitParameters.m_pParentAddress)
{
}

} // namespace pandora
