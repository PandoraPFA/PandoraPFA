/**
 *  @file   PandoraPFANew/include/Helpers/GeometryHelper.h
 * 
 *  @brief  Header file for the geometry helper class.
 * 
 *  $Log: $
 */
#ifndef GEOMETRY_HELPER_H
#define GEOMETRY_HELPER_H 1

#include "Api/PandoraApi.h"

#include "Xml/tinyxml.h"

namespace pandora
{

/**
 *  @brief  Geometry type enum
 */
enum GeometryType
{
    ENCLOSING_ENDCAP,
    ENCLOSING_BARREL,
    TEST_BEAM
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  GeometryHelper class
 */
class GeometryHelper
{
public:
    /**
     *  @brief  LayerParameters class
     */
    class LayerParameters
    {
    public:
        float       m_closestDistanceToIp;              ///< Closest distance of the layer from the interaction point, units mm
        float       m_nRadiationLengths;                ///< Absorber material in front of layer, units radiation lengths
        float       m_nInteractionLengths;              ///< Absorber material in front of layer, units interaction lengths
        float       m_cumulativeRadiationLengths;       ///< Cumulative subdetector absorber material in front of layer, radiation lengths
        float       m_cumulativeInteractionLengths;     ///< Cumulative subdetector absorber material in front of layer, interaction lengths
    };

    typedef std::vector<LayerParameters> LayerParametersList;
    typedef std::vector<float> LayerPositionList;

    /**
     *  @brief  SubDetectorParameters class
     */
    class SubDetectorParameters
    {
    public:
        /**
         *  @brief  Initialize sub detector parameters
         * 
         *  @param  inputParameters the input sub detector parameters
         *  @param  pLayerPositionsList optional list to receive the distances of the layers from the interaction point
         */
        void Initialize(const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters,
            LayerPositionList *const pLayerPositionList = NULL);

        /**
         *  @brief  Get the inner cylindrical polar r coordinate, origin interaction point, units mm
         * 
         *  @return The inner cylindrical polar r coordinate
         */
        float GetInnerRCoordinate() const;

        /**
         *  @brief  Get the inner cylindrical polar z coordinate, origin interaction point, units mm
         * 
         *  @return The inner cylindrical polar z coordinate
         */
        float GetInnerZCoordinate() const;

        /**
         *  @brief  Get the inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
         * 
         *  @return The inner cylindrical polar phi coordinate
         */
        float GetInnerPhiCoordinate() const;

        /**
         *  @brief  Get the order of symmetry of the innermost edge of subdetector
         * 
         *  @return The order of symmetry of the innermost edge of subdetector
         */
        unsigned int GetInnerSymmetryOrder() const;

        /**
         *  @brief  Get the outer cylindrical polar r coordinate, origin interaction point, units mm
         * 
         *  @return The outer cylindrical polar r coordinate
         */
        float GetOuterRCoordinate() const;

        /**
         *  @brief  Get the outer cylindrical polar z coordinate, origin interaction point, units mm
         * 
         *  @return The outer cylindrical polar z coordinate
         */
        float GetOuterZCoordinate() const;

        /**
         *  @brief  Get the outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
         * 
         *  @return The outer cylindrical polar phi coordinate
         */
        float GetOuterPhiCoordinate() const;

        /**
         *  @brief  Get the order of symmetry of the outermost edge of subdetector
         * 
         *  @return The order of symmetry of the outermost edge of subdetector
         */
        unsigned int GetOuterSymmetryOrder() const;

        /**
         *  @brief  Get the number of layers in the detector section
         * 
         *  @return The number of layers in the detector section
         */
        unsigned int GetNLayers() const;

        /**
         *  @brief  Get the list of layer parameters for the detector section
         * 
         *  @return The list of layer parameters for the detector section
         */
        const LayerParametersList &GetLayerParametersList() const;

