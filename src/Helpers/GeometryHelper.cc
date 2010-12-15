/**
 *  @file   PandoraPFANew/src/Helpers/GeometryHelper.cc
 * 
 *  @brief  Implementation of the geometry helper class.
 * 
 *  $Log: $
 */
 
#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/DetectorGap.h"

#include "Utilities/BFieldCalculator.h"
#include "Utilities/PseudoLayerCalculator.h"

#include <cmath>

namespace pandora
{

bool GeometryHelper::m_instanceFlag = false;
GeometryHelper* GeometryHelper::m_pGeometryHelper = NULL;
GeometryHelper::HitTypeToGranularityMap GeometryHelper::m_hitTypeToGranularityMap = GeometryHelper::GetDefaultHitTypeToGranularityMap();
float GeometryHelper::m_gapTolerance = 0.f;

//------------------------------------------------------------------------------------------------------------------------------------------

GeometryHelper *GeometryHelper::GetInstance()
{
    if (!m_instanceFlag)
    {
        m_pGeometryHelper = new GeometryHelper();
        m_instanceFlag = true;
    }

    return m_pGeometryHelper;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float GeometryHelper::GetBField(const CartesianVector &positionVector) const
{
    return m_pBFieldCalculator->GetBField(positionVector);
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer GeometryHelper::GetPseudoLayer(const CartesianVector &positionVector) const
{
    return m_pPseudoLayerCalculator->GetPseudoLayer(positionVector);
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer GeometryHelper::GetPseudoLayerAtIp() const
{
    return m_pPseudoLayerCalculator->GetPseudoLayerAtIp();
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool GeometryHelper::IsInDetectorGapRegion(const CartesianVector &position) const
{
    for (DetectorGapList::const_iterator iter = m_detectorGapList.begin(), iterEnd = m_detectorGapList.end(); iter != iterEnd; ++iter)
    {
        if ((*iter)->IsInGap(position))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float GeometryHelper::GetMaximumRadius(const unsigned int symmetryOrder, const float phi0, const float x, const float y)
{
    static const float twoPi = static_cast<float>(2. * std::acos(-1.));

    if (symmetryOrder <= 2)
        return std::sqrt((x * x) + (y * y));

    float maxRadius(0.);
    for (unsigned int iSymmetry = 0; iSymmetry < symmetryOrder; ++iSymmetry)
    {
        const float phi = phi0 + ((twoPi * static_cast<float>(iSymmetry)) / static_cast<float>(symmetryOrder));
        const float radius((x * std::cos(phi)) + (y * std::sin(phi)));

        if (radius > maxRadius)
            maxRadius = radius;
    }

    return maxRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float GeometryHelper::GetMaximumRadius(const AngleVector &angleVector, const float x, const float y)
{
    if (angleVector.size() <= 2)
        return std::sqrt((x * x) + (y * y));

    float maxRadius(0.);
    for (AngleVector::const_iterator iter = angleVector.begin(), iterEnd = angleVector.end(); iter != iterEnd; ++iter)
    {
        const float radius((x * iter->first) + (y * iter->second));

        if (radius > maxRadius)
            maxRadius = radius;
    }

    return maxRadius;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void GeometryHelper::FillAngleVector(const unsigned int symmetryOrder, const float phi0, AngleVector &angleVector)
{
    static const float twoPi = static_cast<float>(2. * std::acos(-1.));
    angleVector.clear();

    for (unsigned int iSymmetry = 0; iSymmetry < symmetryOrder; ++iSymmetry)
    {
        const float phi = phi0 + ((twoPi * static_cast<float>(iSymmetry)) / static_cast<float>(symmetryOrder));
        angleVector.push_back(std::make_pair(std::cos(phi), std::sin(phi)));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

GeometryHelper::GeometryHelper() :
    m_isInitialized(false),
    m_pBFieldCalculator(NULL),
    m_pPseudoLayerCalculator(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

GeometryHelper::~GeometryHelper()
{
    for (DetectorGapList::const_iterator iter = m_detectorGapList.begin(), iterEnd = m_detectorGapList.end(); iter != iterEnd; ++iter)
        delete *iter;

    delete m_pBFieldCalculator;
    delete m_pPseudoLayerCalculator;

    m_detectorGapList.clear();
    m_instanceFlag = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::Initialize(const PandoraApi::GeometryParameters &geometryParameters)
{
    try
    {
        if (m_isInitialized)
        {
            std::cout << "GeometryHelper: Singleton already initialized " << std::endl;
            throw StatusCodeException(STATUS_CODE_ALREADY_INITIALIZED);
        }

        if (NULL == m_pBFieldCalculator)
        {
            std::cout << "GeometryHelper: No bfield calculator specified " << std::endl;
            throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
        }

        if (NULL == m_pPseudoLayerCalculator)
        {
            std::cout << "GeometryHelper: No pseudo layer calculator specified " << std::endl;
            throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
        }

        m_mainTrackerInnerRadius = geometryParameters.m_mainTrackerInnerRadius;
        m_mainTrackerOuterRadius = geometryParameters.m_mainTrackerOuterRadius;
        m_mainTrackerZExtent = geometryParameters.m_mainTrackerZExtent;

        m_coilInnerRadius = geometryParameters.m_coilInnerRadius;
        m_coilOuterRadius = geometryParameters.m_coilOuterRadius;
        m_coilZExtent = geometryParameters.m_coilZExtent;

        m_inDetBarrelParameters.Initialize("InnerDetectorBarrelParameters", geometryParameters.m_inDetBarrelParameters);
        m_eCalBarrelParameters.Initialize("ECalBarrelParameters", geometryParameters.m_eCalBarrelParameters);
        m_hCalBarrelParameters.Initialize("HCalBarrelParameters", geometryParameters.m_hCalBarrelParameters);
        m_muonBarrelParameters.Initialize("MuonBarrelParameters", geometryParameters.m_muonBarrelParameters);

        m_inDetEndCapParameters.Initialize("InnerDetectorEndCapParameters", geometryParameters.m_inDetEndCapParameters);
        m_eCalEndCapParameters.Initialize("ECalEndCapParameters", geometryParameters.m_eCalEndCapParameters);
        m_hCalEndCapParameters.Initialize("HCalEndCapParameters", geometryParameters.m_hCalEndCapParameters);
        m_muonEndCapParameters.Initialize("MuonEndCapParameters", geometryParameters.m_muonEndCapParameters);

        for (PandoraApi::GeometryParameters::SubDetectorParametersMap::const_iterator iter = geometryParameters.m_additionalSubDetectors.begin(),
            iterEnd = geometryParameters.m_additionalSubDetectors.end(); iter != iterEnd; ++iter)
        {
            SubDetectorParameters subDetectorParameters;
            subDetectorParameters.Initialize(iter->first, iter->second);

            if (!subDetectorParameters.IsInitialized())
                continue;

            if (!m_additionalSubDetectors.insert(SubDetectorParametersMap::value_type(iter->first, subDetectorParameters)).second)
                throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        m_isInitialized = true;

        try
        {
            m_pBFieldCalculator->Initialize(this);
        }
        catch (StatusCodeException &statusCodeException)
        {
            std::cout << "GeometryHelper: Failed to initialize bfield calculator: " << statusCodeException.ToString() << std::endl;
            throw statusCodeException;
        }

        try
        {
            m_pPseudoLayerCalculator->Initialize(this);
        }
        catch (StatusCodeException &statusCodeException)
        {
            std::cout << "GeometryHelper: Failed to initialize pseudo layer calculator: " << statusCodeException.ToString() << std::endl;
            throw statusCodeException;
        }
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to initialize geometry: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::CreateBoxGap(const PandoraApi::BoxGap::Parameters &gapParameters)
{
    try
    {
        DetectorGap *pDetectorGap = NULL;
        pDetectorGap = new BoxGap(gapParameters);

        if (NULL == pDetectorGap)
            return STATUS_CODE_FAILURE;

        m_detectorGapList.push_back(pDetectorGap);
        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create box gap: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::CreateConcentricGap(const PandoraApi::ConcentricGap::Parameters &gapParameters)
{
    try
    {
        DetectorGap *pDetectorGap = NULL;
        pDetectorGap = new ConcentricGap(gapParameters);

        if (NULL == pDetectorGap)
            return STATUS_CODE_FAILURE;

        m_detectorGapList.push_back(pDetectorGap);
        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create concentric gap: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::SetBFieldCalculator(BFieldCalculator *pBFieldCalculator)
{
    if (m_isInitialized)
        return STATUS_CODE_NOT_ALLOWED;

    delete m_pBFieldCalculator;
    m_pBFieldCalculator = pBFieldCalculator;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::SetPseudoLayerCalculator(PseudoLayerCalculator *pPseudoLayerCalculator)
{
    if (m_isInitialized)
        return STATUS_CODE_NOT_ALLOWED;

    delete m_pPseudoLayerCalculator;
    m_pPseudoLayerCalculator = pPseudoLayerCalculator;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

GeometryHelper::SubDetectorParameters::SubDetectorParameters() :
    m_isInitialized(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void GeometryHelper::SubDetectorParameters::Initialize(const std::string &subDetectorName,
    const PandoraApi::GeometryParameters::SubDetectorParameters &inputParameters)
{
    try
    {
        m_innerRCoordinate = inputParameters.m_innerRCoordinate.Get();
        m_innerZCoordinate = inputParameters.m_innerZCoordinate.Get();
        m_innerPhiCoordinate = inputParameters.m_innerPhiCoordinate.Get();
        m_innerSymmetryOrder = inputParameters.m_innerSymmetryOrder.Get();
        m_outerRCoordinate = inputParameters.m_outerRCoordinate.Get();
        m_outerZCoordinate = inputParameters.m_outerZCoordinate.Get();
        m_outerPhiCoordinate = inputParameters.m_outerPhiCoordinate.Get();
        m_outerSymmetryOrder = inputParameters.m_outerSymmetryOrder.Get();
        m_nLayers = inputParameters.m_nLayers.Get();

        if (inputParameters.m_layerParametersList.empty() || (m_nLayers != inputParameters.m_layerParametersList.size()))
        {
            std::cout << "GeometryHelper: Invalid number of entries in layer parameters list for " << subDetectorName << std::endl;
            throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
        }

        for (PandoraApi::GeometryParameters::LayerParametersList::const_iterator iter = inputParameters.m_layerParametersList.begin();
            iter != inputParameters.m_layerParametersList.end(); ++iter)
        {
            LayerParameters layerParameters;
            layerParameters.m_closestDistanceToIp = iter->m_closestDistanceToIp.Get();
            layerParameters.m_nRadiationLengths = iter->m_nRadiationLengths.Get();
            layerParameters.m_nInteractionLengths = iter->m_nInteractionLengths.Get();

            m_layerParametersList.push_back(layerParameters);
        }

        m_isInitialized = true;
    }
    catch (StatusCodeException &statusCodeException)
    {
        if (STATUS_CODE_NOT_INITIALIZED != statusCodeException.GetStatusCode())
            throw statusCodeException;

        std::cout << "GeometryHelper: " << subDetectorName << " not specified." << std::endl;
        m_isInitialized = false;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

GeometryHelper::HitTypeToGranularityMap GeometryHelper::GetDefaultHitTypeToGranularityMap()
{
    HitTypeToGranularityMap hitTypeToGranularityMap;

    if (!hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(INNER_DETECTOR, FINE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(ECAL, FINE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(HCAL, COARSE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(MUON, VERY_COARSE)).second )
    {
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    return hitTypeToGranularityMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::SetHitTypeGranularity(const HitType hitType, const Granularity granularity)
{
    HitTypeToGranularityMap::iterator iter = m_hitTypeToGranularityMap.find(hitType);

    if (m_hitTypeToGranularityMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    iter->second = granularity;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GapTolerance", m_gapTolerance));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
