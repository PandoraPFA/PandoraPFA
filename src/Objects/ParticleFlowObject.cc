/**
 *  @file   PandoraPFANew/src/Objects/ParticleFlowObject.cc
 * 
 *  @brief  Implementation of the particle flow object class.
 * 
 *  $Log: $
 */

#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"

namespace pandora
{

ParticleFlowObject::ParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters) :
    m_particleId(particleFlowObjectParameters.m_particleId.Get()),
    m_charge(particleFlowObjectParameters.m_charge.Get()),
    m_mass(particleFlowObjectParameters.m_mass.Get()),
    m_energy(particleFlowObjectParameters.m_energy.Get()),
    m_momentum(particleFlowObjectParameters.m_momentum.Get()),
    m_trackList(particleFlowObjectParameters.m_trackList),
    m_clusterList(particleFlowObjectParameters.m_clusterList)
{
    if (particleFlowObjectParameters.m_clusterList.empty() && particleFlowObjectParameters.m_trackList.empty())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    this->ExtractAndStoreTracks(particleFlowObjectParameters);
    this->ExtractAndStoreCaloHits(particleFlowObjectParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ParticleFlowObject::ExtractAndStoreTracks(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters)
{
    for (TrackList::const_iterator iter = particleFlowObjectParameters.m_trackList.begin(),
        iterEnd = particleFlowObjectParameters.m_trackList.end(); iter != iterEnd; ++iter)
    {
        m_trackAddressList.push_back((*iter)->GetParentTrackAddress());
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ParticleFlowObject::ExtractAndStoreCaloHits(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters)
{
    for (ClusterList::const_iterator clusterIter = particleFlowObjectParameters.m_clusterList.begin(),
        clusterIterEnd = particleFlowObjectParameters.m_clusterList.end(); clusterIter != clusterIterEnd; ++clusterIter)
    {
        CaloHitAddressList caloHitAddressList;

        for (OrderedCaloHitList::const_iterator orderedListIter = (*clusterIter)->GetOrderedCaloHitList().begin(),
            orderedListIterEnd = (*clusterIter)->GetOrderedCaloHitList().end(); orderedListIter != orderedListIterEnd; ++orderedListIter)
        {
            for (CaloHitList::const_iterator caloHitIter = orderedListIter->second->begin(),
                caloHitIterEnd = orderedListIter->second->end(); caloHitIter != caloHitIterEnd; ++caloHitIter)
            {
                caloHitAddressList.push_back((*caloHitIter)->GetParentCaloHitAddress());
            }
        }

        m_clusterAddressList.push_back(caloHitAddressList);
    }
}

} // namespace pandora
