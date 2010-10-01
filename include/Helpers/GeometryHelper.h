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

class DetectorGap;
class PseudoLayerCalculator;

//------------------------------------------------------------------------------------------------------------------------------------------

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
        void Initialize(const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters);

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
    typedef std::vector<DetectorGap *> DetectorGapList;

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
    const DetectorGapList &GetDetectorGapList() const;

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
     *  @brief  Whether a specified position is in a detector gap region
     * 
     *  @param  position the specified position
     * 
     *  @return boolean
     */
    bool IsInDetectorGapRegion(const CartesianVector &position) const;

    /**
     *  @brief  Get the maximum polygon radius
     * 
     *  @param  symmetryOrder the polygon symmetry order
     *  @param  phi0 the polygon cylindrical polar phi coordinate
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    float GetMaximumRadius(const unsigned int symmetryOrder, const float phi0, const float x, const float y) const;

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
     *  @brief  Get the maximum hcal barrel radius
     * 
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    float GetMaximumHCalBarrelRadius(const float x, const float y) const;

    /**
     *  @brief  Get the maximum muon barrel radius
     * 
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    float GetMaximumMuonBarrelRadius(const float x, const float y) const;

    /**
     *  @brief  Get the tolerance allowed when declaring a point to be "in" a gap region, units mm
     * 
     *  @return The gap tolerance
     */
    static float GetGapTolerance();

private:
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

    typedef std::vector< std::pair<float, float> > AngleVector;

    /**
     *  @brief  Fill a vector with sine/cosine values for relevant polygon angles
     * 
     *  @param  symmetryOrder the polygon symmetry order
     *  @param  phi0 the polygon cylindrical polar phi coordinate
     *  @param  angleVector the vector to fill with sine/cosine values for relevant polygon angles
     */
    void FillAngleVector(const unsigned int symmetryOrder, const float phi0, AngleVector &angleVector) const;

    /**
     *  @brief  Get the maximum polygon radius, with reference to cached sine/cosine values for relevant polygon angles
     * 
     *  @param  angleVector vector containing cached sine/cosine values
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    float GetMaximumRadius(const AngleVector &angleVector, const float x, const float y) const;

    /**
     *  @brief  Read the cluster helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    bool                        m_isInitialized;            ///< Whether the geometry helper is initialized
    GeometryType                m_geometryType;             ///< The geometry type
    PseudoLayerCalculator      *m_pPseudoLayerCalculator;   ///< Address of the pseudolayer calculator

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
    DetectorGapList             m_detectorGapList;          ///< List of gaps in the active detector volume

    AngleVector                 m_eCalBarrelAngleVector;    ///< The ecal barrel angle vector
    AngleVector                 m_hCalBarrelAngleVector;    ///< The hcal barrel angle vector
    AngleVector                 m_muonBarrelAngleVector;    ///< The muon barrel angle vector

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

inline const GeometryHelper::DetectorGapList &GeometryHelper::GetDetectorGapList() const
{
    return m_detectorGapList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMaximumECalBarrelRadius(const float x, const float y) const
{
    return this->GetMaximumRadius(m_eCalBarrelAngleVector, x, y);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMaximumHCalBarrelRadius(const float x, const float y) const
{
    return this->GetMaximumRadius(m_hCalBarrelAngleVector, x, y);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMaximumMuonBarrelRadius(const float x, const float y) const
{
    return this->GetMaximumRadius(m_muonBarrelAngleVector, x, y);
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
