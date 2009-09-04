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
        PandoraApi::ParticleFlowObject *pParticleFlowObject = NULL;
        pParticleFlowObject = new PandoraApi::ParticleFlowObject;

        if (NULL == pParticleFlowObject)
            return STATUS_CODE_FAILURE;

        pParticleFlowObject->m_energy = particleFlowObjectParameters.m_energy.Get();

        // Store pointers to constituent tracks
        for (TrackList::const_iterator iter = particleFlowObjectParameters.m_trackList.begin(),
            iterEnd = particleFlowObjectParameters.m_trackList.end(); iter != iterEnd; ++iter)
        {
            pParticleFlowObject->m_trackAddressVector.push_back((*iter)->GetParentTrackAddress());
        }

        // Store pointers to constituent hits (organized into clusters)
        for (ClusterList::const_iterator clusterIter = particleFlowObjectParameters.m_clusterList.begin(),
            clusterIterEnd = particleFlowObjectParameters.m_clusterList.end(); clusterIter != clusterIterEnd; ++clusterIter)
        {
            CaloHitAddressVector caloHitAddressVector;

            for (OrderedCaloHitList::const_iterator orderedListIter = (*clusterIter)->GetOrderedCaloHitList()->begin(),
                orderedListIterEnd = (*clusterIter)->GetOrderedCaloHitList()->end(); orderedListIter != orderedListIterEnd; ++orderedListIter)
            {
                for (CaloHitList::const_iterator caloHitIter = orderedListIter->second->begin(),
                    caloHitIterEnd = orderedListIter->second->end(); caloHitIter != caloHitIterEnd; ++caloHitIter)
                {
                    caloHitAddressVector.push_back((*caloHitIter)->GetParentCaloHitAddress());
                }
            }

            pParticleFlowObject->m_clusterAddressVector.push_back(caloHitAddressVector);
        }

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

StatusCode ParticleFlowObjectManager::GetParticleFlowObjects(PandoraApi::ParticleFlowObjectList &particleFlowObjectList) const
{
    particleFlowObjectList = m_particleFlowObjectList;

    if (particleFlowObjectList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::ResetForNextEvent()
{
    for (PandoraApi::ParticleFlowObjectList::iterator iter = m_particleFlowObjectList.begin(),
        iterEnd = m_particleFlowObjectList.end(); iter != iterEnd; ++iter)
    {
        delete *iter;
    }

    m_particleFlowObjectList.clear();

    if (!m_particleFlowObjectList.empty())
        return STATUS_CODE_FAILURE;    

    return STATUS_CODE_SUCCESS;
}

} //  namespace pandora
