/**
 *	@file	PandoraPFANew/include/Objects/CaloHit.h
 * 
 *	@brief	Header file for the calo hit class.
 * 
 *	$Log: $
 */
#ifndef CALO_HIT_H
#define CALO_HIT_H 1

#include "Api/PandoraApi.h"

namespace pandora
{

class MCParticle;

//------------------------------------------------------------------------------------------------------------------------------------------
	
/**
 *	@brief	CaloHit class
 */
class CaloHit
{
public:
	/**
	 *	@brief	Operator< now orders by calo hit raw energy
	 * 
	 *	@param	rhs calo hit to compare with
	 */
	bool operator< (const CaloHit &rhs) const;	

	/**
	 *	@brief	Get address of the mc particle associated with the calo hit
	 * 
	 *	@param	pMCParticle to receive the address of the mc particle
	 */
	StatusCode GetMCParticle(MCParticle *pMCParticle) const;
	
	/**
	 *	@brief	Get pseudo layer for the calo hit
	 * 
	 *	@param	pseudoLayer to receive the pseudo layer
	 */
	StatusCode GetPseudoLayer(PseudoLayer &pseudoLayer) const;

	/**
	 *	@brief	Get the address of the parent calo hit in the user framework
	 */
	 void *GetParentCaloHitAddress() const;

private:
	/**
	 *	@brief	Constructor
	 * 
	 *	@param	parameters the calo hit parameters
	 */
	CaloHit(const PandoraApi::CaloHitParameters &caloHitParameters);

	/**
	 *	@brief	Set the mc particle associated with the calo hit
	 * 
	 *	@param	pMCParticle to receive the address of the mc particle
	 */
	StatusCode SetMCParticle(MCParticle *const pMCParticle);

	/**
	 *	@brief	Set the mc pseudo layer for the calo hit
	 * 
	 *	@param	pseudoLayer the pseudo layer
	 */
	StatusCode SetPseudoLayer(const PseudoLayer &pseudoLayer);

	/**
	 *	@brief	Get the pfo target from the mc particle and set it to be the new mc particle
	 * 
	 */
	StatusCode SetPfoTarget();

	bool			m_isSortedIntoPseudoLayer;	///< Whether the calo hit has been sorted into a pseudo layer
	
	float			m_energy;					///< The calo hit energy
	unsigned int	m_layer;					///< The layer in which the calo hit is located
	PseudoLayer		m_pseudoLayer;				///< The pseudo layer
	
	MCParticle		*m_pMCParticle;				///< The associated MC particle
	void			*m_pParentAddress;			///< The address of the parent calo hit in the user framework

	friend class CaloHitManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline void *CaloHit::GetParentCaloHitAddress() const
{
	return m_pParentAddress;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHit::GetMCParticle(MCParticle *pMCParticle) const
{
	if (NULL == m_pMCParticle)
		return STATUS_CODE_NOT_INITIALIZED;

	pMCParticle = m_pMCParticle;
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHit::GetPseudoLayer(PseudoLayer &pseudoLayer) const
{
	if (!m_isSortedIntoPseudoLayer)
		return STATUS_CODE_NOT_INITIALIZED;

	pseudoLayer = m_pseudoLayer;
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHit::SetMCParticle(MCParticle *const pMCParticle)
{
	if (NULL == pMCParticle)
		return STATUS_CODE_FAILURE;

	m_pMCParticle = pMCParticle;
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHit::SetPseudoLayer(const PseudoLayer &pseudoLayer)
{
	m_pseudoLayer = pseudoLayer;
	m_isSortedIntoPseudoLayer = true;
	
	return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef CALO_HIT_H
