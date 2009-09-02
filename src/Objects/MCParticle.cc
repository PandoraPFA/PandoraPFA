/**
 *  @file   PandoraPFANew/src/Objects/MCParticle.cc
 * 
 *  @brief  Implementation of the mc particle class.
 * 
 *  $Log: $
 */

#include "Objects/MCParticle.h"

namespace pandora
{

MCParticle::MCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters) :
    m_uid(mcParticleParameters.m_pParentAddress),
    m_energy(mcParticleParameters.m_energy),
    m_momentum(mcParticleParameters.m_momentum),
    m_innerRadius(mcParticleParameters.m_innerRadius),
    m_outerRadius(mcParticleParameters.m_outerRadius),
    m_particleId(mcParticleParameters.m_particleId),
    m_pPfoTarget(NULL),
    m_isInitialized(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCParticle::MCParticle(const Uid uid) :
    m_uid(uid),
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
    m_energy = mcParticleParameters.m_energy;
    m_momentum = mcParticleParameters.m_momentum;
    m_innerRadius = mcParticleParameters.m_innerRadius;
    m_outerRadius = mcParticleParameters.m_outerRadius;
    m_particleId = mcParticleParameters.m_particleId;

    m_isInitialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::SetPfoTargetInTree(MCParticle* mcParticle)
{
    if (this->IsPfoTargetSet())
        return STATUS_CODE_SUCCESS;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetPfoTarget(mcParticle));

    for (MCParticleList::iterator iter = m_daughterList.begin(), iterEnd = m_daughterList.end(); iter != iterEnd; ++iter)
    {
       PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetPfoTargetInTree(mcParticle));
    }

    for (MCParticleList::iterator iter = m_parentList.begin(), iterEnd = m_parentList.end(); iter != iterEnd; ++iter)
    {
       PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetPfoTargetInTree(mcParticle));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
