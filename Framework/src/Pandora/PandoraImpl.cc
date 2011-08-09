/**
 *  @file   PandoraPFANew/Framework/src/Pandora/PandoraImpl.cc
 * 
 *  @brief  Implementation of the pandora impl class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/MCManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraImpl.h"
#include "Pandora/PandoraSettings.h"

namespace pandora
{

StatusCode PandoraImpl::PrepareMCParticles() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->SelectPfoTargets());

    UidToMCParticleMap caloHitToPfoTargetMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->CreateCaloHitToPfoTargetMap(caloHitToPfoTargetMap));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->MatchCaloHitsToMCPfoTargets(caloHitToPfoTargetMap));

    UidToMCParticleMap trackToPfoTargetMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->CreateTrackToPfoTargetMap(trackToPfoTargetMap));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->MatchTracksToMCPfoTargets(trackToPfoTargetMap));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pMCManager->DeleteNonPfoTargets());

    return STATUS_CODE_SUCCESS;
}
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::PrepareTracks() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->CreateInputList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->AssociateTracks());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::PrepareCaloHits() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->CreateInputList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->CalculateCaloHitProperties());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializeAlgorithms(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pAlgorithmManager->InitializeAlgorithms(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializePlugins(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pPluginManager->InitializePlugins(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::RunAlgorithm(const std::string &algorithmName) const
{
    return m_pPandora->m_pPandoraContentApiImpl->RunAlgorithm(algorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraImpl::PandoraImpl(Pandora *pPandora) :
    m_pPandora(pPandora)
{
}

} // namespace pandora
