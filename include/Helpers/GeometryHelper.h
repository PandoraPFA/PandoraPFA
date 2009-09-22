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
     *  @brief  Constructor
     */
    GeometryHelper();

private:
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
     *  @brief  Initialize the geometry helper
     * 
     *  @param  geometryParameters the geometry parameters
     */
    StatusCode Initialize(const PandoraApi::GeometryParameters &geometryParameters);

    /**
     *  @brief  Initialize sub detector parameters
     * 
     *  @param  inputParameters the input sub detector parameters
     *  @param  subDetectorParameters the sub detector parameters to initialize
     */
    void InitializeSubDetectorParameters(const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters,
        SubDetectorParameters &subDetectorParameters);

    bool                        m_isInitialized;            ///< Whether the geometry helper is initialized

    SubDetectorParameters       m_eCalBarrelParameters;     ///< The ecal barrel parameters
    SubDetectorParameters       m_hCalBarrelParameters;     ///< The hcal barrel parameters
    SubDetectorParameters       m_eCalEndCapParameters;     ///< the ecal end cap parameters
    SubDetectorParameters       m_hCalEndCapParameters;     ///< The hcal end cap parameter

    float                       m_mainTrackerInnerRadius;   ///< The main tracker inner radius, units mm
    float                       m_mainTrackerOuterRadius;   ///< The main tracker outer radius, units mm
    float                       m_mainTrackerZExtent;       ///< The main tracker z extent, units mm

    float                       m_nRadLengthsInZGap;        ///< Absorber material in barrel/endcap z gap, units radiation lengths
    float                       m_nIntLengthsInZGap;        ///< Absorber material in barrel/endcap z gap, units interaction lengths
    float                       m_nRadLengthsInRadialGap;   ///< Absorber material in barrel/endcap radial gap, radiation lengths
    float                       m_nIntLengthsInRadialGap;   ///< Absorber material in barrel/endcap radial gap, interaction lengths

    friend class PandoraImpl;
    friend class PandoraApiImpl;
};

} // namespace pandora

#endif // #ifndef GEOMETRY_HELPER_H
