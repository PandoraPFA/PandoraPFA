/**
 *  @file   PandoraPFANew/include/Api/PandoraApi.h
 *
 *  @brief  Header file for the pandora api class.
 *
 *  $Log: $
 */
#ifndef PANDORA_API_H
#define PANDORA_API_H 1

#include "Pandora.h"

namespace pandora { class Algorithm; class AlgorithmFactory; }

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraApi class
 */
class PandoraApi
{
public:
    /**
     *  @brief  Object creation helper class
     * 
     *  @param  PARAMETERS the type of object parameters
     */
    template <typename PARAMETERS>
    class ObjectCreationHelper
    {
    public:
        typedef PARAMETERS Parameters;
        
        /**
         *  @brief  Create a new object
         * 
         *  @param  pandora the pandora instance to create the new object
         *  @param  parameters the object parameters
         */
         static StatusCode Create(const pandora::Pandora &pandora, const Parameters &parameters);
    };

    /**
     *  @brief  CaloHitParameters class
     */
    class CaloHitParameters
    {
    public:
        pandora::Float      m_energy;               ///< The calo hit energy
        pandora::UInt       m_layer;                ///< The layer in which the calo hit is located
        pandora::Address    m_pParentAddress;       ///< The address of the parent calo hit in the user framework
    };

    /**
     *  @brief  TrackParameters class
     */
    class TrackParameters
    {
    public:
        pandora::Float      m_momentum;             ///< The track momentum
        pandora::Address    m_pParentAddress;       ///< The address of the parent track in the user framework
    };

    /**
     *  @brief  Geometry parameters
     */
    class GeometryParameters
    {
    public:
        pandora::Float      m_tpcInnerRadius;       ///< The inner tpc radius
    };

    /**
     *  @brief  Monte Carlo particle parameters
     */
    class MCParticleParameters
    {
    public:
        pandora::Float      m_energy;               ///< The MC particle's energy
        pandora::Float      m_momentum;             ///< The MC particle's momentum
        pandora::Float      m_innerRadius;          ///< The MC particle's path's inner radius
        pandora::Float      m_outerRadius;          ///< The MC particle's path's outer radius
        pandora::Int        m_particleId;           ///< The MC particle's ID (PDG code)
        pandora::Address    m_pParentAddress;       ///< The address of the parent MC particle in the user framework
    };

    // Objects available for construction by pandora
    typedef ObjectCreationHelper<CaloHitParameters> CaloHit;
    typedef ObjectCreationHelper<TrackParameters> Track;
    typedef ObjectCreationHelper<GeometryParameters> Geometry;
    typedef ObjectCreationHelper<MCParticleParameters> MCParticle;

    /**
     *  @brief  ParticleFlowObject class
     */
    class ParticleFlowObject
    {
    public:
        float                           m_energy;                   ///< The particle flow object energy
        pandora::TrackAddressList       m_trackAddressList;         ///< The track address list
        pandora::ClusterAddressList     m_clusterAddressList;       ///< The cluster address list
    };

    typedef std::vector<ParticleFlowObject *> ParticleFlowObjectList;

    /**
     *  @brief  Process an event
     * 
     *  @param  pandora the pandora instance to process event
     */
    static StatusCode ProcessEvent(const pandora::Pandora &pandora);

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  pandora the pandora instance to run the algorithms initialize
     *  @param  xmlFileName the name of the xml file containing the settings
     */
    static StatusCode ReadSettings(const pandora::Pandora &pandora, const std::string &xmlFileName);

    /**
     *  @brief  Register an algorithm factory with pandora
     * 
     *  @param  pandora the pandora instance to register the algorithm factory with
     *  @param  algorithmType the type of algorithm that the factory will create
     *  @param  pAlgorithmFactory the address of an algorithm factory instance
     */
    static StatusCode RegisterAlgorithmFactory(const pandora::Pandora &pandora, const std::string &algorithmType,
        pandora::AlgorithmFactory *const pAlgorithmFactory);

    /**
     *  @brief  Set parent-daughter mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pParentAddress address of parent mc particle in the user framework
     *  @param  pDaughterAddress address of daughter mc particle in the user framework
     */
    static StatusCode SetMCParentDaughterRelationship(const pandora::Pandora &pandora, const void *pParentAddress,
        const void *pDaughterAddress);

    /**
     *  @brief  Set calo hit to mc particle relationshipip
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pCaloHitParentAddress address of calo hit in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    static StatusCode SetCaloHitToMCParticleRelationship(const pandora::Pandora &pandora, const void *pCaloHitParentAddress,
        const void *pMCParticleParentAddress, const float mcParticleWeight = 1);

    /**
     *  @brief  Get the particle flow objects
     * 
     *  @param  pandora the pandora instance to get the objects from
     *  @param  pfoVector container to receive the particle flow objects
     */
    static StatusCode GetParticleFlowObjects(const pandora::Pandora &pandora, ParticleFlowObjectList &particleFlowObjectList);

    /**
     *  @brief  Reset pandora to process another event
     * 
     *  @param  pandora the pandora instance to reset
     */
    static StatusCode Reset(const pandora::Pandora &pandora);
};

#endif // #ifndef PANDORA_API_H
