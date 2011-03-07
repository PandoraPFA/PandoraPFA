/**
 *  @file   PandoraPFANew/Framework/src/Utilities/FileReader.cc
 * 
 *  @brief  Implementation of the file reader class.
 * 
 *  $Log: $
 */

#include "Api/PandoraApi.h"

#include "Objects/CaloHit.h"
#include "Objects/Track.h"

#include "Utilities/FileReader.h"

namespace pandora
{

FileReader::FileReader(const pandora::Pandora &pandora, const std::string &fileName) :
    m_pPandora(&pandora),
    m_containerId(UNKNOWN_CONTAINER),
    m_containerPosition(0),
    m_containerSize(0)
{
    m_fileStream.open(fileName.c_str(), std::ios::in | std::ios::binary);

    if (!m_fileStream.is_open() || !m_fileStream.good())
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

FileReader::~FileReader()
{
    m_fileStream.close();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadGeometry()
{
    // What if not geometry?

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadGeometryParameters());

    ComponentId endOfGeometry(UNKNOWN_COMPONENT);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(endOfGeometry));

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadEvent()
{
    // What if not event?

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());

    try
    {
        while (STATUS_CODE_SUCCESS == this->ReadNextEventComponent())
            continue;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << " FileReader::ReadEvent() encountered unrecognized object in file: " << statusCodeException.ToString() << std::endl;
    }

    m_containerId = UNKNOWN_CONTAINER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToNextEvent()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());
    m_fileStream.seekg(m_containerPosition + m_containerSize, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::GoToEvent(const unsigned int eventNumber)
{
    unsigned int nEventsRead(0);
    m_fileStream.seekg(0, std::ios::beg);

    if (!m_fileStream.good())
        return STATUS_CODE_FAILURE;

    while (nEventsRead < eventNumber)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadHeader());
        m_fileStream.seekg(m_containerPosition + m_containerSize, std::ios::beg);

        if (!m_fileStream.good())
            return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadHeader()
{
    unsigned int fileHash;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(fileHash));

    if (pandoraFileHash != fileHash)
        return STATUS_CODE_FAILURE;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(m_containerId));

    if ((EVENT != m_containerId) && (GEOMETRY != m_containerId))
        return STATUS_CODE_FAILURE;

