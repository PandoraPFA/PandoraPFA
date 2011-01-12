/**
 *  @file   PandoraPFANew/Framework/src/Helpers/ParticleIdHelper.cc
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

#include <algorithm>
#include <cmath>
#include <limits>

namespace pandora
{

StatusCode ParticleIdHelper::CalculateShowerProfile(const Cluster *const pCluster, float &showerProfileStart, float &showerProfileDiscrepancy)
{
    // 1. Construct cluster profile.
    const float clusterEnergy(pCluster->GetElectromagneticEnergy() - pCluster->GetIsolatedElectromagneticEnergy());

    if (clusterEnergy <= 0.f || (pCluster->GetNCaloHits() < 1))
        return STATUS_CODE_INVALID_PARAMETER;

    // Extract information from the cluster
    if (GeometryHelper::GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) > FINE)
        return STATUS_CODE_NOT_FOUND;

    const CartesianVector &clusterDirection(pCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pCluster->GetFitToAllHitsResult().GetDirection() : pCluster->GetInitialDirection());

    // Initialize profile
    FloatVector profile;
    for (unsigned int iBin = 0; iBin < m_showerProfileNBins; ++iBin)
    {
        profile.push_back(0.f);
    }

    // Examine layers to construct profile
    float eCalEnergy(0.f), nRadiationLengths(0.f), nRadiationLengthsInLastLayer(0.f);
    unsigned int profileEndBin(0);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    const PseudoLayer innerPseudoLayer(pCluster->GetInnerPseudoLayer());
    static const PseudoLayer firstPseudoLayer(GeometryHelper::GetInstance()->GetPseudoLayerAtIp());

    for (PseudoLayer iLayer = innerPseudoLayer, outerPseudoLayer = pCluster->GetOuterPseudoLayer(); iLayer <= outerPseudoLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if ((orderedCaloHitList.end() == iter) || (iter->second->empty()))
        {
            nRadiationLengths += nRadiationLengthsInLastLayer;
            continue;
        }

        // Extract information from calo hits
        bool isFineGranularity(true);
        float energyInLayer(0.f);
        float nRadiationLengthsInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if (GeometryHelper::GetHitTypeGranularity((*hitIter)->GetHitType()) > FINE)
            {
                isFineGranularity = false;
                break;
            }

            float cosOpeningAngle(std::fabs((*hitIter)->GetCellNormalVector().GetCosOpeningAngle(clusterDirection)));
            cosOpeningAngle = std::max(cosOpeningAngle, m_showerProfileMinCosAngle);

            energyInLayer += (*hitIter)->GetElectromagneticEnergy();
            nRadiationLengthsInLayer += (*hitIter)->GetNCellRadiationLengths() / cosOpeningAngle;
        }

        if (!isFineGranularity)
            break;

        eCalEnergy += energyInLayer;
        nRadiationLengthsInLayer /= static_cast<float>(iter->second->size());
        nRadiationLengthsInLastLayer = nRadiationLengthsInLayer;
        nRadiationLengths += nRadiationLengthsInLayer;

        // Account for layers before start of cluster
        if (innerPseudoLayer == iLayer)
            nRadiationLengths *= static_cast<float>(innerPseudoLayer + 1 - firstPseudoLayer);

        // Finally, create the profile
        const float endPosition(nRadiationLengths / m_showerProfileBinWidth);
        const unsigned int endBin(std::min(static_cast<unsigned int>(endPosition), m_showerProfileNBins - 1));

        const float deltaPosition(nRadiationLengthsInLayer / m_showerProfileBinWidth);

        const float startPosition(endPosition - deltaPosition);
        const unsigned int startBin(static_cast<unsigned int>(startPosition));

        for (unsigned int iBin = startBin; iBin <= endBin; ++iBin)
        {
            float delta(1.f);

            if (startBin == iBin)
            {
                delta -= startPosition - startBin;
            }
            else if (endBin == iBin)
            {
                delta -= 1.f - endPosition + endBin;
            }

            profile[iBin] += energyInLayer * (delta / deltaPosition);
        }
    }

    profileEndBin = std::min(static_cast<unsigned int>(nRadiationLengths / m_showerProfileBinWidth), m_showerProfileNBins);

    if (eCalEnergy <= 0.f)
        return STATUS_CODE_FAILURE;

    // 2. Construct expected cluster profile
    const double a(m_showerProfileParameter0 + m_showerProfileParameter1 * std::log(clusterEnergy / m_showerProfileCriticalEnergy));
#ifdef __GNUC__
    const double gammaA(std::exp(lgamma(a)));
#else
    const double gammaA(0.);
    std::cout << " PandoraPFANew, ParticleIdHelper: TODO - implement gamma function " << std::endl;
    throw StatusCodeException(STATUS_CODE_FAILURE);
#endif

    float t(0.f);
    FloatVector expectedProfile;
    for (unsigned int iBin = 0; iBin < m_showerProfileNBins; ++iBin)
    {
        t += m_showerProfileBinWidth;
        expectedProfile.push_back(static_cast<float>(clusterEnergy / 2. * std::pow(t / 2.f, static_cast<float>(a - 1.)) *
            std::exp(-t / 2.) * m_showerProfileBinWidth / gammaA));
    }

    // 3. Compare the cluster profile with the expected profile
    unsigned int binOffsetAtMinDifference(0);
    float minProfileDifference(std::numeric_limits<float>::max());

    for (unsigned int iBinOffset = 0; iBinOffset < profileEndBin; ++iBinOffset)
    {
        float profileDifference(0.);

        for (unsigned int iBin = 0; iBin < profileEndBin; ++iBin)
        {
            if (iBin < iBinOffset)
            {
                profileDifference += profile[iBin];
            }
            else
            {
                profileDifference += std::fabs(expectedProfile[iBin - iBinOffset] - profile[iBin]);
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

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsElectromagneticShower(const Cluster *const pCluster)
{
    // Reject clusters starting outside inner fine granularity detectors
    if (GeometryHelper::GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) > FINE)
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
    static const PseudoLayer firstPseudoLayer(GeometryHelper::GetInstance()->GetPseudoLayerAtIp());

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
            cosOpeningAngle = std::max(cosOpeningAngle, m_photonIdMinCosAngle);

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

            if (nRadiationLengths > m_photonIdMaxInnerLayerRadLengths)
                return false;
        }

        // Cut on number of radiation lengths before longitudinal layer90
        if (!foundLayer90 && (layer90EnergySum > 0.9f * totalElectromagneticEnergy))
        {
            foundLayer90 = true;

            if ((nRadiationLengths < m_photonIdMinLayer90RadLengths) || (nRadiationLengths > m_photonIdMaxLayer90RadLengths))
                return false;
        }

        // Identify number of radiation lengths before shower max layer
        if (energyInLayer > maxEnergyInlayer)
        {
            showerMaxRadLengths = nRadiationLengths;
            maxEnergyInlayer = energyInLayer;
        }

        // Count energy above specified "high" number of radiation lengths
        if (nRadiationLengths > m_photonIdHighRadLengths)
        {
            energyAboveHighRadLengths += energyInLayer;
        }
    }

    // Cut on longitudinal shower profile properties
    if ((showerMaxRadLengths < m_photonIdMinShowerMaxRadLengths) || (showerMaxRadLengths > m_photonIdMaxShowerMaxRadLengths))
        return false;

    if (energyAboveHighRadLengths > m_photonIdMaxHighRadLengthEnergyFraction * totalElectromagneticEnergy)
        return false;

    // Cut on transverse shower profile properties
    std::sort(hitEnergyDistanceVector.begin(), hitEnergyDistanceVector.end(), ParticleIdHelper::SortHitsByDistance);
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

    if (radial90 > m_photonIdMaxRadial90)
        return false;

    // Anything remaining at this point is classed as an electromagnetic shower
    return true;
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
float ParticleIdHelper::m_photonIdMinCosAngle = 0.3f;
float ParticleIdHelper::m_photonIdMaxInnerLayerRadLengths = 10.f;
float ParticleIdHelper::m_photonIdMinLayer90RadLengths = 4.f;
float ParticleIdHelper::m_photonIdMaxLayer90RadLengths = 30.f;
float ParticleIdHelper::m_photonIdMinShowerMaxRadLengths = 0.f;
float ParticleIdHelper::m_photonIdMaxShowerMaxRadLengths = 25.f;
float ParticleIdHelper::m_photonIdHighRadLengths = 40.f;
float ParticleIdHelper::m_photonIdMaxHighRadLengthEnergyFraction = 0.04f;
float ParticleIdHelper::m_photonIdMaxRadial90 = 40.f;

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
        "PhotonIdMinCosAngle", m_photonIdMinCosAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMaxInnerLayerRadLengths", m_photonIdMaxInnerLayerRadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMinLayer90RadLengths", m_photonIdMinLayer90RadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMaxLayer90RadLengths", m_photonIdMaxLayer90RadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMinShowerMaxRadLengths", m_photonIdMinShowerMaxRadLengths));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMaxShowerMaxRadLengths", m_photonIdMaxShowerMaxRadLengths));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
