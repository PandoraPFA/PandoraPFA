/**
 *  @file   PandoraPFANew/src/Helpers/CaloHitHelper.cc
 * 
 *  @brief  Implementation of the calo hit helper class.
 * 
 *  $Log: $
 */

#include "Helpers/CaloHitHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/OrderedCaloHitList.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>

namespace pandora
{

bool CaloHitHelper::m_isReclustering = false;

CaloHitHelper::CaloHitUsageMap *CaloHitHelper::m_pCurrentUsageMap = NULL;

CaloHitHelper::NameToCaloHitUsageMap CaloHitHelper::m_nameToCaloHitUsageMap;

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitHelper::IsCaloHitAvailable(CaloHit *const pCaloHit)
{
    if (!m_isReclustering)
        return pCaloHit->m_isAvailable;

    CaloHitUsageMap::iterator usageMapIter = m_pCurrentUsageMap->find(pCaloHit);

    if ((m_pCurrentUsageMap->end()) == usageMapIter || !usageMapIter->second)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitHelper::AreCaloHitsAvailable(const CaloHitVector &caloHitVector)
{
    if (!m_isReclustering)
    {
        for (CaloHitVector::const_iterator iter = caloHitVector.begin(), iterEnd = caloHitVector.end(); iter != iterEnd; ++iter)
        {
            if (!(*iter)->m_isAvailable)
                return false;
        }

        return true;
    }

    for (CaloHitVector::const_iterator iter = caloHitVector.begin(), iterEnd = caloHitVector.end(); iter != iterEnd; ++iter)
    {
        CaloHitUsageMap::iterator usageMapIter = m_pCurrentUsageMap->find(*iter);

        if ((m_pCurrentUsageMap->end()) == usageMapIter || !usageMapIter->second)
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CaloHitHelper::GetDensityWeightContribution(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    static const float caloHitMaxSeparation(PandoraSettings::GetInstance()->GetCaloHitMaxSeparation());
    static const float densityWeightPower(static_cast<float>(PandoraSettings::GetInstance()->GetDensityWeightPower()));

    float densityWeightContribution = 0.;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const float positionMagnitude(positionVector.GetMagnitude());

    for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());
        const CartesianVector crossProduct(positionVector.GetCrossProduct(positionDifference));

        if (positionDifference.GetMagnitude() > caloHitMaxSeparation)
            continue;

        const float rN(pow(crossProduct.GetMagnitude() / positionMagnitude, densityWeightPower));

        if (0 == rN)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        densityWeightContribution += (100.f / rN);
    }

    return densityWeightContribution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CaloHitHelper::GetSurroundingEnergyContribution(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    static const float caloHitMaxSeparation(PandoraSettings::GetInstance()->GetCaloHitMaxSeparation());

    float surroundingEnergyContribution = 0;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const bool isHitInBarrelRegion(pCaloHit->GetDetectorRegion() == BARREL);

    for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());

        if (positionDifference.GetMagnitude() > caloHitMaxSeparation)
            continue;

        if(isHitInBarrelRegion)
        {
            const float dZ(fabs(positionDifference.GetZ()));
            const float dPhi(std::sqrt(pow(positionDifference.GetX(), 2) + pow(positionDifference.GetY(), 2)));

            if( (dZ < (1.5 * pCaloHit->GetCellSizeU())) && (dPhi < (1.5 * pCaloHit->GetCellSizeV())) )
                surroundingEnergyContribution += (*iter)->GetHadronicEnergy();
        }
        else
        {
            const float dX(fabs(positionDifference.GetX()));
            const float dY(fabs(positionDifference.GetY()));

            if( (dX < (1.5 * pCaloHit->GetCellSizeU())) && (dY < (1.5 * pCaloHit->GetCellSizeV())) )
                surroundingEnergyContribution += (*iter)->GetHadronicEnergy();
        }
    }

    return surroundingEnergyContribution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int CaloHitHelper::IsolationCountNearbyHits(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    static const float caloHitMaxSeparation(PandoraSettings::GetInstance()->GetCaloHitMaxSeparation());
    static const float isolationCutDistanceECal(PandoraSettings::GetInstance()->GetIsolationCutDistanceECal());
    static const float isolationCutDistanceHCal(PandoraSettings::GetInstance()->GetIsolationCutDistanceHCal());

    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const float positionMagnitude(positionVector.GetMagnitude());
    const float isolationCutDistance((pCaloHit->GetHitType() == ECAL) ? isolationCutDistanceECal : isolationCutDistanceHCal);

    unsigned int nearbyHitsFound = 0;

    for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());
        const CartesianVector crossProduct(positionVector.GetCrossProduct(positionDifference));

        if (positionDifference.GetMagnitude() > caloHitMaxSeparation)
            continue;

        if((crossProduct.GetMagnitude() / positionMagnitude) < isolationCutDistance)
            ++nearbyHitsFound;
    }