    m_containerPosition = m_fileStream.tellg();
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(m_containerSize));

    if (0 == m_containerSize)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadNextEventComponent()
{
    ComponentId componentId(UNKNOWN_COMPONENT);
    const StatusCode statusCode(this->ReadVariable(componentId));

    if (STATUS_CODE_SUCCESS != statusCode)
    {
        if (STATUS_CODE_NOT_FOUND != statusCode)
            throw StatusCodeException(statusCode);

        return STATUS_CODE_NOT_FOUND;
    }

    if (CALO_HIT == componentId)
    {
        return this->ReadCaloHit(false);
    }
    else if (TRACK == componentId)
    {
        return this->ReadTrack(false);
    }
    else if (EVENT_END == componentId)
    {
        m_containerId = UNKNOWN_CONTAINER;
        return STATUS_CODE_NOT_FOUND;
    }

    throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadGeometryParameters()
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    PandoraApi::Geometry::Parameters parameters;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(&(parameters.m_inDetBarrelParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(&(parameters.m_inDetEndCapParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(&(parameters.m_eCalBarrelParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(&(parameters.m_eCalEndCapParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(&(parameters.m_hCalBarrelParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(&(parameters.m_hCalEndCapParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(&(parameters.m_muonBarrelParameters)));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadSubDetector(&(parameters.m_muonEndCapParameters)));

    bool readMainTrackerDetails(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(readMainTrackerDetails));

    if (readMainTrackerDetails)
    {
        float mainTrackerInnerRadius(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mainTrackerInnerRadius));
        float mainTrackerOuterRadius(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mainTrackerOuterRadius));
        float mainTrackerZExtent(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mainTrackerZExtent));
        parameters.m_mainTrackerInnerRadius = mainTrackerInnerRadius;
        parameters.m_mainTrackerOuterRadius = mainTrackerOuterRadius;
        parameters.m_mainTrackerZExtent = mainTrackerZExtent;
    }

    bool readCoilDetails(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(readCoilDetails));

    if (readCoilDetails)
    {
        float coilInnerRadius(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(coilInnerRadius));
        float coilOuterRadius(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(coilOuterRadius));
        float coilZExtent(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(coilZExtent));
        parameters.m_coilInnerRadius = coilInnerRadius;
        parameters.m_coilOuterRadius = coilOuterRadius;
        parameters.m_coilZExtent = coilZExtent;
    }

    // Additional subdetectors

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Geometry::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadSubDetector(PandoraApi::GeometryParameters::SubDetectorParameters *pSubDetectorParameters, bool checkComponentId)
{
    if (GEOMETRY != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (SUB_DETECTOR != componentId)
            return STATUS_CODE_FAILURE;
    }

    bool isInitialized(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isInitialized));

    if (!isInitialized)
        return STATUS_CODE_SUCCESS;

    float innerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerRCoordinate));
    float innerZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerZCoordinate));
    float innerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerPhiCoordinate));
    unsigned int innerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(innerSymmetryOrder));
    float outerRCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerRCoordinate));
    float outerZCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerZCoordinate));
    float outerPhiCoordinate(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerPhiCoordinate));
    unsigned int outerSymmetryOrder(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(outerSymmetryOrder));
    bool isMirroredInZ(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isMirroredInZ));
    unsigned int nLayers(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nLayers));

    pSubDetectorParameters->m_innerRCoordinate = innerRCoordinate;
    pSubDetectorParameters->m_innerZCoordinate = innerZCoordinate;
    pSubDetectorParameters->m_innerPhiCoordinate = innerPhiCoordinate;
    pSubDetectorParameters->m_innerSymmetryOrder = innerSymmetryOrder;
    pSubDetectorParameters->m_outerRCoordinate = outerRCoordinate;
    pSubDetectorParameters->m_outerZCoordinate = outerZCoordinate;
    pSubDetectorParameters->m_outerPhiCoordinate = outerPhiCoordinate;
    pSubDetectorParameters->m_outerSymmetryOrder = outerSymmetryOrder;
    pSubDetectorParameters->m_isMirroredInZ = isMirroredInZ;
    pSubDetectorParameters->m_nLayers = nLayers;

    for (unsigned int iLayer = 0; iLayer < nLayers; ++iLayer)
    {
        float closestDistanceToIp(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(closestDistanceToIp));
        float nRadiationLengths(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nRadiationLengths));
        float nInteractionLengths(0.f);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nInteractionLengths));

        PandoraApi::Geometry::Parameters::LayerParameters layerParameters;
        layerParameters.m_closestDistanceToIp = closestDistanceToIp;
        layerParameters.m_nRadiationLengths = nRadiationLengths;
        layerParameters.m_nInteractionLengths = nInteractionLengths;
        pSubDetectorParameters->m_layerParametersList.push_back(layerParameters);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadCaloHit(bool checkComponentId)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (CALO_HIT != componentId)
            return STATUS_CODE_FAILURE;
    }

    CartesianVector positionVector(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(positionVector));
    CartesianVector expectedDirection(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(expectedDirection));
    CartesianVector cellNormalVector(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellNormalVector));
    float cellSizeU(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellSizeU));
    float cellSizeV(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellSizeV));
    float cellThickness(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(cellThickness));
    float nCellRadiationLengths(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nCellRadiationLengths));
    float nCellInteractionLengths(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nCellInteractionLengths));
    float nRadiationLengthsFromIp(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nRadiationLengthsFromIp));
    float nInteractionLengthsFromIp(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(nInteractionLengthsFromIp));
    float time(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(time));
    float inputEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(inputEnergy));
    float mipEquivalentEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mipEquivalentEnergy));
    float electromagneticEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(electromagneticEnergy));
    float hadronicEnergy(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(hadronicEnergy));
    bool isDigital(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isDigital));
    HitType hitType(ECAL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(hitType));
    DetectorRegion detectorRegion(BARREL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(detectorRegion));
    unsigned int layer(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(layer));
    bool isInOuterSamplingLayer(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isInOuterSamplingLayer));
    void *pParentAddress(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(pParentAddress));

    PandoraApi::CaloHit::Parameters parameters;
    parameters.m_positionVector = positionVector;
    parameters.m_expectedDirection = expectedDirection;
    parameters.m_cellNormalVector = cellNormalVector;
    parameters.m_cellSizeU = cellSizeU;
    parameters.m_cellSizeV = cellSizeV;
    parameters.m_cellThickness = cellThickness;
    parameters.m_nCellRadiationLengths = nCellRadiationLengths;
    parameters.m_nCellInteractionLengths = nCellInteractionLengths;
    parameters.m_nRadiationLengthsFromIp = nRadiationLengthsFromIp;
    parameters.m_nInteractionLengthsFromIp = nInteractionLengthsFromIp;
    parameters.m_time = time;
    parameters.m_inputEnergy = inputEnergy;
    parameters.m_mipEquivalentEnergy = mipEquivalentEnergy;
    parameters.m_electromagneticEnergy = electromagneticEnergy;
    parameters.m_hadronicEnergy = hadronicEnergy;
    parameters.m_isDigital = isDigital;
    parameters.m_hitType = hitType;
    parameters.m_detectorRegion = detectorRegion;
    parameters.m_layer = layer;
    parameters.m_isInOuterSamplingLayer = isInOuterSamplingLayer;
    parameters.m_pParentAddress = pParentAddress;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::CaloHit::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FileReader::ReadTrack(bool checkComponentId)
{
    if (EVENT != m_containerId)
        return STATUS_CODE_FAILURE;

    if (checkComponentId)
    {
        ComponentId componentId(UNKNOWN_COMPONENT);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(componentId));

        if (TRACK != componentId)
            return STATUS_CODE_FAILURE;
    }

    float d0(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(d0));
    float z0(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(z0));
    int particleId(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(particleId));
    int charge(0);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(charge));
    float mass(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(mass));
    CartesianVector momentumAtDca(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(momentumAtDca));
    TrackState trackStateAtStart(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtStart));
    TrackState trackStateAtEnd(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtEnd));
    TrackState trackStateAtCalorimeter(0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(trackStateAtCalorimeter));
    float timeAtCalorimeter(0.f);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(timeAtCalorimeter));
    bool reachesCalorimeter(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(reachesCalorimeter));
    bool isProjectedToEndCap(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(isProjectedToEndCap));
    bool canFormPfo(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(canFormPfo));
    bool canFormClusterlessPfo(false);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(canFormClusterlessPfo));
    void *pParentAddress(NULL);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadVariable(pParentAddress));

    PandoraApi::Track::Parameters parameters;
    parameters.m_d0 = d0;
    parameters.m_z0 = z0;
    parameters.m_particleId = particleId;
    parameters.m_charge = charge;
    parameters.m_mass = mass;
    parameters.m_momentumAtDca = momentumAtDca;
    parameters.m_trackStateAtStart = trackStateAtStart;
    parameters.m_trackStateAtEnd = trackStateAtEnd;
    parameters.m_trackStateAtCalorimeter = trackStateAtCalorimeter;
    parameters.m_timeAtCalorimeter = timeAtCalorimeter;
    parameters.m_reachesCalorimeter = reachesCalorimeter;
    parameters.m_isProjectedToEndCap = isProjectedToEndCap;
    parameters.m_canFormPfo = canFormPfo;
    parameters.m_canFormClusterlessPfo = canFormClusterlessPfo;
    parameters.m_pParentAddress = pParentAddress;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraApi::Track::Create(*m_pPandora, parameters));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
