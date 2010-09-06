/**
 *  @file   PandoraPFANew/src/Helpers/ParticleIdHelper.cc
 * 
 *  @brief  Implementation of the particle id helper class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"
#include "Helpers/ParticleIdHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include <cmath>
#include <limits>

namespace pandora
{

StatusCode ParticleIdHelper::CalculateShowerProfile(const Cluster *const pCluster, float &showerProfileStart, float &showerProfileDiscrepancy)
{
    // 1. Construct cluster profile.
    const float clusterEnergy(pCluster->GetElectromagneticEnergy() - pCluster->GetIsolatedElectromagneticEnergy());

    if(clusterEnergy <= 0.f || (pCluster->GetNCaloHits() < 1))
        return STATUS_CODE_INVALID_PARAMETER;

    // Initialize profile
    float *pProfile = new float[m_showerProfileNBins];
    for(unsigned int iBin = 0; iBin < m_showerProfileNBins; ++iBin)
    {
        pProfile[iBin] = 0.f;
    }

    // Extract information from the cluster
    static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
    const PseudoLayer innerPseudoLayer(pCluster->GetInnerPseudoLayer());

    if (innerPseudoLayer > nECalLayers)
        return STATUS_CODE_NOT_FOUND;

    const CartesianVector &clusterDirection(pCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pCluster->GetFitToAllHitsResult().GetDirection() : pCluster->GetInitialDirection());

    // Examine layers to construct profile
    float eCalEnergy(0.f);
    float nRadiationLengths(0.f);
    float nRadiationLengthsInLastLayer(0.f);
    unsigned int profileEndBin(0);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = innerPseudoLayer; iLayer <= nECalLayers; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if ((orderedCaloHitList.end() == iter) || (iter->second->empty()))
        {
            nRadiationLengths += nRadiationLengthsInLastLayer;
            continue;
        }

        // Extract information from calo hits
        float energyInLayer(0.f);
        float nRadiationLengthsInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            // ATTN: This direction measure introduces a difference wrt old Pandora
            float cosOpeningAngle(std::fabs((*hitIter)->GetNormalVector().GetCosOpeningAngle(clusterDirection)));
            cosOpeningAngle = std::max(cosOpeningAngle, m_showerProfileMinCosAngle);

            energyInLayer += (*hitIter)->GetElectromagneticEnergy();
            nRadiationLengthsInLayer += (*hitIter)->GetNRadiationLengths() / cosOpeningAngle;
        }

        eCalEnergy += energyInLayer;
        nRadiationLengthsInLayer /= static_cast<float>(iter->second->size());
        nRadiationLengthsInLastLayer = nRadiationLengthsInLayer;
        nRadiationLengths += nRadiationLengthsInLayer;

        // Account for layers before start of cluster
        if (innerPseudoLayer == iLayer)
            nRadiationLengths *= static_cast<float>(innerPseudoLayer - TRACK_PROJECTION_LAYER);

        // Finally, create the profile
        const float endPosition(nRadiationLengths / m_showerProfileBinWidth);
        const unsigned int endBin(std::min(static_cast<unsigned int>(endPosition), m_showerProfileNBins - 1));

        const float deltaPosition(nRadiationLengthsInLayer / m_showerProfileBinWidth);

        const float startPosition(endPosition - deltaPosition);
        const unsigned int startBin(static_cast<unsigned int>(startPosition));

        for (unsigned int iBin = startBin; iBin <= endBin; ++iBin)
        {
            // TODO Should delta be a multiple of bin widths?
            float delta(1.f);

            if (startBin == iBin)
            {
                delta -= startPosition - startBin;
            }
            else if (endBin == iBin)
            {
                delta -= 1.f - endPosition + endBin;
            }

            pProfile[iBin] += energyInLayer * (delta / deltaPosition);
        }
    }

    profileEndBin = std::min(static_cast<unsigned int>(nRadiationLengths / m_showerProfileBinWidth), m_showerProfileNBins);

    if (eCalEnergy <= 0.f)
        return STATUS_CODE_FAILURE;

    // 2. Construct expected cluster profile
    const double a(m_showerProfileParameter0 + m_showerProfileParameter1 * std::log(clusterEnergy / m_showerProfileCriticalEnergy));
#ifdef __GNUC__
    const double gammaA(std::exp(gamma(a)));
#else
    const double gammaA(0.);
    std::cout << " PandoraPFANew, ParticleIdHelper: TODO - implement gamma function " << std::endl;
    throw StatusCodeException(STATUS_CODE_FAILURE);
#endif

    float t(0.f);
    float *pExpectedProfile = new float[m_showerProfileNBins];

    for (unsigned int iBin = 0; iBin < m_showerProfileNBins; ++iBin)
    {
        t += m_showerProfileBinWidth;
        pExpectedProfile[iBin] = static_cast<float>(clusterEnergy / 2. * std::pow(t / 2.f, static_cast<float>(a - 1.)) *
            std::exp(-t / 2.) * m_showerProfileBinWidth / gammaA);
    }

    // 3. Compare the cluster profile with the expected profile
    unsigned int binOffsetAtMinDifference(0);
    float minProfileDifference(std::numeric_limits<float>::max());

    for (unsigned int iBinOffset = 0; iBinOffset < nECalLayers; ++iBinOffset)
    {
        float profileDifference(0.);

        for (unsigned int iBin = 0; iBin < profileEndBin; ++iBin)
        {
            if (iBin < iBinOffset)
            {
                profileDifference += pProfile[iBin];
            }
            else
            {
                profileDifference += std::fabs(pExpectedProfile[iBin - iBinOffset] - pProfile[iBin]);
            }
        }

        if (profileDifference < minProfileDifference)
        {
            minProfileDifference = profileDifference;
            binOffsetAtMinDifference = iBinOffset;
        }

        if (profileDifference - minProfileDifference > m_showerProfileMaxDifference)
            break;
    }

    showerProfileStart =  binOffsetAtMinDifference * m_showerProfileBinWidth;
    showerProfileDiscrepancy = minProfileDifference / eCalEnergy;

    delete [] pProfile;
    delete [] pExpectedProfile;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsElectromagneticShower(const Cluster *const pCluster)
{
    // Reject empty clusters
    if (0 == pCluster->GetNCaloHits())
        return false;

    // Reject clusters starting outside ecal
    static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
    const PseudoLayer innerLayer(pCluster->GetInnerPseudoLayer());

    if (innerLayer > nECalLayers)
        return false;

    // Cut on cluster mip fraction
    const float totalElectromagneticEnergy(pCluster->GetElectromagneticEnergy() - pCluster->GetIsolatedElectromagneticEnergy());

    float mipCut(m_photonIdMipCut_0);

    if (totalElectromagneticEnergy > m_photonIdMipCutEnergy_1)
    {
        mipCut = m_photonIdMipCut_1;
    }
    else if (totalElectromagneticEnergy > m_photonIdMipCutEnergy_2)
    {
        mipCut = m_photonIdMipCut_2;
    }
    else if (totalElectromagneticEnergy > m_photonIdMipCutEnergy_3)
    {
        mipCut = m_photonIdMipCut_3;
    }
    else if (totalElectromagneticEnergy > m_photonIdMipCutEnergy_4)
    {
        mipCut = m_photonIdMipCut_4;
    }

    if (pCluster->GetMipFraction() > mipCut)
        return false;

    // Cut on results of fit to all hits in cluster
    float dCosR(0.f);
    float clusterRms(0.f);

    const ClusterHelper::ClusterFitResult &clusterFitResult(pCluster->GetFitToAllHitsResult());
    const CartesianVector innerLayerCentroid(pCluster->GetCentroid(innerLayer));

    if (clusterFitResult.IsFitSuccessful())
    {
        dCosR = clusterFitResult.GetRadialDirectionCosine();
        clusterRms = clusterFitResult.GetRms();
    }

    const float dCosRCut(totalElectromagneticEnergy < m_photonIdDCosRCutEnergy ? m_photonIdDCosRLowECut : m_photonIdDCosRHighECut);

    if (dCosR < dCosRCut)
        return false;

    const float rmsCut(totalElectromagneticEnergy < m_photonIdRmsCutEnergy ? m_photonIdRmsLowECut : m_photonIdRmsHighECut);

    if (clusterRms > rmsCut)
        return false;

    // Compare initial cluster direction with normal to ecal layers - TODO use calo hit normal vectors here
    static const float eCalEndCapInnerZ(GeometryHelper::GetInstance()->GetECalEndCapParameters().GetInnerZCoordinate());
    const bool isEndCap((std::fabs(innerLayerCentroid.GetZ()) > eCalEndCapInnerZ - m_photonIdEndCapZSeparation));

    const float cosTheta(std::fabs(innerLayerCentroid.GetZ()) / innerLayerCentroid.GetMagnitude());
    const float rDotN(isEndCap ? cosTheta : std::sqrt(1.f - cosTheta * cosTheta));

    if (0 == rDotN)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    // Find number of radiation lengths in front of cluster first layer
    static const GeometryHelper *const pGeometryHelper(GeometryHelper::GetInstance());
    static const GeometryHelper::LayerParametersList &barrelLayerList(pGeometryHelper->GetECalBarrelParameters().GetLayerParametersList());
    static const GeometryHelper::LayerParametersList &endCapLayerList(pGeometryHelper->GetECalEndCapParameters().GetLayerParametersList());

    const unsigned int physicalLayer((innerLayer > (1 + TRACK_PROJECTION_LAYER)) ? innerLayer - 1 - TRACK_PROJECTION_LAYER : 0);
    const float nRadiationLengths(isEndCap ? barrelLayerList[physicalLayer].m_cumulativeRadiationLengths : endCapLayerList[physicalLayer].m_cumulativeRadiationLengths);
    const float firstLayerInRadiationLengths(nRadiationLengths / rDotN);

    if (firstLayerInRadiationLengths > m_photonIdRadiationLengthsCut)
        return false;

    // Calculate position of shower max layer and layer by which 90% of cluster energy has been deposited
    bool foundLayer90(false);
    float electromagneticEnergy90(0.f), maxElectromagneticEnergyInlayer(0.f);
    int layer90(std::numeric_limits<int>::max()), showerMaxLayer(std::numeric_limits<int>::max());

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        float electromagneticEnergyInlayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            electromagneticEnergy90 += (*hitIter)->GetElectromagneticEnergy();
            electromagneticEnergyInlayer += (*hitIter)->GetElectromagneticEnergy();
        }

        if (electromagneticEnergyInlayer > maxElectromagneticEnergyInlayer)
        {
            showerMaxLayer = static_cast<int>(iter->first);
            maxElectromagneticEnergyInlayer = electromagneticEnergyInlayer;
        }

        if (!foundLayer90 && (electromagneticEnergy90 > 0.9 * totalElectromagneticEnergy))
        {
            layer90 = static_cast<int>(iter->first);
            foundLayer90 = true;
        }
    }

    // Cut on position of shower max layer
    float showerMaxCut1(m_photonIdShowerMaxCut1_0);
    const float showerMaxCut2(m_photonIdShowerMaxCut2);

    if (totalElectromagneticEnergy > m_photonIdShowerMaxCut1Energy_1)
    {
        showerMaxCut1 = m_photonIdShowerMaxCut1_1;
    }
    else if (totalElectromagneticEnergy > m_photonIdShowerMaxCut1Energy_2)
    {
        showerMaxCut1 = m_photonIdShowerMaxCut1_2;
    }

    if ((showerMaxLayer - innerLayer <= showerMaxCut1 * rDotN) || (showerMaxLayer - innerLayer >= showerMaxCut2 * rDotN))
        return false;

    // Cut on layer by which 90% of cluster energy has been deposited
    const float layer90Cut1(m_photonIdLayer90Cut1);
    const float layer90Cut2(totalElectromagneticEnergy < m_photonIdLayer90Cut2Energy ? m_photonIdLayer90LowECut2 : m_photonIdLayer90HighECut2);

    if ((layer90 - innerLayer <= layer90Cut1 * rDotN) || (layer90 - innerLayer >= layer90Cut2 * rDotN))
        return false;

    if (layer90 > static_cast<int>(nECalLayers) + m_photonIdLayer90MaxLayersFromECal)
        return false;

    // Anything remaining at this point is classed as an electromagnetic shower
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsPhotonFastDefault(const Cluster *const pCluster)
{
    // Already flagged as a fixed photon by an algorithm - overrides fast photon id
    if (pCluster->IsFixedPhoton())
        return true;

    // Cluster with associated tracks is not a photon
    if (!pCluster->GetAssociatedTrackList().empty())
        return false;

    return ParticleIdHelper::IsElectromagneticShower(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsElectronFastDefault(const Cluster *const pCluster)
{
    const TrackList &associatedTrackList(pCluster->GetAssociatedTrackList());

    if (associatedTrackList.empty())
        return false;

    const float electromagneticEnergy(pCluster->GetElectromagneticEnergy());

    if (!ParticleIdHelper::IsElectromagneticShower(pCluster) && ((pCluster->GetInnerPseudoLayer() > m_electronIdMaxInnerLayer) ||
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

bool ParticleIdHelper::IsMuonFastDefault(const Cluster *const pCluster)
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

    const CartesianVector &momentum(pTrack->GetTrackStateAtECal().GetMomentum());
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
                directionCosine += momentum.GetCosOpeningAngle(pCaloHit->GetNormalVector());
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

                energyHCal += pCaloHit->GetHadronicEnergy();
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

// Particle id function pointers
ParticleIdFunction *ParticleIdHelper::m_pPhotonFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pPhotonFullFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pElectronFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pElectronFullFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pMuonFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pMuonFullFunction = NULL;

// Parameter default values
float ParticleIdHelper::m_showerProfileBinWidth = 0.5f;
unsigned int ParticleIdHelper::m_showerProfileNBins = 100;
float ParticleIdHelper::m_showerProfileMinCosAngle = 0.3f;
float ParticleIdHelper::m_showerProfileCriticalEnergy = 0.08f;
float ParticleIdHelper::m_showerProfileParameter0 = 1.25f;
float ParticleIdHelper::m_showerProfileParameter1 = 0.5f;
float ParticleIdHelper::m_showerProfileMaxDifference = 0.1f;

float ParticleIdHelper::m_photonIdMipCut_0 = 0.9f;
float ParticleIdHelper::m_photonIdMipCutEnergy_1 = 15.f;
float ParticleIdHelper::m_photonIdMipCut_1 = 0.3f;
float ParticleIdHelper::m_photonIdMipCutEnergy_2 = 7.5f;
float ParticleIdHelper::m_photonIdMipCut_2 = 0.4f;
float ParticleIdHelper::m_photonIdMipCutEnergy_3 = 3.f;
float ParticleIdHelper::m_photonIdMipCut_3 = 0.6f;
float ParticleIdHelper::m_photonIdMipCutEnergy_4 = 1.5f;
float ParticleIdHelper::m_photonIdMipCut_4 = 0.7f;
float ParticleIdHelper::m_photonIdDCosRCutEnergy = 1.5f;
float ParticleIdHelper::m_photonIdDCosRLowECut = 0.94f;
float ParticleIdHelper::m_photonIdDCosRHighECut = 0.95f;
float ParticleIdHelper::m_photonIdRmsCutEnergy = 40.f;
float ParticleIdHelper::m_photonIdRmsLowECut = 40.f;
float ParticleIdHelper::m_photonIdRmsHighECut = 90.f;
float ParticleIdHelper::m_photonIdEndCapZSeparation = 50.f;
float ParticleIdHelper::m_photonIdRadiationLengthsCut = 10.f;
float ParticleIdHelper::m_photonIdShowerMaxCut1_0 = 0.f;
float ParticleIdHelper::m_photonIdShowerMaxCut2 = 40.f;
float ParticleIdHelper::m_photonIdShowerMaxCut1Energy_1 = 3.f;
float ParticleIdHelper::m_photonIdShowerMaxCut1_1 = 3.f;
float ParticleIdHelper::m_photonIdShowerMaxCut1Energy_2 = 1.5f;
float ParticleIdHelper::m_photonIdShowerMaxCut1_2 = 1.f;
float ParticleIdHelper::m_photonIdLayer90Cut1 = 5.f;
float ParticleIdHelper::m_photonIdLayer90Cut2Energy = 40.f;
float ParticleIdHelper::m_photonIdLayer90LowECut2 = 40.f;
float ParticleIdHelper::m_photonIdLayer90HighECut2 = 50.f;
int ParticleIdHelper::m_photonIdLayer90MaxLayersFromECal = 10;

unsigned int ParticleIdHelper::m_electronIdMaxInnerLayer = 4;
float ParticleIdHelper::m_electronIdMaxEnergy = 5.f;
float ParticleIdHelper::m_electronIdMaxProfileStart = 4.5f;
float ParticleIdHelper::m_electronIdMaxProfileDiscrepancy = 0.6f;
float ParticleIdHelper::m_electronIdProfileDiscrepancyForAutoId = 0.5f;
float ParticleIdHelper::m_electronIdMaxResidualEOverP = 0.2f;

unsigned int ParticleIdHelper::m_muonIdMaxInnerLayer = 10;
float ParticleIdHelper::m_muonIdMinTrackEnergy = 2.5f;
unsigned int ParticleIdHelper::m_muonIdMinECalLayers = 20;
unsigned int ParticleIdHelper::m_muonIdMinHCalLayers = 20;
bool ParticleIdHelper::m_muonIdShouldPerformGapCheck = true;
unsigned int ParticleIdHelper::m_muonIdMinHCalLayersForGapCheck = 4;
unsigned int ParticleIdHelper::m_muonIdMinMuonHitsForGapCheck = 3;
float ParticleIdHelper::m_muonIdECalEnergyCut0 = 1.f;
float ParticleIdHelper::m_muonIdECalEnergyCut1 = 0.05f;
float ParticleIdHelper::m_muonIdHCalEnergyCut0 = 5.f;
float ParticleIdHelper::m_muonIdHCalEnergyCut1 = 0.15f;
unsigned int ParticleIdHelper::m_muonIdMinECalLayersForFit = 10;
unsigned int ParticleIdHelper::m_muonIdMinHCalLayersForFit = 10;
unsigned int ParticleIdHelper::m_muonIdMinMuonLayersForFit = 4;
unsigned int ParticleIdHelper::m_muonIdECalFitInnerLayer = 1;
unsigned int ParticleIdHelper::m_muonIdECalFitOuterLayer = 30;
unsigned int ParticleIdHelper::m_muonIdHCalFitInnerLayer = 31;
unsigned int ParticleIdHelper::m_muonIdHCalFitOuterLayer = 79;
float ParticleIdHelper::m_muonIdECalRmsCut0 = 20.f;
float ParticleIdHelper::m_muonIdECalRmsCut1 = 0.2f;
float ParticleIdHelper::m_muonIdECalMaxRmsCut = 50.f;
float ParticleIdHelper::m_muonIdHCalRmsCut0 = 40.f;
float ParticleIdHelper::m_muonIdHCalRmsCut1 = 0.1f;
float ParticleIdHelper::m_muonIdHCalMaxRmsCut = 80.f;
float ParticleIdHelper::m_muonIdECalMipFractionCut0 = 0.8f;
float ParticleIdHelper::m_muonIdECalMipFractionCut1 = 0.05f;
float ParticleIdHelper::m_muonIdECalMaxMipFractionCut = 0.6f;
float ParticleIdHelper::m_muonIdHCalMipFractionCut0 = 0.8f;
float ParticleIdHelper::m_muonIdHCalMipFractionCut1 = 0.02f;
float ParticleIdHelper::m_muonIdHCalMaxMipFractionCut = 0.75f;
float ParticleIdHelper::m_muonIdECalHitsPerLayerCut0 = 2.f;
float ParticleIdHelper::m_muonIdECalHitsPerLayerCut1 = 0.02f;
float ParticleIdHelper::m_muonIdECalMaxHitsPerLayerCut = 5.f;
float ParticleIdHelper::m_muonIdHCalHitsPerLayerCut0 = 1.5f;
float ParticleIdHelper::m_muonIdHCalHitsPerLayerCut1 = 0.05f;
float ParticleIdHelper::m_muonIdHCalMaxHitsPerLayerCut = 8.f;
float ParticleIdHelper::m_muonIdCurlingTrackEnergy = 4.f;
float ParticleIdHelper::m_muonIdInBarrelHitFraction = 0.05f;
float ParticleIdHelper::m_muonIdTightMipFractionCut = 0.85f;
float ParticleIdHelper::m_muonIdTightMipFractionECalCut = 0.8f;
float ParticleIdHelper::m_muonIdTightMipFractionHCalCut = 0.875f;
unsigned int ParticleIdHelper::m_muonIdMinMuonHitsCut = 2;
unsigned int ParticleIdHelper::m_muonIdMinMuonTrackSegmentHitsCut = 8;
float ParticleIdHelper::m_muonIdMuonRmsCut = 25.f;
float ParticleIdHelper::m_muonIdMaxMuonHitsCut0 = 0.f;
float ParticleIdHelper::m_muonIdMaxMuonHitsCut1 = 0.2f;
float ParticleIdHelper::m_muonIdMaxMuonHitsCutMinValue = 30.f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleIdHelper::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Shower profile settings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerProfileBinWidth", m_showerProfileBinWidth));

    if (0.f == m_showerProfileBinWidth)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerProfileNBins", m_showerProfileNBins));

    if (0 == m_showerProfileNBins)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerProfileMinCosAngle", m_showerProfileMinCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerProfileCriticalEnergy", m_showerProfileCriticalEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerProfileParameter0", m_showerProfileParameter0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerProfileParameter1", m_showerProfileParameter1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerProfileMaxDifference", m_showerProfileMaxDifference));

    // Electromagnetic shower id settings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_0", m_photonIdMipCut_0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCutEnergy_1", m_photonIdMipCutEnergy_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_1", m_photonIdMipCut_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCutEnergy_2", m_photonIdMipCutEnergy_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_2", m_photonIdMipCut_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCutEnergy_3", m_photonIdMipCutEnergy_3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_3", m_photonIdMipCut_3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCutEnergy_4", m_photonIdMipCutEnergy_4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_4", m_photonIdMipCut_4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdDCosRCutEnergy", m_photonIdDCosRCutEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdDCosRLowECut", m_photonIdDCosRLowECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdDCosRHighECut", m_photonIdDCosRHighECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdRmsCutEnergy", m_photonIdRmsCutEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdRmsLowECut", m_photonIdRmsLowECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdRmsHighECut", m_photonIdRmsHighECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdEndCapZSeparation", m_photonIdEndCapZSeparation));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdRadiationLengthsCut", m_photonIdRadiationLengthsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerMaxCut1_0", m_photonIdShowerMaxCut1_0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerMaxCut2", m_photonIdShowerMaxCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerMaxCut1Energy_1", m_photonIdShowerMaxCut1Energy_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerMaxCut1_1", m_photonIdShowerMaxCut1_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerMaxCut1Energy_2", m_photonIdShowerMaxCut1Energy_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerMaxCut1_2", m_photonIdShowerMaxCut1_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90Cut1", m_photonIdLayer90Cut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90Cut2Energy", m_photonIdLayer90Cut2Energy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90LowECut2", m_photonIdLayer90LowECut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90HighECut2", m_photonIdLayer90HighECut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90MaxLayersFromECal", m_photonIdLayer90MaxLayersFromECal));

    // Fast electron id settings
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

    // Fast muon id settings
   PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMaxInnerLayer", m_muonIdMaxInnerLayer));

   PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonIdMinTrackEnergy", m_muonIdMinTrackEnergy));

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

} // namespace pandora
