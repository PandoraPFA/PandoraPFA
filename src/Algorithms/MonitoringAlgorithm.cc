/**
 *  @file   PandoraPFANew/src/Algorithms/MonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the monitoring algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/MonitoringAlgorithm.h"

using namespace pandora;

StatusCode MonitoringAlgorithm::Run()
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
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    for (OrderedCaloHitList::const_iterator iter = pOrderedCaloHitList->begin(), iterEnd = pOrderedCaloHitList->end();
        iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator caloHitIter = iter->second->begin(), caloHitIterEnd = iter->second->end();
            caloHitIter != caloHitIterEnd; ++caloHitIter)
        {
            PANDORA_MONITORING_API(Fill1DHistogram(PseudoLayerHistName, (*caloHitIter)->GetPseudoLayer()));
            PANDORA_MONITORING_API(Fill1DHistogram(DensityWeightHistName, (*caloHitIter)->GetDensityWeight()));
            PANDORA_MONITORING_API(Fill1DHistogram(SurroundingEnergyHistName, (*caloHitIter)->GetSurroundingEnergy()));
            PANDORA_MONITORING_API(Fill1DHistogram(PossibleMipFlagHistName, (*caloHitIter)->IsPossibleMip()));
            PANDORA_MONITORING_API(Fill1DHistogram(IsIsolatedFlagHistName, (*caloHitIter)->IsIsolated()));
        }
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

StatusCode MonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "MonitoringFileName", m_monitoringFileName));

    return STATUS_CODE_SUCCESS;
}
