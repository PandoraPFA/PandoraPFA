/**
 *  @file   PandoraPFANew/src/Managers/PluginManager.cc
 * 
 *  @brief  Implementation of the pandora plugin manager class.
 * 
 *  $Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Managers/PluginManager.h"

#include "Pandora/PandoraPlugins.h"

namespace pandora
{

PluginManager::PluginManager()
{
    PANDORA_REGISTER_ALL_PLUGINS();
}

//------------------------------------------------------------------------------------------------------------------------------------------

PluginManager::~PluginManager()
{
    m_energyCorrectionFunctionMap.clear();
    m_particleIdFunctionMap.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::RegisterEnergyCorrectionFunction(const std::string &functionName, EnergyCorrectionFunction *pEnergyCorrectionFunction)
{
    if (!m_energyCorrectionFunctionMap.insert(EnergyCorrectionFunctionMap::value_type(functionName, pEnergyCorrectionFunction)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::RegisterParticleIdFunction(const std::string &functionName, ParticleIdFunction *pParticleIdFunction)
{
    if (!m_particleIdFunctionMap.insert(ParticleIdFunctionMap::value_type(functionName, pParticleIdFunction)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::AssignEnergyCorrectionFunctions(const StringVector &functionNames, EnergyCorrectionFunctionVector &energyCorrectionFunctionVector) const
{
    for (StringVector::const_iterator nameIter = functionNames.begin(), nameIterEnd = functionNames.end(); nameIter != nameIterEnd; ++nameIter)
    {
        EnergyCorrectionFunctionMap::const_iterator iter = m_energyCorrectionFunctionMap.find(*nameIter);

        if (m_energyCorrectionFunctionMap.end() == iter)
            return STATUS_CODE_NOT_FOUND;

        energyCorrectionFunctionVector.push_back(iter->second);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::AssignParticleIdFunction(const std::string &functionName, ParticleIdFunction *&pParticleIdFunction) const
{
    ParticleIdFunctionMap::const_iterator iter = m_particleIdFunctionMap.find(functionName);

    if (m_particleIdFunctionMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    pParticleIdFunction = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::InitializePlugins(const TiXmlHandle *const pXmlHandle)
{
    StringVector energyCorrectionFunctionNames;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(*pXmlHandle,
        "EnergyCorrectionFunctionNames", energyCorrectionFunctionNames));

    if (!energyCorrectionFunctionNames.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignEnergyCorrectionFunctions(energyCorrectionFunctionNames,
            EnergyCorrectionsHelper::m_energyCorrectionFunctionVector));
    }

    std::string photonFastFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFastFunctionName", photonFastFunctionName));

    if (!photonFastFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(photonFastFunctionName,
            ParticleIdHelper::m_pPhotonFastFunction));
    }

    std::string photonFullFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullFunctionName", photonFullFunctionName));

    if (!photonFullFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(photonFullFunctionName,
            ParticleIdHelper::m_pPhotonFullFunction));
    }

    std::string electronFastFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronFastFunctionName", electronFastFunctionName));

    if (!electronFastFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(electronFastFunctionName,
            ParticleIdHelper::m_pElectronFastFunction));
    }

    std::string electronFullFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronFullFunctionName", electronFullFunctionName));

    if (!electronFullFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(electronFullFunctionName,
            ParticleIdHelper::m_pElectronFullFunction));
    }

    std::string muonFastFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MuonFastFunctionName", muonFastFunctionName));

    if (!muonFastFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(muonFastFunctionName,
            ParticleIdHelper::m_pMuonFastFunction));
    }

    std::string muonFullFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MuonFullFunctionName", muonFullFunctionName));

    if (!muonFullFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(muonFullFunctionName,
            ParticleIdHelper::m_pMuonFullFunction));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
