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
    StatusCode GetParticleFlowObjects(PandoraApi::ParticleFlowObjectList &particleFlowObjectList) const;
    
    /**
     *  @brief  Reset the particle flow object manager
     */
    StatusCode ResetForNextEvent();

    PandoraApi::ParticleFlowObjectList  m_particleFlowObjectList;   ///< The particle flow object list

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
};
    
} // namespace pandora

#endif // #ifndef PARTICLE_FLOW_OBJECT_MANAGER_H
