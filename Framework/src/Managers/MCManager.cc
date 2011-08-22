/**
 *  @file   PandoraPFANew/Framework/src/Managers/MCManager.cc
 * 
 *  @brief  Implementation of the mc manager class.
 * 
 *  $Log: $
 */

#include "Managers/MCManager.h"

#include "Objects/MCParticle.h"

#include "Pandora/PandoraSettings.h"

namespace pandora
{

MCManager::~MCManager()
{
    (void) this->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::CreateMCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters)
{
    try
    {
        UidToMCParticleMap::iterator iter = m_uidToMCParticleMap.find(mcParticleParameters.m_pParentAddress.Get());

        if (m_uidToMCParticleMap.end() != iter)
        {
            iter->second->SetProperties(mcParticleParameters);
        }
        else
        {
            MCParticle *pMCParticle = new MCParticle(mcParticleParameters);

            if (!m_uidToMCParticleMap.insert(UidToMCParticleMap::value_type(mcParticleParameters.m_pParentAddress.Get(), pMCParticle)).second)
                return STATUS_CODE_FAILURE;
        }

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create mc particle: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::RetrieveExistingOrCreateEmptyMCParticle(const Uid mcParticleUid, MCParticle *&pMCParticle)
{
    UidToMCParticleMap::const_iterator iter = m_uidToMCParticleMap.find(mcParticleUid);

    if(m_uidToMCParticleMap.end() != iter) 
    {
        pMCParticle = iter->second;

        if (NULL == pMCParticle)
            return STATUS_CODE_FAILURE;
    }
    else
    {
        pMCParticle = new MCParticle(mcParticleUid);

        if ((NULL == pMCParticle) || (!m_uidToMCParticleMap.insert(UidToMCParticleMap::value_type(mcParticleUid, pMCParticle)).second))
            return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SetMCParentDaughterRelationship(const Uid parentUid, const Uid daughterUid)
{
    MCParticle* pParent = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, RetrieveExistingOrCreateEmptyMCParticle(parentUid, pParent));

    MCParticle* pDaughter = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, RetrieveExistingOrCreateEmptyMCParticle(daughterUid, pDaughter));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pParent->AddDaughter(pDaughter));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pDaughter->AddParent(pParent));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SetUidToMCParticleRelationship(const Uid objectUid, const Uid mcParticleUid, const float mcParticleWeight,
    UidRelationMap &uidRelationMap)
{
    UidRelationMap::iterator iter = uidRelationMap.find(objectUid);

    if (uidRelationMap.end() != iter)
    {
        if (mcParticleWeight > iter->second.m_weight)
            iter->second = UidAndWeight(mcParticleUid, mcParticleWeight);
    }
    else
    {
        if (!uidRelationMap.insert(UidRelationMap::value_type(objectUid, UidAndWeight(mcParticleUid, mcParticleWeight))).second)
            return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SelectPfoTargets()
{
    for (UidToMCParticleMap::iterator iter = m_uidToMCParticleMap.begin(), iterEnd = m_uidToMCParticleMap.end(); iter != iterEnd; ++iter)
    {
        MCParticleList mcPfoList;

        if (iter->second->IsRootParticle())
        {
            PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_INITIALIZED, !=, this->ApplyPfoSelectionRules(iter->second, mcPfoList));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::ApplyPfoSelectionRules(MCParticle *const pMCParticle, MCParticleList &mcPfoList) const
{
    if (!pMCParticle->IsInitialized())
        return STATUS_CODE_NOT_INITIALIZED;

    static const float selectionRadius(PandoraSettings::GetMCPfoSelectionRadius());
    static const float selectionMomentum(PandoraSettings::GetMCPfoSelectionMomentum());
    static const float selectionEnergyCutOffProtonsNeutrons(PandoraSettings::GetMCPfoSelectionLowEnergyNeutronProtonCutOff());

    const int particleId(pMCParticle->GetParticleId());

    // ATTN: don't take particles from previously used decay chains; could happen because mc particles can have multiple parents.
    // Of those, some don't know the daughter.
    if ((pMCParticle->GetOuterRadius() > selectionRadius) &&
        (pMCParticle->GetInnerRadius() <= selectionRadius) &&
        (pMCParticle->GetMomentum().GetMagnitude() > selectionMomentum) &&
        (mcPfoList.find(pMCParticle) == mcPfoList.end()) &&
        !((particleId == PROTON || particleId == NEUTRON) && pMCParticle->GetEnergy() < selectionEnergyCutOffProtonsNeutrons )
        )
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pMCParticle->SetPfoTargetInTree(pMCParticle, true));
        mcPfoList.insert(pMCParticle);
    }
    else
    {
        // MC particle has not yet crossed boundary - set it as its own pfo target.
        // --> don't do this any more (only MCParticles which cross the boundary can be MCPFOs)
        // PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pMCParticle->SetPfoTarget(pMCParticle));

        for(MCParticleList::iterator iter = pMCParticle->m_daughterList.begin(), iterEnd = pMCParticle->m_daughterList.end();
            iter != iterEnd; ++iter)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ApplyPfoSelectionRules(*iter, mcPfoList));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::CreateUidToPfoTargetMap(UidToMCParticleMap &uidToPfoTargetMap, const UidRelationMap &uidRelationMap) const
{
    if (m_uidToMCParticleMap.empty())
        return STATUS_CODE_SUCCESS;

    static const bool shouldCollapseMCParticlesToPfoTarget(PandoraSettings::ShouldCollapseMCParticlesToPfoTarget());

    for (UidRelationMap::const_iterator relationIter = uidRelationMap.begin(), relationIterEnd = uidRelationMap.end();
        relationIter != relationIterEnd; ++relationIter)
    {
        UidToMCParticleMap::const_iterator mcParticleIter = m_uidToMCParticleMap.find(relationIter->second.m_uid);

        if ((m_uidToMCParticleMap.end() == mcParticleIter) || (!mcParticleIter->second->IsInitialized()))
            continue;

        MCParticle *pMCParticle = NULL;

        if (!shouldCollapseMCParticlesToPfoTarget)
        {
            pMCParticle = mcParticleIter->second;
        }
        else
        {
            pMCParticle = mcParticleIter->second->m_pPfoTarget;
        }

        if (pMCParticle != NULL)
        {
            if (!uidToPfoTargetMap.insert(UidToMCParticleMap::value_type(relationIter->first, pMCParticle)).second)
                return STATUS_CODE_ALREADY_PRESENT;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::GetMCParticleList(MCParticleList &mcParticleList) const
{
    for(UidToMCParticleMap::const_iterator iter = m_uidToMCParticleMap.begin(), iterEnd = m_uidToMCParticleMap.end(); iter != iterEnd; ++iter)
    {
        mcParticleList.insert(iter->second);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::DeleteNonPfoTargets()
{
    static const bool shouldCollapseMCParticlesToPfoTarget(PandoraSettings::ShouldCollapseMCParticlesToPfoTarget());

    if (!shouldCollapseMCParticlesToPfoTarget)
        return STATUS_CODE_SUCCESS;

    for (UidToMCParticleMap::iterator iter = m_uidToMCParticleMap.begin(); iter != m_uidToMCParticleMap.end();)
    {
        MCParticle *pMCParticle = iter->second;

        if (!pMCParticle->IsPfoTarget())
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveMCParticleRelationships(pMCParticle));
            delete pMCParticle;
            m_uidToMCParticleMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::RemoveMCParticleRelationships(MCParticle *const pMCParticle)
{
    MCParticleList &parentList(pMCParticle->m_parentList);
    for (MCParticleList::const_iterator iter = parentList.begin(), iterEnd = parentList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->RemoveDaughter(pMCParticle));
    }

    MCParticleList &daughterList(pMCParticle->m_daughterList);
    for (MCParticleList::const_iterator iter = daughterList.begin(), iterEnd = daughterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->RemoveParent(pMCParticle));
    }

    parentList.clear();
    daughterList.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::ResetForNextEvent()
{
    for (UidToMCParticleMap::iterator iter = m_uidToMCParticleMap.begin(), iterEnd = m_uidToMCParticleMap.end(); iter != iterEnd; ++iter)
        delete iter->second;

    m_uidToMCParticleMap.clear();
    m_caloHitToMCParticleMap.clear();
    m_trackToMCParticleMap.clear();

    if (!m_uidToMCParticleMap.empty() || !m_caloHitToMCParticleMap.empty() || !m_trackToMCParticleMap.empty())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
