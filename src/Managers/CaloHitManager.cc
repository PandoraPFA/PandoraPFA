/**
 *  @file   PandoraPFANew/src/Managers/CaloHitManager.cc
 * 
 *  @brief  Implementation of the calo hit manager class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"

#include "Managers/CaloHitManager.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"

#include <cmath>

namespace pandora
{

const std::string CaloHitManager::INPUT_LIST_NAME = "input";

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::~CaloHitManager()
{
    (void) this->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateCaloHit(const PandoraApi::CaloHitParameters &caloHitParameters)
{
    try
    {
        CaloHit *pCaloHit = NULL;
        pCaloHit = new CaloHit(caloHitParameters);

        if (NULL == pCaloHit)
            return STATUS_CODE_FAILURE;

        m_inputCaloHitVector.push_back(pCaloHit);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create calo hit: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::OrderInputCaloHits()
{
    OrderedCaloHitList orderedCaloHitList;

    for (CaloHitVector::iterator iter = m_inputCaloHitVector.begin(), iterEnd = m_inputCaloHitVector.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignToPseudoLayer(*iter));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.AddCaloHit(*iter));
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, SaveList(orderedCaloHitList, INPUT_LIST_NAME));
    m_currentListName = INPUT_LIST_NAME;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::AssignToPseudoLayer(CaloHit *const pCaloHit) const
{
    return pCaloHit->SetPseudoLayer(pCaloHit->GetLayer()); // TODO don't just use input layer
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::GetList(const std::string &listName, const OrderedCaloHitList *&pOrderedCaloHitList) const
{
    NameToOrderedCaloHitListMap::const_iterator iter = m_nameToOrderedCaloHitListMap.find(listName);
    
    if (m_nameToOrderedCaloHitListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    pOrderedCaloHitList = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &orderedCaloHitListName)
{
    if (m_nameToOrderedCaloHitListMap.end() == m_nameToOrderedCaloHitListMap.find(orderedCaloHitListName))
        return STATUS_CODE_NOT_FOUND;

    if ((m_algorithmInfoMap.size() > 1) || (m_savedLists.end() == m_savedLists.find(orderedCaloHitListName)))
        return STATUS_CODE_NOT_ALLOWED;

    m_currentListName = orderedCaloHitListName;

    AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);    

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    iter->second.m_parentListName = orderedCaloHitListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const OrderedCaloHitList &orderedCaloHitList,
    std::string &temporaryListName)
{
    if (orderedCaloHitList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);    

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

    if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
        return STATUS_CODE_ALREADY_PRESENT;

    m_nameToOrderedCaloHitListMap[temporaryListName] = new OrderedCaloHitList(orderedCaloHitList);
    m_currentListName = temporaryListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ClusterList &clusterList,
    std::string &temporaryListName)
{
    if (clusterList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    const OrderedCaloHitList orderedCaloHitList = (*clusterList.begin())->GetOrderedCaloHitList();
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CreateTemporaryListAndSetCurrent(pAlgorithm, orderedCaloHitList, temporaryListName));

    for (ClusterList::const_iterator iter = ++(clusterList.begin()), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_nameToOrderedCaloHitListMap[temporaryListName]->Add((*iter)->GetOrderedCaloHitList()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::SaveList(const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName)
{
    if (m_nameToOrderedCaloHitListMap.end() != m_nameToOrderedCaloHitListMap.find(newListName))
        return STATUS_CODE_ALREADY_PRESENT;

    if (!m_nameToOrderedCaloHitListMap.insert(NameToOrderedCaloHitListMap::value_type(newListName, new OrderedCaloHitList)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    *(m_nameToOrderedCaloHitListMap[newListName]) = orderedCaloHitList;
    m_savedLists.insert(newListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::AddCaloHitsToList(const std::string &listName, const OrderedCaloHitList &orderedCaloHitList)
{
    NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, listIter->second->Add(orderedCaloHitList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
    
StatusCode CaloHitManager::AddCaloHitsToList(const std::string &listName, const ClusterList &clusterList)
{
    if (clusterList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, listIter->second->Add((*iter)->GetOrderedCaloHitList()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::RemoveCaloHitsFromList(const std::string &listName, const OrderedCaloHitList &orderedCaloHitList)
{
    NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, listIter->second->Remove(orderedCaloHitList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::RemoveCaloHitsFromList(const std::string &listName, const ClusterList &clusterList)
{
    if (clusterList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, listIter->second->Remove((*iter)->GetOrderedCaloHitList()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::MatchCaloHitsToMCPfoTargets(const UidToMCParticleMap &caloHitToPfoTargetMap)
{
    for (CaloHitVector::iterator iter = m_inputCaloHitVector.begin(), iterEnd = m_inputCaloHitVector.end(); iter != iterEnd; ++iter)
    {
        UidToMCParticleMap::const_iterator pfoTargetIter = caloHitToPfoTargetMap.find((*iter)->GetParentCaloHitAddress());

        if (caloHitToPfoTargetMap.end() != pfoTargetIter)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetMCParticle(pfoTargetIter->second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::RegisterAlgorithm(const Algorithm *const pAlgorithm)
{
    if (m_algorithmInfoMap.end() != m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_ALREADY_PRESENT;

    AlgorithmInfo algorithmInfo;
    algorithmInfo.m_parentListName = m_currentListName;
    algorithmInfo.m_numberOfListsCreated = 0;

    if (!m_algorithmInfoMap.insert(AlgorithmInfoMap::value_type(pAlgorithm, algorithmInfo)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);
    
    if (m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        NameToOrderedCaloHitListMap::iterator iter = m_nameToOrderedCaloHitListMap.find(*listNameIter);
        
        if (m_nameToOrderedCaloHitListMap.end() == iter)
            return STATUS_CODE_FAILURE;

        m_nameToOrderedCaloHitListMap.erase(iter);
    }

    algorithmListIter->second.m_temporaryListNames.clear();
    m_currentListName = algorithmListIter->second.m_parentListName;

    if (isAlgorithmFinished)
        m_algorithmInfoMap.erase(algorithmListIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ResetForNextEvent()
{
    for (CaloHitVector::iterator iter = m_inputCaloHitVector.begin(), iterEnd = m_inputCaloHitVector.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_inputCaloHitVector.clear();

    for (NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.begin();
        listIter != m_nameToOrderedCaloHitListMap.end(); ++listIter)
    {
        delete listIter->second;
        m_nameToOrderedCaloHitListMap.erase(listIter);
    }

    m_nameToOrderedCaloHitListMap.clear();
    m_savedLists.clear();
    m_currentListName.clear();

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