    private:
        float                   m_innerRCoordinate;     ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
        float                   m_innerZCoordinate;     ///< Inner cylindrical polar z coordinate, origin interaction point, units mm
        float                   m_innerPhiCoordinate;   ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
        unsigned int            m_innerSymmetryOrder;   ///< Order of symmetry of the innermost edge of subdetector
        float                   m_outerRCoordinate;     ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
        float                   m_outerZCoordinate;     ///< Outer cylindrical polar z coordinate, origin interaction point, units mm
        float                   m_outerPhiCoordinate;   ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
        unsigned int            m_outerSymmetryOrder;   ///< Order of symmetry of the outermost edge of subdetector
        unsigned int            m_nLayers;              ///< The number of layers in the detector section
        LayerParametersList     m_layerParametersList;  ///< The list of layer parameters for the detector section
    };

    typedef std::map<std::string, SubDetectorParameters> SubDetectorParametersMap;

    /**
     *  @brief  Gap class
     */
    class Gap
    {
    public:
        /**
         *  @brief  Destructor
         */
        virtual ~Gap();

        /**
         *  @brief  Whether a specified position lies within the gap
         * 
         *  @param  positionVector the position vector
         * 
         *  @return boolean
         */
        virtual bool IsInGap(const CartesianVector &positionVector) const = 0;
    };

    typedef std::vector<Gap *> GapList;
    typedef std::vector<CartesianVector> VertexPointList;

    /**
     *  @brief  BoxGap class
     */
    class BoxGap : public Gap
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  gapParameters the gap parameters
         */
        BoxGap(const PandoraApi::BoxGap::Parameters &gapParameters);

        bool IsInGap(const CartesianVector &positionVector) const;

        const CartesianVector   m_vertex;               ///< Cartesian coordinates of a gap vertex, units mm
        const CartesianVector   m_side1;                ///< Cartesian vector describing first side meeting vertex, units mm
        const CartesianVector   m_side2;                ///< Cartesian vector describing second side meeting vertex, units mm
        const CartesianVector   m_side3;                ///< Cartesian vector describing third side meeting vertex, units mm
    };

    /**
     *  @brief  ConcentricGap class
     */
    class ConcentricGap : public Gap
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  gapParameters the gap parameters
         */
        ConcentricGap(const PandoraApi::ConcentricGap::Parameters &gapParameters);

        bool IsInGap(const CartesianVector &positionVector) const;

        const float             m_minZCoordinate;       ///< Min cylindrical polar z coordinate, origin interaction point, units mm
        const float             m_maxZCoordinate;       ///< Max cylindrical polar z coordinate, origin interaction point, units mm
        const float             m_innerRCoordinate;     ///< Inner cylindrical polar r coordinate, origin interaction point, units mm
        const float             m_innerPhiCoordinate;   ///< Inner cylindrical polar phi coordinate (angle wrt cartesian x axis)
        const unsigned int      m_innerSymmetryOrder;   ///< Order of symmetry of the innermost edge of gap
        const float             m_outerRCoordinate;     ///< Outer cylindrical polar r coordinate, origin interaction point, units mm
        const float             m_outerPhiCoordinate;   ///< Outer cylindrical polar phi coordinate (angle wrt cartesian x axis)
        const unsigned int      m_outerSymmetryOrder;   ///< Order of symmetry of the outermost edge of gap

    private:
        VertexPointList         m_innerVertexPointList; ///< The vertex points of the inner polygon
        VertexPointList         m_outerVertexPointList; ///< The vertex points of the outer polygon
    };

    /**
     *  @brief  Get the geometry helper singleton
     */
    static GeometryHelper *GetInstance();

    /**
     *  @brief  Get the geometry type
     * 
     *  @return the geometry type
     */
    GeometryType GetGeometryType() const;

    /**
     *  @brief  Get the ecal barrel parameters
     * 
     *  @return The ecal barrel parameters
     */
    const SubDetectorParameters &GetECalBarrelParameters() const;

    /**
     *  @brief  Get the ecal end cap parameters
     * 
     *  @return The ecal end cap parameters
     */
    const SubDetectorParameters &GetECalEndCapParameters() const;

    /**
     *  @brief  Get the hcal barrel parameters
     * 
     *  @return The hcal barrel parameters
     */
    const SubDetectorParameters &GetHCalBarrelParameters() const;

    /**
     *  @brief  Get the hcal end cap parameters
     * 
     *  @return The hcal end cap parameters
     */
    const SubDetectorParameters &GetHCalEndCapParameters() const;

