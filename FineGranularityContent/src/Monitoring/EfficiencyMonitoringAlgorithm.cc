/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Monitoring/EfficiencyMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the efficiency monitoring algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Monitoring/EfficiencyMonitoringAlgorithm.h"

using namespace pandora;

EfficiencyMonitoringAlgorithm::~EfficiencyMonitoringAlgorithm()
{
    PANDORA_MONITORING_API(SaveTree("EffTree", m_monitoringFileName, "UPDATE"));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EfficiencyMonitoringAlgorithm::Run()
{
    // Extract the mc particle - this algorithm is intended to work only with single particle samples
    MCParticleList mcParticleList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetMCParticleList(*this, mcParticleList));

    if (1 != mcParticleList.size())
    {
        std::cout << "EfficiencyMonitoring - Algorithm works only with single particle samples, nParticles " << mcParticleList.size() << std::endl;
        return STATUS_CODE_SUCCESS;
    }

    const MCParticle *const pMCParticle(*(mcParticleList.begin()));

    // Extract the mc particle properties
    const float mcEnergy(pMCParticle->GetEnergy());
    const int mcPDGCode(pMCParticle->GetParticleId());

    if (mcEnergy < m_mcThresholdEnergy)
    {
        std::cout << "EfficiencyMonitoring - MC particle energy below threshold " << mcEnergy << "( < " << m_mcThresholdEnergy << ")" <<std::endl;
        return STATUS_CODE_SUCCESS;
    }

    float radius(0.f), phi(0.f), theta(0.f);
    const CartesianVector &mcPosition(pMCParticle->GetEndpoint());
    mcPosition.GetSphericalCoordinates(radius, phi, theta);

    // Extract the most energetic pfo
    const PfoList *pPfoList= NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pPfoList));

    PfoVector pfoVector(pPfoList->begin(), pPfoList->end());
    std::sort(pfoVector.begin(), pfoVector.end(), ParticleFlowObject::SortByEnergy);
    const ParticleFlowObject *const pMostEnergeticPfo((!pfoVector.empty()) ? *(pfoVector.begin()) : NULL);

    // Extract the pfo properties
    float recoEnergy((pMostEnergeticPfo != NULL) ? pMostEnergeticPfo->GetEnergy() : 0.f);
    int recoPDGCode((pMostEnergeticPfo != NULL) ? pMostEnergeticPfo->GetParticleId() : 0);

    // Look for specific case of photon conversion to e+e-
    int isPhotonConversion(0);

    if ((mcPDGCode == PHOTON) && (std::abs(recoPDGCode) == E_MINUS) && (pPfoList->size() == 2))
    {
        ParticleFlowObject *pPfo1(*(pPfoList->begin()));
        ParticleFlowObject *pPfo2(*(pPfoList->rbegin()));

        if ((pPfo1->GetParticleId() == -(pPfo2->GetParticleId())) && (std::abs(pPfo1->GetParticleId()) == E_MINUS))
        {
            recoPDGCode = PHOTON;
            recoEnergy = pPfo1->GetEnergy() + pPfo2->GetEnergy();
            isPhotonConversion = 1;
        }
    }

    // Fill tree with information for this single particle event
    PANDORA_MONITORING_API(SetTreeVariable("EffTree", "mcPDGCode", mcPDGCode));
    PANDORA_MONITORING_API(SetTreeVariable("EffTree", "recoPDGCode", recoPDGCode));
    PANDORA_MONITORING_API(SetTreeVariable("EffTree", "mcEnergy", mcEnergy));
    PANDORA_MONITORING_API(SetTreeVariable("EffTree", "recoEnergy", recoEnergy));
    PANDORA_MONITORING_API(SetTreeVariable("EffTree", "radius", radius));
    PANDORA_MONITORING_API(SetTreeVariable("EffTree", "phi", phi));
    PANDORA_MONITORING_API(SetTreeVariable("EffTree", "theta", theta));
    PANDORA_MONITORING_API(SetTreeVariable("EffTree", "isPhotonConversion", isPhotonConversion));
    PANDORA_MONITORING_API(FillTree("EffTree"));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EfficiencyMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MonitoringFileName", m_monitoringFileName));

    m_mcThresholdEnergy = 0.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MCThresholdEnergy", m_mcThresholdEnergy));

    return STATUS_CODE_SUCCESS;
}
