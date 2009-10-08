/**
 *  @file   PandoraPFANew/include/Objects/ParticleFlowObject.h
 * 
 *  @brief  Header file for the particle flow object class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_FLOW_OBJECT_H
#define PARTICLE_FLOW_OBJECT_H 1

#include "Api/PandoraContentApi.h"

namespace pandora
{

/**
 *  @brief  ParticleFlowObject class
 */
class ParticleFlowObject
{
public:
    /**
     *  @brief  Get the particle flow object id (PDG code)
     * 
     *  @return The particle flow object id
     */
    int GetParticleId() const;

    /**
     *  @brief  Get particle flow object charge sign
     * 
     *  @return The particle flow object charge sign
     */
    int GetChargeSign() const;

    /**
     *  @brief  Get particle flow object mass
     * 
     *  @return The particle flow object mass
     */
    float GetMass() const;

    /**
     *  @brief  Get the particle flow object energy
     * 
     *  @return The particle flow object energy
     */
    float GetEnergy() const;

    /**
     *  @brief  Get particle flow object momentum vector
     * 
     *  @return The particle flow object momentum vector
     */
    const CartesianVector &GetMomentum() const;

    /**
     *  @brief  Get track address list
     * 
     *  @return The track address list
     */
    const TrackAddressList &GetTrackAddressList() const;

    /**
     *  @brief  Get the cluster address list
     * 
     *  @return The cluster address list
     */
    const ClusterAddressList &GetClusterAddressList() const;

private:
    /**
     *  @brief  Constructor
     */
    ParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters);

    /**
     *  @brief  Extract tracks from parameters and store in the particle flow object
     * 
     *  @param  particleFlowObjectParameters the particle flow object parameters
     */
    void ExtractAndStoreTracks(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters);

    /**
     *  @brief  Extract calo hits from parameters and store (grouped into clusters) in the particle flow object
     * 
     *  @param  particleFlowObjectParameters the particle flow object parameters
     */
    void ExtractAndStoreCaloHits(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters);

    int                     m_particleId;               ///< The particle flow object id (PDG code)
    int                     m_chargeSign;               ///< The particle flow object charge sign
    float                   m_mass;                     ///< The particle flow object mass
    float                   m_energy;                   ///< The particle flow object energy
    CartesianVector         m_momentum;                 ///< The particle flow object momentum
    TrackAddressList        m_trackAddressList;         ///< The track address list
    ClusterAddressList      m_clusterAddressList;       ///< The cluster address list

    friend class ParticleFlowObjectManager;

    ADD_TEST_CLASS_FRIENDS;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline int ParticleFlowObject::GetParticleId() const
{
    return m_particleId;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int ParticleFlowObject::GetChargeSign() const
{
    return m_chargeSign;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ParticleFlowObject::GetMass() const
{
    return m_mass;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ParticleFlowObject::GetEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &ParticleFlowObject::GetMomentum() const
{
    return m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackAddressList &ParticleFlowObject::GetTrackAddressList() const
{
    return m_trackAddressList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ClusterAddressList &ParticleFlowObject::GetClusterAddressList() const
{
    return m_clusterAddressList;
}

} // namespace pandora

#endif // #ifndef PARTICLE_FLOW_OBJECT_H
