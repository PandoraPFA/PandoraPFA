/**
 *  @file   PandoraPFANew/Framework/src/Managers/TrackManager.cc
 * 
 *  @brief  Implementation of the track manager class.
 * 
 *  $Log: $
 */

#include "Managers/TrackManager.h"

#include "Objects/Track.h"

namespace pandora
{

const std::string TrackManager::NULL_LIST_NAME = "NullList";
const std::string TrackManager::INPUT_LIST_NAME = "Input";

//------------------------------------------------------------------------------------------------------------------------------------------

TrackManager::TrackManager() :
    m_currentListName(NULL_LIST_NAME)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateNullList());
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackManager::~TrackManager()
{
    (void) this->ResetForNextEvent();
    this->DeleteNullList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::CreateTrack(const PandoraApi::TrackParameters &trackParameters)
{
    try
    {
        Track *pTrack = NULL;
        pTrack = new Track(trackParameters);

        if (NULL == pTrack)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        m_inputTrackVector.push_back(pTrack);

        if (!m_uidToTrackMap.insert(UidToTrackMap::value_type(pTrack->GetParentTrackAddress(), pTrack)).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create track: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::CreateNullList()
{
    if (!m_nameToTrackListMap.empty() || !m_savedLists.empty())
        return STATUS_CODE_NOT_ALLOWED;

    m_nameToTrackListMap[NULL_LIST_NAME] = new TrackList;
    m_savedLists.insert(NULL_LIST_NAME);

    if (m_nameToTrackListMap.end() == m_nameToTrackListMap.find(NULL_LIST_NAME))
        return STATUS_CODE_FAILURE;

    if (m_savedLists.end() == m_savedLists.find(NULL_LIST_NAME))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TrackManager::DeleteNullList()
{
    NameToTrackListMap::iterator iter = m_nameToTrackListMap.find(NULL_LIST_NAME);

    if (m_nameToTrackListMap.end() != iter)
    {
        delete iter->second;
        m_nameToTrackListMap.erase(iter);
    }

    StringSet::iterator savedListsIter = m_savedLists.find(NULL_LIST_NAME);

    if (m_savedLists.end() != savedListsIter)
    {
        m_savedLists.erase(savedListsIter);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::CreateInputTrackList()
{
    NameToTrackListMap::iterator existingListIter = m_nameToTrackListMap.find(INPUT_LIST_NAME);

    if (m_nameToTrackListMap.end() != existingListIter)
        m_nameToTrackListMap.erase(existingListIter);

    TrackList trackList(m_inputTrackVector.begin(), m_inputTrackVector.end());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, SaveList(trackList, INPUT_LIST_NAME));
    m_currentListName = INPUT_LIST_NAME;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::GetList(const std::string &listName, const TrackList *&pTrackList) const
{
    NameToTrackListMap::const_iterator iter = m_nameToTrackListMap.find(listName);

    if (m_nameToTrackListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    pTrackList = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &trackListName)
{
    if (m_nameToTrackListMap.end() == m_nameToTrackListMap.find(trackListName))
        return STATUS_CODE_NOT_FOUND;

    // ATTN: Previously couldn't replace lists unless called from a top-level algorithm: return if (m_algorithmInfoMap.size() > 1)
    //       Then algorithhm parent list was only replaced for algorithm calling this function.
    if (m_savedLists.end() == m_savedLists.find(trackListName))
        return STATUS_CODE_NOT_ALLOWED;

    if (m_algorithmInfoMap.end() == m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_FAILURE;

    m_currentListName = trackListName;

    for (AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.begin(), iterEnd = m_algorithmInfoMap.end(); iter != iterEnd; ++iter)
    {
        iter->second.m_parentListName = trackListName;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const TrackList &trackList,
    std::string &temporaryListName)
{
    // ATTN: Previously couldn't create an empty temporary list. Now allow this for reclustering without reference to tracks.
    AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

    if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
        return STATUS_CODE_ALREADY_PRESENT;

    m_nameToTrackListMap[temporaryListName] = new TrackList(trackList);
    m_currentListName = temporaryListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SaveList(const TrackList &trackList, const std::string &newListName)
{
    if (m_nameToTrackListMap.end() != m_nameToTrackListMap.find(newListName))
        return this->AddTracksToList(newListName, trackList);

    if (!m_nameToTrackListMap.insert(NameToTrackListMap::value_type(newListName, new TrackList)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    *(m_nameToTrackListMap[newListName]) = trackList;
    m_savedLists.insert(newListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AddTracksToList(const std::string &listName, const TrackList &trackList)
{
    NameToTrackListMap::iterator listIter = m_nameToTrackListMap.find(listName);

    if (m_nameToTrackListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    TrackList *pSavedTrackList = listIter->second;

    if (pSavedTrackList == &trackList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        if (!pSavedTrackList->insert(TrackList::value_type(*iter)).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveTracksFromList(const std::string &listName, const TrackList &trackList)
{
    NameToTrackListMap::iterator listIter = m_nameToTrackListMap.find(listName);

    if (m_nameToTrackListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    TrackList *pSavedTrackList = listIter->second;

    if (pSavedTrackList == &trackList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        TrackList::iterator savedTrackIter = pSavedTrackList->find(*iter);

        if (pSavedTrackList->end() != savedTrackIter)
            pSavedTrackList->erase(savedTrackIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::MatchTracksToMCPfoTargets(const UidToMCParticleMap &trackToPfoTargetMap)
{
    if (trackToPfoTargetMap.empty())
        return STATUS_CODE_SUCCESS;

    for (TrackVector::const_iterator iter = m_inputTrackVector.begin(), iterEnd = m_inputTrackVector.end(); iter != iterEnd; ++iter)
    {
        UidToMCParticleMap::const_iterator pfoTargetIter = trackToPfoTargetMap.find((*iter)->GetParentTrackAddress());

        if (trackToPfoTargetMap.end() != pfoTargetIter)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetMCParticle(pfoTargetIter->second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RegisterAlgorithm(const Algorithm *const pAlgorithm)
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

StatusCode TrackManager::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        NameToTrackListMap::iterator iter = m_nameToTrackListMap.find(*listNameIter);

        if (m_nameToTrackListMap.end() == iter)
            return STATUS_CODE_FAILURE;

        delete iter->second;
        m_nameToTrackListMap.erase(iter);
    }

    algorithmListIter->second.m_temporaryListNames.clear();
    m_currentListName = algorithmListIter->second.m_parentListName;

    if (isAlgorithmFinished)
        m_algorithmInfoMap.erase(algorithmListIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::ResetForNextEvent()
{
    for (TrackVector::iterator iter = m_inputTrackVector.begin(), iterEnd = m_inputTrackVector.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_inputTrackVector.clear();

    for (NameToTrackListMap::iterator iter = m_nameToTrackListMap.begin(); iter != m_nameToTrackListMap.end();)
    {
        iter->second->clear();
        delete iter->second;
        m_nameToTrackListMap.erase(iter++);
    }

    m_nameToTrackListMap.clear();
    m_savedLists.clear();

    m_currentListName = NULL_LIST_NAME;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateNullList());

    m_uidToTrackMap.clear();
    m_parentDaughterRelationMap.clear();
    m_siblingRelationMap.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SetTrackParentDaughterRelationship(const Uid parentUid, const Uid daughterUid)
{
    m_parentDaughterRelationMap.insert(TrackRelationMap::value_type(parentUid, daughterUid));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SetTrackSiblingRelationship(const Uid firstSiblingUid, const Uid secondSiblingUid)
{
    m_siblingRelationMap.insert(TrackRelationMap::value_type(firstSiblingUid, secondSiblingUid));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AssociateTracks() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddParentDaughterAssociations());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddSiblingAssociations());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AddParentDaughterAssociations() const
{
    for (TrackRelationMap::const_iterator uidIter = m_parentDaughterRelationMap.begin(), uidIterEnd = m_parentDaughterRelationMap.end();
        uidIter != uidIterEnd; ++uidIter)
    {
        UidToTrackMap::const_iterator parentIter = m_uidToTrackMap.find(uidIter->first);
        UidToTrackMap::const_iterator daughterIter = m_uidToTrackMap.find(uidIter->second);

        if ((m_uidToTrackMap.end() == parentIter) || (m_uidToTrackMap.end() == daughterIter))
            continue;

        // ATTN: Very easy to duplicate this information. Now tolerate this without returning error.
        const StatusCode firstStatusCode(parentIter->second->AddDaughter(daughterIter->second));
        const StatusCode secondStatusCode(daughterIter->second->AddParent(parentIter->second));

        if (firstStatusCode != secondStatusCode)
            return STATUS_CODE_FAILURE;

        if ((firstStatusCode != STATUS_CODE_SUCCESS) && (firstStatusCode != STATUS_CODE_ALREADY_PRESENT))
            return firstStatusCode;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AddSiblingAssociations() const
{
    for (TrackRelationMap::const_iterator uidIter = m_siblingRelationMap.begin(), uidIterEnd = m_siblingRelationMap.end();
        uidIter != uidIterEnd; ++uidIter)
    {
        UidToTrackMap::const_iterator firstSiblingIter = m_uidToTrackMap.find(uidIter->first);
        UidToTrackMap::const_iterator secondSiblingIter = m_uidToTrackMap.find(uidIter->second);

        if ((m_uidToTrackMap.end() == firstSiblingIter) || (m_uidToTrackMap.end() == secondSiblingIter))
            continue;

        // ATTN: Very easy to duplicate this information. Now tolerate this without returning error.
        const StatusCode firstStatusCode(firstSiblingIter->second->AddSibling(secondSiblingIter->second));
        const StatusCode secondStatusCode(secondSiblingIter->second->AddSibling(firstSiblingIter->second));

        if (firstStatusCode != secondStatusCode)
            return STATUS_CODE_FAILURE;

        if ((firstStatusCode != STATUS_CODE_SUCCESS) && (firstStatusCode != STATUS_CODE_ALREADY_PRESENT))
            return firstStatusCode;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveAllClusterAssociations() const
{
    for (TrackVector::const_iterator iter = m_inputTrackVector.begin(), iterEnd = m_inputTrackVector.end(); iter != iterEnd; ++iter)
    {
        (*iter)->m_pAssociatedCluster = NULL;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveCurrentClusterAssociations(TrackToClusterMap &danglingClusters) const
{
    NameToTrackListMap::const_iterator listIter = m_nameToTrackListMap.find(m_currentListName);

    if (m_nameToTrackListMap.end() == listIter)
        return STATUS_CODE_FAILURE;

    for (TrackList::iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
    {
        Cluster *&pAssociatedCluster = (*iter)->m_pAssociatedCluster;

        if (NULL == pAssociatedCluster)
            continue;

        if (!danglingClusters.insert(TrackToClusterMap::value_type(*iter, pAssociatedCluster)).second)
            return STATUS_CODE_FAILURE;

        pAssociatedCluster = NULL;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveClusterAssociations(const TrackList &trackList) const
{
    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        (*iter)->m_pAssociatedCluster = NULL;
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
