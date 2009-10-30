/**
 *  @file   PandoraPFANew/src/Helpers/GeometryHelper.cc
 * 
 *  @brief  Implementation of the geometry helper class.
 * 
 *  $Log: $
 */
 
#include "Helpers/GeometryHelper.h"

#include <cmath>

namespace pandora
{

bool GeometryHelper::m_instanceFlag = false;

GeometryHelper* GeometryHelper::m_pGeometryHelper = NULL;

//------------------------------------------------------------------------------------------------------------------------------------------

GeometryHelper *GeometryHelper::GetInstance()
{
    if(!m_instanceFlag)
    {
        m_pGeometryHelper = new GeometryHelper();
        m_instanceFlag = true;
    }

    return m_pGeometryHelper;
}

//------------------------------------------------------------------------------------------------------------------------------------------

GeometryHelper::GeometryHelper() :
    m_isInitialized(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

GeometryHelper::~GeometryHelper()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::Initialize(const PandoraApi::GeometryParameters &geometryParameters)
{
    try
    {
        if (m_isInitialized)
            return STATUS_CODE_ALREADY_INITIALIZED;

        m_mainTrackerInnerRadius = geometryParameters.m_mainTrackerInnerRadius.Get();
        m_mainTrackerOuterRadius = geometryParameters.m_mainTrackerOuterRadius.Get();
        m_mainTrackerZExtent     = geometryParameters.m_mainTrackerZExtent.Get();

        m_coilInnerRadius        = geometryParameters.m_coilInnerRadius.Get();
        m_coilOuterRadius        = geometryParameters.m_coilOuterRadius.Get();
        m_coilZExtent            = geometryParameters.m_coilZExtent.Get();

        m_nRadLengthsInZGap      = geometryParameters.m_nRadLengthsInZGap.Get();
        m_nIntLengthsInZGap      = geometryParameters.m_nIntLengthsInZGap.Get();
        m_nRadLengthsInRadialGap = geometryParameters.m_nRadLengthsInRadialGap.Get();
        m_nIntLengthsInRadialGap = geometryParameters.m_nIntLengthsInRadialGap.Get();

        m_eCalBarrelParameters.Initialize(geometryParameters.m_eCalBarrelParameters, &m_barrelLayerPositions);
        m_hCalBarrelParameters.Initialize(geometryParameters.m_hCalBarrelParameters, &m_barrelLayerPositions);
        m_eCalEndCapParameters.Initialize(geometryParameters.m_eCalEndCapParameters, &m_endCapLayerPositions);
        m_hCalEndCapParameters.Initialize(geometryParameters.m_hCalEndCapParameters, &m_endCapLayerPositions);

        for (PandoraApi::GeometryParameters::SubDetectorParametersList::const_iterator iter = geometryParameters.m_additionalSubDetectors.begin(),
            iterEnd = geometryParameters.m_additionalSubDetectors.end(); iter != iterEnd; ++iter)
        {
            SubDetectorParameters subDetectorParameters;
            subDetectorParameters.Initialize(*iter);
            m_additionalSubDetectors.push_back(subDetectorParameters);
        }

        if (0 == geometryParameters.m_eCalBarrelParameters.m_outerZCoordinate.Get())
        {
            m_geometryType = TEST_BEAM;
        }
        else if (geometryParameters.m_eCalEndCapParameters.m_outerRCoordinate.Get() > geometryParameters.m_eCalBarrelParameters.m_innerRCoordinate.Get())
        {
            m_geometryType = ENCLOSING_ENDCAP;
        }
        else
        {
            m_geometryType = ENCLOSING_BARREL;
        }

        m_isInitialized = true;

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to initialize geometry: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

void GeometryHelper::SubDetectorParameters::Initialize(const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters,
    LayerPositionList *const pLayerPositionList)
{
    m_innerRCoordinate      = inputParameters.m_innerRCoordinate.Get();
    m_innerZCoordinate      = inputParameters.m_innerZCoordinate.Get();
    m_innerPhiCoordinate    = inputParameters.m_innerPhiCoordinate.Get();
    m_innerSymmetryOrder    = inputParameters.m_innerSymmetryOrder.Get();
    m_outerRCoordinate      = inputParameters.m_outerRCoordinate.Get();
    m_outerZCoordinate      = inputParameters.m_outerZCoordinate.Get();
    m_outerPhiCoordinate    = inputParameters.m_outerPhiCoordinate.Get();
    m_outerSymmetryOrder    = inputParameters.m_outerSymmetryOrder.Get();
    m_nLayers               = inputParameters.m_nLayers.Get();

    if (inputParameters.m_layerParametersList.empty() || (m_nLayers != inputParameters.m_layerParametersList.size()))
    {
        std::cout << "GeometryHelper: Invalid number of entries in layer parameters list." << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    for (PandoraApi::GeometryParameters::LayerParametersList::const_iterator iter = inputParameters.m_layerParametersList.begin();
        iter != inputParameters.m_layerParametersList.end(); ++iter)
    {
        LayerParameters layerParameters;
        layerParameters.m_closestDistanceToIp   = iter->m_closestDistanceToIp.Get();
        layerParameters.m_nRadiationLengths     = iter->m_nRadiationLengths.Get();
        layerParameters.m_nInteractionLengths   = iter->m_nInteractionLengths.Get();

        m_layerParametersList.push_back(layerParameters);

        if (NULL != pLayerPositionList)
            pLayerPositionList->push_back(layerParameters.m_closestDistanceToIp);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::FindBarrelLayer(float radius, unsigned int &layer, bool shouldApplyOverlapCorrection) const
{
    static const float overlapCorrection(GetECalBarrelParameters().GetInnerRCoordinate() *
        ((GetECalEndCapParameters().GetInnerZCoordinate() / GetECalBarrelParameters().GetOuterZCoordinate()) - 1.));

    if (shouldApplyOverlapCorrection && (ENCLOSING_ENDCAP == GetGeometryType()))
        radius -= overlapCorrection;

    for (unsigned int iLayer = 0, iLayerEnd = m_barrelLayerPositions.size(); iLayer < iLayerEnd; ++iLayer)
    {
        const float separation = m_barrelLayerPositions[iLayer] - radius;

        if (separation > 0)
        {
            layer = iLayer;

            if ((layer > 0) && ((radius - m_barrelLayerPositions[iLayer - 1]) < separation))
                layer = iLayer - 1;

            return STATUS_CODE_SUCCESS;
        }
    }

    if (m_hCalBarrelParameters.GetOuterRCoordinate() > radius)
    {
        layer = m_barrelLayerPositions.size();
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::FindEndCapLayer(float zCoordinate, unsigned int &layer, bool shouldApplyOverlapCorrection) const
{
    static const float overlapCorrection(GetECalEndCapParameters().GetInnerZCoordinate() *
        ((GetECalBarrelParameters().GetInnerRCoordinate() / GetECalEndCapParameters().GetOuterRCoordinate()) - 1.));

    if (shouldApplyOverlapCorrection && (ENCLOSING_BARREL == GetGeometryType()))
        zCoordinate -= overlapCorrection;

    for (unsigned int iLayer = 0, iLayerEnd = m_endCapLayerPositions.size(); iLayer < iLayerEnd; ++iLayer)
    {
        const float separation = m_endCapLayerPositions[iLayer] - zCoordinate;

        if (separation > 0)
        {
            layer = iLayer;

            if ((layer > 0) && ((zCoordinate - m_endCapLayerPositions[iLayer - 1]) < separation))
                layer = iLayer - 1;

            return STATUS_CODE_SUCCESS;
        }
    }

    if (m_hCalEndCapParameters.GetOuterZCoordinate() > zCoordinate)
    {
        layer = m_endCapLayerPositions.size();
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float GeometryHelper::GetMaximumRadius(float x, float y) const
{
    static const unsigned int symmetryOrder = GetECalBarrelParameters().GetInnerSymmetryOrder();
    static const float phi0 = GetECalBarrelParameters().GetInnerPhiCoordinate();

    if (symmetryOrder <= 2)
        return std::sqrt((x * x) + (y * y));

    float maxRadius = 0;
    static const float twoPi = 2. * std::acos(-1.);

    // TODO shorten/remove these calculations
    for (unsigned int iSymmetry = 0; iSymmetry < symmetryOrder; ++iSymmetry)
    {
        const float phi = phi0 + ((twoPi * static_cast<float>(iSymmetry)) / static_cast<float>(symmetryOrder));
        const float radius = (x * std::cos(phi)) + (y * std::sin(phi));

        if(radius > maxRadius)
            maxRadius = radius;
    }

    return maxRadius;
}

} // namespace pandora
