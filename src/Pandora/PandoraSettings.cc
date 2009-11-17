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

        m_mcPfoSelectionRadius = 300.;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MCPfoSelectionRadius", m_mcPfoSelectionRadius));

        m_caloHitMaxSeparation = 100;
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

        m_isolationDensityWeightCutECal = 0.5;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationDensityWeightCutECal", m_isolationDensityWeightCutECal));

        m_isolationDensityWeightCutHCal = 0.25;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationDensityWeightCutHCal", m_isolationDensityWeightCutHCal));

        m_isolationNLayers = 2;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationNLayers", m_isolationNLayers));

        m_isolationCutDistanceECal = 50;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationCutDistanceECal", m_isolationCutDistanceECal));

        m_isolationCutDistanceHCal = 250;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationCutDistanceHCal", m_isolationCutDistanceHCal));

        m_isolationMaxNearbyHits = 2;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsolationMaxNearbyHits", m_isolationMaxNearbyHits));

        m_mipLikeMipCut = 5;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MipLikeMipCut", m_mipLikeMipCut));

        m_mipNCellsForNearbyHit = 1;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MipNCellsForNearbyHit", m_mipNCellsForNearbyHit));

        m_mipMaxNearbyHits = 1;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MipMaxNearbyHits", m_mipMaxNearbyHits));

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
