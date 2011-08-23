/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Monitoring/CaloHitMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the calo hit monitoring algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Monitoring/CaloHitMonitoringAlgorithm.h"

using namespace pandora;

StatusCode CaloHitMonitoringAlgorithm::Run()
{
    static unsigned int eventNumber = 1;
    const std::string eventNumberString(TypeToString(eventNumber));

    // Create histograms
    const std::string PseudoLayerHistName("PseudoLayer_" + eventNumberString);
    const std::string DensityWeightHistName("DensityWeight_" + eventNumberString);
    const std::string SurroundingEnergyHistName("SurroundingEnergy_" + eventNumberString);
    const std::string PossibleMipFlagHistName("PossibleMipFlag_" + eventNumberString);
    const std::string IsIsolatedFlagHistName("IsIsolatedFlag_" + eventNumberString);

    PANDORA_MONITORING_API(Create1DHistogram(PseudoLayerHistName, PseudoLayerHistName, 101, -0.5, 100.5));
    PANDORA_MONITORING_API(Create1DHistogram(DensityWeightHistName, DensityWeightHistName, 101, -0.5, 100.5));
    PANDORA_MONITORING_API(Create1DHistogram(SurroundingEnergyHistName, SurroundingEnergyHistName, 101, -0.95, 10.05));
    PANDORA_MONITORING_API(Create1DHistogram(PossibleMipFlagHistName, PossibleMipFlagHistName, 2, -0.5, 1.5));
    PANDORA_MONITORING_API(Create1DHistogram(IsIsolatedFlagHistName, IsIsolatedFlagHistName, 2, -0.5, 1.5));

    // Loop over current calo hit list and fill histograms
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentCaloHitList(*this, pCaloHitList));

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        PANDORA_MONITORING_API(Fill1DHistogram(PseudoLayerHistName, (*iter)->GetPseudoLayer()));
        PANDORA_MONITORING_API(Fill1DHistogram(DensityWeightHistName, (*iter)->GetDensityWeight()));
        PANDORA_MONITORING_API(Fill1DHistogram(SurroundingEnergyHistName, (*iter)->GetSurroundingEnergy()));
        PANDORA_MONITORING_API(Fill1DHistogram(PossibleMipFlagHistName, (*iter)->IsPossibleMip()));
        PANDORA_MONITORING_API(Fill1DHistogram(IsIsolatedFlagHistName, (*iter)->IsIsolated()));
    }

    // Append histograms to file
    PANDORA_MONITORING_API(SaveAndCloseHistogram(PseudoLayerHistName, m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(DensityWeightHistName, m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(SurroundingEnergyHistName, m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(PossibleMipFlagHistName, m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(IsIsolatedFlagHistName, m_monitoringFileName, "UPDATE"));

    ++eventNumber;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MonitoringFileName", m_monitoringFileName));

    return STATUS_CODE_SUCCESS;
}