    return nearbyHitsFound;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int CaloHitHelper::MipCountNearbyHits(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    static const float caloHitMaxSeparation(PandoraSettings::GetInstance()->GetCaloHitMaxSeparation());
    static const float mipNCellsForNearbyHit(PandoraSettings::GetInstance()->GetMipNCellsForNearbyHit() + 0.5f);

    unsigned int nearbyHitsFound = 0;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const bool isHitInBarrelRegion(pCaloHit->GetDetectorRegion() == BARREL);

    for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());

        if (positionDifference.GetMagnitude() > caloHitMaxSeparation)
            continue;

        if(isHitInBarrelRegion)
        {
            const float dZ(fabs(positionDifference.GetZ()));
            const float dPhi(std::sqrt(pow(positionDifference.GetX(), 2) + pow(positionDifference.GetY(), 2)));

            if( (dZ < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeU())) && (dPhi < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeV())) )
                ++nearbyHitsFound;
        }
        else
        {
            const float dX(fabs(positionDifference.GetX()));
            const float dY(fabs(positionDifference.GetY()));

            if( (dX < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeU())) && (dY < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeV())) )
                ++nearbyHitsFound;
        }
    }

    return nearbyHitsFound;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable)
{
    if (!m_isReclustering)
    {
        pCaloHit->m_isAvailable = isAvailable;
        return STATUS_CODE_SUCCESS;
    }

    CaloHitUsageMap::iterator usageMapIter = m_pCurrentUsageMap->find(pCaloHit);

    if (m_pCurrentUsageMap->end() == usageMapIter)
        return STATUS_CODE_NOT_FOUND;

    usageMapIter->second = isAvailable;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::SetCaloHitAvailability(CaloHitVector &caloHitVector, bool isAvailable)
{
    if (!m_isReclustering)
    {
        for (CaloHitVector::iterator iter = caloHitVector.begin(), iterEnd = caloHitVector.end(); iter != iterEnd; ++iter)
            (*iter)->m_isAvailable = isAvailable;

        return STATUS_CODE_SUCCESS;
    }

    for (CaloHitVector::iterator iter = caloHitVector.begin(), iterEnd = caloHitVector.end(); iter != iterEnd; ++iter)
    {
        CaloHitUsageMap::iterator usageMapIter = m_pCurrentUsageMap->find(*iter);

        if (m_pCurrentUsageMap->end() == usageMapIter)
            return STATUS_CODE_NOT_FOUND;

        usageMapIter->second = isAvailable;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::CreateInitialCaloHitUsageMap(const std::string &usageMapName, const OrderedCaloHitList *pOrderedCaloHitList)
{
    if (!m_isReclustering)
        return STATUS_CODE_NOT_ALLOWED;

    m_pCurrentUsageMap = new CaloHitUsageMap;

    if (!m_nameToCaloHitUsageMap.insert(NameToCaloHitUsageMap::value_type(usageMapName, m_pCurrentUsageMap)).second)
        return STATUS_CODE_FAILURE;

    for (OrderedCaloHitList::const_iterator iter = pOrderedCaloHitList->begin(), iterEnd = pOrderedCaloHitList->end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator caloHitIter = iter->second->begin(), caloHitIterEnd = iter->second->end();
            caloHitIter != caloHitIterEnd; ++caloHitIter)
        {
            if (!m_pCurrentUsageMap->insert(CaloHitUsageMap::value_type(*caloHitIter, false)).second)
                return STATUS_CODE_FAILURE;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::CreateAdditionalCaloHitUsageMap(const std::string &usageMapName)
{
    if (!m_isReclustering)
        return STATUS_CODE_NOT_ALLOWED;

    CaloHitUsageMap *pCaloHitUsageMap = new CaloHitUsageMap(*m_pCurrentUsageMap);

    if (!m_nameToCaloHitUsageMap.insert(NameToCaloHitUsageMap::value_type(usageMapName, pCaloHitUsageMap)).second)
        return STATUS_CODE_FAILURE;

    for (CaloHitUsageMap::iterator iter = pCaloHitUsageMap->begin(), iterEnd = pCaloHitUsageMap->end(); iter != iterEnd; ++iter)
        iter->second = true;

    m_pCurrentUsageMap = pCaloHitUsageMap;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::ApplyCaloHitUsageMap(const std::string &usageMapName)
{
    if (!m_isReclustering)
        return STATUS_CODE_NOT_ALLOWED;

    NameToCaloHitUsageMap::const_iterator usageMapIter = m_nameToCaloHitUsageMap.find(usageMapName);

    if (m_nameToCaloHitUsageMap.end() == usageMapIter)
        return STATUS_CODE_NOT_FOUND;

    for (CaloHitUsageMap::const_iterator caloHitIter = usageMapIter->second->begin(), caloHitIterEnd = usageMapIter->second->end();
        caloHitIter != caloHitIterEnd; ++caloHitIter)
    {
        caloHitIter->first->m_isAvailable = caloHitIter->second;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::ClearCaloHitUsageMaps()
{
    if (m_isReclustering)
        return STATUS_CODE_NOT_ALLOWED;

    for (NameToCaloHitUsageMap::iterator iter = m_nameToCaloHitUsageMap.begin(), iterEnd = m_nameToCaloHitUsageMap.end(); iter != iterEnd; ++iter)
        delete iter->second;

    m_nameToCaloHitUsageMap.clear();

    if (!m_nameToCaloHitUsageMap.empty())
        return STATUS_CODE_FAILURE;

    m_pCurrentUsageMap = NULL;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitHelper::CalculateCaloHitProperties(CaloHit *const pCaloHit, const OrderedCaloHitList *const pOrderedCaloHitList)
{
    // Read settings
    static const bool useSimpleIsolationScheme(PandoraSettings::GetInstance()->ShouldUseSimpleIsolationScheme());
    static const float isolationMaxNearbyHits(static_cast<float>(PandoraSettings::GetInstance()->GetIsolationMaxNearbyHits()));
    static const float mipLikeMipCut(PandoraSettings::GetInstance()->GetMipLikeMipCut());
    static const unsigned int mipMaxNearbyHits(PandoraSettings::GetInstance()->GetMipMaxNearbyHits());
    static const PseudoLayer densityWeightNLayers(PandoraSettings::GetInstance()->GetDensityWeightNLayers());
    static const PseudoLayer isolationNLayers(PandoraSettings::GetInstance()->GetIsolationNLayers());

    // Calculate number of adjacent pseudolayers to examine
    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());
    const PseudoLayer densityWeightMaxLayer(pseudoLayer + densityWeightNLayers);
    const PseudoLayer densityWeightMinLayer((pseudoLayer < densityWeightNLayers) ? 0 : pseudoLayer - densityWeightNLayers);
    const PseudoLayer isolationMaxLayer(pseudoLayer + isolationNLayers);
    const PseudoLayer isolationMinLayer((pseudoLayer < isolationNLayers) ? 0 : pseudoLayer - isolationNLayers);

    // Initialize variables
    float densityWeight = 0;
    unsigned int isolationNearbyHits = 0;
    bool isIsolated = true;

    // Loop over adjacent pseudolayers
    for(PseudoLayer iPseudoLayer = std::min(densityWeightMinLayer, isolationMinLayer);
        iPseudoLayer <= std::max(densityWeightMaxLayer, isolationMaxLayer); ++iPseudoLayer)
    {
        OrderedCaloHitList::const_iterator adjacentPseudoLayerIter = pOrderedCaloHitList->find(iPseudoLayer);

        if (pOrderedCaloHitList->end() == adjacentPseudoLayerIter)
            continue;

        CaloHitList *pCaloHitList = adjacentPseudoLayerIter->second;

        // Density weight
        if ((densityWeightMinLayer <= iPseudoLayer) && (densityWeightMaxLayer >= iPseudoLayer))
            densityWeight += CaloHitHelper::GetDensityWeightContribution(pCaloHit, pCaloHitList);

        // IsIsolated flag
        if (!useSimpleIsolationScheme && isIsolated && (isolationMinLayer <= iPseudoLayer) && (isolationMaxLayer >= iPseudoLayer))
        {
            isolationNearbyHits += CaloHitHelper::IsolationCountNearbyHits(pCaloHit, pCaloHitList);
            isIsolated = isolationNearbyHits < isolationMaxNearbyHits;
        }

        // Surrounding energy and possible mip flag
        if (pseudoLayer == iPseudoLayer)
        {
            pCaloHit->AddSurroundingEnergy(CaloHitHelper::GetSurroundingEnergyContribution(pCaloHit, pCaloHitList));

            const CartesianVector &positionVector(pCaloHit->GetPositionVector());

            const float angularCorrection( (BARREL == pCaloHit->GetDetectorRegion()) ?
                positionVector.GetMagnitude() / std::sqrt(pow(positionVector.GetX(), 2) + pow(positionVector.GetY(), 2)) :
                positionVector.GetMagnitude() / std::fabs(positionVector.GetZ()) );

            if (pCaloHit->GetMipEquivalentEnergy() <= (mipLikeMipCut * angularCorrection) &&
                (mipMaxNearbyHits >= CaloHitHelper::MipCountNearbyHits(pCaloHit, pCaloHitList)))
            {
                pCaloHit->SetPossibleMipFlag(true);
            }
        }
    }

    if (!useSimpleIsolationScheme && isIsolated)
        pCaloHit->SetIsolatedFlag(true);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCaloHit->SetDensityWeight(densityWeight));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitHelper::ApplySimpleIsolationScheme(const CaloHitVector &caloHitVector)
{
    static const float isolationDensityWeightCutECal(PandoraSettings::GetInstance()->GetIsolationDensityWeightCutECal());
    static const float isolationDensityWeightCutHCal(PandoraSettings::GetInstance()->GetIsolationDensityWeightCutHCal());

    for (CaloHitVector::const_iterator iter = caloHitVector.begin(), iterEnd = caloHitVector.end(); iter != iterEnd; ++iter)
    {
        CaloHit *pCaloHit = *iter;
        const float isolationDensityWeightCut((ECAL == pCaloHit->GetHitType()) ? isolationDensityWeightCutECal : isolationDensityWeightCutHCal);

        if (pCaloHit->GetDensityWeight() < isolationDensityWeightCut)
            pCaloHit->SetIsolatedFlag(true);
    }
}

} // namespace pandora