    /**
     *  @brief  Get the muon detector barrel parameters
     * 
     *  @return The muon detector barrel parameters
     */
    const SubDetectorParameters &GetMuonBarrelParameters() const;

    /**
     *  @brief  Get the muon detector end cap parameters
     * 
     *  @return The muon detector end cap parameters
     */
    const SubDetectorParameters &GetMuonEndCapParameters() const;

    /**
     *  @brief  Get the main tracker inner radius, units mm
     * 
     *  @return The main tracker inner radius
     */
    float GetMainTrackerInnerRadius() const;

    /**
     *  @brief  Get the main tracker outer radius, units mm
     * 
     *  @return The main tracker outer radius
     */
    float GetMainTrackerOuterRadius() const;

    /**
     *  @brief  Get the main tracker z extent, units mm
     * 
     *  @return The main tracker z extent
     */
    float GetMainTrackerZExtent() const;

    /**
     *  @brief  Get the coil inner radius, units mm
     * 
     *  @return The coil inner radius
     */
    float GetCoilInnerRadius() const;

    /**
     *  @brief  Get the coil outer radius, units mm
     * 
     *  @return The coil outer radius
     */
    float GetCoilOuterRadius() const;

    /**
     *  @brief  Get the coil z extent, units mm
     * 
     *  @return The coil z extent
     */
    float GetCoilZExtent() const;

    /**
     *  @brief  Get the detector magnetic field (assumed constant), units Tesla
     * 
     *  @return The detector magnetic field
     */
    float GetBField() const;

    /**
     *  @brief  Get the absorber material in barrel/endcap z gap, units radiation lengths
     * 
     *  @return The absorber material in barrel/endcap z gap
     */
    float GetNRadiationLengthsInZGap() const;

    /**
     *  @brief  Get the absorber material in barrel/endcap z gap, units interaction lengths
     * 
     *  @return The absorber material in barrel/endcap z gap
     */
    float GetNInteractionLengthsInZGap() const;

    /**
     *  @brief  Get the absorber material in barrel/endcap radial gap, units radiation lengths
     * 
     *  @return The absorber material in barrel/endcap radial gap
     */
    float GetNRadiationLengthsInRadialGap() const;

    /**
     *  @brief  Get the absorber material in barrel/endcap radial gap, units interaction lengths
     * 
     *  @return The absorber material in barrel/endcap radial gap
     */
    float GetNInteractionLengthsInRadialGap() const;

    /**
     *  @brief  Get the map from name to parameters for any additional sub detectors
     * 
     *  @return The map from name to parameters
     */
    const SubDetectorParametersMap &GetAdditionalSubDetectors() const;

    /**
     *  @brief  Get the list of gaps in the active detector volume
     * 
     *  @return The list of gaps in the active detector volume
     */
    const GapList &GetGapList() const;

    /**
     *  @brief  Get the appropriate pseudolayer for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the appropriate pseudolayer
     */
    PseudoLayer GetPseudoLayer(const CartesianVector &positionVector) const;

    /**
     *  @brief  Get the appropriate pseudolayer for a specified position vector
     * 
     *  @param  positionVector the specified position
     *  @param  pseudoLayer to receive the appropriate pseudolayer
     */
    StatusCode GetPseudoLayer(const CartesianVector &positionVector, PseudoLayer &pseudoLayer) const;

    /**
     *  @brief  Whether a specified position is outside of the ecal region
     * 
     *  @param  position the specified position
     * 
     *  @return boolean
     */
    bool IsOutsideECal(const CartesianVector &position) const;

    /**
     *  @brief  Whether a specified position is outside of the hcal region
     * 
     *  @param  position the specified position
     * 
     *  @return boolean
     */
    bool IsOutsideHCal(const CartesianVector &position) const;

    /**
     *  @brief  Whether a specified position is in the gap region between the ecal barrel and ecal endcap
     *
     *  @param  position the specified position
     *
     *  @return boolean
     */
    bool IsInECalGapRegion(const CartesianVector &position) const;

    /**
     *  @brief  Whether a specified position is in a gap region
     * 
     *  @param  position the specified position
     * 
     *  @return boolean
     */
    bool IsInGapRegion(const CartesianVector &position) const;

