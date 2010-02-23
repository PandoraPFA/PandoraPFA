/**
 *  @file   PandoraPFANew/src/Helpers/ParticleIdHelper.cc
 * 
 *  @brief  Implementation of the particle id helper class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"
#include "Helpers/ParticleIdHelper.h"

#include "Objects/Cluster.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>

namespace pandora
{

StatusCode ParticleIdHelper::CalculateShowerProfile(Cluster *const pCluster, float &showerProfileStart, float &showerProfileDiscrepancy)
{
    // 1. Construct cluster profile.
    const float clusterEnergy(pCluster->GetElectromagneticEnergy());

    if(clusterEnergy <= 0.f || (pCluster->GetNCaloHits() < 1))
        return STATUS_CODE_INVALID_PARAMETER;

    // Profile settings
    static const PandoraSettings *const pPandoraSettings(PandoraSettings::GetInstance());

    static const float binWidth(pPandoraSettings->GetShowerProfileBinWidth());
    static const unsigned int nBins(pPandoraSettings->GetShowerProfileNBins());
    unsigned int profileEndBin(0);

    float profile[nBins];
    for(unsigned int iBin = 0; iBin < nBins; ++iBin)
    {
        profile[iBin] = 0.f;
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
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = innerPseudoLayer; iLayer <= nECalLayers; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if ((orderedCaloHitList.end() == iter) || (iter->second->empty()))
        {
            nRadiationLengths += nRadiationLengthsInLastLayer;
            profileEndBin = std::min(static_cast<unsigned int>(nRadiationLengths / binWidth), nBins);
            continue;
        }

        // Extract information from calo hits
        float energyInLayer(0.f);
        float nRadiationLengthsInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const float openingAngle((*hitIter)->GetNormalVector().GetOpeningAngle(clusterDirection));
            float cosOpeningAngle(std::fabs(std::cos(openingAngle)));

            static const float minCosOpeningAngle(pPandoraSettings->GetShowerProfileMinCosAngle());
            cosOpeningAngle = std::max(cosOpeningAngle, minCosOpeningAngle);

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
        const float endPosition(nRadiationLengths / binWidth);
        const unsigned int endBin(std::min(static_cast<unsigned int>(endPosition), nBins));

        const float deltaPosition(nRadiationLengthsInLayer / binWidth);

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

            profile[iBin] += energyInLayer * (delta / deltaPosition);
        }

        profileEndBin = endBin;
    }

    if (eCalEnergy <= 0.f)
        return STATUS_CODE_FAILURE;

    // 2. Construct expected cluster profile
    static const float criticalEnergy(pPandoraSettings->GetShowerProfileCriticalEnergy());
    static const float parameter0(pPandoraSettings->GetShowerProfileParameter0());
    static const float parameter1(pPandoraSettings->GetShowerProfileParameter1());

    const double a(parameter0 + parameter1 * std::log(clusterEnergy / criticalEnergy));

    const double gammaA(std::exp(gamma(a)));

    float t(0.);
    float expectedProfile[nBins];

    for (unsigned int iBin = 0; iBin < nBins; ++iBin)
    {
        t += binWidth;
        expectedProfile[iBin] = static_cast<float>(clusterEnergy / 2. * std::pow(t / 2.f, static_cast<float>(a - 1.)) *
            std::exp(-t / 2.) * binWidth / gammaA);
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

        static const float maxDifference(pPandoraSettings->GetShowerProfileMaxDifference());

        if (profileDifference - minProfileDifference > maxDifference)
            break;
    }

    showerProfileStart =  binOffsetAtMinDifference * binWidth;
    showerProfileDiscrepancy = minProfileDifference / eCalEnergy;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
