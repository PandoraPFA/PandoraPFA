/**
 *  @file   PandoraPFANew/src/Helpers/GeometryHelper.cc
 * 
 *  @brief  Implementation of the geometry helper class.
 * 
 *  $Log: $
 */
 
#include "Helpers/GeometryHelper.h"

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

        m_eCalBarrelParameters.Initialize(geometryParameters.m_eCalBarrelParameters);
        m_hCalBarrelParameters.Initialize(geometryParameters.m_hCalBarrelParameters);
        m_eCalEndCapParameters.Initialize(geometryParameters.m_eCalEndCapParameters);
        m_hCalEndCapParameters.Initialize(geometryParameters.m_hCalEndCapParameters);

        for (PandoraApi::GeometryParameters::SubDetectorParametersList::const_iterator iter = geometryParameters.m_additionalSubDetectors.begin(),
            iterEnd = geometryParameters.m_additionalSubDetectors.end(); iter != iterEnd; ++iter)
        {
            SubDetectorParameters subDetectorParameters;
            subDetectorParameters.Initialize(*iter);
            m_additionalSubDetectors.push_back(subDetectorParameters);
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

void GeometryHelper::SubDetectorParameters::Initialize(const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters)
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
    }
}

} // namespace pandora
