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

StatusCode GeometryHelper::GetPseudoLayer(const CartesianVector &positionVector, PseudoLayer &pseudoLayer) const
{
    try
    {
        pseudoLayer = m_pPseudoLayerCalculator->GetPseudoLayer(positionVector);
    }
    catch (StatusCodeException &statusCodeException)
    {
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool GeometryHelper::IsOutsideECal(const CartesianVector &position) const
{
    static const float eCalBarrelOuterR(GetECalBarrelParameters().GetOuterRCoordinate());
    static const float eCalEndCapOuterZ(GetECalEndCapParameters().GetOuterZCoordinate());

    if (position.GetZ() > eCalEndCapOuterZ)
        return true;

    static const unsigned int eCalBarrelOuterSymmetry(GetECalBarrelParameters().GetOuterSymmetryOrder());
    static const float eCalBarrelOuterPhi(GetECalBarrelParameters().GetOuterPhiCoordinate());

    static const float eCalBarrelMaxRadius(this->GetMaximumRadius(eCalBarrelOuterSymmetry, eCalBarrelOuterPhi, 0.f, eCalBarrelOuterR));
    const float maxRadius(this->GetMaximumRadius(eCalBarrelOuterSymmetry, eCalBarrelOuterPhi, position.GetX(), position.GetY()));

    if (maxRadius > eCalBarrelMaxRadius)
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool GeometryHelper::IsOutsideHCal(const CartesianVector &position) const
{
    static const float hCalBarrelOuterR(GetHCalBarrelParameters().GetOuterRCoordinate());
    static const float hCalEndCapOuterZ(GetHCalEndCapParameters().GetOuterZCoordinate());

    if (position.GetZ() > hCalEndCapOuterZ)
        return true;

    static const unsigned int hCalBarrelOuterSymmetry(GetHCalBarrelParameters().GetOuterSymmetryOrder());
    static const float hCalBarrelOuterPhi(GetHCalBarrelParameters().GetOuterPhiCoordinate());

    static const float hCalBarrelMaxRadius(this->GetMaximumRadius(hCalBarrelOuterSymmetry, hCalBarrelOuterPhi, 0.f, hCalBarrelOuterR));
    const float maxRadius(this->GetMaximumRadius(hCalBarrelOuterSymmetry, hCalBarrelOuterPhi, position.GetX(), position.GetY()));

    if (maxRadius > hCalBarrelMaxRadius)
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool GeometryHelper::IsInECalGapRegion(const CartesianVector &position) const
{
    if (ENCLOSING_ENDCAP == m_geometryType)
    {
        static const float eCalBarrelOuterZCoordinate(GetECalBarrelParameters().GetOuterZCoordinate());
        static const float eCalEndCapInnerZCoordinate(GetECalEndCapParameters().GetInnerZCoordinate());

        const float z(std::fabs(position.GetZ()));

        return ((z > eCalBarrelOuterZCoordinate) && (z < eCalEndCapInnerZCoordinate));
    }
    else if (ENCLOSING_BARREL == m_geometryType)
    {
        static const float eCalEndCapOuterRCoordinate(GetECalEndCapParameters().GetOuterRCoordinate());
        static const float eCalBarrelInnerZCoordinate(GetECalBarrelParameters().GetInnerRCoordinate());

        const float x(position.GetX()), y(position.GetY());
        const float r(std::sqrt((x * x) + (y * y)));

        return ((r > eCalEndCapOuterRCoordinate) && (r < eCalBarrelInnerZCoordinate));
    }

    return false;
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

float GeometryHelper::GetMaximumRadius(const unsigned int symmetryOrder, const float phi0, const float x, const float y) const
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

    if (NULL != m_pBFieldCalculator)
        delete m_pBFieldCalculator;

    if (NULL != m_pPseudoLayerCalculator)
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
        m_muonBarrelParameters.Initialize(geometryParameters.m_muonBarrelParameters);

        m_eCalEndCapParameters.Initialize(geometryParameters.m_eCalEndCapParameters);
        m_hCalEndCapParameters.Initialize(geometryParameters.m_hCalEndCapParameters);
        m_muonEndCapParameters.Initialize(geometryParameters.m_muonEndCapParameters);

        for (PandoraApi::GeometryParameters::SubDetectorParametersMap::const_iterator iter = geometryParameters.m_additionalSubDetectors.begin(),
            iterEnd = geometryParameters.m_additionalSubDetectors.end(); iter != iterEnd; ++iter)
        {
            SubDetectorParameters subDetectorParameters;
            subDetectorParameters.Initialize(iter->second);

            if (!m_additionalSubDetectors.insert(SubDetectorParametersMap::value_type(iter->first, subDetectorParameters)).second)
                throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        if (0.f == m_eCalBarrelParameters.GetOuterZCoordinate())
        {
            m_geometryType = TEST_BEAM;
        }
        else if (m_eCalEndCapParameters.GetOuterRCoordinate() > m_eCalBarrelParameters.GetInnerRCoordinate())
        {
            m_geometryType = ENCLOSING_ENDCAP;
        }
        else
        {
            m_geometryType = ENCLOSING_BARREL;
        }

        this->FillAngleVector(m_eCalBarrelParameters.GetInnerSymmetryOrder(), m_eCalBarrelParameters.GetInnerPhiCoordinate(), m_eCalBarrelAngleVector);
        this->FillAngleVector(m_hCalBarrelParameters.GetInnerSymmetryOrder(), m_hCalBarrelParameters.GetInnerPhiCoordinate(), m_hCalBarrelAngleVector);
        this->FillAngleVector(m_muonBarrelParameters.GetInnerSymmetryOrder(), m_muonBarrelParameters.GetInnerPhiCoordinate(), m_muonBarrelAngleVector);

        m_isInitialized = true;
        m_pBFieldCalculator->Initialize(this);
        m_pPseudoLayerCalculator->Initialize(this);
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

void GeometryHelper::FillAngleVector(const unsigned int symmetryOrder, const float phi0, AngleVector &angleVector) const
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

float GeometryHelper::GetMaximumRadius(const AngleVector &angleVector, const float x, const float y) const
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

StatusCode GeometryHelper::SetBFieldCalculator(BFieldCalculator *pBFieldCalculator)
{
    if (m_isInitialized)
        return STATUS_CODE_NOT_ALLOWED;

    if (NULL != m_pBFieldCalculator)
        delete m_pBFieldCalculator;

    m_pBFieldCalculator = pBFieldCalculator;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::SetPseudoLayerCalculator(PseudoLayerCalculator *pPseudoLayerCalculator)
{
    if (m_isInitialized)
        return STATUS_CODE_NOT_ALLOWED;

    if (NULL != m_pPseudoLayerCalculator)
        delete m_pPseudoLayerCalculator;

    m_pPseudoLayerCalculator = pPseudoLayerCalculator;

    return STATUS_CODE_SUCCESS;
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

    float cumulativeRadiationLengths(0.);
    float cumulativeInteractionLengths(0.);

    for (PandoraApi::GeometryParameters::LayerParametersList::const_iterator iter = inputParameters.m_layerParametersList.begin();
        iter != inputParameters.m_layerParametersList.end(); ++iter)
    {
        LayerParameters layerParameters;
        layerParameters.m_closestDistanceToIp = iter->m_closestDistanceToIp.Get();

        const float nRadiationLengths(iter->m_nRadiationLengths.Get());
        const float nInteractionLengths(iter->m_nInteractionLengths.Get());

        cumulativeRadiationLengths += nRadiationLengths;
        cumulativeInteractionLengths += nInteractionLengths;

        layerParameters.m_nRadiationLengths = nRadiationLengths;
        layerParameters.m_nInteractionLengths = nInteractionLengths;
        layerParameters.m_cumulativeRadiationLengths = cumulativeRadiationLengths;
        layerParameters.m_cumulativeInteractionLengths = cumulativeInteractionLengths;

        m_layerParametersList.push_back(layerParameters);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

float GeometryHelper::m_gapTolerance = 0.f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GapTolerance", m_gapTolerance));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
