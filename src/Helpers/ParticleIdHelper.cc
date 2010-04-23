/**
 *  @file   PandoraPFANew/src/Helpers/ParticleIdHelper.cc
 * 
 *  @brief  Implementation of the particle id helper class.
 * 
 *  $Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Helpers/ParticleIdHelper.h"

#include <cmath>
#include <limits>

namespace pandora
{

StatusCode ParticleIdHelper::CalculateShowerProfile(Cluster *const pCluster, float &showerProfileStart, float &showerProfileDiscrepancy)
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
            profileEndBin = std::min(static_cast<unsigned int>(nRadiationLengths / m_showerProfileBinWidth), m_showerProfileNBins);
            continue;
        }

        // Extract information from calo hits
        float energyInLayer(0.f);
        float nRadiationLengthsInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const float openingAngle((*hitIter)->GetNormalVector().GetOpeningAngle(clusterDirection));
            float cosOpeningAngle(std::fabs(std::cos(openingAngle)));

            // ATTN: This direction measure introduces a difference wrt old Pandora
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
        const unsigned int endBin(std::min(static_cast<unsigned int>(endPosition), m_showerProfileNBins));

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

        profileEndBin = endBin;
    }

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

bool ParticleIdHelper::IsPhotonFast(Cluster *const pCluster)
{
    // Already flagged as a photon by full photon id algorithm? - overrides fast photon id
    if (pCluster->IsPhoton())
        return true;

    // Reject empty clusters, such as track seeds
    if (0 == pCluster->GetNCaloHits())
        return false;

    // Cluster with associated tracks is not a photon
    if (!pCluster->GetAssociatedTrackList().empty())
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
        return false;;

    if (layer90 > static_cast<int>(nECalLayers) + m_photonIdLayer90MaxLayersFromECal)
        return false;

    // Anything remaining at this point is classed as a photon
    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsPhotonFull(Cluster *const pCluster)
{
    // ATTN This is only a simple placeholder photon id function. The full photon id is a likelihood-based method, to be extracted
    //      from the Photon Clustering algorithm.

    const unsigned int nCaloHits(pCluster->GetNCaloHits());

    if ((nCaloHits <= m_photonFullMinCaloHits) || (nCaloHits >= m_photonFullMaxCaloHits))
        return false;

    const float electromagneticEnergy(pCluster->GetElectromagneticEnergy());

    if (electromagneticEnergy > m_photonFullMaxEnergy)
        return false;

    if (pCluster->GetInnerPseudoLayer() >= m_photonFullMaxInnerLayer)
        return false;

    const ClusterHelper::ClusterFitResult &clusterFitResult(pCluster->GetFitToAllHitsResult());

    if (clusterFitResult.IsFitSuccessful())
    {
        const float radialDirectionCosine(clusterFitResult.GetRadialDirectionCosine());

        if (radialDirectionCosine > m_photonFullMaxDCosR)
            return true;

        if ((electromagneticEnergy < m_photonFullLowEnergyCut) && (radialDirectionCosine > m_photonFullLowEnergyMaxDCosR))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsElectronFast(Cluster *const pCluster)
{
    const TrackList &associatedTrackList(pCluster->GetAssociatedTrackList());

    if (associatedTrackList.empty())
        return false;

    const float electromagneticEnergy(pCluster->GetElectromagneticEnergy());

    if (!pCluster->IsPhotonFast() && ((pCluster->GetInnerPseudoLayer() > m_electronIdMaxInnerLayer) || (electromagneticEnergy > m_electronIdMaxEnergy)))
        return false;

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
float ParticleIdHelper::m_photonIdRmsHighECut = 50.f;
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

unsigned int ParticleIdHelper::m_photonFullMinCaloHits = 0;
unsigned int ParticleIdHelper::m_photonFullMaxCaloHits = 25;
float ParticleIdHelper::m_photonFullMaxEnergy = 1.f;
unsigned int ParticleIdHelper::m_photonFullMaxInnerLayer = 14;
float ParticleIdHelper::m_photonFullMaxDCosR = 0.9f;
float ParticleIdHelper::m_photonFullLowEnergyCut = 0.5f;
float ParticleIdHelper::m_photonFullLowEnergyMaxDCosR = 0.8f;

unsigned int ParticleIdHelper::m_electronIdMaxInnerLayer = 4;
float ParticleIdHelper::m_electronIdMaxEnergy = 5.f;
float ParticleIdHelper::m_electronIdMaxProfileStart = 2.5f;
float ParticleIdHelper::m_electronIdMaxProfileDiscrepancy = 0.6f;
float ParticleIdHelper::m_electronIdProfileDiscrepancyForAutoId = 0.5f;
float ParticleIdHelper::m_electronIdMaxResidualEOverP = 0.2f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleIdHelper::ReadSettings(const TiXmlHandle *const pXmlHandle)
{
    // Shower profile settings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileBinWidth", m_showerProfileBinWidth));

    if (0.f == m_showerProfileBinWidth)
        return STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileNBins", m_showerProfileNBins));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileMinCosAngle", m_showerProfileMinCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileCriticalEnergy", m_showerProfileCriticalEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileParameter0", m_showerProfileParameter0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileParameter1", m_showerProfileParameter1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileMaxDifference", m_showerProfileMaxDifference));

    // Fast photon id settings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCut_0", m_photonIdMipCut_0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCutEnergy_1", m_photonIdMipCutEnergy_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCut_1", m_photonIdMipCut_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCutEnergy_2", m_photonIdMipCutEnergy_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCut_2", m_photonIdMipCut_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCutEnergy_3", m_photonIdMipCutEnergy_3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCut_3", m_photonIdMipCut_3));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCutEnergy_4", m_photonIdMipCutEnergy_4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdMipCut_4", m_photonIdMipCut_4));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdDCosRCutEnergy", m_photonIdDCosRCutEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdDCosRLowECut", m_photonIdDCosRLowECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdDCosRHighECut", m_photonIdDCosRHighECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdRmsCutEnergy", m_photonIdRmsCutEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdRmsLowECut", m_photonIdRmsLowECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdRmsHighECut", m_photonIdRmsHighECut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdEndCapZSeparation", m_photonIdEndCapZSeparation));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdRadiationLengthsCut", m_photonIdRadiationLengthsCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdShowerMaxCut1_0", m_photonIdShowerMaxCut1_0));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdShowerMaxCut2", m_photonIdShowerMaxCut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdShowerMaxCut1Energy_1", m_photonIdShowerMaxCut1Energy_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdShowerMaxCut1_1", m_photonIdShowerMaxCut1_1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdShowerMaxCut1Energy_2", m_photonIdShowerMaxCut1Energy_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdShowerMaxCut1_2", m_photonIdShowerMaxCut1_2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdLayer90Cut1", m_photonIdLayer90Cut1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdLayer90Cut2Energy", m_photonIdLayer90Cut2Energy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdLayer90LowECut2", m_photonIdLayer90LowECut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdLayer90HighECut2", m_photonIdLayer90HighECut2));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonIdLayer90MaxLayersFromECal", m_photonIdLayer90MaxLayersFromECal));

    // Full photon id settings
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullMinCaloHits", m_photonFullMinCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullMaxCaloHits", m_photonFullMaxCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullMaxEnergy", m_photonFullMaxEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullMaxInnerLayer", m_photonFullMaxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullMaxDCosR", m_photonFullMaxDCosR));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullLowEnergyCut", m_photonFullLowEnergyCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "PhotonFullLowEnergyMaxDCosR", m_photonFullLowEnergyMaxDCosR));

    // Fast electron id settings
   PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronIdMaxInnerLayer", m_electronIdMaxInnerLayer));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronIdMaxEnergy", m_electronIdMaxEnergy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronIdMaxProfileStart", m_electronIdMaxProfileStart));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronIdMaxProfileDiscrepancy", m_electronIdMaxProfileDiscrepancy));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronIdProfileDiscrepancyForAutoId", m_electronIdProfileDiscrepancyForAutoId));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ElectronIdMaxResidualEOverP", m_electronIdMaxResidualEOverP));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