    /**
     *  @brief  Populate list of polygon vertices, assuming regular polygon in XY plane at constant z coordinate
     * 
     *  @brief  rCoordinate polygon r coordinate
     *  @brief  zCoordinate polygon z coordinate
     *  @brief  phiCoordinate polygon phi coordinate
     *  @brief  symmetryOrder polygon symmetry order
     *  @param  vertexPointList to receive the vertex point list, with vertexPointList[symmetryOrder] = vertexPointList[0]
     */
    static void GetPolygonVertices(const float rCoordinate, const float zCoordinate, const float phiCoordinate,
        const unsigned int symmetryOrder, VertexPointList &vertexPointList);

    /**
     *  @brief  Winding number test for a point in a 2D polygon in the XY plane (z coordinates are ignored)
     * 
     *  @param  point the test point
     *  @param  vertexPointList vertex points of a polygon, with vertexPointList[symmetryOrder] = vertexPointList[0]
     *  @param  symmetryOrder order of symmetry of polygon
     * 
     *  @return whether point is inside polygon
     */
    static bool IsIn2DPolygon(const CartesianVector &point, const VertexPointList &vertexPointList, const unsigned int symmetryOrder);

    /**
     *  @brief  Get the tolerance allowed when declaring a point to be "in" a gap region, units mm
     * 
     *  @return The gap tolerance
     */
    static float GetGapTolerance();

private:
    /**
     *  @brief  Create box gap
     * 
     *  @param  gapParameters the gap parameters
     */
    StatusCode CreateBoxGap(const PandoraApi::BoxGap::Parameters &gapParameters);

    /**
     *  @brief  Create concentric gap
     * 
     *  @param  gapParameters the gap parameters
     */
    StatusCode CreateConcentricGap(const PandoraApi::ConcentricGap::Parameters &gapParameters);

    /**
     *  @brief  Find the layer number corresponding to a specified radial position in the barrel
     * 
     *  @param  radius the radial distance to the ip
     *  @param  layer to receive the layer number
     *  @param  shouldApplyOverlapCorrection whether to apply an overlap correction
     */
    StatusCode FindBarrelLayer(float radius, unsigned int &layer, bool shouldApplyOverlapCorrection = false) const;

    /**
     *  @brief  Find the layer number corresponding to specified z position in the endcap
     * 
     *  @param  zCoordinate the z distance to the ip
     *  @param  layer to receive the layer number
     *  @param  shouldApplyOverlapCorrection whether to apply an overlap correction
     */
    StatusCode FindEndCapLayer(float zCoordinate, unsigned int &layer, bool shouldApplyOverlapCorrection = false) const;

    /**
     *  @brief  Find the layer number corresponding to a specified position, via reference to a specified layer position list
     * 
     *  @param  position the specified position
     *  @param  layerPositionList the specified layer position list
     *  @param  layer to receive the layer number
     */
    StatusCode FindMatchingLayer(const float position, const LayerPositionList &layerPositionList, unsigned int &layer) const;

    /**
     *  @brief  Get the maximum ecal barrel radius
     * 
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    float GetMaximumECalBarrelRadius(const float x, const float y) const;

    /**
     *  @brief  Constructor
     */
    GeometryHelper();

    /**
     *  @brief  Destructor
     */
    ~GeometryHelper();

    /**
     *  @brief  Initialize the geometry helper
     * 
     *  @param  geometryParameters the geometry parameters
     */
    StatusCode Initialize(const PandoraApi::GeometryParameters &geometryParameters);

