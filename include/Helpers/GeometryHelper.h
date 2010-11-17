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
class BFieldCalculator;
class PseudoLayerCalculator;

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
    };

    typedef std::vector<LayerParameters> LayerParametersList;

    /**
     *  @brief  SubDetectorParameters class
     */
    class SubDetectorParameters
    {
    public:
        /**
         *  @brief  Default constructor
         */
        SubDetectorParameters();

        /**
         *  @brief  Initialize sub detector parameters
         * 
         *  @param  subDetectorName the sub detector name
         *  @param  inputParameters the input sub detector parameters
         */
        void Initialize(const std::string &subDetectorName, const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters);

        /**
         *  @brief  Whether the sub detector parameters have been initialized
         *
         *  @return boolean
         */
        bool IsInitialized() const;

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
        bool                    m_isInitialized;        ///< Whether the sub detector parameters have been initialized
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
     *  @brief  Get the bfield value for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the bfield, units Tesla
     */
    float GetBField(const CartesianVector &positionVector) const;

    /**
     *  @brief  Get the appropriate pseudolayer for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the appropriate pseudolayer
     */
    PseudoLayer GetPseudoLayer(const CartesianVector &positionVector) const;

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
    static float GetMaximumRadius(const unsigned int symmetryOrder, const float phi0, const float x, const float y);

    typedef std::vector< std::pair<float, float> > AngleVector;

    /**
     *  @brief  Get the maximum polygon radius, with reference to cached sine/cosine values for relevant polygon angles
     * 
     *  @param  angleVector vector containing cached sine/cosine values
     *  @param  x the cartesian x coordinate
     *  @param  y the cartesian y coordinate
     * 
     *  @return the maximum radius
     */
    static float GetMaximumRadius(const AngleVector &angleVector, const float x, const float y);

    /**
     *  @brief  Fill a vector with sine/cosine values for relevant polygon angles
     * 
     *  @param  symmetryOrder the polygon symmetry order
     *  @param  phi0 the polygon cylindrical polar phi coordinate
     *  @param  angleVector the vector to fill with sine/cosine values for relevant polygon angles
     */
    static void FillAngleVector(const unsigned int symmetryOrder, const float phi0, AngleVector &angleVector);

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

    /**
     *  @brief  Set the bfield calculator
     * 
     *  @param  pBFieldCalculator address of the bfield calculator
     */
    StatusCode SetBFieldCalculator(BFieldCalculator *pBFieldCalculator);

    /**
     *  @brief  Set the pseudo layer calculator
     * 
     *  @param  pPseudoLayerCalculator address of the pseudo layer calculator
     */
    StatusCode SetPseudoLayerCalculator(PseudoLayerCalculator *pPseudoLayerCalculator);

    /**
     *  @brief  Read the cluster helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    bool                        m_isInitialized;            ///< Whether the geometry helper is initialized
    BFieldCalculator           *m_pBFieldCalculator;        ///< Address of the bfield calculator
    PseudoLayerCalculator      *m_pPseudoLayerCalculator;   ///< Address of the pseudolayer calculator

    SubDetectorParameters       m_eCalBarrelParameters;     ///< The ecal barrel parameters
    SubDetectorParameters       m_eCalEndCapParameters;     ///< The ecal end cap parameters
    SubDetectorParameters       m_hCalBarrelParameters;     ///< The hcal barrel parameters
    SubDetectorParameters       m_hCalEndCapParameters;     ///< The hcal end cap parameters
    SubDetectorParameters       m_muonBarrelParameters;     ///< The muon detector barrel parameters
    SubDetectorParameters       m_muonEndCapParameters;     ///< The muon detector end cap parameters

    InputFloat                  m_mainTrackerInnerRadius;   ///< The main tracker inner radius, units mm
    InputFloat                  m_mainTrackerOuterRadius;   ///< The main tracker outer radius, units mm
    InputFloat                  m_mainTrackerZExtent;       ///< The main tracker z extent, units mm
    InputFloat                  m_coilInnerRadius;          ///< The coil inner radius, units mm
    InputFloat                  m_coilOuterRadius;          ///< The coil outer radius, units mm
    InputFloat                  m_coilZExtent;              ///< The coil z extent, units mm

    SubDetectorParametersMap    m_additionalSubDetectors;   ///< Map from name to parameters for any additional subdetectors
    DetectorGapList             m_detectorGapList;          ///< List of gaps in the active detector volume

    static bool                 m_instanceFlag;             ///< The geometry helper instance flag
    static GeometryHelper      *m_pGeometryHelper;          ///< The geometry helper instance

    static float                m_gapTolerance;             ///< Tolerance allowed when declaring a point to be "in" a gap region, units mm

    friend class Pandora;
    friend class PandoraApiImpl;
    friend class PandoraSettings;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetECalBarrelParameters() const
{
    if (!m_eCalBarrelParameters.IsInitialized())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_eCalBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetECalEndCapParameters() const
{
    if (!m_eCalEndCapParameters.IsInitialized())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_eCalEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetHCalBarrelParameters() const
{
    if (!m_hCalBarrelParameters.IsInitialized())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_hCalBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetHCalEndCapParameters() const
{
    if (!m_hCalEndCapParameters.IsInitialized())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_hCalEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetMuonBarrelParameters() const
{
    if (!m_muonBarrelParameters.IsInitialized())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_muonBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetMuonEndCapParameters() const
{
    if (!m_muonEndCapParameters.IsInitialized())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_muonEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerInnerRadius() const
{
    return m_mainTrackerInnerRadius.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerOuterRadius() const
{
    return m_mainTrackerOuterRadius.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetMainTrackerZExtent() const
{
    return m_mainTrackerZExtent.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilInnerRadius() const
{
    return m_coilInnerRadius.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilOuterRadius() const
{
    return m_coilOuterRadius.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetCoilZExtent() const
{
    return m_coilZExtent.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParametersMap &GeometryHelper::GetAdditionalSubDetectors() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_additionalSubDetectors;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::DetectorGapList &GeometryHelper::GetDetectorGapList() const
{
    return m_detectorGapList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::GetGapTolerance()
{
    return m_gapTolerance;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline bool GeometryHelper::SubDetectorParameters::IsInitialized() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerRCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_innerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerZCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_innerZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerPhiCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_innerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetInnerSymmetryOrder() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_innerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterRCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_outerRCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterZCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_outerZCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterPhiCoordinate() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_outerPhiCoordinate;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetOuterSymmetryOrder() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_outerSymmetryOrder;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetNLayers() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_nLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::LayerParametersList &GeometryHelper::SubDetectorParameters::GetLayerParametersList() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_layerParametersList;
}

} // namespace pandora

#endif // #ifndef GEOMETRY_HELPER_H
