/**
 *	@file PandoraPFANew/src/Api/PandoraApi.cc
 * 
 *	@brief Redirection for pandora api class to its implementation.
 * 
 *	$Log: $
 */

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"

template <typename PARAMETERS>
StatusCode PandoraApi::ObjectCreationHelper<PARAMETERS>::Create(const pandora::Pandora &pandora, const Parameters &parameters)
{
	return pandora.GetPandoraApiImpl()->CreateObject(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApi::ProcessEvent(const pandora::Pandora &pandora)
{
	return pandora.GetPandoraApiImpl()->ProcessEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApi::InitializeAlgorithms(const pandora::Pandora &pandora)
{
	return pandora.GetPandoraApiImpl()->InitializeAlgorithms();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApi::RegisterAlgorithmFactory(const pandora::Pandora &pandora, const std::string &algorithmType,
	pandora::AlgorithmFactory *const pAlgorithmFactory)
{
	return pandora.GetPandoraApiImpl()->RegisterAlgorithmFactory(algorithmType, pAlgorithmFactory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SetMCParentDaughterRelationship(const pandora::Pandora &pandora, const void *pParentAddress, const void *pDaughterAddress)
{
	return pandora.GetPandoraApiImpl()->SetMCParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SetCaloHitToMCParticleRelationship(const pandora::Pandora &pandora, const void *pCaloHitParentAddress,
	const void *pMCParticleParentAddress, const float mcParticleWeight)
{
	return pandora.GetPandoraApiImpl()->SetCaloHitToMCParticleRelationship(pCaloHitParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApi::GetParticleFlowObjects(const pandora::Pandora &pandora, PandoraApi::ParticleFlowObjectList &particleFlowObjectList)
{
	return pandora.GetPandoraApiImpl()->GetParticleFlowObjects(particleFlowObjectList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApi::Reset(const pandora::Pandora &pandora)
{
	return pandora.GetPandoraApiImpl()->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class PandoraApi::ObjectCreationHelper<PandoraApi::CaloHitParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::TrackParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::GeometryParameters>;
template class PandoraApi::ObjectCreationHelper<PandoraApi::MCParticleParameters>;
