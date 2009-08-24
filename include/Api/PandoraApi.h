/**
 *	@file	PandoraPFANew/include/Api/PandoraApi.h
 *
 * 	@brief	Header file for the pandora api class.
 *
 *	$Log: $
 */
#ifndef PANDORA_API_H
#define PANDORA_API_H 1

#include "Pandora.h"

namespace pandora { class Algorithm; }

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *	@brief	PandoraApi class
 */
class PandoraApi
{
public:
	/**
	 *	@brief	Object creation helper class
	 * 
	 *	@param	PARAMETERS the type of object parameters
	 */
	template <typename PARAMETERS>
	class ObjectCreationHelper
	{
	public:
		typedef PARAMETERS Parameters;
		
		/**
		 *	@brief	Create a new object
		 * 
		 *	@param	pandora the pandora instance to create the new object
		 *	@param	parameters the object parameters
		 */
		 static StatusCode Create(const pandora::Pandora &pandora, const Parameters &parameters);
	};

	/**
	 *	@brief	CaloHitParameters class
	 */
	class CaloHitParameters
	{
	public:
		float	m_energy;			///< The calo hit energy
		void	*m_pParentAddress;	///< The address of the parent calo hit in the user framework
	};

	/**
	 *	@brief	TrackParameters class
	 */
	class TrackParameters
	{
	public:
		float	m_momentum;			///< The track momentum
		void	*m_pParentAddress;	///< The address of the parent track in the user framework
	};

	/**
	 *	@brief	Geometry parameters
	 */
	class GeometryParameters
	{
	public:
		float	m_tpcInnerRadius;	///< The inner tpc radius
	};

	/**
	 *	@brief	Monte Carlo particle parameters
	 */
	class MCParticleParameters
	{
	public:
		float	m_energy;	        ///< The MC particle's energy
		float	m_momentum;			///< The MC particle's momentum
		float	m_innerRadius;		///< The MC particle's path's inner radius
		float	m_outerRadius;		///< The MC particle's path's outer radius
		int		m_particleId;		///< The MC particle's ID (PDG code)
		void	*m_pParentAddress;	///< The address of the parent MC particle in the user framework
	};

	// Objects available for construction by pandora
	typedef ObjectCreationHelper<CaloHitParameters> CaloHit;
	typedef ObjectCreationHelper<TrackParameters> Track;
	typedef ObjectCreationHelper<GeometryParameters> Geometry;	
	typedef ObjectCreationHelper<GeometryParameters> MCParticle;	

	/**
	 *	@brief	Process an event
	 * 
	 *	@param	pandora the pandora instance to process event
	 */
	static StatusCode ProcessEvent(const pandora::Pandora &pandora);
	
	/**
	 *	@brief	Register an algorithm with pandora
	 * 
	 *	@param	pandora the pandora instance to register the algorithm with
	 *	@param	pAlgorithm address of an algorithm instance
	 *	@param	algorithmName the algorithm name
	 */
	static StatusCode RegisterAlgorithm(const pandora::Pandora &pandora, const std::string &algorithmName,
		pandora::Algorithm *const pAlgorithm);

	/**
	 *	@brief	Set parent-daughter mc particle relationship
	 * 
	 *	@param	pandora the pandora instance to register the relationship with
	 *	@param	pParentAddress address of parent mc particle in the user framework
	 *	@param	pDaughterAddress address of daughter mc particle in the user framework
	 */
	static StatusCode SetMCParentDaughterRelationship(const pandora::Pandora &pandora, const void *pParentAddress,
		const void *pDaughterAddress);

	/**
	 *	@brief	Set calo hit to mc particle relationshipip
	 * 
	 *	@param	pandora the pandora instance to register the relationship with
	 *	@param	pCaloHitParentAddress address of calo hit in the user framework
	 *	@param	pMCParticleParentAddress address of mc particle in the user framework
	 *	@param	mcParticleWeight weighting to assign to the mc particle
	 */
	static StatusCode SetCaloHitToMCParticleRelationship(const pandora::Pandora &pandora, const void *pCaloHitParentAddress,
		const void *pMCParticleParentAddress, const float mcParticleWeight = 1);

	/**
	 *	@brief	ParticleFlowObject class
	 */
	class ParticleFlowObject
	{
	public:
		float							m_energy;					///< The particle flow object energy
		pandora::TrackAddressVector		m_trackAddressVector;		///< The track address vector
		pandora::ClusterAddressVector	m_clusterAddressVector;		///< The cluster address vector
	};	

	typedef std::vector<ParticleFlowObject *> ParticleFlowObjectList;

	/**
	 *	@brief	Get the particle flow objects
	 * 
	 *	@param	pandora the pandora instance to get the objects from
	 * 	@param	pfoVector container to receive the particle flow objects
	 */
	static StatusCode GetParticleFlowObjects(const pandora::Pandora &pandora, ParticleFlowObjectList &particleFlowObjectList);

	/**
	 *	@brief	Reset pandora to process another event
	 * 
	 *	@param	pandora the pandora instance to reset
	 */
	static StatusCode Reset(const pandora::Pandora &pandora);
};

#endif // #ifndef PANDORA_API_H
