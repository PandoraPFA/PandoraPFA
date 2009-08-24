/**
 *	@file	PandoraPFANew/include/Pandora.h
 * 
 *	@brief	Header file for the pandora class.
 * 
 *	$Log: $
 */
#ifndef PANDORA_MAIN_H
#define PANDORA_MAIN_H 1

#include "BasicTypes.h"
#include "StatusCodes.h"

namespace pandora
{

class Algorithm;
class CaloHitManager;
class ClusterManager;
class GeometryHelper;
class MCManager;
class PandoraApiImpl;
class PandoraContentApiImpl;
class ParticleFlowObjectManager;
class TrackManager;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *	@brief Pandora class
 */
class Pandora
{
public:
	/**
	 *	@brief	Default constructor	
	 */
	Pandora();

	/**
	 *	@brief	Destructor	
	 */
	~Pandora();
	
	/**
	 *	@brief	Process event
	 */
	StatusCode ProcessEvent();
	
	/**
	 *	@brief	Get the pandora api impl
	 * 
	 *	@return	Address of the pandora api impl
	 */
	const PandoraApiImpl *const GetPandoraApiImpl() const;

	/**
	 *	@brief	Get the pandora content api impl
	 * 
	 *	@return	Address of the pandora content api impl
	 */
	const PandoraContentApiImpl *const GetPandoraContentApiImpl() const;

private:
	/**
	 *	@brief	Register pandora internal algorithms
	 */
	StatusCode RegisterInternalAlgorithms();

	typedef std::map<const std::string, Algorithm *const> AlgorithmMap;
	
	CaloHitManager					*m_pCaloHitManager;				///< The hit manager
	ClusterManager					*m_pClusterManager;				///< The cluster manager
	GeometryHelper					*m_pGeometryHelper;				///< The geometry helper
	MCManager						*m_pMCManager;					///< The MC manager
	ParticleFlowObjectManager		*m_pParticleFlowObjectManager;	///< The particle flow object manager
	TrackManager					*m_pTrackManager;				///< The track manager

	AlgorithmMap					m_algorithmMap;					///< The algorithm map

	PandoraApiImpl					*m_pPandoraApiImpl;				///< The pandora api implementation
	PandoraContentApiImpl			*m_pPandoraContentApiImpl;		///< The pandora content api implementation
	
	friend class PandoraApiImpl;
	friend class PandoraContentApiImpl;	
};

} // namespace pandora

#endif // #ifndef PANDORA_MAIN_H
