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

class AlgorithmManager;
class CaloHitManager;
class ClusterManager;
class GeometryHelper;
class MCManager;
class PandoraApiImpl;
class PandoraContentApiImpl;
class ParticleFlowObjectManager;
class PandoraSettings;
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
	 *	@brief	Process event
	 */
	StatusCode ProcessEvent();

	/**
	 *	@brief	Read pandora settings
	 * 
	 *	@param	xmlFileName the name of the xml file containing the settings
	 */
	StatusCode ReadSettings(const std::string &xmlFileName);

	AlgorithmManager				*m_pAlgorithmManager;			///< The algorithm manager
	CaloHitManager					*m_pCaloHitManager;				///< The hit manager
	ClusterManager					*m_pClusterManager;				///< The cluster manager
	GeometryHelper					*m_pGeometryHelper;				///< The geometry helper
	MCManager						*m_pMCManager;					///< The MC manager
	ParticleFlowObjectManager		*m_pParticleFlowObjectManager;	///< The particle flow object manager
	TrackManager					*m_pTrackManager;				///< The track manager

	PandoraSettings					*m_pPandoraSettings;			///< The pandora settings

	PandoraApiImpl					*m_pPandoraApiImpl;				///< The pandora api implementation
	PandoraContentApiImpl			*m_pPandoraContentApiImpl;		///< The pandora content api implementation
	
	friend class PandoraApiImpl;
	friend class PandoraContentApiImpl;	
};

} // namespace pandora

#endif // #ifndef PANDORA_MAIN_H
