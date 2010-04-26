/**
 *  @file   PandoraPFANew/src/Helpers/EnergyCorrectionsHelper.cc
 * 
 *  @brief  Implementation of the energy corrections helper class.
 * 
 *  $Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Helpers/EnergyCorrectionsHelper.h"

#include "Objects/Cluster.h"

namespace pandora
{

StatusCode EnergyCorrectionsHelper::EnergyCorrection(Cluster *const pCluster, float &correctedElectromagneticEnergy,
    float &correctedHadronicEnergy)
{
    correctedElectromagneticEnergy = pCluster->GetElectromagneticEnergy();
    correctedHadronicEnergy = pCluster->GetHadronicEnergy();

    // TODO EnergyCorrectionsHelper will later redirect to user-configured energy correction functions
    EnergyCorrectionsHelper::CleanCluster(pCluster, correctedHadronicEnergy);
    EnergyCorrectionsHelper::ScaleHotHadronEnergy(pCluster, correctedHadronicEnergy);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void EnergyCorrectionsHelper::CleanCluster(Cluster *const pCluster, float &correctedHadronicEnergy)
{
    static const PseudoLayer firstPseudoLayer(TRACK_PROJECTION_LAYER + 1);

    const float clusterHadronicEnergy(pCluster->GetHadronicEnergy());

    if (0. == clusterHadronicEnergy)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    bool isOutsideECal(false);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    // Loop over all constituent ecal hits, looking for anomalies
    for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end();
        (layerIter != layerIterEnd) && !isOutsideECal; ++layerIter)
    {
        const PseudoLayer pseudoLayer(layerIter->first);

        for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end();
            hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            if (ECAL != pCaloHit->GetHitType())
            {
                isOutsideECal = true;
                break;
            }

            const float hitHadronicEnergy(pCaloHit->GetHadronicEnergy());

            if ((hitHadronicEnergy > m_minCleanHitEnergy) && (hitHadronicEnergy / clusterHadronicEnergy > m_minCleanHitEnergyFraction))
            {
                // Calculate new energy from surrounding layers
                float energyInPreviousLayer(0.);

                if (pseudoLayer > firstPseudoLayer)
                    energyInPreviousLayer = EnergyCorrectionsHelper::GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer - 1);

                float energyInNextLayer(0.);

                if (pseudoLayer < std::numeric_limits<unsigned int>::max())
                    energyInNextLayer = EnergyCorrectionsHelper::GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer + 1);

                const float energyInCurrentLayer = EnergyCorrectionsHelper::GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer);

                // Calculate new energy estimate for hit and update cluster best energy estimate
                float energyInAdjacentLayers(energyInPreviousLayer + energyInNextLayer);

                if (pseudoLayer > firstPseudoLayer)
                    energyInAdjacentLayers /= 2.f;

                float newHitHadronicEnergy(energyInAdjacentLayers - energyInCurrentLayer + hitHadronicEnergy);
                newHitHadronicEnergy = std::max(newHitHadronicEnergy, m_minCleanCorrectedHitEnergy);

                if (newHitHadronicEnergy < hitHadronicEnergy)
                    correctedHadronicEnergy += newHitHadronicEnergy - hitHadronicEnergy;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void EnergyCorrectionsHelper::ScaleHotHadronEnergy(Cluster *const pCluster, float &correctedHadronicEnergy)
{
    const unsigned int nHitsInCluster(pCluster->GetNCaloHits());

    // Initial hot hadron cuts
    if ((nHitsInCluster < m_minHitsForHotHadron) || (nHitsInCluster > m_maxHitsForHotHadron))
        return;

    if ((pCluster->GetInnerPseudoLayer() < m_hotHadronInnerLayerCut) && (pCluster->GetMipFraction() < m_hotHadronMipFractionCut) &&
        (nHitsInCluster > m_hotHadronNHitsCut))
    {
        return;
    }

    // Finally, check the number of mips per hit
    float clusterMipEnergy(0.);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end();
        layerIter != layerIterEnd; ++layerIter)
    {
        for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end();
            hitIter != hitIterEnd; ++hitIter)
        {
            clusterMipEnergy += (*hitIter)->GetMipEquivalentEnergy();
        }
    }

    const float meanMipsPerHit(clusterMipEnergy / static_cast<float>(nHitsInCluster));

    if ((meanMipsPerHit > 0.f) && (meanMipsPerHit > m_hotHadronMipsPerHit))
        correctedHadronicEnergy *= m_scaledHotHadronMipsPerHit / meanMipsPerHit;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float EnergyCorrectionsHelper::GetHadronicEnergyInLayer(const OrderedCaloHitList &orderedCaloHitList, const PseudoLayer pseudoLayer)
{
    OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(pseudoLayer);

    float hadronicEnergy(0.);

    if (iter != orderedCaloHitList.end())
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            hadronicEnergy += (*hitIter)->GetHadronicEnergy();
        }
    }

    return hadronicEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

// Parameter default values
bool EnergyCorrectionsHelper::m_shouldCleanClusters = true;
bool EnergyCorrectionsHelper::m_shouldScaleHotHadrons = true;
float EnergyCorrectionsHelper::m_minCleanHitEnergy = 1.f;
float EnergyCorrectionsHelper::m_minCleanHitEnergyFraction = 0.2f;
float EnergyCorrectionsHelper::m_minCleanCorrectedHitEnergy = 0.2f;

unsigned int EnergyCorrectionsHelper::m_minHitsForHotHadron = 5;
unsigned int EnergyCorrectionsHelper::m_maxHitsForHotHadron = 100;
unsigned int EnergyCorrectionsHelper::m_hotHadronInnerLayerCut = 10;
float EnergyCorrectionsHelper::m_hotHadronMipFractionCut = 0.4f;
unsigned int EnergyCorrectionsHelper::m_hotHadronNHitsCut = 50;
float EnergyCorrectionsHelper::m_hotHadronMipsPerHit = 15.f;
float EnergyCorrectionsHelper::m_scaledHotHadronMipsPerHit = 5.f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyCorrectionsHelper::ReadSettings(const TiXmlHandle *const pXmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShouldCleanClusters", m_shouldCleanClusters));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShouldScaleHotHadrons", m_shouldScaleHotHadrons));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MinCleanHitEnergy", m_minCleanHitEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MinCleanHitEnergyFraction", m_minCleanHitEnergyFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MinCleanCorrectedHitEnergy", m_minCleanCorrectedHitEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MinHitsForHotHadron", m_minHitsForHotHadron));

    if (0 == m_minHitsForHotHadron)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "MaxHitsForHotHadron", m_maxHitsForHotHadron));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "HotHadronInnerLayerCut", m_hotHadronInnerLayerCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "HotHadronMipFractionCut", m_hotHadronMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "HotHadronNHitsCut", m_hotHadronNHitsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "HotHadronMipsPerHit", m_hotHadronMipsPerHit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ScaledHotHadronMipsPerHit", m_scaledHotHadronMipsPerHit));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
