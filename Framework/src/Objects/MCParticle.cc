/**
 *  @file   PandoraPFANew/Framework/src/Objects/MCParticle.cc
 * 
 *  @brief  Implementation of the mc particle class.
 * 
 *  $Log: $
 */

#include "Objects/MCParticle.h"

namespace pandora
{

MCParticle::MCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters) :
    m_uid(mcParticleParameters.m_pParentAddress.Get()),
    m_energy(mcParticleParameters.m_energy.Get()),
    m_momentum(mcParticleParameters.m_momentum.Get()),
    m_vertex(mcParticleParameters.m_vertex.Get()),
    m_endpoint(mcParticleParameters.m_endpoint.Get()),
    m_innerRadius(mcParticleParameters.m_vertex.Get().GetMagnitude()),
    m_outerRadius(mcParticleParameters.m_endpoint.Get().GetMagnitude()),
    m_particleId(mcParticleParameters.m_particleId.Get()),
    m_pPfoTarget(NULL),
    m_isInitialized(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCParticle::MCParticle(const Uid uid) :
    m_uid(uid),
    m_energy(0.f),
    m_momentum(0.f, 0.f, 0.f),
    m_vertex(0.f, 0.f, 0.f),
    m_endpoint(0.f, 0.f, 0.f),
    m_innerRadius(0.f),
    m_outerRadius(0.f),
    m_particleId(0),
    m_pPfoTarget(NULL),
    m_isInitialized(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCParticle::~MCParticle()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MCParticle::SetProperties(const PandoraApi::MCParticleParameters &mcParticleParameters)
{
    m_energy = mcParticleParameters.m_energy.Get();
    m_momentum = mcParticleParameters.m_momentum.Get();
    m_vertex = mcParticleParameters.m_vertex.Get();
    m_endpoint = mcParticleParameters.m_endpoint.Get();
    m_innerRadius = mcParticleParameters.m_vertex.Get().GetMagnitude();
    m_outerRadius = mcParticleParameters.m_endpoint.Get().GetMagnitude();
    m_particleId = mcParticleParameters.m_particleId.Get();

    m_isInitialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::SetPfoTargetInTree(MCParticle *pMCParticle, bool onlyDaughters)
{
    if (this->IsPfoTargetSet())
        return STATUS_CODE_SUCCESS;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetPfoTarget(pMCParticle));

    for (MCParticleList::iterator iter = m_daughterList.begin(), iterEnd = m_daughterList.end(); iter != iterEnd; ++iter)
    {
       PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetPfoTargetInTree(pMCParticle));
    }

    if(!onlyDaughters)
    {
        for (MCParticleList::iterator iter = m_parentList.begin(), iterEnd = m_parentList.end(); iter != iterEnd; ++iter)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetPfoTargetInTree(pMCParticle));
        }
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
