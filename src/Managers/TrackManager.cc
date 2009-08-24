/**
 *	@file	PandoraPFANew/src/Managers/TrackManager.cc
 * 
 *	@brief	Implementation of the track manager class.
 * 
 *	$Log: $
 */

#include "Managers/TrackManager.h"

#include "Objects/Track.h"

#include <sstream>

namespace pandora
{

const std::string TrackManager::INPUT_LIST_NAME = "input";

//------------------------------------------------------------------------------------------------------------------------------------------

TrackManager::~TrackManager()
{
	(void) this->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::CreateTrack(const PandoraApi::TrackParameters &trackParameters)
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

	iter->second->insert(pTrack);

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::GetCurrentList(const TrackList *pTrackList, std::string &trackListName) const
{
	NameToTrackListMap::const_iterator iter = m_nameToTrackListMap.find(m_currentListName);
	
	if (m_nameToTrackListMap.end() == iter)
		return STATUS_CODE_NOT_INITIALIZED;
	
	pTrackList = iter->second;
	trackListName = m_currentListName;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::GetList(const std::string &listName, const TrackList *pTrackList) const
{
	NameToTrackListMap::const_iterator iter = m_nameToTrackListMap.find(listName);
	
	if (m_nameToTrackListMap.end() == iter)
		return STATUS_CODE_NOT_INITIALIZED;
	
	pTrackList = iter->second;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SetCurrentList(const Algorithm *const pAlgorithm, const std::string &trackListName)
{
	if (m_nameToTrackListMap.end() == m_nameToTrackListMap.find(trackListName))
		return STATUS_CODE_NOT_FOUND;

	if ((m_algorithmInfoMap.size() > 1) || (m_savedLists.end() == m_savedLists.find(trackListName)))
		return STATUS_CODE_NOT_ALLOWED;

	m_currentListName = trackListName;

	AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);	

	if (m_algorithmInfoMap.end() != iter)
	{
		iter->second.m_parentTrackListName = trackListName;
	}
	else
	{
		AlgorithmInfo algorithmInfo;
		algorithmInfo.m_parentTrackListName = trackListName;

		m_algorithmInfoMap[pAlgorithm] = algorithmInfo;
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const TrackList &trackList,
	std::string &temporaryListName)
{
	if (trackList.empty())
		return STATUS_CODE_NOT_INITIALIZED;

	std::ostringstream temporaryListNameStream;
	temporaryListNameStream << pAlgorithm;

	AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);	

	if (m_algorithmInfoMap.end() != iter)
	{
		temporaryListNameStream << "_" << iter->second.m_temporaryTrackListNames.size();
		iter->second.m_temporaryTrackListNames.insert(temporaryListNameStream.str());
	}
	else
	{
		AlgorithmInfo algorithmInfo;
		algorithmInfo.m_parentTrackListName = m_currentListName;
		algorithmInfo.m_temporaryTrackListNames.insert(temporaryListNameStream.str());

		m_algorithmInfoMap[pAlgorithm] = algorithmInfo;
	}
	
	temporaryListName = temporaryListNameStream.str();

	m_nameToTrackListMap[temporaryListName] = new TrackList(trackList);
	m_currentListName = temporaryListName;	

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SaveCurrentList(const std::string &newListName)
{
	if (m_nameToTrackListMap.end() != m_nameToTrackListMap.find(newListName))
		return STATUS_CODE_FAILURE;

	NameToTrackListMap::iterator iter = m_nameToTrackListMap.find(m_currentListName);

	if ((m_nameToTrackListMap.end() == iter) || (iter->second->empty()))
		return STATUS_CODE_NOT_INITIALIZED;

	if (!m_nameToTrackListMap.insert(NameToTrackListMap::value_type(newListName, new TrackList)).second)
		return STATUS_CODE_FAILURE;

	*(m_nameToTrackListMap[newListName]) = *(iter->second);
	m_savedLists.insert(newListName);	
	m_currentListName = newListName;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SaveList(const TrackList &trackList, const std::string &newListName)
{
	if (m_nameToTrackListMap.end() != m_nameToTrackListMap.find(newListName))
		return STATUS_CODE_FAILURE;

	if (!m_nameToTrackListMap.insert(NameToTrackListMap::value_type(newListName, new TrackList)).second)
		return STATUS_CODE_FAILURE;

	*(m_nameToTrackListMap[newListName]) = trackList;
	m_savedLists.insert(newListName);

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::ResetAfterAlgorithmCompletion(const Algorithm *const pAlgorithm)
{
	AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);
	
	if (m_algorithmInfoMap.end() == algorithmListIter)
		return STATUS_CODE_NOT_FOUND;

	for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryTrackListNames.begin(),
		listNameIterEnd = algorithmListIter->second.m_temporaryTrackListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
	{
		NameToTrackListMap::iterator iter = m_nameToTrackListMap.find(*listNameIter);
		
		if (m_nameToTrackListMap.end() == iter)
			return STATUS_CODE_FAILURE;
		
		m_nameToTrackListMap.erase(iter);
	}

	m_currentListName = algorithmListIter->second.m_parentTrackListName;
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

	for (NameToTrackListMap::iterator iter = m_nameToTrackListMap.begin(), iterEnd = m_nameToTrackListMap.end(); iter != iterEnd; ++iter)
	{
		delete iter->second;
		m_nameToTrackListMap.erase(iter);
	}

	m_nameToTrackListMap.clear();
	m_savedLists.clear();
	m_currentListName.clear();

	return STATUS_CODE_SUCCESS;
}

} // namespace pandora