    /**
     *  @brief  Read the cluster helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    bool                        m_isInitialized;            ///< Whether the geometry helper is initialized
    GeometryType                m_geometryType;             ///< The geometry type

    SubDetectorParameters       m_eCalBarrelParameters;     ///< The ecal barrel parameters
    SubDetectorParameters       m_eCalEndCapParameters;     ///< The ecal end cap parameters
    SubDetectorParameters       m_hCalBarrelParameters;     ///< The hcal barrel parameters
    SubDetectorParameters       m_hCalEndCapParameters;     ///< The hcal end cap parameters
    SubDetectorParameters       m_muonBarrelParameters;     ///< The muon detector barrel parameters
    SubDetectorParameters       m_muonEndCapParameters;     ///< The muon detector end cap parameters

    float                       m_mainTrackerInnerRadius;   ///< The main tracker inner radius, units mm
    float                       m_mainTrackerOuterRadius;   ///< The main tracker outer radius, units mm
    float                       m_mainTrackerZExtent;       ///< The main tracker z extent, units mm
    float                       m_coilInnerRadius;          ///< The coil inner radius, units mm
    float                       m_coilOuterRadius;          ///< The coil outer radius, units mm
    float                       m_coilZExtent;              ///< The coil z extent, units mm
    float                       m_bField;                   ///< The detector magnetic field (assumed constant), units Tesla
    float                       m_nRadLengthsInZGap;        ///< Absorber material in barrel/endcap z gap, units radiation lengths
    float                       m_nIntLengthsInZGap;        ///< Absorber material in barrel/endcap z gap, units interaction lengths
    float                       m_nRadLengthsInRadialGap;   ///< Absorber material in barrel/endcap radial gap, radiation lengths
    float                       m_nIntLengthsInRadialGap;   ///< Absorber material in barrel/endcap radial gap, interaction lengths

    SubDetectorParametersMap    m_additionalSubDetectors;   ///< Map from name to parameters for any additional subdetectors
    GapList                     m_gapList;                  ///< List of gaps in the active detector volume

    LayerPositionList           m_barrelLayerPositions;     ///< The barrel layer positions list
    LayerPositionList           m_endCapLayerPositions;     ///< The endcap layer positions list

    static bool                 m_instanceFlag;             ///< The geometry helper instance flag
    static GeometryHelper      *m_pGeometryHelper;          ///< The geometry helper instance

    static float                m_gapTolerance;             ///< Tolerance allowed when declaring a point to be "in" a gap region, units mm

    friend class Pandora;
    friend class PandoraApiImpl;
    friend class PandoraSettings;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline GeometryType GeometryHelper::GetGeometryType() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_geometryType;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetECalBarrelParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_eCalBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetECalEndCapParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_eCalEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetHCalBarrelParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_hCalBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetHCalEndCapParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_hCalEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetMuonBarrelParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_muonBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetMuonEndCapParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_muonEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerInnerRadius() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_mainTrackerInnerRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerOuterRadius() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_mainTrackerOuterRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerZExtent() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_mainTrackerZExtent;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilInnerRadius() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_coilInnerRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilOuterRadius() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_coilOuterRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilZExtent() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_coilZExtent;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetBField() const
{
    return m_bField;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetNRadiationLengthsInZGap() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_nRadLengthsInZGap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetNInteractionLengthsInZGap() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_nIntLengthsInZGap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetNRadiationLengthsInRadialGap() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_nRadLengthsInRadialGap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetNInteractionLengthsInRadialGap() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_nIntLengthsInRadialGap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParametersMap &GeometryHelper::GetAdditionalSubDetectors() const
{
    return m_additionalSubDetectors;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::GapList &GeometryHelper::GetGapList() const
{
    return m_gapList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode GeometryHelper::GetPseudoLayer(const CartesianVector &positionVector, PseudoLayer &pseudoLayer) const
{
    try
    {
        pseudoLayer = this->GetPseudoLayer(positionVector);
    }
    catch (StatusCodeException &statusCodeException)
    {
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetGapTolerance()
{
    return m_gapTolerance;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerRCoordinate() const
{
    return m_innerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerZCoordinate() const
{
    return m_innerZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerPhiCoordinate() const
{
    return m_innerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetInnerSymmetryOrder() const
{
    return m_innerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterRCoordinate() const
{
    return m_outerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterZCoordinate() const
{
    return m_outerZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterPhiCoordinate() const
{
    return m_outerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetOuterSymmetryOrder() const
{
    return m_outerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetNLayers() const
{
    return m_nLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::LayerParametersList &GeometryHelper::SubDetectorParameters::GetLayerParametersList() const
{
    return m_layerParametersList;
}

} // namespace pandora

#endif // #ifndef GEOMETRY_HELPER_H
