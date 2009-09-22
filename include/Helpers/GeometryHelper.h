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

namespace pandora
{

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
        float                   m_distanceFromIp;           ///< The distance of the layer from the interaction point, units mm
        float                   m_nRadiationLengths;        ///< Absorber material in front of layer, units radiation lengths
        float                   m_nInteractionLengths;      ///< Absorber material in front of layer, units interaction lengths
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
         */
        void Initialize(const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters);

        /**
         *  @brief  Get the distance of the innermost layer from interaction point, units mm
         * 
         *  @return The distance of the innermost layer from interaction point
         */
        float GetInnerDistanceFromIp() const;

        /**
         *  @brief  Get the order of symmetry of the innermost layer
         * 
         *  @return The order of symmetry of the innermost layer
         */
        unsigned int GetInnerSymmetry() const;

        /**
         *  @brief  Get the orientation of the innermost layer wrt the vertical
         * 
         *  @return The orientation of the innermost layer wrt the vertical
         */
        float GetInnerAngle() const;

        /**
         *  @brief  Get the distance of the outermost layer from interaction point, units mm
         * 
         *  @return The distance of the outermost layer from interaction point
         */
        float GetOuterDistanceFromIp() const;

        /**
         *  @brief  Get the order of symmetry of the outermost layer
         * 
         *  @return The order of symmetry of the outermost layer
         */
        unsigned int GetOuterSymmetry() const;

        /**
         *  @brief  Get orientation of the outermost layer wrt the vertical
         * 
         *  @return The orientation of the outermost layer wrt the vertical
         */
        float GetOuterAngle() const;

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
        float                   m_innerDistanceFromIp;      ///< Distance of the innermost layer from interaction point, units mm
        unsigned int            m_innerSymmetry;            ///< Order of symmetry of the innermost layer
        float                   m_innerAngle;               ///< Orientation of the innermost layer wrt the vertical
        float                   m_outerDistanceFromIp;      ///< Distance of the outermost layer from interaction point, units mm
        unsigned int            m_outerSymmetry;            ///< Order of symmetry of the outermost layer
        float                   m_outerAngle;               ///< Orientation of the outermost layer wrt the vertical
        unsigned int            m_nLayers;                  ///< The number of layers in the detector section
        LayerParametersList     m_layerParametersList;      ///< The list of layer parameters for the detector section
    };

    /**
     *  @brief  Get the geometry helper singleton
     */
    static GeometryHelper *GetInstance();

    /**
     *  @brief  Get the ecal barrel parameters
     * 
     *  @return The ecal barrel parameters
     */
    const SubDetectorParameters &GetECalBarrelParameters() const;

    /**
     *  @brief  Get the hcal barrel parameters
     * 
     *  @return The hcal barrel parameters
     */
    const SubDetectorParameters &GetHCalBarrelParameters() const;

    /**
     *  @brief  Get the ecal end cap parameters
     * 
     *  @return The ecal end cap parameters
     */
    const SubDetectorParameters &GetECalEndCapParameters() const;

    /**
     *  @brief  Get the hcal end cap parameters
     * 
     *  @return The hcal end cap parameters
     */
    const SubDetectorParameters &GetHCalEndCapParameters() const;

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

private:
    /**
     *  @brief  Constructor
     */
    GeometryHelper();

    /**
     *  @brief  Initialize the geometry helper
     * 
     *  @param  geometryParameters the geometry parameters
     */
    StatusCode Initialize(const PandoraApi::GeometryParameters &geometryParameters);

    bool                        m_isInitialized;            ///< Whether the geometry helper is initialized

    SubDetectorParameters       m_eCalBarrelParameters;     ///< The ecal barrel parameters
    SubDetectorParameters       m_hCalBarrelParameters;     ///< The hcal barrel parameters
    SubDetectorParameters       m_eCalEndCapParameters;     ///< The ecal end cap parameters
    SubDetectorParameters       m_hCalEndCapParameters;     ///< The hcal end cap parameter

    float                       m_mainTrackerInnerRadius;   ///< The main tracker inner radius, units mm
    float                       m_mainTrackerOuterRadius;   ///< The main tracker outer radius, units mm
    float                       m_mainTrackerZExtent;       ///< The main tracker z extent, units mm

    float                       m_nRadLengthsInZGap;        ///< Absorber material in barrel/endcap z gap, units radiation lengths
    float                       m_nIntLengthsInZGap;        ///< Absorber material in barrel/endcap z gap, units interaction lengths
    float                       m_nRadLengthsInRadialGap;   ///< Absorber material in barrel/endcap radial gap, radiation lengths
    float                       m_nIntLengthsInRadialGap;   ///< Absorber material in barrel/endcap radial gap, interaction lengths

    static bool                 m_instanceFlag;             ///< The geometry helper instance flag
    static GeometryHelper      *m_pGeometryHelper;          ///< The geometry helper instance

    friend class PandoraApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetECalBarrelParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_eCalBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetHCalBarrelParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_hCalBarrelParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetECalEndCapParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_eCalEndCapParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const GeometryHelper::SubDetectorParameters &GeometryHelper::GetHCalEndCapParameters() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_hCalEndCapParameters;
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
//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerDistanceFromIp() const
{
    return m_innerDistanceFromIp;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetInnerSymmetry() const
{
    return m_innerSymmetry;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetInnerAngle() const
{
    return m_innerAngle;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterDistanceFromIp() const
{
    return m_outerDistanceFromIp;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int GeometryHelper::SubDetectorParameters::GetOuterSymmetry() const
{
    return m_outerSymmetry;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GeometryHelper::SubDetectorParameters::GetOuterAngle() const
{
    return m_outerAngle;
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
