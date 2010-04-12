/**
 *    @file PandoraPFANew/src/Pandora/PandoraSettings.cc
 * 
 *    @brief Implementation of the pandora settings class.
 * 
 *  $Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Helpers/ParticleIdHelper.h"

#include "Pandora/PandoraSettings.h"

namespace pandora
{

bool PandoraSettings::m_instanceFlag = false;

PandoraSettings* PandoraSettings::m_pPandoraSettings = NULL;

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraSettings *PandoraSettings::GetInstance()
{
    if(!m_instanceFlag)
    {
        m_pPandoraSettings = new PandoraSettings();
        m_instanceFlag = true;
    }

    return m_pPandoraSettings;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraSettings::PandoraSettings() :
    m_isInitialized(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraSettings::~PandoraSettings()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::Initialize(const TiXmlHandle *const pXmlHandle)
{
    try
    {
        if (m_isInitialized)
            return STATUS_CODE_ALREADY_INITIALIZED;

        // Helper function settings
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ParticleIdHelper::ReadSettings(pXmlHandle));

        // Global pandora settings
        m_isMonitoringEnabled = false;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsMonitoringEnabled", m_isMonitoringEnabled));

        m_hadronicEnergyResolution = 0.6f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "HadronicEnergyResolution", m_hadronicEnergyResolution));

        if (0.f == m_hadronicEnergyResolution)
            return STATUS_CODE_INVALID_PARAMETER;

        m_caloHitMaxSeparation = 100.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "CaloHitMaxSeparation", m_caloHitMaxSeparation));

        m_densityWeightPower = 2;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "DensityWeightPower", m_densityWeightPower));

        m_densityWeightNLayers = 2;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "DensityWeightNLayers", m_densityWeightNLayers));

        m_shouldUseSimpleIsolationScheme = true;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShouldUseSimpleIsolationScheme", m_shouldUseSimpleIsolationScheme));

        m_isolationDensityWeightCutECal = 0.5f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationDensityWeightCutECal", m_isolationDensityWeightCutECal));

        m_isolationDensityWeightCutHCal = 0.25f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationDensityWeightCutHCal", m_isolationDensityWeightCutHCal));

        m_isolationNLayers = 2;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationNLayers", m_isolationNLayers));

        m_isolationCutDistanceECal = 50.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationCutDistanceECal", m_isolationCutDistanceECal));

        m_isolationCutDistanceHCal = 250.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationCutDistanceHCal", m_isolationCutDistanceHCal));

        m_isolationMaxNearbyHits = 2;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationMaxNearbyHits", m_isolationMaxNearbyHits));

        m_mipLikeMipCut = 5.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MipLikeMipCut", m_mipLikeMipCut));

        m_mipNCellsForNearbyHit = 1;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MipNCellsForNearbyHit", m_mipNCellsForNearbyHit));

        m_mipMaxNearbyHits = 1;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MipMaxNearbyHits", m_mipMaxNearbyHits));

        // Cluster contact parameters
        m_contactConeCosineHalfAngle1 = 0.9f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactConeCosineHalfAngle1", m_contactConeCosineHalfAngle1));

        m_contactConeCosineHalfAngle2 = 0.95f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactConeCosineHalfAngle2", m_contactConeCosineHalfAngle2));

        m_contactConeCosineHalfAngle3 = 0.985f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactConeCosineHalfAngle3", m_contactConeCosineHalfAngle3));

        m_contactCloseHitDistance1 = 100.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactCloseHitDistance1", m_contactCloseHitDistance1));

        m_contactCloseHitDistance2 = 50.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactCloseHitDistance2", m_contactCloseHitDistance2));

        m_contactDistanceThreshold = 2.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactDistanceThreshold", m_contactDistanceThreshold));

        m_contactHelixComparisonMipFractionCut = 0.8f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactHelixComparisonMipFractionCut", m_contactHelixComparisonMipFractionCut));

        m_contactHelixComparisonStartOffset = 20;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactHelixComparisonStartOffset", m_contactHelixComparisonStartOffset));

        m_contactHelixComparisonStartOffsetMip = 20;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactHelixComparisonStartOffsetMip", m_contactHelixComparisonStartOffsetMip));

        m_contactNHelixComparisonLayers = 9;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactNHelixComparisonLayers", m_contactNHelixComparisonLayers));

        m_contactMaxLayersCrossedByHelix = 100;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactMaxLayersCrossedByHelix", m_contactMaxLayersCrossedByHelix));

        m_contactMaxTrackClusterDeltaZ = 250.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ContactMaxTrackClusterDeltaZ", m_contactMaxTrackClusterDeltaZ));

        // Shower start parameters
        m_showerStartMipFraction = 0.8f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerStartMipFraction", m_showerStartMipFraction));

        m_showerStartNonMipLayers = 2;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerStartNonMipLayers", m_showerStartNonMipLayers));

        m_leavingNOuterLayersToExamine = 4;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "LeavingNOuterLayersToExamine", m_leavingNOuterLayersToExamine));

        m_leavingMipLikeNOccupiedLayers = 4;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "LeavingMipLikeNOccupiedLayers", m_leavingMipLikeNOccupiedLayers));

        m_leavingShowerLikeNOccupiedLayers = 3;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "LeavingShowerLikeNOccupiedLayers", m_leavingShowerLikeNOccupiedLayers));

        m_leavingShowerLikeEnergyInOuterLayers = 1.0f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "LeavingShowerLikeEnergyInOuterLayers", m_leavingShowerLikeEnergyInOuterLayers));

        m_mcPfoSelectionRadius = 500.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MCPfoSelectionRadius", m_mcPfoSelectionRadius));

        m_mcPfoSelectionMomentum = 0.01;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MCPfoSelectionMomentum", m_mcPfoSelectionMomentum));

        m_mcPfoSelectionLowEnergyNPCutOff = 1.2;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MCPfoSelectionProtonNeutronEnergyCutOff", m_mcPfoSelectionLowEnergyNPCutOff));

        m_isInitialized = true;

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to initialize pandora settings: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

} // namespace pandora
