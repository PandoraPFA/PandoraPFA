/**
 *	@file	PandoraPFANew/src/Api/PandoraApiImpl.cc
 * 
 *	@brief	Implementation of the pandora api class.
 * 
 *	$Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/TrackManager.h"

#include "GeometryHelper.h"

namespace pandora
{

template <typename PARAMETERS>
StatusCode PandoraApiImpl::CreateObject(const PARAMETERS &parameters) const
{
	return STATUS_CODE_FAILURE;
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::CaloHitParameters>(const PandoraApi::CaloHitParameters &caloHitParameters) const
{
	return m_pPandora->m_pCaloHitManager->CreateCaloHit(caloHitParameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::TrackParameters>(const PandoraApi::TrackParameters &trackParameters) const
{
	return m_pPandora->m_pTrackManager->CreateTrack(trackParameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::GeometryParameters>(const PandoraApi::GeometryParameters &geometryParameters) const
{
	return m_pPandora->m_pGeometryHelper->Initialize(geometryParameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::MCParticleParameters>(const PandoraApi::MCParticleParameters &mcParticleParameters) const
{
	return m_pPandora->m_pMCManager->CreateMCParticle(mcParticleParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::ProcessEvent() const
{
	return m_pPandora->ProcessEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::InitializeAlgorithms() const
{
	return m_pPandora->m_pAlgorithmManager->InitializeAlgorithms();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::RegisterAlgorithmFactory(const std::string &algorithmType, AlgorithmFactory *const pAlgorithmFactory) const
{
	return m_pPandora->m_pAlgorithmManager->RegisterAlgorithmFactory(algorithmType, pAlgorithmFactory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetMCParentDaughterRelationship(const void *pParentAddress, const void *pDaughterAddress) const
{
	return m_pPandora->m_pMCManager->SetMCParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetCaloHitToMCParticleRelationship(const void *pCaloHitParentAddress, const void *pMCParticleParentAddress,
	const float mcParticleWeight) const
{
	return m_pPandora->m_pMCManager->SetCaloHitToMCParticleRelationship(pCaloHitParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::GetParticleFlowObjects(PandoraApi::ParticleFlowObjectList &particleFlowObjectList) const
{
	return m_pPandora->m_pParticleFlowObjectManager->GetParticleFlowObjects(particleFlowObjectList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::ResetForNextEvent() const
{
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetForNextEvent());
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetForNextEvent());
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pParticleFlowObjectManager->ResetForNextEvent());
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetForNextEvent());

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraApiImpl::PandoraApiImpl(Pandora *pPandora) :
	m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode PandoraApiImpl::CreateObject<PandoraApi::CaloHitParameters>(const PandoraApi::CaloHitParameters &caloHitParameters) const;
template StatusCode PandoraApiImpl::CreateObject<PandoraApi::TrackParameters>(const PandoraApi::TrackParameters &trackParameters) const;
template StatusCode PandoraApiImpl::CreateObject<PandoraApi::GeometryParameters>(const PandoraApi::GeometryParameters &geometryParameters) const;
template StatusCode PandoraApiImpl::CreateObject<PandoraApi::MCParticleParameters>(const PandoraApi::MCParticleParameters &mcParticleParameters) const;

} // namespace pandora
