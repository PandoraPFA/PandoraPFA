/**
 *  @file   PandoraPFANew/include/Api/PandoraApi.h
 *
 *  @brief  Header file for the pandora api class.
 *
 *  $Log: $
 */
#ifndef PANDORA_API_H
#define PANDORA_API_H 1

#include "Objects/ParticleFlowObject.h"

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
        pandora::InputFloat             m_cellSizeU;                ///< Dimension of cell (up in ENDCAP, along beam in BARREL), units mm
        pandora::InputFloat             m_cellSizeV;                ///< Dimension of cell (perpendicular to u and thickness), units mm
        pandora::InputFloat             m_cellThickness;            ///< Thickness of cell, units mm
        pandora::InputFloat             m_nRadiationLengths;        ///< Absorber material in front of cell, units radiation lengths
        pandora::InputFloat             m_nInteractionLengths;      ///< Absorber material in front of cell, units interaction lengths
        pandora::InputFloat             m_time;                     ///< Time of (earliest) energy deposition in this cell, units ns
        pandora::InputFloat             m_inputEnergy;              ///< Corrected energy of calorimeter cell in user framework, units GeV
        pandora::InputFloat             m_mipEquivalentEnergy;      ///< The calibrated mip equivalent energy, units mip
        pandora::InputFloat             m_electromagneticEnergy;    ///< The calibrated electromagnetic energy measure, units GeV
        pandora::InputFloat             m_hadronicEnergy;           ///< The calibrated hadronic energy measure, units GeV
        pandora::InputBool              m_isDigital;                ///< Whether cell should be treated as digital
        pandora::InputHitType           m_hitType;                  ///< The type of calorimeter hit
        pandora::InputDetectorRegion    m_detectorRegion;           ///< Region of the detector in which the calo hit is located
        pandora::InputUInt              m_layer;                    ///< The subdetector readout layer number
        pandora::InputBool              m_isInOuterSamplingLayer;   ///< Whether cell is in one of the outermost detector sampling layers
        pandora::InputAddress           m_pParentAddress;           ///< Address of the parent calo hit in the user framework
    };

    /**
     *  @brief  TrackParameters class
     */
    class TrackParameters
    {
    public:
        pandora::InputFloat             m_d0;                       ///< The 2D impact parameter wrt (0,0), units mm
        pandora::InputFloat             m_z0;                       ///< The z coordinate at the 2D distance of closest approach, units mm
        pandora::InputInt               m_chargeSign;               ///< The charge sign of the tracked particle
        pandora::InputFloat             m_mass;                     ///< The mass of the tracked particle, units GeV
        pandora::InputInt               m_particleId;               ///< The PDG code of the tracked particle
        pandora::InputCartesianVector   m_momentumAtDca;            ///< Track momentum at the 2D distance of closest approach, units GeV
        pandora::InputTrackState        m_trackStateAtStart;        ///< Track state at the start of the track, units mm and GeV
        pandora::InputTrackState        m_trackStateAtEnd;          ///< Track state at the end of the track, units mm and GeV
        pandora::InputTrackState        m_trackStateAtECal;         ///< The (sometimes projected) track state at the ecal, units mm and GeV
        pandora::InputBool              m_reachesECal;              ///< Whether the track reaches the ecal
        pandora::InputBool              m_canFormPfo;               ///< Whether track should form a pfo, if it has an associated cluster
        pandora::InputBool              m_canFormClusterlessPfo;    ///< Whether track should form a pfo, even if it has no associated cluster
        pandora::InputTrackStateList    m_calorimeterProjections;   ///< A list of alternative track state projections to the calorimeters
        pandora::InputAddress           m_pParentAddress;           ///< Address of the parent track in the user framework
    };

    /**
     *  @brief  MCParticleParameters class
     */
    class MCParticleParameters
    {
    public:
        pandora::InputFloat             m_energy;                   ///< The MC particle's energy
        pandora::InputCartesianVector   m_momentum;                 ///< The MC particle's momentum
        pandora::InputFloat             m_innerRadius;              ///< The MC particle's path's inner radius
        pandora::InputFloat             m_outerRadius;              ///< The MC particle's path's outer radius
        pandora::InputInt               m_particleId;               ///< The MC particle's ID (PDG code)
        pandora::InputAddress           m_pParentAddress;           ///< Address of the parent MC particle in the user framework
    };

    /**
     *  @brief  GeometryParameters class
     */
    class GeometryParameters
    {
    public:
        /**
         *  @brief  LayerParameters class
         */
        class LayerParameters
        {
        public:
            pandora::InputFloat         m_closestDistanceToIp;      ///< Closest distance of the layer from the interaction point, units mm
            pandora::InputFloat         m_nRadiationLengths;        ///< Absorber material in front of layer, units radiation lengths
            pandora::InputFloat         m_nInteractionLengths;      ///< Absorber material in front of layer, units interaction lengths
        };

        typedef std::vector<LayerParameters> LayerParametersList;

        /**
         *  @brief  SubDetectorParameters class
         */
        class SubDetectorParameters
        {
        public:
            pandora::InputFloat         m_innerRCoordinate;         ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
            pandora::InputFloat         m_innerZCoordinate;         ///< Inner cylindrical polar z coordinate, origin interaction point, units mm
            pandora::InputFloat         m_innerPhiCoordinate;       ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
            pandora::InputUInt          m_innerSymmetryOrder;       ///< Order of symmetry of the innermost edge of subdetector
            pandora::InputFloat         m_outerRCoordinate;         ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
            pandora::InputFloat         m_outerZCoordinate;         ///< Outer cylindrical polar z coordinate, origin interaction point, units mm
            pandora::InputFloat         m_outerPhiCoordinate;       ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
            pandora::InputUInt          m_outerSymmetryOrder;       ///< Order of symmetry of the outermost edge of subdetector
            pandora::InputUInt          m_nLayers;                  ///< The number of layers in the detector section
            LayerParametersList         m_layerParametersList;      ///< The list of layer parameters for the detector section
        };

        typedef std::map<std::string, SubDetectorParameters> SubDetectorParametersMap;

        SubDetectorParameters           m_eCalBarrelParameters;     ///< The ecal barrel parameters
        SubDetectorParameters           m_hCalBarrelParameters;     ///< The hcal barrel parameters
        SubDetectorParameters           m_eCalEndCapParameters;     ///< the ecal end cap parameters
        SubDetectorParameters           m_hCalEndCapParameters;     ///< The hcal end cap parameter

        pandora::InputFloat             m_mainTrackerInnerRadius;   ///< The main tracker inner radius, units mm
        pandora::InputFloat             m_mainTrackerOuterRadius;   ///< The main tracker outer radius, units mm
        pandora::InputFloat             m_mainTrackerZExtent;       ///< The main tracker z extent, units mm

        pandora::InputFloat             m_coilInnerRadius;          ///< The coil inner radius, units mm
        pandora::InputFloat             m_coilOuterRadius;          ///< The coil outer radius, units mm
        pandora::InputFloat             m_coilZExtent;              ///< The coil z extent, units mm
        pandora::InputFloat             m_bField;                   ///< The detector magnetic field (assumed constant), units Tesla

        pandora::InputFloat             m_nRadLengthsInZGap;        ///< Absorber material in barrel/endcap z gap, units radiation lengths
        pandora::InputFloat             m_nIntLengthsInZGap;        ///< Absorber material in barrel/endcap z gap, units interaction lengths
        pandora::InputFloat             m_nRadLengthsInRadialGap;   ///< Absorber material in barrel/endcap radial gap, radiation lengths
        pandora::InputFloat             m_nIntLengthsInRadialGap;   ///< Absorber material in barrel/endcap radial gap, interaction lengths

        SubDetectorParametersMap        m_additionalSubDetectors;   ///< Map from name to parameters for any additional subdetectors
    };

    // Objects available for construction by pandora
    typedef ObjectCreationHelper<CaloHitParameters> CaloHit;
    typedef ObjectCreationHelper<TrackParameters> Track;
    typedef ObjectCreationHelper<MCParticleParameters> MCParticle;
    typedef ObjectCreationHelper<GeometryParameters> Geometry;

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
    static StatusCode GetParticleFlowObjects(const pandora::Pandora &pandora, pandora::ParticleFlowObjectList &particleFlowObjectList);

    /**
     *  @brief  Reset pandora to process another event
     * 
     *  @param  pandora the pandora instance to reset
     */
    static StatusCode Reset(const pandora::Pandora &pandora);
};

#endif // #ifndef PANDORA_API_H
