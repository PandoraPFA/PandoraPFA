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
    m_hadEnergyCorrectionFunctionMap.clear();
    m_emEnergyCorrectionFunctionMap.clear();
    m_particleIdFunctionMap.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::RegisterEnergyCorrectionFunction(const std::string &functionName, const EnergyCorrectionType energyCorrectionType,
    EnergyCorrectionFunction *pEnergyCorrectionFunction)
{
    EnergyCorrectionFunctionMap &energyCorrectionFunctionMap(this->GetEnergyCorrectionFunctionMap(energyCorrectionType));

    if (!energyCorrectionFunctionMap.insert(EnergyCorrectionFunctionMap::value_type(functionName, pEnergyCorrectionFunction)).second)
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

StatusCode PluginManager::AssignEnergyCorrectionFunctions(const StringVector &functionNames, const EnergyCorrectionType energyCorrectionType,
    EnergyCorrectionFunctionVector &energyCorrectionFunctionVector)
{
    EnergyCorrectionFunctionMap &energyCorrectionFunctionMap(this->GetEnergyCorrectionFunctionMap(energyCorrectionType));

    for (StringVector::const_iterator nameIter = functionNames.begin(), nameIterEnd = functionNames.end(); nameIter != nameIterEnd; ++nameIter)
    {
        EnergyCorrectionFunctionMap::const_iterator iter = energyCorrectionFunctionMap.find(*nameIter);

        if (energyCorrectionFunctionMap.end() == iter)
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

PluginManager::EnergyCorrectionFunctionMap &PluginManager::GetEnergyCorrectionFunctionMap(const EnergyCorrectionType energyCorrectionType)
{
    switch (energyCorrectionType)
    {
    case HADRONIC:
        return m_hadEnergyCorrectionFunctionMap;

    case ELECTROMAGNETIC:
        return m_emEnergyCorrectionFunctionMap;

    default:
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PluginManager::InitializePlugins(const TiXmlHandle *const pXmlHandle)
{
    // Energy correction functions
    StringVector hadEnergyCorrectionFunctionNames;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(*pXmlHandle,
        "HadronicEnergyCorrectionFunctions", hadEnergyCorrectionFunctionNames));

    if (!hadEnergyCorrectionFunctionNames.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignEnergyCorrectionFunctions(hadEnergyCorrectionFunctionNames,
            HADRONIC, EnergyCorrectionsHelper::m_hadEnergyCorrectionFunctions));
    }

    StringVector emEnergyCorrectionFunctionNames;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(*pXmlHandle,
        "ElectromagneticEnergyCorrectionFunctions", emEnergyCorrectionFunctionNames));

    if (!emEnergyCorrectionFunctionNames.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignEnergyCorrectionFunctions(emEnergyCorrectionFunctionNames,
            ELECTROMAGNETIC, EnergyCorrectionsHelper::m_emEnergyCorrectionFunctions));
    }

    // Particle id functions
    std::string photonFastFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFastFunction", photonFastFunctionName));

    if (!photonFastFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(photonFastFunctionName,
            ParticleIdHelper::m_pPhotonFastFunction));
    }

    std::string photonFullFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullFunction", photonFullFunctionName));

    if (!photonFullFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(photonFullFunctionName,
            ParticleIdHelper::m_pPhotonFullFunction));
    }

    std::string electronFastFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronFastFunction", electronFastFunctionName));

    if (!electronFastFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(electronFastFunctionName,
            ParticleIdHelper::m_pElectronFastFunction));
    }

    std::string electronFullFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronFullFunction", electronFullFunctionName));

    if (!electronFullFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(electronFullFunctionName,
            ParticleIdHelper::m_pElectronFullFunction));
    }

    std::string muonFastFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MuonFastFunction", muonFastFunctionName));

    if (!muonFastFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(muonFastFunctionName,
            ParticleIdHelper::m_pMuonFastFunction));
    }

    std::string muonFullFunctionName;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MuonFullFunction", muonFullFunctionName));

    if (!muonFullFunctionName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignParticleIdFunction(muonFullFunctionName,
            ParticleIdHelper::m_pMuonFullFunction));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
