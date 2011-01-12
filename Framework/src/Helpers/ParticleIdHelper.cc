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

// Particle id function pointers
ParticleIdFunction *ParticleIdHelper::m_pEmShowerFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pEmShowerFullFunction = NULL;
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

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
