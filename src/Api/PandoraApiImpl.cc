/**
 *  @file   PandoraPFANew/src/Api/PandoraApiImpl.cc
 * 
 *  @brief  Implementation of the pandora api class.
 * 
 *  $Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"

#include "Helpers/CaloHitHelper.h"
#include "Helpers/GeometryHelper.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"

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
StatusCode PandoraApiImpl::CreateObject<PandoraApi::MCParticleParameters>(const PandoraApi::MCParticleParameters &mcParticleParameters) const
{
    return m_pPandora->m_pMCManager->CreateMCParticle(mcParticleParameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::GeometryParameters>(const PandoraApi::GeometryParameters &geometryParameters) const
{
    return m_pPandora->m_pGeometryHelper->Initialize(geometryParameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::BoxGapParameters>(const PandoraApi::BoxGapParameters &gapParameters) const
{
    return m_pPandora->m_pGeometryHelper->CreateBoxGap(gapParameters);
}

template <>
StatusCode PandoraApiImpl::CreateObject<PandoraApi::ConcentricGapParameters>(const PandoraApi::ConcentricGapParameters &gapParameters) const
{
    return m_pPandora->m_pGeometryHelper->CreateConcentricGap(gapParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::ProcessEvent() const
{
    return m_pPandora->ProcessEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::ReadSettings(const std::string &xmlFileName) const
{
    return m_pPandora->ReadSettings(xmlFileName);
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

StatusCode PandoraApiImpl::SetTrackParentDaughterRelationship(const void *pParentAddress, const void *pDaughterAddress) const
{
    return m_pPandora->m_pTrackManager->SetTrackParentDaughterRelationship(pParentAddress, pDaughterAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetTrackSiblingRelationship(const void *pFirstSiblingAddress, const void *pSecondSiblingAddress) const
{
    return m_pPandora->m_pTrackManager->SetTrackSiblingRelationship(pFirstSiblingAddress, pSecondSiblingAddress);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetCaloHitToMCParticleRelationship(const void *pCaloHitParentAddress, const void *pMCParticleParentAddress,
    const float mcParticleWeight) const
{
    return m_pPandora->m_pMCManager->SetCaloHitToMCParticleRelationship(pCaloHitParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetTrackToMCParticleRelationship(const void *pTrackParentAddress, const void *pMCParticleParentAddress,
    const float mcParticleWeight) const
{
    return m_pPandora->m_pMCManager->SetTrackToMCParticleRelationship(pTrackParentAddress, pMCParticleParentAddress, mcParticleWeight);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::GetParticleFlowObjects(ParticleFlowObjectList &particleFlowObjectList) const
{
    return m_pPandora->m_pParticleFlowObjectManager->GetParticleFlowObjects(particleFlowObjectList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetBFieldCalculator(BFieldCalculator *pBFieldCalculator) const
{
    return m_pPandora->m_pGeometryHelper->SetBFieldCalculator(pBFieldCalculator);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetPseudoLayerCalculator(PseudoLayerCalculator *pPseudoLayerCalculator) const
{
    return m_pPandora->m_pGeometryHelper->SetPseudoLayerCalculator(pPseudoLayerCalculator);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::SetHitTypeGranularity(const HitType hitType, const Granularity granularity) const
{
    return m_pPandora->m_pGeometryHelper->SetHitTypeGranularity(hitType, granularity);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::RegisterEnergyCorrectionFunction(const std::string &functionName, const EnergyCorrectionType energyCorrectionType,
    EnergyCorrectionFunction *pEnergyCorrectionFunction) const
{
    return m_pPandora->m_pPluginManager->RegisterEnergyCorrectionFunction(functionName, energyCorrectionType, pEnergyCorrectionFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::RegisterParticleIdFunction(const std::string &functionName, ParticleIdFunction *pParticleIdFunction) const
{
    return m_pPandora->m_pPluginManager->RegisterParticleIdFunction(functionName, pParticleIdFunction);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraApiImpl::ResetForNextEvent() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pParticleFlowObjectManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetForNextEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ClearCaloHitUsageMaps());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraApiImpl::PandoraApiImpl(Pandora *pPandora) :
    m_pPandora(pPandora)
{
}

} // namespace pandora
