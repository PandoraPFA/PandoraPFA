/**
 *  @file   PandoraPFANew/src/Helpers/CaloHitHelper.cc
 * 
 *  @brief  Implementation of the calo hit helper class.
 * 
 *  $Log: $
 */

#include "Helpers/CaloHitHelper.h"
#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"

#include <cmath>

namespace pandora
{

unsigned int CaloHitHelper::m_nReclusteringProcesses = 0;
CaloHitHelper::CaloHitUsageMap *CaloHitHelper::m_pCurrentUsageMap = NULL;
CaloHitHelper::UsageMapVector CaloHitHelper::m_parentCaloHitUsageMaps;
CaloHitHelper::NestedUsageMapNames CaloHitHelper::m_nestedUsageMapNames;
CaloHitHelper::NameToCaloHitUsageMap CaloHitHelper::m_nameToCaloHitUsageMap;

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitHelper::IsCaloHitAvailable(CaloHit *const pCaloHit)
{
    if (0 == m_nReclusteringProcesses)
        return pCaloHit->m_isAvailable;

    CaloHitUsageMap::iterator usageMapIter = m_pCurrentUsageMap->find(pCaloHit);

    if ((m_pCurrentUsageMap->end()) == usageMapIter || !usageMapIter->second)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitHelper::AreCaloHitsAvailable(const CaloHitList &caloHitList)
{
    if (0 == m_nReclusteringProcesses)
    {
        for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
        {
            if (!(*iter)->m_isAvailable)
                return false;
        }

        return true;
    }

    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        CaloHitUsageMap::iterator usageMapIter = m_pCurrentUsageMap->find(*iter);

        if ((m_pCurrentUsageMap->end()) == usageMapIter || !usageMapIter->second)
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitHelper::RemoveUnavailableCaloHits(CaloHitList &caloHitList)
{
    for (CaloHitList::iterator iter = caloHitList.begin(); iter != caloHitList.end();)
    {
        if (!CaloHitHelper::IsCaloHitAvailable(*iter))
        {
            caloHitList.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::RemoveUnavailableCaloHits(OrderedCaloHitList &orderedCaloHitList)
{
    CaloHitList unavailableHits;

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if (!CaloHitHelper::IsCaloHitAvailable(*hitIter))
            {
                unavailableHits.insert(*hitIter);
            }
        }
    }

    if (!unavailableHits.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Remove(unavailableHits))
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CaloHitHelper::GetDensityWeightContribution(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    static const float caloHitMaxSeparationSquared(m_caloHitMaxSeparation * m_caloHitMaxSeparation);

    float densityWeightContribution = 0.;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const float positionMagnitude(positionVector.GetMagnitude());

    for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());

        if (positionDifference.GetMagnitudeSquared() > caloHitMaxSeparationSquared)
            continue;

        const CartesianVector crossProduct(positionVector.GetCrossProduct(positionDifference));

        const float r(crossProduct.GetMagnitude() / positionMagnitude);
        float rN(1.);

        for (unsigned int i = 0; i < m_densityWeightPower; ++i)
            rN *= r;

        if (0 == rN)
            continue;

        densityWeightContribution += (m_densityWeightContribution / rN);
    }

    return densityWeightContribution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CaloHitHelper::GetSurroundingEnergyContribution(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    float surroundingEnergyContribution = 0;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const bool isHitInBarrelRegion(pCaloHit->GetDetectorRegion() == BARREL);

    for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());

        if (positionDifference.GetMagnitude() > m_caloHitMaxSeparation)
            continue;

        if(isHitInBarrelRegion)
        {
            const float dX(std::fabs(positionDifference.GetX()));
            const float dY(std::fabs(positionDifference.GetY()));
            const float dZ(std::fabs(positionDifference.GetZ()));
            const float dPhi(std::sqrt(dX * dX + dY * dY));

            if( (dZ < (1.5 * pCaloHit->GetCellSizeU())) && (dPhi < (1.5 * pCaloHit->GetCellSizeV())) )
                surroundingEnergyContribution += (*iter)->GetHadronicEnergy();
        }
        else
        {
            const float dX(std::fabs(positionDifference.GetX()));
            const float dY(std::fabs(positionDifference.GetY()));

            if( (dX < (1.5 * pCaloHit->GetCellSizeU())) && (dY < (1.5 * pCaloHit->GetCellSizeV())) )
                surroundingEnergyContribution += (*iter)->GetHadronicEnergy();
        }
    }

    return surroundingEnergyContribution;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int CaloHitHelper::IsolationCountNearbyHits(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const float positionMagnitude(positionVector.GetMagnitude());
    const float isolationCutDistance((GeometryHelper::GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
        m_isolationCutDistanceFine : m_isolationCutDistanceCoarse);

    unsigned int nearbyHitsFound = 0;

    for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());
        const CartesianVector crossProduct(positionVector.GetCrossProduct(positionDifference));

        if (positionDifference.GetMagnitude() > (m_isolationCaloHitMaxSeparation))
            continue;

        if((crossProduct.GetMagnitude() / positionMagnitude) < isolationCutDistance)
            ++nearbyHitsFound;
    }

    return nearbyHitsFound;
}

//------------------------------------------------------------------------------------------------------------------------------------------

unsigned int CaloHitHelper::MipCountNearbyHits(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList)
{
    static const float mipNCellsForNearbyHit(m_mipNCellsForNearbyHit + 0.5f);

    unsigned int nearbyHitsFound = 0;
    const CartesianVector &positionVector(pCaloHit->GetPositionVector());
    const bool isHitInBarrelRegion(pCaloHit->GetDetectorRegion() == BARREL);

    for(CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference(positionVector - (*iter)->GetPositionVector());

        if (positionDifference.GetMagnitude() > m_caloHitMaxSeparation)
            continue;

        if(isHitInBarrelRegion)
        {
            const float dX(std::fabs(positionDifference.GetX()));
            const float dY(std::fabs(positionDifference.GetY()));
            const float dZ(std::fabs(positionDifference.GetZ()));
            const float dPhi(std::sqrt(dX * dX + dY * dY));

            if( (dZ < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeU())) && (dPhi < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeV())) )
                ++nearbyHitsFound;
        }
        else
        {
            const float dX(std::fabs(positionDifference.GetX()));
            const float dY(std::fabs(positionDifference.GetY()));

            if( (dX < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeU())) && (dY < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeV())) )
                ++nearbyHitsFound;
        }
    }

    return nearbyHitsFound;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
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

