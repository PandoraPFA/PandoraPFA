/**
 *  @file   PandoraPFANew/src/PandoraImpl.cc
 * 
 *  @brief  Implementation of the pandora impl class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/MCManager.h"
#include "Managers/TrackManager.h"

#include "Pandora.h"
#include "PandoraImpl.h"
#include "PandoraSettings.h"

namespace pandora
{

StatusCode PandoraImpl::MatchObjectsToMCPfoTargets() const
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

StatusCode PandoraImpl::OrderInputCaloHits() const
{
    return m_pPandora->m_pCaloHitManager->OrderInputCaloHits();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::InitializeAlgorithms(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pAlgorithmManager->InitializeAlgorithms(pXmlHandle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraImpl::ReadPandoraSettings(const TiXmlHandle *const pXmlHandle) const
{
    return m_pPandora->m_pPandoraSettings->Read(pXmlHandle);
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
