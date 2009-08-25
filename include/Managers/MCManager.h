/**
 *	@file	PandoraPFANew/include/Managers/MCManager.h
 * 
 *	@brief	Header file for the mc particle manager class.
 * 
 *	$Log: $
 */
#ifndef MC_MANAGER_H
#define MC_MANAGER_H 1

#include "Api/PandoraApi.h"

#include "Objects/MCParticle.h"

namespace pandora
{
	
/**
 *	@brief MCManager class
 */
class MCManager
{
public:
	/**
	 *	@brief	MCPfoSelection class
	 */
	class MCPfoSelection
	{
	public:
		/**
		 *	@brief	Apply mc pfo selection rules 
		 *
		 *	@params	mcRootParticle address of the mc root particle
		 */
		virtual StatusCode ApplySelectionRules(MCParticle *const mcRootParticle) const;
	};

	/**
	 *	@brief	Constructor
	 */
	MCManager();

	/**
	 *	@brief	Constructor 
	 *
	 *	@params	mcPfoSelection the mc pfo selection instance
	 */
	MCManager(const MCPfoSelection* mcPfoSelection);

	/**
	 *	@brief	Destructor
	 */
	~MCManager();

private:
	/**
	 *	@brief	Create a mc particle
	 * 
	 *	@param	mcParticleParameters the mc particle parameters
	 */
	StatusCode CreateMCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters);

	/**
	 *	@brief	Retrieve a mc particle, creating an empty mc particle if it does not already exist
	 * 
	 *	@param	mcParticleUid the unique identifier of the mc particle
	 *	@param	pMCParticle to receive the address of the mc particle
	 */
	StatusCode RetrieveExistingOrCreateEmptyMCParticle(const Uid mcParticleUid, MCParticle *&pMCParticle);
	
	/**
	 *	@brief	Set mc particle relationship
	 * 
	 *	@param	parentUid the parent unique identifier
	 *	@param	daughterUid the daughter unique identifier
	 */
	StatusCode SetMCParentDaughterRelationship(const Uid parentUid, const Uid daughterUid);

	/**
	 *	@brief	Set calo hit to mc particle relationship
	 * 
	 *	@param	caloHitUid the calo hit unique identifier
	 *	@param	mcParticleUid the mc particle unique identifier
	 *	@param	mcParticleWeight weighting to assign to the mc particle
	 */
	StatusCode SetCaloHitToMCParticleRelationship(const Uid caloHitUid, const Uid mcParticleUid, const float mcParticleWeight);

	/**
	 *	@brief	Select pfo targets
	 */
	StatusCode SelectPfoTargets();
	
	/**
	 *	@brief	Create a map relating calo hit uid to mc pfo target
	 * 
	 *	@param	caloHitToPfoTargetMap to receive the calo hit uid to mc pfo target map
	 */
	StatusCode CreateCaloHitToPfoTargetMap(UidToMCParticleMap &caloHitToPfoTargetMap) const;
	
	/**
	 *	@brief	Delete non pfo targets
	 */
	StatusCode DeleteNonPfoTargets();

	/**
	 *	@brief	Reset the mc manager
	 */	
	StatusCode ResetForNextEvent();
	
	/**
	 *	@brief	UidAndWeight class
	 */	
	class UidAndWeight
	{
	public:
		/**
		 *	@brief	UidAndWeight class
		 * 
		 *	@param	uid the unique identifier
		 *	@param	weight the weight
		 */	
		UidAndWeight(const Uid uid, const float weight);

		Uid					m_uid;						///< The unique identifier
		float				m_weight;					///< The weight
	};

	typedef std::map<Uid, UidAndWeight> UidRelationMap;

	UidToMCParticleMap		m_uidToMCParticleMap;		///< The uid to mc particle map
	UidRelationMap			m_caloHitToMCParticleMap;	///< The calo hit to mc particle relation map

	const MCPfoSelection	*m_pMCPfoSelection;			///< The mc pfo selection instance

	friend class PandoraApiImpl;
	friend class PandoraContentApiImpl;
	friend class TestMCManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline MCManager::UidAndWeight::UidAndWeight(const Uid uid, const float weight) :
	m_uid(uid),
	m_weight(weight)
{
}

} // namespace pandora

#endif // #ifndef MC_MANAGER_H
