/**
 *	@file PandoraPFANew/src/Pandora.cc
 * 
 *	@brief Implementation of the pandora class.
 * 
 *	$Log: $
 */

#include "GeometryHelper.h"
#include "Pandora.h"

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"
#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/TrackManager.h"

namespace pandora
{

Pandora::Pandora() :
	m_pAlgorithmManager(new AlgorithmManager(this)),
	m_pCaloHitManager(new CaloHitManager),
	m_pClusterManager(new ClusterManager),
	m_pGeometryHelper(new GeometryHelper),
	m_pMCManager(new MCManager),
	m_pParticleFlowObjectManager(new ParticleFlowObjectManager),
	m_pTrackManager(new TrackManager),
	m_pPandoraApiImpl(new PandoraApiImpl(this)),
	m_pPandoraContentApiImpl(new PandoraContentApiImpl(this))	
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

Pandora::~Pandora()
{
	delete m_pAlgorithmManager;
	delete m_pCaloHitManager;
	delete m_pClusterManager;
	delete m_pGeometryHelper;
	delete m_pMCManager;
	delete m_pParticleFlowObjectManager;
	delete m_pTrackManager;
	delete m_pPandoraApiImpl;
	delete m_pPandoraContentApiImpl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::ProcessEvent()
{
	std::cout << "Pandora process event" << std::endl;

	// Prepare event
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MatchCaloHitsToMCPfoTargets(*this));
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::OrderInputCaloHits(*this));

	// Loop over algorithms
	const StringVector *const pPandoraAlgorithms = m_pAlgorithmManager->GetPandoraAlgorithms();

	for (StringVector::const_iterator iter = pPandoraAlgorithms->begin(), iterEnd = pPandoraAlgorithms->end(); iter != iterEnd; ++iter)
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunAlgorithm(*this, *iter));

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraApiImpl *const Pandora::GetPandoraApiImpl() const
{
	return m_pPandoraApiImpl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraContentApiImpl *const Pandora::GetPandoraContentApiImpl() const
{
	return m_pPandoraContentApiImpl;
}

} // namespace pandora
