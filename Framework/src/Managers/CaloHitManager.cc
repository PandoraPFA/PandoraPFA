/**
 *  @file   PandoraPFANew/Framework/src/Managers/CaloHitManager.cc
 * 
 *  @brief  Implementation of the calo hit manager class.
 * 
 *  $Log: $
 */

#include "Helpers/CaloHitHelper.h"
#include "Helpers/GeometryHelper.h"

#include "Managers/CaloHitManager.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>

namespace pandora
{

const std::string CaloHitManager::NULL_LIST_NAME = "NullList";
const std::string CaloHitManager::INPUT_LIST_NAME = "Input";

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::CaloHitManager() :
    m_currentListName(NULL_LIST_NAME)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateNullList());
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::~CaloHitManager()
{
    (void) this->ResetForNextEvent();
    this->DeleteNullList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS>
StatusCode CaloHitManager::CreateCaloHit(const PARAMETERS &parameters)
{
    return STATUS_CODE_FAILURE;
}

template <>
StatusCode CaloHitManager::CreateCaloHit(const PandoraApi::RectangularCaloHitParameters &parameters)
{
    try
    {
        CaloHit *pCaloHit = NULL;
        pCaloHit = new RectangularCaloHit(parameters);

        if (NULL == pCaloHit)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        m_inputCaloHitVector.push_back(pCaloHit);
        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create calo hit: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

template <>
StatusCode CaloHitManager::CreateCaloHit(const PandoraApi::PointingCaloHitParameters &parameters)
{
    try
    {
        CaloHit *pCaloHit = NULL;
        pCaloHit = new PointingCaloHit(parameters);

        if (NULL == pCaloHit)
            throw StatusCodeException(STATUS_CODE_FAILURE);

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

StatusCode CaloHitManager::CreateNullList()
{
    if (!m_nameToOrderedCaloHitListMap.empty() || !m_savedLists.empty())
        return STATUS_CODE_NOT_ALLOWED;

    m_nameToOrderedCaloHitListMap[NULL_LIST_NAME] = new OrderedCaloHitList;
    m_savedLists.insert(NULL_LIST_NAME);

    if (m_nameToOrderedCaloHitListMap.end() == m_nameToOrderedCaloHitListMap.find(NULL_LIST_NAME))
        return STATUS_CODE_FAILURE;

    if (m_savedLists.end() == m_savedLists.find(NULL_LIST_NAME))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitManager::DeleteNullList()
{
    NameToOrderedCaloHitListMap::iterator iter = m_nameToOrderedCaloHitListMap.find(NULL_LIST_NAME);

    if (m_nameToOrderedCaloHitListMap.end() != iter)
    {
        delete iter->second;
        m_nameToOrderedCaloHitListMap.erase(iter);
    }

    StringSet::iterator savedListsIter = m_savedLists.find(NULL_LIST_NAME);

    if (m_savedLists.end() != savedListsIter)
    {
        m_savedLists.erase(savedListsIter);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::OrderInputCaloHits()
{
    NameToOrderedCaloHitListMap::iterator existingListIter = m_nameToOrderedCaloHitListMap.find(INPUT_LIST_NAME);

    if (m_nameToOrderedCaloHitListMap.end() != existingListIter)
        m_nameToOrderedCaloHitListMap.erase(existingListIter);

    OrderedCaloHitList orderedCaloHitList;

    for (CaloHitVector::iterator iter = m_inputCaloHitVector.begin(), iterEnd = m_inputCaloHitVector.end(); iter != iterEnd; ++iter)
    {
        try
        {
            PseudoLayer pseudoLayer = GeometryHelper::GetPseudoLayer((*iter)->GetPositionVector());
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetPseudoLayer(pseudoLayer));
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(*iter));
        }
        catch (StatusCodeException &statusCodeException)
        {
            std::cout << "Failed to assign hit to pseudolayer, " << statusCodeException.ToString() << std::endl;
        }
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, SaveList(orderedCaloHitList, INPUT_LIST_NAME));
    m_currentListName = INPUT_LIST_NAME;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CalculateCaloHitProperties() const
{
    try
    {
        NameToOrderedCaloHitListMap::const_iterator iter = m_nameToOrderedCaloHitListMap.find(INPUT_LIST_NAME);

        if (m_nameToOrderedCaloHitListMap.end() == iter)
            return STATUS_CODE_NOT_INITIALIZED;

        for (OrderedCaloHitList::const_iterator pseudoLayerIter = iter->second->begin(), pseudoLayerIterEnd = iter->second->end();
            pseudoLayerIter != pseudoLayerIterEnd; ++pseudoLayerIter)
        {
            for (CaloHitList::iterator caloHitIter = pseudoLayerIter->second->begin(), caloHitIterEnd = pseudoLayerIter->second->end();
                caloHitIter != caloHitIterEnd; ++caloHitIter)
            {
                CaloHitHelper::CalculateCaloHitProperties(*caloHitIter, iter->second);
            }
        }
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "CaloHitManager: Failed to calculate calo hit properties, " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
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

    // ATTN: Previously couldn't replace lists unless called from a top-level algorithm: return if (m_algorithmInfoMap.size() > 1)
    //       Then algorithhm parent list was only replaced for algorithm calling this function.
    if (m_savedLists.end() == m_savedLists.find(orderedCaloHitListName))
        return STATUS_CODE_NOT_ALLOWED;

    if (m_algorithmInfoMap.end() == m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_FAILURE;

    m_currentListName = orderedCaloHitListName;

    for (AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.begin(), iterEnd = m_algorithmInfoMap.end(); iter != iterEnd; ++iter)
    {
        iter->second.m_parentListName = orderedCaloHitListName;
    }

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

    const OrderedCaloHitList &orderedCaloHitList((*clusterList.begin())->GetOrderedCaloHitList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CreateTemporaryListAndSetCurrent(pAlgorithm, orderedCaloHitList, temporaryListName));

    OrderedCaloHitList *pTemporaryOrderedCaloHitList = m_nameToOrderedCaloHitListMap[temporaryListName];
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pTemporaryOrderedCaloHitList->Add((*clusterList.begin())->GetIsolatedCaloHitList()));

    for (ClusterList::const_iterator iter = ++(clusterList.begin()), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pTemporaryOrderedCaloHitList->Add((*iter)->GetOrderedCaloHitList()));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pTemporaryOrderedCaloHitList->Add((*iter)->GetIsolatedCaloHitList()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::SaveList(const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName)
{
    if (m_nameToOrderedCaloHitListMap.end() != m_nameToOrderedCaloHitListMap.find(newListName))
        return this->AddCaloHitsToList(newListName, orderedCaloHitList);

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

    if (listIter->second == &orderedCaloHitList)
        return STATUS_CODE_INVALID_PARAMETER;

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

    if (listIter->second == &orderedCaloHitList)
        return STATUS_CODE_INVALID_PARAMETER;

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
    if (caloHitToPfoTargetMap.empty())
        return STATUS_CODE_SUCCESS;

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

        iter->second->Reset();
        delete iter->second;
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

    for (NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.begin(); listIter != m_nameToOrderedCaloHitListMap.end();)
    {
        listIter->second->Reset();
        delete listIter->second;
        m_nameToOrderedCaloHitListMap.erase(listIter++);
    }

    m_nameToOrderedCaloHitListMap.clear();
    m_savedLists.clear();

    m_currentListName = NULL_LIST_NAME;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateNullList());

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
