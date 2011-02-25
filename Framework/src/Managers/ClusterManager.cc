/**
 *  @file PandoraPFANew/Framework/src/Managers/ClusterManager.cc
 * 
 *  @brief Implementation of the cluster manager class.
 * 
 *  $Log: $
 */

#include "Managers/ClusterManager.h"

#include "Objects/Cluster.h"

namespace pandora
{

const std::string ClusterManager::NULL_LIST_NAME = "NullList";

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterManager::ClusterManager() :
    m_canMakeNewClusters(false),
    m_currentListName(NULL_LIST_NAME)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateNullList());
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterManager::~ClusterManager()
{
    (void) this->ResetForNextEvent();
    this->DeleteNullList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename CLUSTER_PARAMETERS>
StatusCode ClusterManager::CreateCluster(CLUSTER_PARAMETERS *pClusterParameters, Cluster *&pCluster)
{
    try
    {
        if (!m_canMakeNewClusters)
            return STATUS_CODE_NOT_ALLOWED;

        NameToClusterListMap::iterator iter = m_nameToClusterListMap.find(m_currentListName);

        if (m_nameToClusterListMap.end() == iter)
            return STATUS_CODE_NOT_INITIALIZED;

        pCluster = NULL;
        pCluster = new Cluster(pClusterParameters);

        if (NULL == pCluster)
            return STATUS_CODE_FAILURE;

        if (!iter->second->insert(pCluster).second)
            return STATUS_CODE_FAILURE;

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create cluster: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::CreateNullList()
{
    if (!m_nameToClusterListMap.empty() || !m_savedLists.empty())
        return STATUS_CODE_NOT_ALLOWED;

    m_nameToClusterListMap[NULL_LIST_NAME] = new ClusterList;
    m_savedLists.insert(NULL_LIST_NAME);

    if (m_nameToClusterListMap.end() == m_nameToClusterListMap.find(NULL_LIST_NAME))
        return STATUS_CODE_FAILURE;

    if (m_savedLists.end() == m_savedLists.find(NULL_LIST_NAME))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ClusterManager::DeleteNullList()
{
    NameToClusterListMap::iterator iter = m_nameToClusterListMap.find(NULL_LIST_NAME);

    if (m_nameToClusterListMap.end() != iter)
    {
        delete iter->second;
        m_nameToClusterListMap.erase(iter);
    }

    StringSet::iterator savedListsIter = m_savedLists.find(NULL_LIST_NAME);

    if (m_savedLists.end() != savedListsIter)
    {
        m_savedLists.erase(savedListsIter);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::GetList(const std::string &listName, const ClusterList *&pClusterList) const
{
    NameToClusterListMap::const_iterator iter = m_nameToClusterListMap.find(listName);

    if (m_nameToClusterListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    pClusterList = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &clusterListName)
{
    if (m_nameToClusterListMap.end() == m_nameToClusterListMap.find(clusterListName))
        return STATUS_CODE_NOT_FOUND;

    // ATTN: Previously couldn't replace lists unless called from a top-level algorithm: return if (m_algorithmInfoMap.size() > 1)
    //       Then algorithhm parent list was only replaced for algorithm calling this function.
    if (m_savedLists.end() == m_savedLists.find(clusterListName))
        return STATUS_CODE_NOT_ALLOWED;

    if (m_algorithmInfoMap.end() == m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_FAILURE;

    m_canMakeNewClusters = false;
    m_currentListName = clusterListName;

    for (AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.begin(), iterEnd = m_algorithmInfoMap.end(); iter != iterEnd; ++iter)
    {
        iter->second.m_parentListName = clusterListName;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::TemporarilyReplaceCurrentList(const std::string &clusterListName)
{
    if (m_nameToClusterListMap.end() == m_nameToClusterListMap.find(clusterListName))
        return STATUS_CODE_NOT_FOUND;

    m_canMakeNewClusters = false;
    m_currentListName = clusterListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MakeTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, std::string &temporaryListName)
{
    AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

    if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
        return STATUS_CODE_ALREADY_PRESENT;

    m_nameToClusterListMap[temporaryListName] = new ClusterList;
    m_currentListName = temporaryListName;
    m_canMakeNewClusters = true;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MoveClustersToTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const std::string &originalListName,
    std::string &temporaryListName, const ClusterList &clustersToMove)
{
    if (clustersToMove.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    NameToClusterListMap::iterator originalClusterListIter = m_nameToClusterListMap.find(originalListName);

    if (m_nameToClusterListMap.end() == originalClusterListIter)
        return STATUS_CODE_NOT_FOUND;

    if (originalClusterListIter->second->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, MakeTemporaryListAndSetCurrent(pAlgorithm, temporaryListName));
    NameToClusterListMap::iterator temporaryClusterListIter = m_nameToClusterListMap.find(temporaryListName);

    if (m_nameToClusterListMap.end() == temporaryClusterListIter)
        return STATUS_CODE_FAILURE;

    if ((originalClusterListIter->second == &clustersToMove) || (temporaryClusterListIter->second == &clustersToMove))
        return STATUS_CODE_INVALID_PARAMETER;

    for (ClusterList::const_iterator clusterIter = clustersToMove.begin(), clusterIterEnd = clustersToMove.end();
        clusterIter != clusterIterEnd; ++clusterIter)
    {
        ClusterList::iterator originalClusterIter = originalClusterListIter->second->find(*clusterIter);

        if (originalClusterListIter->second->end() == originalClusterIter)
            return STATUS_CODE_NOT_FOUND;

        if (!temporaryClusterListIter->second->insert(*originalClusterIter).second)
            return STATUS_CODE_ALREADY_PRESENT;

        originalClusterListIter->second->erase(originalClusterIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::SaveClusters(const Algorithm *const /*pAlgorithm*/, const std::string &targetListName,
    const std::string &sourceListName)
{
    NameToClusterListMap::iterator sourceClusterListIter = m_nameToClusterListMap.find(sourceListName);

    if (m_nameToClusterListMap.end() == sourceClusterListIter)
        return STATUS_CODE_NOT_FOUND;

    if (sourceClusterListIter->second->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    NameToClusterListMap::iterator targetClusterListIter = m_nameToClusterListMap.find(targetListName);

    if (m_nameToClusterListMap.end() == targetClusterListIter)
    {
        m_nameToClusterListMap[targetListName] = new ClusterList;
        targetClusterListIter = m_nameToClusterListMap.find(targetListName);

        if (m_nameToClusterListMap.end() == targetClusterListIter)
            return STATUS_CODE_FAILURE;
    }

    for (ClusterList::iterator clusterIter = sourceClusterListIter->second->begin(), clusterIterEnd = sourceClusterListIter->second->end();
        clusterIter != clusterIterEnd; ++clusterIter)
    {
        if (!targetClusterListIter->second->insert(*clusterIter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    m_savedLists.insert(targetListName);

    sourceClusterListIter->second->clear();

    // ATTN: Previously used to delete the emptied cluster list immediately, but algorithm users often tended to try to
    //       access deleted cluster list via a dangling pointer. Now they simply continue to access an empty list (preferable).
    // PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, RemoveEmptyClusterList(pAlgorithm, sourceListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::SaveClusters(const Algorithm *const /*pAlgorithm*/, const std::string &targetListName,
    const std::string &sourceListName, const ClusterList &clustersToSave)
{
    if (clustersToSave.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    NameToClusterListMap::iterator sourceClusterListIter = m_nameToClusterListMap.find(sourceListName);

    if (m_nameToClusterListMap.end() == sourceClusterListIter)
        return STATUS_CODE_NOT_FOUND;

    if (sourceClusterListIter->second->empty())
        return STATUS_CODE_NOT_INITIALIZED;

    NameToClusterListMap::iterator targetClusterListIter = m_nameToClusterListMap.find(targetListName);

    if (m_nameToClusterListMap.end() == targetClusterListIter)
    {
        m_nameToClusterListMap[targetListName] = new ClusterList;
        targetClusterListIter = m_nameToClusterListMap.find(targetListName);

        if (m_nameToClusterListMap.end() == targetClusterListIter)
            return STATUS_CODE_FAILURE;
    }

    if ((sourceClusterListIter->second == &clustersToSave) || (targetClusterListIter->second == &clustersToSave))
        return STATUS_CODE_INVALID_PARAMETER;

    for (ClusterList::const_iterator clusterIter = clustersToSave.begin(); clusterIter != clustersToSave.end();)
    {
        Cluster *pCluster = *clusterIter;
        ++clusterIter;

        ClusterList::iterator sourceClusterIter = sourceClusterListIter->second->find(pCluster);

        if (sourceClusterListIter->second->end() == sourceClusterIter)
            return STATUS_CODE_NOT_FOUND;

        if (!targetClusterListIter->second->insert(*sourceClusterIter).second)
            return STATUS_CODE_ALREADY_PRESENT;

        sourceClusterListIter->second->erase(sourceClusterIter);
    }

    m_savedLists.insert(targetListName);

    // ATTN: Previously used to delete the emptied cluster list immediately, but algorithm users often tended to try to
    //       access deleted cluster list via a dangling pointer. Now they simply continue to access an empty list (preferable).
    // if (sourceClusterListIter->second->empty())
    //     PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, RemoveEmptyClusterList(pAlgorithm, sourceListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->RemoveCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->AddIsolatedCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit)
{
    return pCluster->RemoveIsolatedCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::DeleteCluster(Cluster *pCluster, const std::string &listName)
{
    NameToClusterListMap::iterator listIter = m_nameToClusterListMap.find(listName);

    if (m_nameToClusterListMap.end() == listIter)
        return STATUS_CODE_NOT_INITIALIZED;

    ClusterList::iterator deletionIter = listIter->second->find(pCluster);

    if (listIter->second->end() == deletionIter)
        return STATUS_CODE_NOT_FOUND;

    delete pCluster;
    listIter->second->erase(deletionIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::DeleteClusters(const ClusterList &clusterList, const std::string &listName)
{
    NameToClusterListMap::iterator listIter = m_nameToClusterListMap.find(listName);

    if (m_nameToClusterListMap.end() == listIter)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listIter->second == &clusterList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (ClusterList::const_iterator clusterIter = clusterList.begin(), clusterIterEnd = clusterList.end(); clusterIter != clusterIterEnd;
        ++clusterIter)
    {
        Cluster *pCluster = *clusterIter;

        ClusterList::iterator deletionIter = listIter->second->find(pCluster);

        if (listIter->second->end() == deletionIter)
            return STATUS_CODE_NOT_FOUND;

        delete pCluster;
        listIter->second->erase(deletionIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::DeleteTemporaryClusterList(const Algorithm *const pAlgorithm, const std::string &clusterListName)
{
    NameToClusterListMap::iterator clusterListIter = m_nameToClusterListMap.find(clusterListName);

    if (m_nameToClusterListMap.end() == clusterListIter)
        return STATUS_CODE_NOT_FOUND;

    if (m_algorithmInfoMap.end() == m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_NOT_ALLOWED;

    for (ClusterList::iterator clusterIter = clusterListIter->second->begin(), clusterIterEnd = clusterListIter->second->end();
        clusterIter != clusterIterEnd; ++clusterIter)
    {
        delete (*clusterIter);
    }

    clusterListIter->second->clear();
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveEmptyClusterList(pAlgorithm, clusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete)
{
    if (pClusterToEnlarge == pClusterToDelete)
        return STATUS_CODE_INVALID_PARAMETER;

    NameToClusterListMap::iterator listIter = m_nameToClusterListMap.find(m_currentListName);

    if (m_nameToClusterListMap.end() == listIter)
        return STATUS_CODE_NOT_INITIALIZED;

    ClusterList::iterator clusterToEnlargeIter = listIter->second->find(pClusterToEnlarge);
    ClusterList::iterator clusterToDeleteIter = listIter->second->find(pClusterToDelete);

    if ((listIter->second->end() == clusterToEnlargeIter) || (listIter->second->end() == clusterToDeleteIter))
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pClusterToEnlarge->AddHitsFromSecondCluster(pClusterToDelete));

    delete pClusterToDelete;
    listIter->second->erase(clusterToDeleteIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
    const std::string &deleteListName)
{
    if (pClusterToEnlarge == pClusterToDelete)
        return STATUS_CODE_INVALID_PARAMETER;

    NameToClusterListMap::iterator enlargeListIter = m_nameToClusterListMap.find(enlargeListName);
    NameToClusterListMap::iterator deleteListIter = m_nameToClusterListMap.find(deleteListName);

    if ((m_nameToClusterListMap.end() == enlargeListIter) || (m_nameToClusterListMap.end() == deleteListIter))
        return STATUS_CODE_NOT_INITIALIZED;

    ClusterList::iterator clusterToEnlargeIter = enlargeListIter->second->find(pClusterToEnlarge);
    ClusterList::iterator clusterToDeleteIter = deleteListIter->second->find(pClusterToDelete);

    if ((enlargeListIter->second->end() == clusterToEnlargeIter) || (deleteListIter->second->end() == clusterToDeleteIter))
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pClusterToEnlarge->AddHitsFromSecondCluster(pClusterToDelete));

    delete pClusterToDelete;
    deleteListIter->second->erase(clusterToDeleteIter);

    return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RegisterAlgorithm(const Algorithm *const pAlgorithm)
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

StatusCode ClusterManager::GetClustersToBeDeleted(const Algorithm *const pAlgorithm, ClusterList &clusterList) const
{
    AlgorithmInfoMap::const_iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        NameToClusterListMap::const_iterator clusterListIter = m_nameToClusterListMap.find(*listNameIter);

        if (m_nameToClusterListMap.end() == clusterListIter)
            return STATUS_CODE_FAILURE;

        for (ClusterList::iterator clusterIter = clusterListIter->second->begin(), clusterIterEnd = clusterListIter->second->end();
            clusterIter != clusterIterEnd; ++clusterIter)
        {
            clusterList.insert(*clusterIter);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    m_canMakeNewClusters = false;

    AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        NameToClusterListMap::iterator clusterListIter = m_nameToClusterListMap.find(*listNameIter);

        if (m_nameToClusterListMap.end() == clusterListIter)
            return STATUS_CODE_FAILURE;

        for (ClusterList::iterator clusterIter = clusterListIter->second->begin(), clusterIterEnd = clusterListIter->second->end();
            clusterIter != clusterIterEnd; ++clusterIter)
        {
            delete (*clusterIter);
        }

        delete clusterListIter->second;
        m_nameToClusterListMap.erase(clusterListIter);
    }

    algorithmListIter->second.m_temporaryListNames.clear();
    m_currentListName = algorithmListIter->second.m_parentListName;

    if (isAlgorithmFinished)
        m_algorithmInfoMap.erase(algorithmListIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::ResetForNextEvent()
{
    m_canMakeNewClusters = false;

    for (NameToClusterListMap::iterator iter = m_nameToClusterListMap.begin(); iter != m_nameToClusterListMap.end();)
    {
        for (ClusterList::iterator clusterIter = iter->second->begin(), clusterIterEnd = iter->second->end(); 
            clusterIter != clusterIterEnd; ++clusterIter)
        {
            delete (*clusterIter);
        }

        iter->second->clear();
        delete iter->second;
        m_nameToClusterListMap.erase(iter++);
    }

    m_nameToClusterListMap.clear();
    m_algorithmInfoMap.clear();
    m_savedLists.clear();

    m_currentListName = NULL_LIST_NAME;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateNullList());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveEmptyClusterList(const Algorithm *const pAlgorithm, const std::string &clusterListName)
{
    NameToClusterListMap::iterator clusterListIter = m_nameToClusterListMap.find(clusterListName);

    if (m_nameToClusterListMap.end() == clusterListIter)
        return STATUS_CODE_NOT_FOUND;

    if (!clusterListIter->second->empty())
        return STATUS_CODE_NOT_ALLOWED;

    delete clusterListIter->second;
    m_nameToClusterListMap.erase(clusterListIter);

    AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    if (clusterListName == m_currentListName)
    {
        m_canMakeNewClusters = false;
        m_currentListName = algorithmListIter->second.m_parentListName;
    }

    // Remove record from algorithm info if cluster list was a temporary list
    StringSet *pStringSet = &(algorithmListIter->second.m_temporaryListNames);
    StringSet::iterator temporaryListNameIter = pStringSet->find(clusterListName);

    if (pStringSet->end() != temporaryListNameIter)
        pStringSet->erase(temporaryListNameIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveAllTrackAssociations() const
{
    for (NameToClusterListMap::const_iterator iter = m_nameToClusterListMap.begin(); iter != m_nameToClusterListMap.end(); ++iter)
    {
        for (ClusterList::iterator clusterIter = iter->second->begin(), clusterIterEnd = iter->second->end(); 
            clusterIter != clusterIterEnd; ++clusterIter)
        {
            (*clusterIter)->m_associatedTrackList.clear();
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveCurrentTrackAssociations(TrackList &danglingTracks) const
{
    NameToClusterListMap::const_iterator iter = m_nameToClusterListMap.find(m_currentListName);

    if (m_nameToClusterListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    for (ClusterList::iterator clusterIter = iter->second->begin(), clusterIterEnd = iter->second->end(); clusterIter != clusterIterEnd;
        ++clusterIter)
    {
        TrackList &associatedTrackList((*clusterIter)->m_associatedTrackList);

        if (associatedTrackList.empty())
            continue;

        danglingTracks.insert(associatedTrackList.begin(), associatedTrackList.end());
        associatedTrackList.clear();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterManager::RemoveTrackAssociations(const TrackToClusterMap &trackToClusterList) const
{
    for (TrackToClusterMap::const_iterator iter = trackToClusterList.begin(), iterEnd = trackToClusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->RemoveTrackAssociation(iter->first));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode ClusterManager::CreateCluster<CaloHit>(CaloHit *pCaloHit, Cluster *&pCluster);
template StatusCode ClusterManager::CreateCluster<CaloHitList>(CaloHitList *pCaloHitList, Cluster *&pCluster);
template StatusCode ClusterManager::CreateCluster<Track>(Track *pTrack, Cluster *&pCluster);

} // namespace pandora
