/**
 *  @file   PandoraPFANew/include/Api/PandoraApi.h
 *
 *  @brief  Header file for the pandora api class.
 *
 *  $Log: $
 */
#ifndef PANDORA_API_H
#define PANDORA_API_H 1

#include "Pandora/Pandora.h"
#include "Pandora/PandoraInputTypes.h"

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
        pandora::InputCartesianVector   m_positionVector;           ///< Position vector of center of calorimeter cell, units mm
        pandora::InputCartesianVector   m_normalVector;             ///< Unit normal to sampling layer, pointing outwards from the origin
        pandora::InputFloat             m_cellSizeU;                ///< Dimension of cell (u = x in ENDCAP, u = r in BARREL), units mm
        pandora::InputFloat             m_cellSizeV;                ///< Dimension of cell (v = y in ENDCAP, v = r*phi in BARREL), units mm
        pandora::InputFloat             m_cellSizeZ;                ///< Dimension of cell, z-coordinate, units mm
        pandora::InputFloat             m_nRadiationLengths;        ///< Absorber material in front of cell, units radiation lengths
        pandora::InputFloat             m_nInteractionLengths;      ///< Absorber material in front of cell, units interaction lengths
        pandora::InputFloat             m_energy;                   ///< Corrected energy of the calorimeter cell, units GeV
        pandora::InputFloat             m_time;                     ///< Time of (earliest) energy deposition in this cell, units ns
        pandora::InputBool              m_isDigital;                ///< Whether cell should be treated as digital
        pandora::InputHitType           m_hitType;                  ///< The type of calorimeter hit
        pandora::InputDetectorRegion    m_detectorRegion;           ///< Region of the detector in which the calo hit is located
        pandora::InputUInt              m_layer;                    ///< The subdetector readout layer number
        pandora::InputAddress           m_pParentAddress;           ///< The address of the parent calo hit in the user framework
    };

    /**
     *  @brief  TrackParameters class
     */
    class TrackParameters
    {
    public:
        pandora::InputFloat             m_d0;                       ///< The 2D impact parameter wrt (0,0)
        pandora::InputFloat             m_z0;                       ///< The z coordinate at the 2D distance of closest approach
        pandora::InputCartesianVector   m_momentumAtDca;            ///< The track momentum at the 2D distance of closest approach
        pandora::InputTrackState        m_trackStateAtStart;        ///< The track state at the start of the track
        pandora::InputTrackState        m_trackStateAtEnd;          ///< The track state at the end of the track
        pandora::InputTrackState        m_trackStateAtECal;         ///< The (sometimes projected) track state at the ecal
        pandora::InputBool              m_reachesECal;              ///< Whether the track has reaches the ecal
        pandora::InputTrackStateList    m_calorimeterProjections;   ///< A list of alternative track state projections to the calorimeters
        pandora::InputAddress           m_pParentAddress;           ///< The address of the parent track in the user framework
    };

    /**
     *  @brief  Geometry parameters
     */
    class GeometryParameters
    {
    public:
        pandora::InputFloat             m_tpcInnerRadius;           ///< The inner tpc radius
    };

    /**
     *  @brief  Monte Carlo particle parameters
     */
    class MCParticleParameters
    {
    public:
        pandora::InputFloat             m_energy;                   ///< The MC particle's energy
        pandora::InputFloat             m_momentum;                 ///< The MC particle's momentum
        pandora::InputFloat             m_innerRadius;              ///< The MC particle's path's inner radius
        pandora::InputFloat             m_outerRadius;              ///< The MC particle's path's outer radius
        pandora::InputInt               m_particleId;               ///< The MC particle's ID (PDG code)
        pandora::InputAddress           m_pParentAddress;           ///< The address of the parent MC particle in the user framework
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
     *  @brief  Set parent-daughter track relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pParentAddress address of parent track in the user framework
     *  @param  pDaughterAddress address of daughter track in the user framework
     */
    static StatusCode SetTrackParentDaughterRelationship(const pandora::Pandora &pandora, const void *pParentAddress,
        const void *pDaughterAddress);

    /**
     *  @brief  Set sibling track relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pFirstSiblingAddress address of first sibling track in the user framework
     *  @param  pSecondSiblingAddress address of second sibling track in the user framework
     */
    static StatusCode SetTrackSiblingRelationship(const pandora::Pandora &pandora, const void *pFirstSiblingAddress,
        const void *pSecondSiblingAddress);

    /**
     *  @brief  Set calo hit to mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pCaloHitParentAddress address of calo hit in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    static StatusCode SetCaloHitToMCParticleRelationship(const pandora::Pandora &pandora, const void *pCaloHitParentAddress,
        const void *pMCParticleParentAddress, const float mcParticleWeight = 1);

    /**
     *  @brief  Set track to mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pTrackParentAddress address of track in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    static StatusCode SetTrackToMCParticleRelationship(const pandora::Pandora &pandora, const void *pTrackParentAddress,
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
