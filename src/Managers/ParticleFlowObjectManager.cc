/**
 *  @file   PandoraPFANew/src/Managers/ParticleFlowObjectManager.cc
 * 
 *  @brief  Implementation of the particle flow object manager class.
 * 
 *  $Log: $
 */

#include "Managers/ParticleFlowObjectManager.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"

namespace pandora
{

ParticleFlowObjectManager::~ParticleFlowObjectManager()
{
    (void) this->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters)
{
    try
    {
        ParticleFlowObject *pParticleFlowObject = NULL;
        pParticleFlowObject = new ParticleFlowObject(particleFlowObjectParameters);

        if (NULL == pParticleFlowObject)
            return STATUS_CODE_FAILURE;

        m_particleFlowObjectList.push_back(pParticleFlowObject);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create particle flow object: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::GetParticleFlowObjects(ParticleFlowObjectList &particleFlowObjectList) const
{
    particleFlowObjectList = m_particleFlowObjectList;

    if (particleFlowObjectList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::ResetForNextEvent()
{
    for (ParticleFlowObjectList::iterator iter = m_particleFlowObjectList.begin(), iterEnd = m_particleFlowObjectList.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_particleFlowObjectList.clear();

    if (!m_particleFlowObjectList.empty())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

} //  namespace pandora
