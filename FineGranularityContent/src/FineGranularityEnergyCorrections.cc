/**
 *  @file   PandoraPFANew/FineGranularityContent/src/FineGranularityEnergyCorrections.cc
 * 
 *  @brief  Implementation of the fine granularity energy corrections class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"
#include "Helpers/ReclusterHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include "FineGranularityEnergyCorrections.h"

using namespace pandora;

void FineGranularityEnergyCorrections::CleanCluster(const Cluster *const pCluster, float &correctedHadronicEnergy)
{
    static const PseudoLayer firstPseudoLayer(GeometryHelper::GetPseudoLayerAtIp());

    const float clusterHadronicEnergy(pCluster->GetHadronicEnergy());

    if (0. == clusterHadronicEnergy)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    bool isFineGranularity(true);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    // Loop over all constituent inner layer fine granularity hits, looking for anomalies
    for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end();
        (layerIter != layerIterEnd) && isFineGranularity; ++layerIter)
    {
        const PseudoLayer pseudoLayer(layerIter->first);

        for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end();
            hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            if (GeometryHelper::GetHitTypeGranularity((*hitIter)->GetHitType()) > FINE)
            {
                isFineGranularity = false;
                break;
            }

            const float hitHadronicEnergy(pCaloHit->GetHadronicEnergy());

            if ((hitHadronicEnergy > m_minCleanHitEnergy) && (hitHadronicEnergy / clusterHadronicEnergy > m_minCleanHitEnergyFraction))
            {
                // Calculate new energy from surrounding layers
                float energyInPreviousLayer(0.);

                if (pseudoLayer > firstPseudoLayer)
                    energyInPreviousLayer = FineGranularityEnergyCorrections::GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer - 1);

                float energyInNextLayer(0.);

                if (pseudoLayer < std::numeric_limits<unsigned int>::max())
                    energyInNextLayer = FineGranularityEnergyCorrections::GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer + 1);

                const float energyInCurrentLayer = FineGranularityEnergyCorrections::GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer);

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

void FineGranularityEnergyCorrections::ScaleHotHadronEnergy(const Cluster *const pCluster, float &correctedHadronicEnergy)
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

void FineGranularityEnergyCorrections::ApplyMuonEnergyCorrection(const Cluster *const pCluster, float &correctedHadronicEnergy)
{
    bool containsMuonHit(false);
    unsigned int nMuonHitsInInnerLayer(0);
    PseudoLayer muonInnerLayer(std::numeric_limits<unsigned int>::max());

    // Extract muon-based properties from the cluster
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if ((*hitIter)->GetHitType() == MUON)
            {
                containsMuonHit = true;
                ++nMuonHitsInInnerLayer;
            }
        }

        if (containsMuonHit)
        {
            muonInnerLayer = iter->first;
            break;
        }
    }

    if (!containsMuonHit)
        return;

    // Check whether energy deposits are likely to have been lost in coil region
    const CartesianVector muonInnerLayerCentroid(pCluster->GetCentroid(muonInnerLayer));
    const float centroidX(muonInnerLayerCentroid.GetX()), centroidY(muonInnerLayerCentroid.GetY());

    const float muonInnerLayerRadius(std::sqrt(centroidX * centroidX + centroidY * centroidY));
    static const float coilInnerRadius(GeometryHelper::GetCoilInnerRadius());

    if (muonInnerLayerRadius < coilInnerRadius)
        return;

    const TrackList &trackList(pCluster->GetAssociatedTrackList());

    if (pCluster->GetInnerPseudoLayer() == muonInnerLayer)
    {
        // Energy correction for standalone muon cluster
        correctedHadronicEnergy += m_muonHitEnergy * static_cast<float>(nMuonHitsInInnerLayer);
    }
    else if (trackList.empty())
    {
        // Energy correction for neutral hadron cluster spilling into coil and muon detectors
        correctedHadronicEnergy += m_coilEnergyLossCorrection;
    }
    else
    {
        // Energy correction for charged hadron cluster spilling into coil and muon detectors
        if (nMuonHitsInInnerLayer < m_minMuonHitsInInnerLayer)
            return;

        float trackEnergySum(0.);

        for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
        {
            trackEnergySum += (*iter)->GetEnergyAtDca();
        }

        const float oldChi(ReclusterHelper::GetTrackClusterCompatibility(correctedHadronicEnergy, trackEnergySum));
        const float newChi(ReclusterHelper::GetTrackClusterCompatibility(correctedHadronicEnergy + m_coilEnergyLossCorrection, trackEnergySum));

        if ((oldChi < m_coilEnergyCorrectionChi) && (std::fabs(newChi) < std::fabs(oldChi)))
        {
            correctedHadronicEnergy += m_coilEnergyLossCorrection;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float FineGranularityEnergyCorrections::GetHadronicEnergyInLayer(const OrderedCaloHitList &orderedCaloHitList, const PseudoLayer pseudoLayer)
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
bool FineGranularityEnergyCorrections::m_shouldCleanClusters = true;
bool FineGranularityEnergyCorrections::m_shouldScaleHotHadrons = true;
float FineGranularityEnergyCorrections::m_minCleanHitEnergy = 1.f;
float FineGranularityEnergyCorrections::m_minCleanHitEnergyFraction = 0.2f;
float FineGranularityEnergyCorrections::m_minCleanCorrectedHitEnergy = 0.2f;

unsigned int FineGranularityEnergyCorrections::m_minHitsForHotHadron = 5;
unsigned int FineGranularityEnergyCorrections::m_maxHitsForHotHadron = 100;
unsigned int FineGranularityEnergyCorrections::m_hotHadronInnerLayerCut = 10;
float FineGranularityEnergyCorrections::m_hotHadronMipFractionCut = 0.4f;
unsigned int FineGranularityEnergyCorrections::m_hotHadronNHitsCut = 50;
float FineGranularityEnergyCorrections::m_hotHadronMipsPerHit = 15.f;
float FineGranularityEnergyCorrections::m_scaledHotHadronMipsPerHit = 5.f;

float FineGranularityEnergyCorrections::m_muonHitEnergy = 0.5f;
float FineGranularityEnergyCorrections::m_coilEnergyLossCorrection = 10.f;
unsigned int FineGranularityEnergyCorrections::m_minMuonHitsInInnerLayer = 3;
float FineGranularityEnergyCorrections::m_coilEnergyCorrectionChi = 3.f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FineGranularityEnergyCorrections::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldCleanClusters", m_shouldCleanClusters));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldScaleHotHadrons", m_shouldScaleHotHadrons));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCleanHitEnergy", m_minCleanHitEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCleanHitEnergyFraction", m_minCleanHitEnergyFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCleanCorrectedHitEnergy", m_minCleanCorrectedHitEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsForHotHadron", m_minHitsForHotHadron));

    if (0 == m_minHitsForHotHadron)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxHitsForHotHadron", m_maxHitsForHotHadron));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HotHadronInnerLayerCut", m_hotHadronInnerLayerCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HotHadronMipFractionCut", m_hotHadronMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HotHadronNHitsCut", m_hotHadronNHitsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HotHadronMipsPerHit", m_hotHadronMipsPerHit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ScaledHotHadronMipsPerHit", m_scaledHotHadronMipsPerHit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonHitEnergy", m_muonHitEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CoilEnergyLossCorrection", m_coilEnergyLossCorrection));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinMuonHitsInInnerLayer", m_minMuonHitsInInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CoilEnergyCorrectionChi", m_coilEnergyCorrectionChi));

    return STATUS_CODE_SUCCESS;
}
