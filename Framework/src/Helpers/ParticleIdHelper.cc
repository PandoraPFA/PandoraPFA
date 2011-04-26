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

#include <cmath>
#include <limits>

namespace pandora
{

ParticleIdFunction *ParticleIdHelper::m_pEmShowerFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pEmShowerFullFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pPhotonFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pPhotonFullFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pElectronFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pElectronFullFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pMuonFastFunction = NULL;
ParticleIdFunction *ParticleIdHelper::m_pMuonFullFunction = NULL;

ShowerProfileCalculator *ParticleIdHelper::m_pShowerProfileCalculator = NULL;

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsEmShowerFast(const Cluster *const pCluster)
{
    if (NULL == m_pEmShowerFastFunction)
        return false;

    return (*m_pEmShowerFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsEmShowerFull(const Cluster *const pCluster)
{
    if (NULL == m_pEmShowerFullFunction)
        return false;

    return (*m_pEmShowerFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsPhotonFast(const Cluster *const pCluster)
{
    if (pCluster->IsFixedPhoton())
        return true;

    if (NULL == m_pPhotonFastFunction)
        return false;

    return (*m_pPhotonFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsPhotonFull(const Cluster *const pCluster)
{
    if (pCluster->IsFixedPhoton())
        return true;

    if (NULL == m_pPhotonFullFunction)
        return false;

    return (*m_pPhotonFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsElectronFast(const Cluster *const pCluster)
{
    if (pCluster->IsFixedElectron())
        return true;

    if (NULL == m_pElectronFastFunction)
        return false;

    return (*m_pElectronFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsElectronFull(const Cluster *const pCluster)
{
    if (pCluster->IsFixedElectron())
        return true;

    if (NULL == m_pElectronFullFunction)
        return false;

    return (*m_pElectronFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsMuonFast(const Cluster *const pCluster)
{
    if (pCluster->IsFixedMuon())
        return true;

    if (NULL == m_pMuonFastFunction)
        return false;

    return (*m_pMuonFastFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ParticleIdHelper::IsMuonFull(const Cluster *const pCluster)
{
    if (pCluster->IsFixedMuon())
        return true;

   if (NULL == m_pMuonFullFunction)
        return false;

    return (*m_pMuonFullFunction)(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ParticleIdHelper::CalculateShowerProfile(const Cluster *const pCluster, float &profileStart, float &profileDiscrepancy)
{
    profileStart = std::numeric_limits<float>::max();
    profileDiscrepancy = std::numeric_limits<float>::max();

    if (NULL != m_pShowerProfileCalculator)
        m_pShowerProfileCalculator->CalculateShowerProfile(pCluster, profileStart, profileDiscrepancy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ParticleIdHelper::GetShowerPeaks(const Cluster *const pCluster, const PseudoLayer maxPseudoLayer, ShowerPeakList &showerPeakList)
{
    const int nBins(41);                    // TODO make configurable
    const int nOffsetBins(nBins / 2);
    const float peakThreshold(0.025f);      // TODO make configurable
    const unsigned int maxPeaksToFind(3);   // TODO make configurable

    const CartesianVector innerLayerCentroid(pCluster->GetCentroid(pCluster->GetInnerPseudoLayer()));
    const CartesianVector uAxis(CartesianVector(innerLayerCentroid.GetY(), -innerLayerCentroid.GetX(), 0.f).GetUnitVector());
    const CartesianVector vAxis(uAxis.GetCrossProduct(innerLayerCentroid).GetUnitVector());

    // Create 2d shower profile
    TwoDShowerProfile showerProfile(nBins, ShowerProfile(nBins, ShowerProfileEntry()));
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > maxPseudoLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;
            const float cellSizeZ((BARREL == pCaloHit->GetDetectorRegion()) ? pCaloHit->GetCellSizeU() : pCaloHit->GetCellSizeV());

            if (std::fabs(cellSizeZ) < std::numeric_limits<float>::epsilon())
                continue;

            const CartesianVector hitCoordinates((pCaloHit->GetPositionVector() - innerLayerCentroid) * (1.f / cellSizeZ));
            const float uValue(hitCoordinates.GetDotProduct(uAxis));
            const float vValue(hitCoordinates.GetDotProduct(vAxis));

            const int uBin(static_cast<int>(uValue + 0.5f) + nOffsetBins);
            const int vBin(static_cast<int>(vValue + 0.5f) + nOffsetBins);

            if ((uBin >= 0) && (vBin >= 0) && (uBin < nBins) && (vBin < nBins))
            {
                showerProfile[uBin][vBin].m_energy += pCaloHit->GetElectromagneticEnergy();
                showerProfile[uBin][vBin].m_caloHitList.insert(pCaloHit);
            }
        }
    }

    // Mask low pulse-height regions
    for (int uBin = 0; uBin < nBins; ++uBin)
    {
        for (int vBin = 0; vBin < nBins; ++vBin)
        {
            if (showerProfile[uBin][vBin].m_energy < peakThreshold)
                showerProfile[uBin][vBin].m_isAvailable = false;
        }
    }

    // Search for peaks in profile
    unsigned int nPeaks(0);

    while (true)
    {
        // Identify current peak
        float peakEnergy(0.f);
        CaloHitList peakCaloHitList;

        int peakUBin(0);
        int peakVBin(0);

        for (int uBin = 0; uBin < nBins; ++uBin)
        {
            for (int vBin = 0; vBin < nBins; ++vBin)
            {
                if (showerProfile[uBin][vBin].m_isAvailable && (showerProfile[uBin][vBin].m_energy > peakEnergy))
                {
                    peakEnergy = showerProfile[uBin][vBin].m_energy;
                    peakUBin = uBin;
                    peakVBin = vBin;
                }
            }
        }

        if (peakEnergy < peakThreshold)
            break;

        // Begin to extract peak properties
        nPeaks++;
        peakCaloHitList = showerProfile[peakUBin][peakVBin].m_caloHitList;

        const float uPeakBinDifference(peakUBin - nOffsetBins);
        const float vPeakBinDifference(peakVBin - nOffsetBins);
        float uBar(uPeakBinDifference * peakEnergy);
        float vBar(vPeakBinDifference * peakEnergy);
        float uuBar(uPeakBinDifference * uPeakBinDifference * peakEnergy);
        float vvBar(vPeakBinDifference * vPeakBinDifference * peakEnergy);

        // Examine region around the peak
        typedef std::pair<int, int> BinCoordinates;
        typedef std::vector<BinCoordinates> BinCoordinatesList;

        BinCoordinatesList binCoordinatesList(nBins, BinCoordinates(0, 0));
        binCoordinatesList[0] = BinCoordinates(peakUBin, peakVBin);

        while (true)
        {
            int peakStartBin(0);
            int peakEndBin(0);
            int peakCurrentBin(peakEndBin);

            for (int peakBin = peakStartBin; peakBin <= peakEndBin; ++peakBin)
            {
                const int uCentralBin(binCoordinatesList[peakBin].first);
                const int vCentralBin(binCoordinatesList[peakBin].second);

                if ((uCentralBin < 0) || (vCentralBin < 0) || (uCentralBin >= nBins) || (vCentralBin >= nBins))
                    continue;

                const float centralEnergy(showerProfile[uCentralBin][vCentralBin].m_energy);
                showerProfile[uCentralBin][vCentralBin].m_isAvailable = false;

                const int uStartBin((uCentralBin > 0) ? uCentralBin - 1 : 0);
                const int uEndBin((uCentralBin < nBins - 1) ? uCentralBin + 1 : nBins -1);

                for (int uBin = uStartBin; uBin <= uEndBin; ++uBin)
                {
                    const int vStartBin((vCentralBin > 0) ? vCentralBin - 1 : 0);
                    const int vEndBin((vCentralBin < nBins - 1) ? vCentralBin + 1 : nBins -1);

                    for (int vBin = vStartBin; vBin <= vEndBin; ++vBin)
                    {
                        ShowerProfileEntry &showerProfileEntry(showerProfile[uBin][vBin]);

                        if (showerProfileEntry.m_isAvailable && (showerProfileEntry.m_energy < 2.f * centralEnergy))
                        {
                            const float energy(showerProfileEntry.m_energy);
                            peakEnergy += energy;

                            const float uBinDifference(uBin - nOffsetBins);
                            const float vBinDifference(vBin - nOffsetBins);
                            uBar += uBinDifference * energy;
                            vBar += vBinDifference * energy;
                            uuBar += uBinDifference * uBinDifference * energy;
                            vvBar += vBinDifference * vBinDifference * energy;

                            peakCaloHitList.insert(showerProfileEntry.m_caloHitList.begin(), showerProfileEntry.m_caloHitList.end());
                            showerProfile[uBin][vBin].m_isAvailable = false;

                            peakCurrentBin++;
                            binCoordinatesList[peakCurrentBin] = BinCoordinates(uBin, vBin);
                        }
                    }
                }
            }

            if (peakCurrentBin == peakEndBin)
                break;

            peakStartBin = peakEndBin + 1;
            peakEndBin = peakCurrentBin;
        }

        // Store peak properties
        uBar /= peakEnergy;
        vBar /= peakEnergy;
        uuBar /= peakEnergy;
        vvBar /= peakEnergy;
        const float meanSquareDeviation(uuBar + vvBar - uBar * uBar - vBar * vBar);

        if (meanSquareDeviation > 0.f)
        {
            const float peakRms((meanSquareDeviation > 0.f) ? std::sqrt(meanSquareDeviation) : std::numeric_limits<float>::max());
            showerPeakList.push_back(ShowerPeak(peakEnergy, peakRms, peakCaloHitList));
        }

        if (nPeaks > maxPeaksToFind)
            break;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleIdHelper::SetShowerProfileCalculator(ShowerProfileCalculator *pShowerProfileCalculator)
{
    delete m_pShowerProfileCalculator;
    m_pShowerProfileCalculator = pShowerProfileCalculator;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ParticleIdHelper::ReadSettings(const TiXmlHandle *const pXmlHandle)
{
    // Read main particle id helper settings
    /*TiXmlElement *pXmlElement(pXmlHandle->FirstChild("ParticleIdHelper").Element());

    if (NULL != pXmlElement)
    {
        const TiXmlHandle xmlHandle(pXmlElement);
    }*/

    // Read shower profile calculator settings
    TiXmlElement *pShowerProfileXmlElement(pXmlHandle->FirstChild("ShowerProfileCalculator").Element());

    if ((NULL != pShowerProfileXmlElement) && (NULL != m_pShowerProfileCalculator))
    {
        const TiXmlHandle xmlHandle(pShowerProfileXmlElement);
        m_pShowerProfileCalculator->ReadSettings(&xmlHandle);
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
