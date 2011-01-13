/**
 *  @file   PandoraPFANew/FineGranularityContent/src/FineGranularityParticleId.cc
 * 
 *  @brief  Implementation of the fine granularity particle id class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include "FineGranularityParticleId.h"

#include <algorithm>
#include <cmath>
#include <limits>

using namespace pandora;

bool FineGranularityParticleId::FineGranularityEmShowerId(const Cluster *const pCluster)
{
    // Reject clusters starting outside inner fine granularity detectors
    if (GeometryHelper::GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) > FINE)
        return false;

    // Cut on cluster mip fraction
    const float totalElectromagneticEnergy(pCluster->GetElectromagneticEnergy() - pCluster->GetIsolatedElectromagneticEnergy());

    float mipCut(m_emShowerIdMipCut_0);

    if (totalElectromagneticEnergy > m_emShowerIdMipCutEnergy_1)
    {
        mipCut = m_emShowerIdMipCut_1;
    }
    else if (totalElectromagneticEnergy > m_emShowerIdMipCutEnergy_2)
    {
        mipCut = m_emShowerIdMipCut_2;
    }
    else if (totalElectromagneticEnergy > m_emShowerIdMipCutEnergy_3)
    {
        mipCut = m_emShowerIdMipCut_3;
    }
    else if (totalElectromagneticEnergy > m_emShowerIdMipCutEnergy_4)
    {
        mipCut = m_emShowerIdMipCut_4;
    }

    if (pCluster->GetMipFraction() > mipCut)
        return false;

    // Cut on results of fit to all hits in cluster
    float dCosR(0.f);
    float clusterRms(0.f);

    const ClusterHelper::ClusterFitResult &clusterFitResult(pCluster->GetFitToAllHitsResult());

    if (clusterFitResult.IsFitSuccessful())
    {
        dCosR = clusterFitResult.GetRadialDirectionCosine();
        clusterRms = clusterFitResult.GetRms();
    }

    const float dCosRCut(totalElectromagneticEnergy < m_emShowerIdDCosRCutEnergy ? m_emShowerIdDCosRLowECut : m_emShowerIdDCosRHighECut);

    if (dCosR < dCosRCut)
        return false;

    const float rmsCut(totalElectromagneticEnergy < m_emShowerIdRmsCutEnergy ? m_emShowerIdRmsLowECut : m_emShowerIdRmsHighECut);

    if (clusterRms > rmsCut)
        return false;

    const CartesianVector &clusterDirection(pCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pCluster->GetFitToAllHitsResult().GetDirection() : pCluster->GetInitialDirection());

    const CartesianVector &clusterIntercept(pCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pCluster->GetFitToAllHitsResult().GetIntercept() : CartesianVector(0.f, 0.f, 0.f));

    // Calculate properties of longitudinal shower profile: layer90 and shower max layer
    bool foundLayer90(false);
    float layer90EnergySum(0.f), showerMaxRadLengths(0.f), energyAboveHighRadLengths(0.f);
    float nRadiationLengths(0.f), nRadiationLengthsInLastLayer(0.f), maxEnergyInlayer(0.f);
    HitEnergyDistanceVector hitEnergyDistanceVector;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    const PseudoLayer innerPseudoLayer(pCluster->GetInnerPseudoLayer());
    static const PseudoLayer firstPseudoLayer(GeometryHelper::GetPseudoLayerAtIp());

    for (PseudoLayer iLayer = innerPseudoLayer, outerPseudoLayer = pCluster->GetOuterPseudoLayer(); iLayer <= outerPseudoLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if ((orderedCaloHitList.end() == iter) || (iter->second->empty()))
        {
            nRadiationLengths += nRadiationLengthsInLastLayer;
            continue;
        }

        // Extract information from the calo hits
        float energyInLayer(0.f);
        float nRadiationLengthsInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            float cosOpeningAngle(std::fabs((*hitIter)->GetCellNormalVector().GetCosOpeningAngle(clusterDirection)));
            cosOpeningAngle = std::max(cosOpeningAngle, m_emShowerIdMinCosAngle);

            const float hitEnergy((*hitIter)->GetElectromagneticEnergy());
            energyInLayer += hitEnergy;
            nRadiationLengthsInLayer += (*hitIter)->GetNCellRadiationLengths() / cosOpeningAngle;

            const float radialDistance(((*hitIter)->GetPositionVector() - clusterIntercept).GetCrossProduct(clusterDirection).GetMagnitude());
            hitEnergyDistanceVector.push_back(HitEnergyDistance(hitEnergy, radialDistance));
        }

        layer90EnergySum += energyInLayer;
        nRadiationLengthsInLayer /= static_cast<float>(iter->second->size());
        nRadiationLengthsInLastLayer = nRadiationLengthsInLayer;
        nRadiationLengths += nRadiationLengthsInLayer;

        // Cut on number of radiation lengths before cluster start
        if (innerPseudoLayer == iLayer)
        {
            nRadiationLengths *= static_cast<float>(innerPseudoLayer + 1 - firstPseudoLayer);

            if (nRadiationLengths > m_emShowerIdMaxInnerLayerRadLengths)
                return false;
        }

        // Cut on number of radiation lengths before longitudinal layer90
        if (!foundLayer90 && (layer90EnergySum > 0.9f * totalElectromagneticEnergy))
        {
            foundLayer90 = true;

            if ((nRadiationLengths < m_emShowerIdMinLayer90RadLengths) || (nRadiationLengths > m_emShowerIdMaxLayer90RadLengths))
                return false;
        }

        // Identify number of radiation lengths before shower max layer
        if (energyInLayer > maxEnergyInlayer)
        {
            showerMaxRadLengths = nRadiationLengths;
            maxEnergyInlayer = energyInLayer;
        }

        // Count energy above specified "high" number of radiation lengths
        if (nRadiationLengths > m_emShowerIdHighRadLengths)
        {
            energyAboveHighRadLengths += energyInLayer;
        }
    }

    // Cut on longitudinal shower profile properties
    if ((showerMaxRadLengths < m_emShowerIdMinShowerMaxRadLengths) || (showerMaxRadLengths > m_emShowerIdMaxShowerMaxRadLengths))
        return false;

    if (energyAboveHighRadLengths > m_emShowerIdMaxHighRadLengthEnergyFraction * totalElectromagneticEnergy)
        return false;

    // Cut on transverse shower profile properties
    std::sort(hitEnergyDistanceVector.begin(), hitEnergyDistanceVector.end(), FineGranularityParticleId::SortHitsByDistance);
    float radial90EnergySum(0.f), radial90(std::numeric_limits<float>::max());

    for (HitEnergyDistanceVector::const_iterator iter = hitEnergyDistanceVector.begin(), iterEnd = hitEnergyDistanceVector.end(); iter != iterEnd; ++iter)
    {
        radial90EnergySum += iter->first;

        if (radial90EnergySum > 0.9f * totalElectromagneticEnergy)
        {
            radial90 = iter->second;
            break;
        }
    }

    if (radial90 > m_emShowerIdMaxRadial90)
        return false;

    // Anything remaining at this point is classed as an electromagnetic shower
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool FineGranularityParticleId::FineGranularityPhotonId(const Cluster *const pCluster)
{
    // Cluster with associated tracks is not a photon
    if (!pCluster->GetAssociatedTrackList().empty())
        return false;

    return FineGranularityParticleId::FineGranularityEmShowerId(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool FineGranularityParticleId::FineGranularityElectronId(const Cluster *const pCluster)
{
    const TrackList &associatedTrackList(pCluster->GetAssociatedTrackList());

    if (associatedTrackList.empty())
        return false;

    const float electromagneticEnergy(pCluster->GetElectromagneticEnergy());

    if (!FineGranularityParticleId::FineGranularityEmShowerId(pCluster) && ((pCluster->GetInnerPseudoLayer() > m_electronIdMaxInnerLayer) ||
        (electromagneticEnergy > m_electronIdMaxEnergy)))
    {
        return false;
    }

    const float showerProfileStart(pCluster->GetShowerProfileStart());
    const float showerProfileDiscrepancy(pCluster->GetShowerProfileDiscrepancy());

    if ((showerProfileStart > m_electronIdMaxProfileStart) || (showerProfileDiscrepancy > m_electronIdMaxProfileDiscrepancy))
        return false;

    if (showerProfileDiscrepancy < m_electronIdProfileDiscrepancyForAutoId)
        return true;

    for (TrackList::const_iterator iter = associatedTrackList.begin(), iterEnd = associatedTrackList.end(); iter != iterEnd; ++iter)
    {
        const float momentumAtDca((*iter)->GetMomentumAtDca().GetMagnitude());

        if (0.f == momentumAtDca)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        const float eOverP(electromagneticEnergy / momentumAtDca);

        if (std::fabs(eOverP - 1.f) < m_electronIdMaxResidualEOverP)
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool FineGranularityParticleId::FineGranularityMuonId(const Cluster *const pCluster)
{
    // Simple pre-selection cuts
    if (pCluster->GetInnerPseudoLayer() > m_muonIdMaxInnerLayer)
        return false;

    const TrackList &trackList(pCluster->GetAssociatedTrackList());

    if (trackList.size() != 1)
        return false;

    // For now only try to identify "high" energy muons
    Track *pTrack = *(trackList.begin());

    if (pTrack->GetEnergyAtDca() < m_muonIdMinTrackEnergy)
        return false;

    // Calculate cut variables
    unsigned int nECalHits(0), nHCalHits(0), nMuonHits(0), nECalMipHits(0), nHCalMipHits(0), nHCalEndCapHits(0), nHCalBarrelHits(0);
    float energyECal(0.), energyHCal(0.), directionCosine(0.);

    typedef std::set<PseudoLayer> LayerList;
    LayerList pseudoLayersECal, pseudoLayersHCal, pseudoLayersMuon, layersECal, layersHCal;

    const CartesianVector &momentum(pTrack->GetTrackStateAtCalorimeter().GetMomentum());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());
            const unsigned int layer(pCaloHit->GetLayer());

            if (pCaloHit->GetHitType() == ECAL)
            {
                nECalHits++;

                if (pCaloHit->IsPossibleMip())
                    nECalMipHits++;

                pseudoLayersECal.insert(pseudoLayer);
                layersECal.insert(layer);

                energyECal += pCaloHit->GetHadronicEnergy();
                directionCosine += momentum.GetCosOpeningAngle(pCaloHit->GetCellNormalVector());
            }

            else if (pCaloHit->GetHitType() == HCAL)
            {
                nHCalHits++;

                if(pCaloHit->IsPossibleMip())
                    nHCalMipHits++;

                if (pCaloHit->GetDetectorRegion() == BARREL)
                    nHCalBarrelHits++;

                if (pCaloHit->GetDetectorRegion() == ENDCAP)
                    nHCalEndCapHits++;

                pseudoLayersHCal.insert(pseudoLayer);
                layersHCal.insert(layer);

                energyHCal += std::min(m_muonIdMaxHCalHitEnergy, pCaloHit->GetHadronicEnergy());
            }

            else if (pCaloHit->GetHitType() == MUON)
            {
                nMuonHits++;
                pseudoLayersMuon.insert(pseudoLayer);
            }
        }
    }

    // Simple cuts on layer counts
    const PseudoLayer nPseudoLayersECal(pseudoLayersECal.size());
    const PseudoLayer nPseudoLayersHCal(pseudoLayersHCal.size());

    if ((nPseudoLayersECal < m_muonIdMinECalLayers) && (layersECal.size() < m_muonIdMinECalLayers))
        return false;

    if ((nPseudoLayersHCal < m_muonIdMinHCalLayers) && (layersHCal.size() < m_muonIdMinHCalLayers))
    {
        if (!m_muonIdShouldPerformGapCheck || (nPseudoLayersHCal < m_muonIdMinHCalLayersForGapCheck) || (nMuonHits < m_muonIdMinMuonHitsForGapCheck))
            return false;

        if (!ClusterHelper::DoesClusterCrossGapRegion(pCluster, *(pseudoLayersHCal.begin()), *(pseudoLayersHCal.rbegin())))
            return false;
    }

    // Calculate energies per layer
    float energyECalDCos(0.), nHitsPerLayerECal(0.), nHitsPerLayerHCal(0.), mipFractionECal(0.), mipFractionHCal(0.);

    if ((nECalHits > 0) && (nPseudoLayersECal > 0))
    {
        directionCosine /= static_cast<float>(nECalHits);
        energyECalDCos = energyECal * directionCosine;

        mipFractionECal = static_cast<float>(nECalMipHits) / static_cast<float>(nECalHits);
        nHitsPerLayerECal = static_cast<float>(nECalHits) / static_cast<float>(nPseudoLayersECal);
    }

    if ((nHCalHits > 0) && (nPseudoLayersHCal > 0))
    {
        mipFractionHCal = static_cast<float>(nHCalMipHits) / static_cast<float>(nHCalHits);
        nHitsPerLayerHCal = static_cast<float>(nHCalHits) / static_cast<float>(nPseudoLayersHCal);
    }

    // Loose energy cuts
    const float trackEnergy(pTrack->GetEnergyAtDca());
    const float eCalEnergyCut(m_muonIdECalEnergyCut0 + (m_muonIdECalEnergyCut1 * trackEnergy));
    const float hCalEnergyCut(m_muonIdHCalEnergyCut0 + (m_muonIdHCalEnergyCut1 * trackEnergy));

    if ((energyECalDCos > eCalEnergyCut) || (energyHCal > hCalEnergyCut))
        return false;

    // Calculate event shape variables for ecal
    float eCalRms(std::numeric_limits<float>::max());
    int nECalCutsFailed(0);

    if (nPseudoLayersECal > m_muonIdMinECalLayersForFit)
    {
        ClusterHelper::ClusterFitResult newFitResult;
        ClusterHelper::FitLayers(pCluster, m_muonIdECalFitInnerLayer, m_muonIdECalFitOuterLayer, newFitResult);

        if (newFitResult.IsFitSuccessful())
            eCalRms = newFitResult.GetRms();
    }

    const float rmsECalCut(std::min(m_muonIdECalRmsCut0 + (trackEnergy * m_muonIdECalRmsCut1), m_muonIdECalMaxRmsCut));

    if (eCalRms > rmsECalCut)
        nECalCutsFailed++;

    const float mipFractionECalCut(std::min(m_muonIdECalMipFractionCut0 - (trackEnergy * m_muonIdECalMipFractionCut1), m_muonIdECalMaxMipFractionCut));

    if (mipFractionECal < mipFractionECalCut)
        nECalCutsFailed++;

    const float nHitsPerLayerECalCut(std::min(m_muonIdECalHitsPerLayerCut0 + (trackEnergy * m_muonIdECalHitsPerLayerCut1), m_muonIdECalMaxHitsPerLayerCut));

    if (nHitsPerLayerECal > nHitsPerLayerECalCut)
        nECalCutsFailed++;

    // Calculate event shape variables for hcal
    // ATTN rms cut should be made wrt Kalman filter fit: cut makes no sense for tracks which loop back in hcal barrel
    float hCalRms(std::numeric_limits<float>::max());
    int nHCalCutsFailed(0);

    if (nPseudoLayersHCal > m_muonIdMinHCalLayersForFit)
    {
        ClusterHelper::ClusterFitResult newFitResult;
        ClusterHelper::FitLayers(pCluster, m_muonIdHCalFitInnerLayer, m_muonIdHCalFitOuterLayer, newFitResult);

        if (newFitResult.IsFitSuccessful())
            hCalRms = newFitResult.GetRms();
    }

    const bool inBarrel((nHCalEndCapHits == 0) ||
        (static_cast<float>(nHCalBarrelHits) / static_cast<float>(nHCalBarrelHits + nHCalEndCapHits) >= m_muonIdInBarrelHitFraction));

    float rmsHCalCut(std::min(m_muonIdHCalRmsCut0 + (trackEnergy * m_muonIdHCalRmsCut1), m_muonIdHCalMaxRmsCut));

    if ((trackEnergy < m_muonIdCurlingTrackEnergy) && inBarrel)
        rmsHCalCut = m_muonIdHCalMaxRmsCut;

    if (hCalRms > rmsHCalCut)
    {
        // Impose tight cuts on MipFraction
        if ((trackEnergy > m_muonIdCurlingTrackEnergy) || !inBarrel)
        {
            nHCalCutsFailed++;
        }
        else if ((pCluster->GetMipFraction() < m_muonIdTightMipFractionCut) || (mipFractionECal < m_muonIdTightMipFractionECalCut) ||
                (mipFractionHCal < m_muonIdTightMipFractionHCalCut))
        {
            nHCalCutsFailed++;
        }
    }

    const float mipFractionHCalCut(std::min(m_muonIdHCalMipFractionCut0 - (trackEnergy * m_muonIdHCalMipFractionCut1), m_muonIdHCalMaxMipFractionCut));

    if (mipFractionHCal < mipFractionHCalCut)
        nHCalCutsFailed++;

    const float nHitsPerlayerHCalCut(std::min(m_muonIdHCalHitsPerLayerCut0 + (trackEnergy * m_muonIdHCalHitsPerLayerCut1), m_muonIdHCalMaxHitsPerLayerCut));

    if (nHitsPerLayerHCal > nHitsPerlayerHCalCut)
        nHCalCutsFailed++;

    // Calculate event shape variables for muon
    float muonRms(std::numeric_limits<float>::max());
    int nMuonCutsPassed(0);

    if (pseudoLayersMuon.size() > m_muonIdMinMuonLayersForFit)
    { 
        ClusterHelper::ClusterFitResult newFitResult;
        ClusterHelper::FitLayers(pCluster, *pseudoLayersMuon.begin(), *pseudoLayersMuon.rbegin(), newFitResult);

        if (newFitResult.IsFitSuccessful())
            muonRms = newFitResult.GetRms();
    }

    const float maxMuonHitsCut(std::max(m_muonIdMaxMuonHitsCut0 + (m_muonIdMaxMuonHitsCut1 * trackEnergy), m_muonIdMaxMuonHitsCutMinValue));

    if ((nMuonHits > m_muonIdMinMuonHitsCut) && (nMuonHits < maxMuonHitsCut))
        nMuonCutsPassed++;

    if ((nMuonHits > m_muonIdMinMuonTrackSegmentHitsCut) && (muonRms < m_muonIdMuonRmsCut))
        nMuonCutsPassed++;

    // Make final decision
    const int nCutsFailed(nECalCutsFailed + nHCalCutsFailed - nMuonCutsPassed);

    return (nCutsFailed <= 0);
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int FineGranularityParticleId::m_electronIdMaxInnerLayer = 4;
float FineGranularityParticleId::m_electronIdMaxEnergy = 5.f;
float FineGranularityParticleId::m_electronIdMaxProfileStart = 4.5f;
float FineGranularityParticleId::m_electronIdMaxProfileDiscrepancy = 0.6f;
float FineGranularityParticleId::m_electronIdProfileDiscrepancyForAutoId = 0.5f;
float FineGranularityParticleId::m_electronIdMaxResidualEOverP = 0.2f;

unsigned int FineGranularityParticleId::m_muonIdMaxInnerLayer = 10;
float FineGranularityParticleId::m_muonIdMinTrackEnergy = 2.5f;
float FineGranularityParticleId::m_muonIdMaxHCalHitEnergy = 1.f;
unsigned int FineGranularityParticleId::m_muonIdMinECalLayers = 20;
unsigned int FineGranularityParticleId::m_muonIdMinHCalLayers = 20;
bool FineGranularityParticleId::m_muonIdShouldPerformGapCheck = true;
unsigned int FineGranularityParticleId::m_muonIdMinHCalLayersForGapCheck = 4;
unsigned int FineGranularityParticleId::m_muonIdMinMuonHitsForGapCheck = 3;
float FineGranularityParticleId::m_muonIdECalEnergyCut0 = 1.f;
float FineGranularityParticleId::m_muonIdECalEnergyCut1 = 0.05f;
float FineGranularityParticleId::m_muonIdHCalEnergyCut0 = 5.f;
float FineGranularityParticleId::m_muonIdHCalEnergyCut1 = 0.15f;
unsigned int FineGranularityParticleId::m_muonIdMinECalLayersForFit = 10;
unsigned int FineGranularityParticleId::m_muonIdMinHCalLayersForFit = 10;
unsigned int FineGranularityParticleId::m_muonIdMinMuonLayersForFit = 4;
unsigned int FineGranularityParticleId::m_muonIdECalFitInnerLayer = 1;
unsigned int FineGranularityParticleId::m_muonIdECalFitOuterLayer = 30;
unsigned int FineGranularityParticleId::m_muonIdHCalFitInnerLayer = 31;
unsigned int FineGranularityParticleId::m_muonIdHCalFitOuterLayer = 79;
float FineGranularityParticleId::m_muonIdECalRmsCut0 = 20.f;
float FineGranularityParticleId::m_muonIdECalRmsCut1 = 0.2f;
float FineGranularityParticleId::m_muonIdECalMaxRmsCut = 50.f;
float FineGranularityParticleId::m_muonIdHCalRmsCut0 = 40.f;
float FineGranularityParticleId::m_muonIdHCalRmsCut1 = 0.1f;
float FineGranularityParticleId::m_muonIdHCalMaxRmsCut = 80.f;
float FineGranularityParticleId::m_muonIdECalMipFractionCut0 = 0.8f;
float FineGranularityParticleId::m_muonIdECalMipFractionCut1 = 0.05f;
float FineGranularityParticleId::m_muonIdECalMaxMipFractionCut = 0.6f;
float FineGranularityParticleId::m_muonIdHCalMipFractionCut0 = 0.8f;
float FineGranularityParticleId::m_muonIdHCalMipFractionCut1 = 0.02f;
float FineGranularityParticleId::m_muonIdHCalMaxMipFractionCut = 0.75f;
float FineGranularityParticleId::m_muonIdECalHitsPerLayerCut0 = 2.f;
float FineGranularityParticleId::m_muonIdECalHitsPerLayerCut1 = 0.02f;
float FineGranularityParticleId::m_muonIdECalMaxHitsPerLayerCut = 5.f;
float FineGranularityParticleId::m_muonIdHCalHitsPerLayerCut0 = 1.5f;
float FineGranularityParticleId::m_muonIdHCalHitsPerLayerCut1 = 0.05f;
float FineGranularityParticleId::m_muonIdHCalMaxHitsPerLayerCut = 8.f;
float FineGranularityParticleId::m_muonIdCurlingTrackEnergy = 4.f;
float FineGranularityParticleId::m_muonIdInBarrelHitFraction = 0.05f;
float FineGranularityParticleId::m_muonIdTightMipFractionCut = 0.85f;
float FineGranularityParticleId::m_muonIdTightMipFractionECalCut = 0.8f;
float FineGranularityParticleId::m_muonIdTightMipFractionHCalCut = 0.875f;
unsigned int FineGranularityParticleId::m_muonIdMinMuonHitsCut = 2;
unsigned int FineGranularityParticleId::m_muonIdMinMuonTrackSegmentHitsCut = 8;
float FineGranularityParticleId::m_muonIdMuonRmsCut = 25.f;
float FineGranularityParticleId::m_muonIdMaxMuonHitsCut0 = 0.f;
float FineGranularityParticleId::m_muonIdMaxMuonHitsCut1 = 0.2f;
float FineGranularityParticleId::m_muonIdMaxMuonHitsCutMinValue = 30.f;

float FineGranularityParticleId::m_emShowerIdMipCut_0 = 0.9f;
float FineGranularityParticleId::m_emShowerIdMipCutEnergy_1 = 15.f;
float FineGranularityParticleId::m_emShowerIdMipCut_1 = 0.3f;
float FineGranularityParticleId::m_emShowerIdMipCutEnergy_2 = 7.5f;
float FineGranularityParticleId::m_emShowerIdMipCut_2 = 0.4f;
float FineGranularityParticleId::m_emShowerIdMipCutEnergy_3 = 3.f;
float FineGranularityParticleId::m_emShowerIdMipCut_3 = 0.6f;
float FineGranularityParticleId::m_emShowerIdMipCutEnergy_4 = 1.5f;
float FineGranularityParticleId::m_emShowerIdMipCut_4 = 0.7f;
float FineGranularityParticleId::m_emShowerIdDCosRCutEnergy = 1.5f;
float FineGranularityParticleId::m_emShowerIdDCosRLowECut = 0.94f;
float FineGranularityParticleId::m_emShowerIdDCosRHighECut = 0.95f;
float FineGranularityParticleId::m_emShowerIdRmsCutEnergy = 40.f;
float FineGranularityParticleId::m_emShowerIdRmsLowECut = 40.f;
float FineGranularityParticleId::m_emShowerIdRmsHighECut = 90.f;
float FineGranularityParticleId::m_emShowerIdMinCosAngle = 0.3f;
float FineGranularityParticleId::m_emShowerIdMaxInnerLayerRadLengths = 10.f;
float FineGranularityParticleId::m_emShowerIdMinLayer90RadLengths = 4.f;
float FineGranularityParticleId::m_emShowerIdMaxLayer90RadLengths = 30.f;
float FineGranularityParticleId::m_emShowerIdMinShowerMaxRadLengths = 0.f;
float FineGranularityParticleId::m_emShowerIdMaxShowerMaxRadLengths = 25.f;
float FineGranularityParticleId::m_emShowerIdHighRadLengths = 40.f;
float FineGranularityParticleId::m_emShowerIdMaxHighRadLengthEnergyFraction = 0.04f;
float FineGranularityParticleId::m_emShowerIdMaxRadial90 = 40.f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FineGranularityParticleId::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Electromagnetic shower id settings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCut_0", m_emShowerIdMipCut_0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCutEnergy_1", m_emShowerIdMipCutEnergy_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCut_1", m_emShowerIdMipCut_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCutEnergy_2", m_emShowerIdMipCutEnergy_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCut_2", m_emShowerIdMipCut_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCutEnergy_3", m_emShowerIdMipCutEnergy_3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCut_3", m_emShowerIdMipCut_3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCutEnergy_4", m_emShowerIdMipCutEnergy_4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMipCut_4", m_emShowerIdMipCut_4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdDCosRCutEnergy", m_emShowerIdDCosRCutEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdDCosRLowECut", m_emShowerIdDCosRLowECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdDCosRHighECut", m_emShowerIdDCosRHighECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdRmsCutEnergy", m_emShowerIdRmsCutEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdRmsLowECut", m_emShowerIdRmsLowECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdRmsHighECut", m_emShowerIdRmsHighECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMinCosAngle", m_emShowerIdMinCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMaxInnerLayerRadLengths", m_emShowerIdMaxInnerLayerRadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMinLayer90RadLengths", m_emShowerIdMinLayer90RadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMaxLayer90RadLengths", m_emShowerIdMaxLayer90RadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMinShowerMaxRadLengths", m_emShowerIdMinShowerMaxRadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EmShowerIdMaxShowerMaxRadLengths", m_emShowerIdMaxShowerMaxRadLengths));

    // Electron id settings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ElectronIdMaxInnerLayer", m_electronIdMaxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ElectronIdMaxEnergy", m_electronIdMaxEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ElectronIdMaxProfileStart", m_electronIdMaxProfileStart));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ElectronIdMaxProfileDiscrepancy", m_electronIdMaxProfileDiscrepancy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ElectronIdProfileDiscrepancyForAutoId", m_electronIdProfileDiscrepancyForAutoId));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ElectronIdMaxResidualEOverP", m_electronIdMaxResidualEOverP));

    // Muon id settings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMaxInnerLayer", m_muonIdMaxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinTrackEnergy", m_muonIdMinTrackEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMaxHCalHitEnergy", m_muonIdMaxHCalHitEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinECalLayers", m_muonIdMinECalLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinHCalLayers", m_muonIdMinHCalLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdShouldPerformGapCheck", m_muonIdShouldPerformGapCheck));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinHCalLayersForGapCheck", m_muonIdMinHCalLayersForGapCheck));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinMuonHitsForGapCheck", m_muonIdMinMuonHitsForGapCheck));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalEnergyCut0", m_muonIdECalEnergyCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalEnergyCut1", m_muonIdECalEnergyCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalEnergyCut0", m_muonIdHCalEnergyCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalEnergyCut1", m_muonIdHCalEnergyCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinECalLayersForFit", m_muonIdMinECalLayersForFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinHCalLayersForFit", m_muonIdMinHCalLayersForFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinMuonLayersForFit", m_muonIdMinMuonLayersForFit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalFitInnerLayer", m_muonIdECalFitInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalFitOuterLayer", m_muonIdECalFitOuterLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalFitInnerLayer", m_muonIdHCalFitInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalFitOuterLayer", m_muonIdHCalFitOuterLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalRmsCut0", m_muonIdECalRmsCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalRmsCut1", m_muonIdECalRmsCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalMaxRmsCut", m_muonIdECalMaxRmsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalRmsCut0", m_muonIdHCalRmsCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalRmsCut1", m_muonIdHCalRmsCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalMaxRmsCut", m_muonIdHCalMaxRmsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalMipFractionCut0", m_muonIdECalMipFractionCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalMipFractionCut1", m_muonIdECalMipFractionCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalMaxMipFractionCut", m_muonIdECalMaxMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalMipFractionCut0", m_muonIdHCalMipFractionCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalMipFractionCut1", m_muonIdHCalMipFractionCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalMaxMipFractionCut", m_muonIdHCalMaxMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalHitsPerLayerCut0", m_muonIdECalHitsPerLayerCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalHitsPerLayerCut1", m_muonIdECalHitsPerLayerCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdECalMaxHitsPerLayerCut", m_muonIdECalMaxHitsPerLayerCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalHitsPerLayerCut0", m_muonIdHCalHitsPerLayerCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalHitsPerLayerCut1", m_muonIdHCalHitsPerLayerCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdHCalMaxHitsPerLayerCut", m_muonIdHCalMaxHitsPerLayerCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdCurlingTrackEnergy", m_muonIdCurlingTrackEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdInBarrelHitFraction", m_muonIdInBarrelHitFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdTightMipFractionCut", m_muonIdTightMipFractionCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdTightMipFractionECalCut", m_muonIdTightMipFractionECalCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdTightMipFractionHCalCut", m_muonIdTightMipFractionHCalCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinMuonHitsCut", m_muonIdMinMuonHitsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinMuonTrackSegmentHitsCut", m_muonIdMinMuonTrackSegmentHitsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMuonRmsCut", m_muonIdMuonRmsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMaxMuonHitsCut0", m_muonIdMaxMuonHitsCut0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMaxMuonHitsCut1", m_muonIdMaxMuonHitsCut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMaxMuonHitsCutMinValue", m_muonIdMaxMuonHitsCutMinValue));

    return STATUS_CODE_SUCCESS;
}
