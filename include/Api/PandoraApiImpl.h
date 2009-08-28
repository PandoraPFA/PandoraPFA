/**
 *	@file	PandoraPFANew/include/Api/PandoraApiImpl.h
 *
 * 	@brief	Header file for the pandora api implementation class.
 * 
 *	$Log: $
 */
#ifndef PANDORA_API_IMPL_H
#define PANDORA_API_IMPL_H 1

#include "PandoraApi.h"

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *	@brief PandoraApiImpl class
 */
class PandoraApiImpl
{
public:
	/**
	 *	@brief	Create an object for pandora
	 * 
	 *	@param	parameters the object parameters
	 */
	template <typename PARAMETERS>
	StatusCode CreateObject(const PARAMETERS &parameters) const;

	/**
	 *	@brief	Process event
	 */
	StatusCode ProcessEvent() const;

	/**
	 *	@brief	Read pandora settings
	 * 
	 *	@param	xmlFileName the name of the xml file containing the settings
	 */
	StatusCode ReadSettings(const std::string &xmlFileName) const;

	/**
	 *	@brief	Register an algorithm factory with pandora
	 * 
	 *	@param	algorithmType the type of algorithm that the factory will create
	 *	@param	pAlgorithmFactory the address of an algorithm factory instance
	 */
	StatusCode RegisterAlgorithmFactory(const std::string &algorithmType, AlgorithmFactory *const pAlgorithmFactory) const;
	
	/**
	 *	@brief	Set parent-daughter mc particle relationship
	 * 
	 *	@param	pParentAddress address of parent mc particle in the user framework
	 *	@param	pDaughterAddress address of daughter mc particle in the user framework
	 */
	StatusCode SetMCParentDaughterRelationship(const void *pParentAddress, const void *pDaughterAddress) const;

	/**
	 *	@brief	Set calo hit to mc particle relationship
	 * 
	 *	@param	pCaloHitParentAddress address of calo hit in the user framework
	 *	@param	pMCParticleParentAddress address of mc particle in the user framework
	 *	@param	mcParticleWeight weighting to assign to the mc particle
	 */
	StatusCode SetCaloHitToMCParticleRelationship(const void *pCaloHitParentAddress, const void *pMCParticleParentAddress,
		const float mcParticleWeight) const;

	/**
	 *	@brief	Get the particle flow objects
	 * 
	 * 	@param	particleFlowObjectList container to receive the particle flow objects
	 */
	StatusCode GetParticleFlowObjects(PandoraApi::ParticleFlowObjectList &particleFlowObjectList) const;

	/**
	 *	@brief	Reset pandora to process another event
	 */
	StatusCode ResetForNextEvent() const;

private:
	/**
	 *	@brief	Constructor
	 * 
	 *	@param	pPandora address of the pandora object to interface
	 */
	PandoraApiImpl(Pandora *pPandora);

	Pandora	*m_pPandora;	///< The pandora object to provide an interface to
	
	friend class Pandora;
};

} // namespace pandora

#endif // #ifndef PANDORA_API_IMPL_H
