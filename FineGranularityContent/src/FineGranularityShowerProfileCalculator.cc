/**
 *  @file   PandoraPFANew/FineGranularityContent/src/FineGranularityShowerProfileCalculator.cc
 * 
 *  @brief  Implementation of the fine granularity pseudo layer calculator class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"

#include "FineGranularityShowerProfileCalculator.h"

#include <cmath>
#include <limits>

using namespace pandora;

void FineGranularityShowerProfileCalculator::CalculateShowerProfile(const Cluster *const pCluster, float &profileStart, float &profileDiscrepancy) const
{
    // 1. Construct cluster profile.
    const float clusterEnergy(pCluster->GetElectromagneticEnergy() - pCluster->GetIsolatedElectromagneticEnergy());

    if (clusterEnergy <= 0.f || (pCluster->GetNCaloHits() < 1))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    // Extract information from the cluster
    if (GeometryHelper::GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) > FINE)
        return;

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
    static const PseudoLayer firstPseudoLayer(GeometryHelper::GetPseudoLayerAtIp());

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
        throw StatusCodeException(STATUS_CODE_FAILURE);

    // 2. Construct expected cluster profile
    const double a(m_showerProfileParameter0 + m_showerProfileParameter1 * std::log(clusterEnergy / m_showerProfileCriticalEnergy));
#ifdef __GNUC__
    const double gammaA(std::exp(lgamma(a)));
#else
    const double gammaA(0.);
    std::cout << " PandoraPFANew/FineGranularityContent/ShowerProfileCalculator: TODO - implement gamma function " << std::endl;
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

    profileStart =  binOffsetAtMinDifference * m_showerProfileBinWidth;
    profileDiscrepancy = minProfileDifference / eCalEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

// Parameter default values
float FineGranularityShowerProfileCalculator::m_showerProfileBinWidth = 0.5f;
unsigned int FineGranularityShowerProfileCalculator::m_showerProfileNBins = 100;
float FineGranularityShowerProfileCalculator::m_showerProfileMinCosAngle = 0.3f;
float FineGranularityShowerProfileCalculator::m_showerProfileCriticalEnergy = 0.08f;
float FineGranularityShowerProfileCalculator::m_showerProfileParameter0 = 1.25f;
float FineGranularityShowerProfileCalculator::m_showerProfileParameter1 = 0.5f;
float FineGranularityShowerProfileCalculator::m_showerProfileMaxDifference = 0.1f;

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityShowerProfileCalculator::ReadSettings(const TiXmlHandle *const pXmlHandle)
{
    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileBinWidth", m_showerProfileBinWidth));

    if (0.f == m_showerProfileBinWidth)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileNBins", m_showerProfileNBins));

    if (0 == m_showerProfileNBins)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileMinCosAngle", m_showerProfileMinCosAngle));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileCriticalEnergy", m_showerProfileCriticalEnergy));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileParameter0", m_showerProfileParameter0));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileParameter1", m_showerProfileParameter1));

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(*pXmlHandle,
        "ShowerProfileMaxDifference", m_showerProfileMaxDifference));
}
