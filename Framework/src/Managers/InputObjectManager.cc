/**
 *  @file   PandoraPFANew/Framework/src/Managers/InputObjectManager.cc
 * 
 *  @brief  Implementation of the input object manager class.
 * 
 *  $Log: $
 */

#include "Managers/InputObjectManager.h"

#include "Objects/CaloHit.h"
#include "Objects/Track.h"

namespace pandora
{

template<typename T>
const std::string InputObjectManager<T>::INPUT_LIST_NAME = "Input";

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
InputObjectManager<T>::InputObjectManager() :
    Manager<T>()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
InputObjectManager<T>::~InputObjectManager()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::CreateInputList()
{
    typename Manager<T>::NameToListMap::iterator existingListIter = Manager<T>::m_nameToListMap.find(INPUT_LIST_NAME);

    if (Manager<T>::m_nameToListMap.end() == existingListIter)
        return STATUS_CODE_FAILURE;

    Manager<T>::m_currentListName = INPUT_LIST_NAME;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ObjectList &objectList,
    std::string &temporaryListName)
{
    typename Manager<T>::AlgorithmInfoMap::iterator iter = Manager<T>::m_algorithmInfoMap.find(pAlgorithm);

    if (Manager<T>::m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

    if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
        return STATUS_CODE_ALREADY_PRESENT;

    Manager<T>::m_nameToListMap[temporaryListName] = new ObjectList(objectList);
    Manager<T>::m_currentListName = temporaryListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::SaveList(const ObjectList &objectList, const std::string &newListName)
{
    if (Manager<T>::m_nameToListMap.end() != Manager<T>::m_nameToListMap.find(newListName))
        return this->AddObjectsToList(newListName, objectList);

    if (!Manager<T>::m_nameToListMap.insert(typename Manager<T>::NameToListMap::value_type(newListName, new ObjectList)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    *(Manager<T>::m_nameToListMap[newListName]) = objectList;
    Manager<T>::m_savedLists.insert(newListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::AddObjectsToList(const std::string &listName, const ObjectList &objectList)
{
    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(listName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    ObjectList *pSavedList = listIter->second;

    if (pSavedList == &objectList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (typename ObjectList::const_iterator iter = objectList.begin(), iterEnd = objectList.end(); iter != iterEnd; ++iter)
    {
        if (!pSavedList->insert(*iter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::RemoveObjectsFromList(const std::string &listName, const ObjectList &objectList)
{
    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(listName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    ObjectList *pSavedList = listIter->second;

    if (pSavedList == &objectList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (typename ObjectList::const_iterator iter = objectList.begin(), iterEnd = objectList.end(); iter != iterEnd; ++iter)
    {
        typename ObjectList::iterator savedObjectIter = pSavedList->find(*iter);

        if (pSavedList->end() != savedObjectIter)
            pSavedList->erase(savedObjectIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::EraseAllContent()
{
    typename Manager<T>::NameToListMap::const_iterator inputIter = Manager<T>::m_nameToListMap.find(INPUT_LIST_NAME);

    if (Manager<T>::m_nameToListMap.end() == inputIter)
    {
        std::cout << "InputObjectManager::EraseAllContent cannot retrieve list " << std::endl;
    }
    else
    {
        for (typename ObjectList::iterator iter = inputIter->second->begin(), iterEnd = inputIter->second->end(); iter != iterEnd; ++iter)
            delete *iter;
    }

    return Manager<T>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode InputObjectManager<T>::CreateInitialLists()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, Manager<T>::CreateInitialLists());
    Manager<T>::m_nameToListMap[INPUT_LIST_NAME] = new ObjectList;
    Manager<T>::m_savedLists.insert(INPUT_LIST_NAME);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class InputObjectManager<CaloHit *>;
template class InputObjectManager<Track *>;

} // namespace pandora
