/**
 *	@file	PandoraPFANew/include/Managers/TrackManager.h
 * 
 *	@brief	Header file for the track manager class.
 * 
 *	$Log: $
 */
#ifndef TRACK_MANAGER_H
#define TRACK_MANAGER_H 1

#include "Api/PandoraApi.h"

namespace pandora
{

/**
 *	@brief TrackManager class
 */
class TrackManager
{
public:
	/**
	 *	@brief	Default constructor
	 */
	TrackManager();
	
	/**
	 *	@brief	Destructor
	 */
	~TrackManager();

private:
	/**
	 *	@brief	Create track
	 * 
	 *	@param	trackParameters the track parameters
	 */
	StatusCode CreateTrack(const PandoraApi::TrackParameters &trackParameters);

	/**
	 *	@brief	Get the current track list name
	 * 
	 *	@param	trackListName to receive the current track list name
	 */
	StatusCode GetCurrentListName(std::string &trackListName) const;

	/**
	 *	@brief	Get the algorithm input track list name
	 * 
	 *	@param	pAlgorithm address of the algorithm
	 *	@param	trackListName to receive the algorithm input track list name
	 */
	StatusCode GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &trackListName) const;
	
	/**
	 *	@brief	Get the current track list
	 * 
	 *	@param	pTrackList to receive the current track list
	 *	@param	trackListName to receive the name of the current track list
	 */
	StatusCode GetCurrentList(const TrackList *&pTrackList, std::string &trackListName) const;

	/**
	 *	@brief	Get the algorithm input track list
	 * 
	 *	@param	pAlgorithm address of the algorithm
	 *	@param	pTrackList to receive the algorithm input track list
	 *	@param	trackListName to receive the name of the algorithm input track list
	 */
	StatusCode GetAlgorithmInputList(const Algorithm *const pAlgorithm, const TrackList *&pTrackList, std::string &trackListName) const;
	
	/**
	 *	@brief	Get a track list
	 * 
 	 *	@param	listName the name of the list
	 *	@param	pTrackList to receive the track list
	 */
	StatusCode GetList(const std::string &listName, const TrackList *&pTrackList) const;
	
	/**
	 *	@brief	Replace the current and algorithm input lists with a pre-existing list
	 *
	 *	@param	pAlgorithm address of the algorithm changing the current track list
	 *	@param	trackListName the name of the new current (and algorithm input) track list
	 */	
	StatusCode ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &trackListName);

	/**
	 *	@brief	Change the current track list to a specified temporary list of tracks
	 *
	 *	@param	pAlgorithm address of the algorithm changing the current track list
	 *	@param	trackList the specified temporary list of tracks
	 *	@param	temporaryListName to receive the name of the temporary list
	 */	
	StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const TrackList &trackList, 
		std::string &temporaryListName);

	/**
	 *	@brief	Save a list of tracks as a new list with a specified name
	 * 
	 *	@param	pTrackList the list of tracks
	 *	@param	newListName the new list name
	 */	
	StatusCode SaveList(const TrackList &trackList, const std::string &newListName);

	/**
	 *	@brief	Register an algorithm with the track manager
	 * 
	 *	@param	pAlgorithm address of the algorithm
	 */
	StatusCode RegisterAlgorithm(const Algorithm *const pAlgorithm);

	/**
	 *	@brief	Remove temporary lists and reset the current track list to that when algorithm was initialized
	 * 
	 *	@param	pAlgorithm address of the algorithm altering the lists
	 */
	StatusCode ResetAfterAlgorithmCompletion(const Algorithm *const pAlgorithm);

	/**
	 *	@brief	Reset the track manager
	 */	
	StatusCode ResetForNextEvent();

	/**
	 *	@brief	AlgorithmInfo class
	 */	
	class AlgorithmInfo
	{
	public:
		std::string					m_parentListName;				///< The current track list when algorithm was initialized
		StringSet					m_temporaryListNames;			///< The temporary track list names		
	};

	typedef std::map<std::string, TrackList *> NameToTrackListMap;
	typedef std::map<const Algorithm *, AlgorithmInfo> AlgorithmInfoMap;

	NameToTrackListMap				m_nameToTrackListMap;			///< The name to track list map
	AlgorithmInfoMap				m_algorithmInfoMap;				///< The algorithm info map

	std::string						m_currentListName;				///< The name of the current track list
	StringSet						m_savedLists;					///< The set of saved track lists

	static const std::string		INPUT_LIST_NAME;				///< The name of the input track list

	friend class PandoraApiImpl;
	friend class PandoraContentApiImpl;	
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode TrackManager::GetCurrentListName(std::string &trackListName) const
{
	if (m_currentListName.empty())
		return STATUS_CODE_NOT_INITIALIZED;
		
	trackListName = m_currentListName;
	
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode TrackManager::GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &trackListName) const
{
	AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);	

	if (m_algorithmInfoMap.end() == iter)
		return this->GetCurrentListName(trackListName);

	trackListName = iter->second.m_parentListName;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode TrackManager::GetCurrentList(const TrackList *&pTrackList, std::string &trackListName) const
{
	trackListName = m_currentListName;

	return this->GetList(trackListName, pTrackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode TrackManager::GetAlgorithmInputList(const Algorithm *const pAlgorithm, const TrackList *&pTrackList,
	std::string &trackListName) const
{
	AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);	

	if (m_algorithmInfoMap.end() != iter)
	{
		trackListName = iter->second.m_parentListName;
	}
	else
	{
		trackListName = m_currentListName;
	}
	
	return this->GetList(trackListName, pTrackList);	
}

} // namespace pandora

#endif // #ifndef TRACK_MANAGER_H