StatusCode CaloHitHelper::SetCaloHitAvailability(CaloHitList &caloHitList, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
    {
        for (CaloHitList::iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
            (*iter)->m_isAvailable = isAvailable;

        return STATUS_CODE_SUCCESS;
    }

    for (CaloHitList::iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        CaloHitUsageMap::iterator usageMapIter = m_pCurrentUsageMap->find(*iter);

        if (m_pCurrentUsageMap->end() == usageMapIter)
            return STATUS_CODE_NOT_FOUND;

        usageMapIter->second = isAvailable;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::CreateInitialCaloHitUsageMap(const std::string &usageMapName, const ClusterList &clusterList)
{
    OrderedCaloHitList orderedCaloHitList;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add((*iter)->GetOrderedCaloHitList()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add((*iter)->GetIsolatedCaloHitList()));
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::CreateInitialCaloHitUsageMap(usageMapName, &orderedCaloHitList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::CreateInitialCaloHitUsageMap(const std::string &usageMapName, const OrderedCaloHitList *pOrderedCaloHitList)
{
    if (0 < m_nReclusteringProcesses++)
        m_parentCaloHitUsageMaps.push_back(m_pCurrentUsageMap);

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

    m_nestedUsageMapNames.push_back(new StringVector(1, usageMapName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::CreateAdditionalCaloHitUsageMap(const std::string &usageMapName)
{
    if (0 == m_nReclusteringProcesses)
        return STATUS_CODE_NOT_ALLOWED;

    CaloHitUsageMap *pCaloHitUsageMap = new CaloHitUsageMap(*m_pCurrentUsageMap);

    if (!m_nameToCaloHitUsageMap.insert(NameToCaloHitUsageMap::value_type(usageMapName, pCaloHitUsageMap)).second)
        return STATUS_CODE_FAILURE;

    for (CaloHitUsageMap::iterator iter = pCaloHitUsageMap->begin(), iterEnd = pCaloHitUsageMap->end(); iter != iterEnd; ++iter)
        iter->second = true;

    m_pCurrentUsageMap = pCaloHitUsageMap;
    m_nestedUsageMapNames.back()->push_back(usageMapName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::ApplyCaloHitUsageMap(const std::string &usageMapName)
{
    if (0 == m_nReclusteringProcesses)
        return STATUS_CODE_NOT_ALLOWED;

    NameToCaloHitUsageMap::const_iterator usageMapIter = m_nameToCaloHitUsageMap.find(usageMapName);

    if (m_nameToCaloHitUsageMap.end() == usageMapIter)
        return STATUS_CODE_NOT_FOUND;

    if (1 == m_nReclusteringProcesses--)
    {
        for (CaloHitUsageMap::const_iterator caloHitIter = usageMapIter->second->begin(), caloHitIterEnd = usageMapIter->second->end();
            caloHitIter != caloHitIterEnd; ++caloHitIter)
        {
            caloHitIter->first->m_isAvailable = caloHitIter->second;
        }

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ClearCaloHitUsageMaps());
    }
    else
    {
        m_pCurrentUsageMap = m_parentCaloHitUsageMaps.back();
        m_parentCaloHitUsageMaps.pop_back();

        for (CaloHitUsageMap::const_iterator caloHitIter = usageMapIter->second->begin(), caloHitIterEnd = usageMapIter->second->end();
            caloHitIter != caloHitIterEnd; ++caloHitIter)
        {
            CaloHitUsageMap::iterator currentMapIter = m_pCurrentUsageMap->find(caloHitIter->first);

            if (m_pCurrentUsageMap->end() == currentMapIter)
                return STATUS_CODE_FAILURE;

            currentMapIter->second = caloHitIter->second;
        }

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ClearMostRecentCaloHitUsageMaps());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::ClearCaloHitUsageMaps()
{
    for (NameToCaloHitUsageMap::iterator iter = m_nameToCaloHitUsageMap.begin(), iterEnd = m_nameToCaloHitUsageMap.end(); iter != iterEnd; ++iter)
        delete iter->second;

    for (NestedUsageMapNames::iterator iter = m_nestedUsageMapNames.begin(), iterEnd = m_nestedUsageMapNames.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_nameToCaloHitUsageMap.clear();
    m_parentCaloHitUsageMaps.clear();
    m_nestedUsageMapNames.clear();

    if (!m_nameToCaloHitUsageMap.empty() || !m_parentCaloHitUsageMaps.empty() || !m_nestedUsageMapNames.empty())
        return STATUS_CODE_FAILURE;

    m_pCurrentUsageMap = NULL;
    m_nReclusteringProcesses = 0;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::ClearMostRecentCaloHitUsageMaps()
{
    StringVector *pUsageMapNames(m_nestedUsageMapNames.back());

    for (StringVector::const_iterator iter = pUsageMapNames->begin(), iterEnd = pUsageMapNames->end(); iter != iterEnd; ++iter)
    {
        NameToCaloHitUsageMap::iterator usageMapIter = m_nameToCaloHitUsageMap.find(*iter);

        if (m_nameToCaloHitUsageMap.end() == usageMapIter)
            return STATUS_CODE_NOT_FOUND;

        delete usageMapIter->second;
        m_nameToCaloHitUsageMap.erase(usageMapIter);
    }

    delete pUsageMapNames;
    m_nestedUsageMapNames.pop_back();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitHelper::CalculateCaloHitProperties(CaloHit *const pCaloHit, const OrderedCaloHitList *const pOrderedCaloHitList)
{
    // Calculate number of adjacent pseudolayers to examine
    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());
    const PseudoLayer densityWeightMaxLayer(pseudoLayer + m_densityWeightNLayers);
    const PseudoLayer densityWeightMinLayer((pseudoLayer < m_densityWeightNLayers) ? 0 : pseudoLayer - m_densityWeightNLayers);
    const PseudoLayer isolationMaxLayer(pseudoLayer + m_isolationNLayers);
    const PseudoLayer isolationMinLayer((pseudoLayer < m_isolationNLayers) ? 0 : pseudoLayer - m_isolationNLayers);

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
        if (!m_shouldUseSimpleIsolationScheme && isIsolated && (isolationMinLayer <= iPseudoLayer) && (isolationMaxLayer >= iPseudoLayer))
        {
            isolationNearbyHits += CaloHitHelper::IsolationCountNearbyHits(pCaloHit, pCaloHitList);
            isIsolated = isolationNearbyHits < m_isolationMaxNearbyHits;
        }

        // Surrounding energy and possible mip flag
        if (pseudoLayer == iPseudoLayer)
        {
            pCaloHit->AddSurroundingEnergy(CaloHitHelper::GetSurroundingEnergyContribution(pCaloHit, pCaloHitList));

            if (MUON == pCaloHit->GetHitType())
            {
                pCaloHit->SetPossibleMipFlag(true);
                continue;
            }

            const CartesianVector &positionVector(pCaloHit->GetPositionVector());

            const float x(positionVector.GetX());
            const float y(positionVector.GetY());

            const float angularCorrection( (BARREL == pCaloHit->GetDetectorRegion()) ?
                positionVector.GetMagnitude() / std::sqrt(x * x + y * y) :
                positionVector.GetMagnitude() / std::fabs(positionVector.GetZ()) );

            if ((pCaloHit->GetMipEquivalentEnergy() <= (m_mipLikeMipCut * angularCorrection) || pCaloHit->IsDigital()) &&
                (m_mipMaxNearbyHits >= CaloHitHelper::MipCountNearbyHits(pCaloHit, pCaloHitList)))
            {
                pCaloHit->SetPossibleMipFlag(true);
            }
        }
    }

    if (!m_shouldUseSimpleIsolationScheme && isIsolated)
        pCaloHit->SetIsolatedFlag(true);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCaloHit->SetDensityWeight(densityWeight));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitHelper::ApplySimpleIsolationScheme(const CaloHitVector &caloHitVector)
{
    for (CaloHitVector::const_iterator iter = caloHitVector.begin(), iterEnd = caloHitVector.end(); iter != iterEnd; ++iter)
    {
        CaloHit *pCaloHit = *iter;
        const float isolationDensityWeightCut((GeometryHelper::GetHitTypeGranularity(pCaloHit->GetHitType()) <= FINE) ?
            m_isolationDensityWeightCutFine : m_isolationDensityWeightCutCoarse);

        if (pCaloHit->GetDensityWeight() < isolationDensityWeightCut)
            pCaloHit->SetIsolatedFlag(true);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CaloHitHelper::m_caloHitMaxSeparation = 100.f;
float CaloHitHelper::m_isolationCaloHitMaxSeparation = 1000.f;
float CaloHitHelper::m_densityWeightContribution = 100.f;
unsigned int CaloHitHelper::m_densityWeightPower = 2;
unsigned int CaloHitHelper::m_densityWeightNLayers = 2;
bool CaloHitHelper::m_shouldUseSimpleIsolationScheme = false;
float CaloHitHelper::m_isolationDensityWeightCutFine = 0.1f;
float CaloHitHelper::m_isolationDensityWeightCutCoarse = 0.1f;
unsigned int CaloHitHelper::m_isolationNLayers = 2;
float CaloHitHelper::m_isolationCutDistanceFine = 25.f;
float CaloHitHelper::m_isolationCutDistanceCoarse = 200.f;
unsigned int CaloHitHelper::m_isolationMaxNearbyHits = 2;
float CaloHitHelper::m_mipLikeMipCut = 5.f;
unsigned int CaloHitHelper::m_mipNCellsForNearbyHit = 2;
unsigned int CaloHitHelper::m_mipMaxNearbyHits = 1;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitHelper::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CaloHitMaxSeparation", m_caloHitMaxSeparation));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolationCaloHitMaxSeparation", m_isolationCaloHitMaxSeparation));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DensityWeightContribution", m_densityWeightContribution));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DensityWeightPower", m_densityWeightPower));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DensityWeightNLayers", m_densityWeightNLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseSimpleIsolationScheme", m_shouldUseSimpleIsolationScheme));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolationDensityWeightCutFine", m_isolationDensityWeightCutFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolationDensityWeightCutCoarse", m_isolationDensityWeightCutCoarse));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolationNLayers", m_isolationNLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolationCutDistanceFine", m_isolationCutDistanceFine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolationCutDistanceCoarse", m_isolationCutDistanceCoarse));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolationMaxNearbyHits", m_isolationMaxNearbyHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipLikeMipCut", m_mipLikeMipCut));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipNCellsForNearbyHit", m_mipNCellsForNearbyHit));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MipMaxNearbyHits", m_mipMaxNearbyHits));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
