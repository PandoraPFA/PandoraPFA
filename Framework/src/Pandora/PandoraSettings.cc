/**
 *    @file PandoraPFANew/Framework/src/Pandora/PandoraSettings.cc
 * 
 *    @brief Implementation of the pandora settings class.
 * 
 *  $Log: $
 */

#include "Helpers/CaloHitHelper.h"
#include "Helpers/ClusterHelper.h"
#include "Helpers/EnergyCorrectionsHelper.h"
#include "Helpers/FragmentRemovalHelper.h"
#include "Helpers/GeometryHelper.h"
#include "Helpers/ParticleIdHelper.h"
#include "Helpers/ReclusterHelper.h"
#include "Helpers/XmlHelper.h"

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
    m_instanceFlag = false;
    m_settingsFunctionToNameMap.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::RegisterSettingsFunction(const std::string &xmlTagName, SettingsFunction *pSettingsFunction)
{
    if (!m_settingsFunctionToNameMap.insert(SettingsFunctionToNameMap::value_type(pSettingsFunction, xmlTagName)).second)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::Initialize(const TiXmlHandle *const pXmlHandle)
{
    try
    {
        if (m_isInitialized)
            return STATUS_CODE_ALREADY_INITIALIZED;

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ClusterHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, EnergyCorrectionsHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, FragmentRemovalHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, GeometryHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ParticleIdHelper::ReadSettings(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ReclusterHelper::ReadSettings(pXmlHandle));

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RunRegisteredSettingsFunctions(pXmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ReadGlobalSettings(pXmlHandle));

        m_isInitialized = true;

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to initialize pandora settings: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::RunRegisteredSettingsFunctions(const TiXmlHandle *const pXmlHandle) const
{
    for (SettingsFunctionToNameMap::const_iterator iter = m_settingsFunctionToNameMap.begin(), iterEnd = m_settingsFunctionToNameMap.end();
        iter != iterEnd; ++iter)
    {
        TiXmlElement *pXmlElement = pXmlHandle->FirstChild(iter->second).Element();

        if (NULL != pXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*(iter->first))(TiXmlHandle(pXmlElement)));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::ReadGlobalSettings(const TiXmlHandle *const pXmlHandle)
{
    m_isMonitoringEnabled = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "IsMonitoringEnabled", m_isMonitoringEnabled));

    m_shouldDisplayAlgorithmInfo = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShouldDisplayAlgorithmInfo", m_shouldDisplayAlgorithmInfo));

    m_electromagneticEnergyResolution = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectromagneticEnergyResolution", m_electromagneticEnergyResolution));

    if (0.f == m_electromagneticEnergyResolution)
        return STATUS_CODE_INVALID_PARAMETER;

    m_hadronicEnergyResolution = 0.6f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "HadronicEnergyResolution", m_hadronicEnergyResolution));

    if (0.f == m_hadronicEnergyResolution)
        return STATUS_CODE_INVALID_PARAMETER;

    m_mcPfoSelectionRadius = 500.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MCPfoSelectionRadius", m_mcPfoSelectionRadius));

    m_mcPfoSelectionMomentum = 0.01f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MCPfoSelectionMomentum", m_mcPfoSelectionMomentum));

    m_mcPfoSelectionLowEnergyNPCutOff = 1.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MCPfoSelectionProtonNeutronEnergyCutOff", m_mcPfoSelectionLowEnergyNPCutOff));

    m_shouldCollapseMCParticlesToPfoTarget = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShouldCollapseMCParticlesToPfoTarget", m_shouldCollapseMCParticlesToPfoTarget));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
