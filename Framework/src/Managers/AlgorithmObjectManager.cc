/**
 *  @file   PandoraPFANew/Framework/src/Managers/AlgorithmObjectManager.cc
 * 
 *  @brief  Implementation of the algorithm object manager class.
 * 
 *  $Log: $
 */

#include "Managers/AlgorithmObjectManager.h"

#include "Objects/Cluster.h"

namespace pandora
{

template<typename T>
AlgorithmObjectManager<T>::AlgorithmObjectManager() :
    Manager<T>(),
    m_canMakeNewObjects(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
AlgorithmObjectManager<T>::~AlgorithmObjectManager()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::MakeTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName)
{
    typename Manager<T>::AlgorithmInfoMap::iterator iter = Manager<T>::m_algorithmInfoMap.find(pAlgorithm);

    if (Manager<T>::m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

    if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
        return STATUS_CODE_ALREADY_PRESENT;

    Manager<T>::m_nameToListMap[temporaryListName] = new ObjectList;
    Manager<T>::m_currentListName = temporaryListName;
    m_canMakeNewObjects = true;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::MoveObjectsToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
    std::string &temporaryListName, const ObjectList &objectsToMove)
{
    if (objectsToMove.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    typename Manager<T>::NameToListMap::iterator originalObjectListIter = Manager<T>::m_nameToListMap.find(originalListName);

    if (Manager<T>::m_nameToListMap.end() == originalObjectListIter)
        return STATUS_CODE_NOT_FOUND;

    if (originalObjectListIter->second->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, MakeTemporaryListAndSetCurrent(pAlgorithm, temporaryListName));
    typename Manager<T>::NameToListMap::iterator temporaryObjectListIter = Manager<T>::m_nameToListMap.find(temporaryListName);

    if (Manager<T>::m_nameToListMap.end() == temporaryObjectListIter)
        return STATUS_CODE_FAILURE;

    if ((originalObjectListIter->second == &objectsToMove) || (temporaryObjectListIter->second == &objectsToMove))
        return STATUS_CODE_INVALID_PARAMETER;

    for (typename ObjectList::const_iterator objectIter = objectsToMove.begin(), objectIterEnd = objectsToMove.end();
        objectIter != objectIterEnd; ++objectIter)
    {
        typename ObjectList::iterator originalObjectIter = originalObjectListIter->second->find(*objectIter);

        if (originalObjectListIter->second->end() == originalObjectIter)
            return STATUS_CODE_NOT_FOUND;

        if (!temporaryObjectListIter->second->insert(*originalObjectIter).second)
            return STATUS_CODE_ALREADY_PRESENT;

        originalObjectListIter->second->erase(originalObjectIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::SaveObjects(const Algorithm *const /*pAlgorithm*/, const std::string &targetListName,
    const std::string &sourceListName)
{
    typename Manager<T>::NameToListMap::iterator sourceObjectListIter = Manager<T>::m_nameToListMap.find(sourceListName);

    if (Manager<T>::m_nameToListMap.end() == sourceObjectListIter)
        return STATUS_CODE_NOT_FOUND;

    if (sourceObjectListIter->second->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    typename Manager<T>::NameToListMap::iterator targetObjectListIter = Manager<T>::m_nameToListMap.find(targetListName);

    if (Manager<T>::m_nameToListMap.end() == targetObjectListIter)
    {
        Manager<T>::m_nameToListMap[targetListName] = new ObjectList;
        targetObjectListIter = Manager<T>::m_nameToListMap.find(targetListName);

        if (Manager<T>::m_nameToListMap.end() == targetObjectListIter)
            return STATUS_CODE_FAILURE;
    }

    for (typename ObjectList::iterator objectIter = sourceObjectListIter->second->begin(), objectIterEnd = sourceObjectListIter->second->end();
        objectIter != objectIterEnd; ++objectIter)
    {
        if (!targetObjectListIter->second->insert(*objectIter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    Manager<T>::m_savedLists.insert(targetListName);
    sourceObjectListIter->second->clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::SaveObjects(const Algorithm *const /*pAlgorithm*/, const std::string &targetListName,
    const std::string &sourceListName, const ObjectList &objectsToSave)
{
    if (objectsToSave.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    typename Manager<T>::NameToListMap::iterator sourceObjectListIter = Manager<T>::m_nameToListMap.find(sourceListName);

    if (Manager<T>::m_nameToListMap.end() == sourceObjectListIter)
        return STATUS_CODE_NOT_FOUND;

    if (sourceObjectListIter->second->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    typename Manager<T>::NameToListMap::iterator targetObjectListIter = Manager<T>::m_nameToListMap.find(targetListName);

    if (Manager<T>::m_nameToListMap.end() == targetObjectListIter)
    {
        Manager<T>::m_nameToListMap[targetListName] = new ObjectList;
        targetObjectListIter = Manager<T>::m_nameToListMap.find(targetListName);

        if (Manager<T>::m_nameToListMap.end() == targetObjectListIter)
            return STATUS_CODE_FAILURE;
    }

    if ((sourceObjectListIter->second == &objectsToSave) || (targetObjectListIter->second == &objectsToSave))
        return STATUS_CODE_INVALID_PARAMETER;

    for (typename ObjectList::const_iterator objectIter = objectsToSave.begin(); objectIter != objectsToSave.end();)
    {
        T *pT = *objectIter;
        ++objectIter;

        typename ObjectList::iterator sourceObjectIter = sourceObjectListIter->second->find(pT);

        if (sourceObjectListIter->second->end() == sourceObjectIter)
            return STATUS_CODE_NOT_FOUND;

        if (!targetObjectListIter->second->insert(*sourceObjectIter).second)
            return STATUS_CODE_ALREADY_PRESENT;

        sourceObjectListIter->second->erase(sourceObjectIter);
    }

    Manager<T>::m_savedLists.insert(targetListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::TemporarilyReplaceCurrentList(const std::string &listName)
{
    if (Manager<T>::m_nameToListMap.end() == Manager<T>::m_nameToListMap.find(listName))
        return STATUS_CODE_NOT_FOUND;

    m_canMakeNewObjects = false;
    Manager<T>::m_currentListName = listName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::DeleteObject(T *pT, const std::string &listName)
{
    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(listName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_INITIALIZED;

    typename ObjectList::iterator deletionIter = listIter->second->find(pT);

    if (listIter->second->end() == deletionIter)
        return STATUS_CODE_NOT_FOUND;

    delete pT;
    listIter->second->erase(deletionIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::DeleteObjects(const ObjectList &objectList, const std::string &listName)
{
    typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.find(listName);

    if (Manager<T>::m_nameToListMap.end() == listIter)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listIter->second == &objectList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (typename ObjectList::const_iterator objectIter = objectList.begin(), objectIterEnd = objectList.end(); objectIter != objectIterEnd; ++objectIter)
    {
        T *pT = *objectIter;

        typename ObjectList::iterator deletionIter = listIter->second->find(pT);

        if (listIter->second->end() == deletionIter)
            return STATUS_CODE_NOT_FOUND;

        delete pT;
        listIter->second->erase(deletionIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::DeleteTemporaryObjects(const Algorithm *const pAlgorithm, const std::string &temporaryListName)
{
    typename Manager<T>::NameToListMap::iterator temporaryListIter = Manager<T>::m_nameToListMap.find(temporaryListName);

    if (Manager<T>::m_nameToListMap.end() == temporaryListIter)
        return STATUS_CODE_NOT_FOUND;

    if (Manager<T>::m_algorithmInfoMap.end() == Manager<T>::m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_NOT_ALLOWED;

    // NEW - CHECK NOT A SAVED LIST
    if (Manager<T>::m_savedLists.end() != Manager<T>::m_savedLists.find(temporaryListName))
        return STATUS_CODE_NOT_ALLOWED;

    for (typename ObjectList::iterator objectIter = temporaryListIter->second->begin(), objectIterEnd = temporaryListIter->second->end();
        objectIter != objectIterEnd; ++objectIter)
    {
        delete (*objectIter);
    }

    temporaryListIter->second->clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::GetObjectsToBeDeleted(const Algorithm *const pAlgorithm, ObjectList &objectList) const
{
    typename Manager<T>::AlgorithmInfoMap::const_iterator algorithmListIter = Manager<T>::m_algorithmInfoMap.find(pAlgorithm);

    if (Manager<T>::m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        typename Manager<T>::NameToListMap::const_iterator objectListIter = Manager<T>::m_nameToListMap.find(*listNameIter);

        if (Manager<T>::m_nameToListMap.end() == objectListIter)
            return STATUS_CODE_FAILURE;

        for (typename ObjectList::const_iterator objectIter = objectListIter->second->begin(), objectIterEnd = objectListIter->second->end();
            objectIter != objectIterEnd; ++objectIter)
        {
            objectList.insert(*objectIter);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    m_canMakeNewObjects = false;

    typename Manager<T>::AlgorithmInfoMap::iterator algorithmListIter = Manager<T>::m_algorithmInfoMap.find(pAlgorithm);

    if (Manager<T>::m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        typename Manager<T>::NameToListMap::iterator objectListIter = Manager<T>::m_nameToListMap.find(*listNameIter);

        if (Manager<T>::m_nameToListMap.end() == objectListIter)
            return STATUS_CODE_FAILURE;

        for (typename ObjectList::const_iterator objectIter = objectListIter->second->begin(), objectIterEnd = objectListIter->second->end();
            objectIter != objectIterEnd; ++objectIter)
        {
            delete (*objectIter);
        }

        delete objectListIter->second;
        Manager<T>::m_nameToListMap.erase(objectListIter);
    }

    algorithmListIter->second.m_temporaryListNames.clear();
    Manager<T>::m_currentListName = algorithmListIter->second.m_parentListName;

    if (isAlgorithmFinished)
        Manager<T>::m_algorithmInfoMap.erase(algorithmListIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
StatusCode AlgorithmObjectManager<T>::EraseAllContent()
{
    m_canMakeNewObjects = false;

    for (typename Manager<T>::NameToListMap::iterator listIter = Manager<T>::m_nameToListMap.begin(), listIterEnd = Manager<T>::m_nameToListMap.end();
        listIter != listIterEnd; ++listIter)
    {
        for (typename ObjectList::iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
            delete (*iter);
    }

    return Manager<T>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template class AlgorithmObjectManager<Cluster>;

} // namespace pandora
