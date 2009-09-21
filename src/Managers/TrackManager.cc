/**
 *  @file   PandoraPFANew/src/Managers/TrackManager.cc
 * 
 *  @brief  Implementation of the track manager class.
 * 
 *  $Log: $
 */

#include "Managers/TrackManager.h"

#include "Objects/Track.h"

namespace pandora
{

const std::string TrackManager::INPUT_LIST_NAME = "input";

//------------------------------------------------------------------------------------------------------------------------------------------

TrackManager::TrackManager() :
    m_currentListName(INPUT_LIST_NAME)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackManager::~TrackManager()
{
    (void) this->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::CreateTrack(const PandoraApi::TrackParameters &trackParameters)
{
    try
    {
        Track *pTrack = NULL;
        pTrack = new Track(trackParameters);

        if (NULL == pTrack)
            return STATUS_CODE_FAILURE;

        NameToTrackListMap::iterator iter = m_nameToTrackListMap.find(INPUT_LIST_NAME);

        if (m_nameToTrackListMap.end() == iter)
        {
            m_nameToTrackListMap[INPUT_LIST_NAME] = new TrackList;
            m_savedLists.insert(INPUT_LIST_NAME);
            iter = m_nameToTrackListMap.find(INPUT_LIST_NAME);

            if (m_nameToTrackListMap.end() == iter)
                return STATUS_CODE_FAILURE;
        }

        if (!iter->second->insert(pTrack).second)
            return STATUS_CODE_FAILURE;

        if (!m_uidToTrackMap.insert(UidToTrackMap::value_type(pTrack->GetParentTrackAddress(), pTrack)).second)
            return STATUS_CODE_FAILURE;

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create track: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
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

    if ((m_algorithmInfoMap.size() > 1) || (m_savedLists.end() == m_savedLists.find(trackListName)))
        return STATUS_CODE_NOT_ALLOWED;

    m_currentListName = trackListName;

    AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);    

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    iter->second.m_parentListName = trackListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const TrackList &trackList,
    std::string &temporaryListName)
{
    if (trackList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

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
        return STATUS_CODE_ALREADY_PRESENT;

    if (!m_nameToTrackListMap.insert(NameToTrackListMap::value_type(newListName, new TrackList)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    *(m_nameToTrackListMap[newListName]) = trackList;
    m_savedLists.insert(newListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::MatchTracksToMCPfoTargets(const UidToMCParticleMap &trackToPfoTargetMap)
{
    NameToTrackListMap::iterator listIter = m_nameToTrackListMap.find(INPUT_LIST_NAME);

    if (m_nameToTrackListMap.end() == listIter)
        return STATUS_CODE_NOT_INITIALIZED;

    for (TrackList::iterator iter = listIter->second->begin(), iterEnd = listIter->second->end(); iter != iterEnd; ++iter)
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

StatusCode TrackManager::ResetAfterAlgorithmCompletion(const Algorithm *const pAlgorithm)
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
        
        m_nameToTrackListMap.erase(iter);
    }

    m_currentListName = algorithmListIter->second.m_parentListName;
    m_algorithmInfoMap.erase(algorithmListIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::ResetForNextEvent()
{
    NameToTrackListMap::iterator inputIter = m_nameToTrackListMap.find(INPUT_LIST_NAME);

    if (m_nameToTrackListMap.end() != inputIter)
    {
        for (TrackList::iterator inputTrackIter = inputIter->second->begin(), inputTrackIterEnd = inputIter->second->end(); 
            inputTrackIter != inputTrackIterEnd; ++inputTrackIter)
        {
            delete *inputTrackIter;
        }
    }

    for (NameToTrackListMap::iterator iter = m_nameToTrackListMap.begin(); iter != m_nameToTrackListMap.end(); ++iter)
    {
        delete iter->second;
        m_nameToTrackListMap.erase(iter);
    }

    m_nameToTrackListMap.clear();
    m_savedLists.clear();
    m_currentListName = INPUT_LIST_NAME;

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

StatusCode TrackManager::AssociateTracks()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddParentDaughterAssociations());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddSiblingAssociations());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AddParentDaughterAssociations()
{
    for (TrackRelationMap::const_iterator uidIter = m_parentDaughterRelationMap.begin(), uidIterEnd = m_parentDaughterRelationMap.end();
        uidIter != uidIterEnd; ++uidIter)
    {
        UidToTrackMap::iterator parentIter = m_uidToTrackMap.find(uidIter->first);
        UidToTrackMap::iterator daughterIter = m_uidToTrackMap.find(uidIter->second);

        if ((m_uidToTrackMap.end() == parentIter) || (m_uidToTrackMap.end() == daughterIter))
            continue;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, parentIter->second->AddDaughter(daughterIter->second));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, daughterIter->second->AddParent(parentIter->second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AddSiblingAssociations()
{
    for (TrackRelationMap::const_iterator uidIter = m_siblingRelationMap.begin(), uidIterEnd = m_siblingRelationMap.end();
        uidIter != uidIterEnd; ++uidIter)
    {
        UidToTrackMap::iterator firstSiblingIter = m_uidToTrackMap.find(uidIter->first);
        UidToTrackMap::iterator secondSiblingIter = m_uidToTrackMap.find(uidIter->second);

        if ((m_uidToTrackMap.end() == firstSiblingIter) || (m_uidToTrackMap.end() == secondSiblingIter))
            continue;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, firstSiblingIter->second->AddSibling(secondSiblingIter->second));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, secondSiblingIter->second->AddSibling(firstSiblingIter->second));
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
