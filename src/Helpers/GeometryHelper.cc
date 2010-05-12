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
        m_bField                 = geometryParameters.m_bField.Get();

        m_nRadLengthsInZGap      = geometryParameters.m_nRadLengthsInZGap.Get();
        m_nIntLengthsInZGap      = geometryParameters.m_nIntLengthsInZGap.Get();
        m_nRadLengthsInRadialGap = geometryParameters.m_nRadLengthsInRadialGap.Get();
        m_nIntLengthsInRadialGap = geometryParameters.m_nIntLengthsInRadialGap.Get();

        m_eCalBarrelParameters.Initialize(geometryParameters.m_eCalBarrelParameters, &m_barrelLayerPositions);
        m_hCalBarrelParameters.Initialize(geometryParameters.m_hCalBarrelParameters, &m_barrelLayerPositions);
        m_muonBarrelParameters.Initialize(geometryParameters.m_muonBarrelParameters, &m_barrelLayerPositions);

        m_eCalEndCapParameters.Initialize(geometryParameters.m_eCalEndCapParameters, &m_endCapLayerPositions);
        m_hCalEndCapParameters.Initialize(geometryParameters.m_hCalEndCapParameters, &m_endCapLayerPositions);
        m_muonEndCapParameters.Initialize(geometryParameters.m_muonEndCapParameters, &m_endCapLayerPositions);

        for (PandoraApi::GeometryParameters::SubDetectorParametersMap::const_iterator iter = geometryParameters.m_additionalSubDetectors.begin(),
            iterEnd = geometryParameters.m_additionalSubDetectors.end(); iter != iterEnd; ++iter)
        {
            SubDetectorParameters subDetectorParameters;
            subDetectorParameters.Initialize(iter->second);

            if (!m_additionalSubDetectors.insert(SubDetectorParametersMap::value_type(iter->first, subDetectorParameters)).second)
                return STATUS_CODE_FAILURE;
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

        std::sort(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end());
        std::sort(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end());

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

        if (NULL != pLayerPositionList)
            pLayerPositionList->push_back(layerParameters.m_closestDistanceToIp);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer GeometryHelper::GetPseudoLayer(const CartesianVector &positionVector) const
{
    static const GeometryHelper::SubDetectorParameters eCalBarrelParameters = this->GetECalBarrelParameters();
    static const GeometryHelper::SubDetectorParameters eCalEndCapParameters = this->GetECalEndCapParameters();

    const float zCoordinate = std::fabs(positionVector.GetZ());
    const float radius = this->GetMaximumRadius(positionVector.GetX(), positionVector.GetY());

    PseudoLayer pseudoLayer;

    if (zCoordinate < eCalEndCapParameters.GetInnerZCoordinate())
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindBarrelLayer(radius, pseudoLayer));
    }
    else if (radius < eCalBarrelParameters.GetInnerRCoordinate())
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindEndCapLayer(zCoordinate, pseudoLayer));
    }
    else
    {
        PseudoLayer bestBarrelLayer;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindBarrelLayer(radius, bestBarrelLayer, true));

        PseudoLayer bestEndCapLayer;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindEndCapLayer(zCoordinate, bestEndCapLayer, true));

        pseudoLayer = std::max(bestBarrelLayer, bestEndCapLayer);
    }

    // Reserve pseudo layer(s) for track projections
    return (1 + TRACK_PROJECTION_LAYER + pseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool GeometryHelper::IsOutsideECal(const CartesianVector &position) const
{
    static const float eCalBarrelOuterRCoordinate(GetECalBarrelParameters().GetOuterRCoordinate());
    static const float eCalEndCapOuterZCoordinate(GetECalEndCapParameters().GetOuterZCoordinate());

    if (position.GetZ() > eCalEndCapOuterZCoordinate)
        return true;

    static const float eCalBarrelMaxRadius(this->GetMaximumRadius(0.f, eCalBarrelOuterRCoordinate));
    const float maxRadius(this->GetMaximumRadius(position.GetX(), position.GetY()));

    if (maxRadius > eCalBarrelMaxRadius)
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool GeometryHelper::IsOutsideHCal(const CartesianVector &position) const
{
    static const float hCalBarrelOuterRCoordinate(GetHCalBarrelParameters().GetOuterRCoordinate());
    static const float hCalEndCapOuterZCoordinate(GetHCalEndCapParameters().GetOuterZCoordinate());

    if (position.GetZ() > hCalEndCapOuterZCoordinate)
        return true;

    static const float hCalBarrelMaxRadius(this->GetMaximumRadius(0.f, hCalBarrelOuterRCoordinate));
    const float maxRadius(this->GetMaximumRadius(position.GetX(), position.GetY()));

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

StatusCode GeometryHelper::FindBarrelLayer(float radius, unsigned int &layer, bool shouldApplyOverlapCorrection) const
{
    static const float overlapCorrection(GetECalBarrelParameters().GetInnerRCoordinate() *
        ((GetECalEndCapParameters().GetInnerZCoordinate() / GetECalBarrelParameters().GetOuterZCoordinate()) - 1.f));

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
        layer = m_barrelLayerPositions.size() - 1;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::FindEndCapLayer(float zCoordinate, unsigned int &layer, bool shouldApplyOverlapCorrection) const
{
    static const float overlapCorrection(GetECalEndCapParameters().GetInnerZCoordinate() *
        ((GetECalBarrelParameters().GetInnerRCoordinate() / GetECalEndCapParameters().GetOuterRCoordinate()) - 1.f));

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
        layer = m_endCapLayerPositions.size() - 1;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float GeometryHelper::GetMaximumRadius(float x, float y) const
{
    static const unsigned int symmetryOrder = GetECalBarrelParameters().GetInnerSymmetryOrder();
    static const float phi0 = GetECalBarrelParameters().GetInnerPhiCoordinate();
    static const float twoPi = static_cast<float>(2. * std::acos(-1.));

    if (symmetryOrder <= 2)
        return std::sqrt((x * x) + (y * y));

    // First time only, create vector containing cosine and sine of relevant angles
    typedef std::vector< std::pair<float, float> > AngleVector;
    static AngleVector angleVector;

    if (angleVector.empty())
    {
        for (unsigned int iSymmetry = 0; iSymmetry < symmetryOrder; ++iSymmetry)
        {
            const float phi = phi0 + ((twoPi * static_cast<float>(iSymmetry)) / static_cast<float>(symmetryOrder));
            angleVector.push_back(std::make_pair(std::cos(phi), std::sin(phi)));
        }
    }

    // Calculate maximum radius using cached angles
    float maxRadius(0.);
    for (AngleVector::const_iterator iter = angleVector.begin(), iterEnd = angleVector.end(); iter != iterEnd; ++iter)
    {
        const float radius((x * iter->first) + (y * iter->second));

        if (radius > maxRadius)
            maxRadius = radius;
    }

    return maxRadius;
}

} // namespace pandora
