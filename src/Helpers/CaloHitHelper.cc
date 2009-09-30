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

bool CaloHitHelper::AreCaloHitsAvailable(InputCaloHitList *const pInputCaloHitList)
{
    if (!m_isReclustering)
    {
        for (InputCaloHitList::iterator iter = pInputCaloHitList->begin(), iterEnd = pInputCaloHitList->end(); iter != iterEnd; ++iter)
        {
            if (!(*iter)->m_isAvailable)
                return false;
        }

        return true;
    }

    for (InputCaloHitList::iterator iter = pInputCaloHitList->begin(), iterEnd = pInputCaloHitList->end(); iter != iterEnd; ++iter)
    {
        CaloHitUsageMap::iterator usageMapIter = m_pCurrentUsageMap->find(*iter);

        if ((m_pCurrentUsageMap->end()) == usageMapIter || !usageMapIter->second)
            return false;
    }

    return true;
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

StatusCode CaloHitHelper::SetCaloHitAvailability(InputCaloHitList *const pInputCaloHitList, bool isAvailable)
{
    if (!m_isReclustering)
    {
        for (InputCaloHitList::iterator iter = pInputCaloHitList->begin(), iterEnd = pInputCaloHitList->end(); iter != iterEnd; ++iter)
            (*iter)->m_isAvailable = isAvailable;

        return STATUS_CODE_SUCCESS;
    }

    for (InputCaloHitList::iterator iter = pInputCaloHitList->begin(), iterEnd = pInputCaloHitList->end(); iter != iterEnd; ++iter)
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
            if (!m_pCurrentUsageMap->insert(CaloHitUsageMap::value_type(*caloHitIter, true)).second)
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

} // namespace pandora

