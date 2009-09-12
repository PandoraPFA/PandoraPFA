/**
 *  @file   PandoraPFANew/include/Objects/MCParticle.h
 * 
 *  @brief  Header file for the mc particle class.
 * 
 *  $Log: $
 */
#ifndef MC_PARTICLE_H
#define MC_PARTICLE_H 1

#include "Api/PandoraApi.h"

#include "Internal.h"

namespace pandora
{

/**
 *  @brief  MCParticle class
 */
class MCParticle 
{
public:
    /**
     *  @brief  Whether the mc particle properties have been initialized
     * 
     *  @return boolean
     */
    bool IsInitialized() const;

    /**
     *  @brief  Whether the mc particle is a root particle
     * 
     *  @return boolean
     */
    bool IsRootParticle() const;

    /**
     *  @brief  Whether the mc particle is a pfo target
     * 
     *  @return boolean
     */
    bool IsPfoTarget() const;

    /**
     *  @brief  Get energy of mc particle
     *
     *  @return the mc particle energy
     */
    float GetEnergy() const;

    /**
     *  @brief  Get momentum of mc particle
     *
     *  @return the mc particle momentum
     */
    float GetMomentum() const;

    /**
     *  @brief Get inner radius of mc particle
     * 
     *  @return the mc particle inner radius
     */	
    float GetInnerRadius() const;

    /**
     *  @brief  Get outer radius of mc particle
     *
     *  @return the mc particle outer radius
     */
    float GetOuterRadius() const;

    /**
     *  @brief  Whether the pfo target been set
     *
     *  @return boolean
     */
    bool IsPfoTargetSet() const;

    /**
     *  @brief  Get pfo target particle
     * 
     *  @param  pMCParticle to receive the address of the pfo target
     */
    StatusCode GetPfoTarget(MCParticle*& pMCParticle) const;

    /**
     *  @brief  Get the mc particle unique identifier
     * 
     *  @return the mc particle unique identifier
     */
    Uid GetUid() const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  mcParticleParameters the mc particle parameters
     */
    MCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters);

    /**
     *  @brief  Constructor
     * 
     *  @param  uid the unique identifier of the mc particle
     */
    MCParticle(const Uid uid);

    /**
     *  @brief  Destructor
     */
    ~MCParticle();

    /**
     *  @brief  Set mc particle properties
     * 
     *  @param  mcParticleParameters the mc particle parametersle id
     */
    void SetProperties(const PandoraApi::MCParticleParameters &mcParticleParameters);

    /**
     *  @brief  Add daughter particle
     * 
     *  @param  mcParticle the daughter particle
     */
    StatusCode AddDaughter(MCParticle* mcParticle);

    /**
     *  @brief  Add parent particle
     * 
     *  @param  mcParticle the parent particle
     */
    StatusCode AddParent(MCParticle *mcParticle);

    /**
     *  @brief  Set pfo target particle
     * 
     *  @param  mcParticle the pfo target particle
     */
    StatusCode SetPfoTarget(MCParticle *mcParticle);

    /**
     *  @brief  Set pfo target for a mc tree
     * 
     *  @param  mcParticle particle in the mc tree
     *  @param  onlyDaughters if "true" go through daughters only, if false
     *          go through parents as well
     */
    StatusCode SetPfoTargetInTree(MCParticle* mcParticle, bool onlyDaughters = false);

    Uid                 m_uid;              ///< Unique ID of the particle

    float               m_energy;           ///< Energy of the particle
    float               m_momentum;         ///< Momentum of the particle
    float               m_innerRadius;      ///< Inner radius of the particle's path
    float               m_outerRadius;      ///< Outer radius of the particle's path
    int                 m_particleId;       ///< Particle id (particle type)

    MCParticle          *m_pPfoTarget;      ///< The pfo target
    MCParticleList      m_daughterList;     ///< The list of mc daughter particles
    MCParticleList      m_parentList;       ///< The list of mc parent particles

    bool                m_isInitialized;    ///< Whether particle information has been initialized

    friend class MCManager;
    friend class MCPfoSelection;

    friend class TestMCManager;
    friend class TestCaloHitManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool MCParticle::IsInitialized() const
{
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool MCParticle::IsRootParticle() const
{
    return m_parentList.empty();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool MCParticle::IsPfoTarget() const
{
    return (this == m_pPfoTarget);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool MCParticle::IsPfoTargetSet() const
{
    return (NULL != m_pPfoTarget);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::GetPfoTarget(MCParticle*& pMCParticle) const
{
    if (NULL == m_pPfoTarget)
        return STATUS_CODE_NOT_INITIALIZED;

    pMCParticle = m_pPfoTarget;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Uid MCParticle::GetUid() const
{
   return m_uid;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::AddDaughter(MCParticle* mcParticle)
{
    if (!m_daughterList.insert(mcParticle).second)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::AddParent(MCParticle *mcParticle)
{
    if (!m_parentList.insert(mcParticle).second)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCParticle::SetPfoTarget(MCParticle *mcParticle)
{
    if (NULL == mcParticle)
        return STATUS_CODE_FAILURE;
    
    m_pPfoTarget = mcParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float MCParticle::GetEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float MCParticle::GetMomentum() const
{
    return m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float MCParticle::GetInnerRadius() const
{
    return m_innerRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float MCParticle::GetOuterRadius() const
{
    return m_outerRadius;
}

} // namespace pandora

#endif // #ifndef MC_PARTICLE_H
