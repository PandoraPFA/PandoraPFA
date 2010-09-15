/**
 *    @file PandoraPFANew/src/Pandora/PandoraSettings.cc
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
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraSettings::Initialize(const TiXmlHandle *const pXmlHandle)
{
    try
    {
        if (m_isInitialized)
            return STATUS_CODE_ALREADY_INITIALIZED;

        // Calo hit helper settings
        TiXmlElement *pXmlElement = pXmlHandle->FirstChild("CaloHitHelper").Element();

        if (NULL != pXmlElement)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ReadSettings(TiXmlHandle(pXmlElement)));

        // Cluster helper settings
        pXmlElement = pXmlHandle->FirstChild("ClusterHelper").Element();

        if (NULL != pXmlElement)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ClusterHelper::ReadSettings(TiXmlHandle(pXmlElement)));

        // Energy corrections helper settings
        pXmlElement = pXmlHandle->FirstChild("EnergyCorrectionsHelper").Element();

        if (NULL != pXmlElement)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, EnergyCorrectionsHelper::ReadSettings(TiXmlHandle(pXmlElement)));

        // Fragment removal helper settings
        pXmlElement = pXmlHandle->FirstChild("FragmentRemovalHelper").Element();

        if (NULL != pXmlElement)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, FragmentRemovalHelper::ReadSettings(TiXmlHandle(pXmlElement)));

        // Geometry helper settings
        pXmlElement = pXmlHandle->FirstChild("GeometryHelper").Element();

        if (NULL != pXmlElement)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, GeometryHelper::ReadSettings(TiXmlHandle(pXmlElement)));

        // Particle id helper settings
        pXmlElement = pXmlHandle->FirstChild("ParticleIdHelper").Element();

        if (NULL != pXmlElement)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ParticleIdHelper::ReadSettings(TiXmlHandle(pXmlElement)));

        // Recluster helper settings
        pXmlElement = pXmlHandle->FirstChild("ReclusterHelper").Element();

        if (NULL != pXmlElement)
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, ReclusterHelper::ReadSettings(TiXmlHandle(pXmlElement)));

        // Global pandora settings
        m_isMonitoringEnabled = false;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "IsMonitoringEnabled", m_isMonitoringEnabled));

        m_shouldDisplayAlgorithmInfo = false;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShouldDisplayAlgorithmInfo", m_shouldDisplayAlgorithmInfo));

        m_electromagneticEnergyResolution = 0.2f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ElectromagneticEnergyResolution", m_electromagneticEnergyResolution));

        if (0.f == m_electromagneticEnergyResolution)
            return STATUS_CODE_INVALID_PARAMETER;

        m_hadronicEnergyResolution = 0.6f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "HadronicEnergyResolution", m_hadronicEnergyResolution));

        if (0.f == m_hadronicEnergyResolution)
            return STATUS_CODE_INVALID_PARAMETER;

        m_mcPfoSelectionRadius = 500.f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MCPfoSelectionRadius", m_mcPfoSelectionRadius));

        m_mcPfoSelectionMomentum = 0.01f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MCPfoSelectionMomentum", m_mcPfoSelectionMomentum));

        m_mcPfoSelectionLowEnergyNPCutOff = 1.2f;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "MCPfoSelectionProtonNeutronEnergyCutOff", m_mcPfoSelectionLowEnergyNPCutOff));

        m_shouldCollapseMCParticlesToPfoTarget = true;
        PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
            "ShouldCollapseMCParticlesToPfoTarget", m_shouldCollapseMCParticlesToPfoTarget));

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
