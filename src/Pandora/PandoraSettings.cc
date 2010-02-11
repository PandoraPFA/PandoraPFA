/**
 *    @file PandoraPFANew/src/Pandora/PandoraSettings.cc
 * 
 *    @brief Implementation of the pandora settings class.
 * 
 *  $Log: $
 */

#include "Algorithms/Algorithm.h"

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

        m_isMonitoringEnabled = false;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsMonitoringEnabled", m_isMonitoringEnabled));

        m_hadronicEnergyResolution = 0.6f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "HadronicEnergyResolution", m_hadronicEnergyResolution));

        if (0.f == m_hadronicEnergyResolution)
            return STATUS_CODE_INVALID_PARAMETER;

        m_mcPfoSelectionRadius = 300.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MCPfoSelectionRadius", m_mcPfoSelectionRadius));

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

        // Shower profile parameters
        m_showerProfileBinWidth = 0.5f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerProfileBinWidth", m_showerProfileBinWidth));

        if (0.f == m_showerProfileBinWidth)
            return STATUS_CODE_INVALID_PARAMETER;

        m_showerProfileNBins = 100;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerProfileNBins", m_showerProfileNBins));

        m_showerProfileMinCosAngle = 0.3f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerProfileMinCosAngle", m_showerProfileMinCosAngle));

        m_showerProfileCriticalEnergy = 0.08f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerProfileCriticalEnergy", m_showerProfileCriticalEnergy));

        m_showerProfileParameter0 = 1.25f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerProfileParameter0", m_showerProfileParameter0));

        m_showerProfileParameter1 = 0.5f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerProfileParameter1", m_showerProfileParameter1));

        m_showerProfileMaxDifference = 0.1f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShowerProfileMaxDifference", m_showerProfileMaxDifference));

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
