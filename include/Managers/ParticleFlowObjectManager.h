/**
 *  @file   PandoraPFANew/include/Managers/ParticleFlowObjectManager.h
 * 
 *  @brief  Header file for the particle flow object manager class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_FLOW_OBJECT_MANAGER_H
#define PARTICLE_FLOW_OBJECT_MANAGER_H 1

#include "Api/PandoraApi.h"
#include "Api/PandoraContentApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class Cluster;
class Track;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ParticleFlowObjectManager class
 */
class ParticleFlowObjectManager
{
public:
    /**
     *  @brief  Destructor
     */
    ~ParticleFlowObjectManager();

private:
    /**
     *  @brief  Create a particle flow object
     * 
     *  @param  particleFlowObjectParameters the particle flow object parameters
     */
    StatusCode CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters);

    /**
     *  @brief  Get the particle flow objects
     * 
     *  @param  particleFlowObjectList container to receive the particle flow objects
     */
    StatusCode GetParticleFlowObjects(ParticleFlowObjectList &particleFlowObjectList) const;

    /**
     *  @brief  Get the current particle flow object list
     * 
     *  @param  pParticleFlowObjectList to receive the address of the current particle flow object list
     */
    StatusCode GetCurrentList(const ParticleFlowObjectList *&pParticleFlowObjectList) const;

    /**
     *  @brief  Add a cluster to a particle flow object
     *
     *  @param  pParticleFlowObject address of the particle flow object to modify
     *  @param  pCluster address of the cluster to add
     */
    StatusCode AddClusterToPfo(ParticleFlowObject *pParticleFlowObject, Cluster *pCluster) const;

    /**
     *  @brief  Add a track to a particle flow object
     *
     *  @param  pParticleFlowObject address of the particle flow object to modify
     *  @param  pTrack address of the track to add
     */
    StatusCode AddTrackToPfo(ParticleFlowObject *pParticleFlowObject, Track *pTrack) const;

    /**
     *  @brief  Remove a cluster from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pParticleFlowObject address of the particle flow object to modify
     *  @param  pCluster address of the cluster to remove
     */
    StatusCode RemoveClusterFromPfo(ParticleFlowObject *pParticleFlowObject, Cluster *pCluster);

    /**
     *  @brief  Remove a track from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pParticleFlowObject address of the particle flow object to modify
     *  @param  pTrack address of the track to remove
     */
    StatusCode RemoveTrackFromPfo(ParticleFlowObject *pParticleFlowObject, Track *pTrack);

    /**
     *  @brief  Delete a particle flow object from the current list
     * 
     *  @param  pParticleFlowObject address of the particle flow object to delete
     */
    StatusCode DeletePfo(ParticleFlowObject *pParticleFlowObject);

    /**
     *  @brief  Reset the particle flow object manager
     */
    StatusCode ResetForNextEvent();

    ParticleFlowObjectList      m_particleFlowObjectList;       ///< The particle flow object list

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;

    ADD_TEST_CLASS_FRIENDS;
};

} // namespace pandora

#endif // #ifndef PARTICLE_FLOW_OBJECT_MANAGER_H
