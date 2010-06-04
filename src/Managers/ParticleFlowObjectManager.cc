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

        if (!m_particleFlowObjectList.insert(pParticleFlowObject).second)
            return STATUS_CODE_FAILURE;

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

StatusCode ParticleFlowObjectManager::GetCurrentList(const ParticleFlowObjectList *&pParticleFlowObjectList) const
{
    if (m_particleFlowObjectList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    pParticleFlowObjectList = &m_particleFlowObjectList;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::AddClusterToPfo(ParticleFlowObject *pParticleFlowObject, Cluster *pCluster) const
{
    ParticleFlowObjectList::const_iterator iter = m_particleFlowObjectList.find(pParticleFlowObject);

    if (m_particleFlowObjectList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    ClusterList &clusterList = (*iter)->m_clusterList;

    if (!clusterList.insert(pCluster).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::AddTrackToPfo(ParticleFlowObject *pParticleFlowObject, Track *pTrack) const
{
    ParticleFlowObjectList::const_iterator iter = m_particleFlowObjectList.find(pParticleFlowObject);

    if (m_particleFlowObjectList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    TrackList &trackList = (*iter)->m_trackList;

    if (!trackList.insert(pTrack).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::RemoveClusterFromPfo(ParticleFlowObject *pParticleFlowObject, Cluster *pCluster)
{
    ParticleFlowObjectList::iterator iter = m_particleFlowObjectList.find(pParticleFlowObject);

    if (m_particleFlowObjectList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    ClusterList &clusterList = (*iter)->m_clusterList;
    ClusterList::iterator clusterIter = clusterList.find(pCluster);

    if (clusterList.end() == clusterIter)
        return STATUS_CODE_NOT_FOUND;

    clusterList.erase(clusterIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::RemoveTrackFromPfo(ParticleFlowObject *pParticleFlowObject, Track *pTrack)
{
    ParticleFlowObjectList::iterator iter = m_particleFlowObjectList.find(pParticleFlowObject);

    if (m_particleFlowObjectList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    TrackList &trackList = (*iter)->m_trackList;
    TrackList::iterator trackIter = trackList.find(pTrack);

    if (trackList.end() == trackIter)
        return STATUS_CODE_NOT_FOUND;

    trackList.erase(trackIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleFlowObjectManager::DeletePfo(ParticleFlowObject *pParticleFlowObject)
{
    ParticleFlowObjectList::iterator iter = m_particleFlowObjectList.find(pParticleFlowObject);

    if (m_particleFlowObjectList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    delete pParticleFlowObject;
    m_particleFlowObjectList.erase(iter);

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
