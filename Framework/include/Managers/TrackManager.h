/**
 *  @file   PandoraPFANew/Framework/include/Managers/TrackManager.h
 * 
 *  @brief  Header file for the track manager class.
 * 
 *  $Log: $
 */
#ifndef TRACK_MANAGER_H
#define TRACK_MANAGER_H 1

#include "Api/PandoraApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class Algorithm;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief TrackManager class
 */
class TrackManager
{
public:
    /**
     *  @brief  Default constructor
     */
    TrackManager();

    /**
     *  @brief  Destructor
     */
    ~TrackManager();

private:
    /**
     *  @brief  Create track
     * 
     *  @param  trackParameters the track parameters
     */
    StatusCode CreateTrack(const PandoraApi::TrackParameters &trackParameters);

    /**
     *  @brief  Create the null track list
     */
    StatusCode CreateNullList();

    /**
     *  @brief  Delete the null track list
     */
    void DeleteNullList();

    /**
     *  @brief  Create the input track list (accessible to algorithms), using contents of the input track vector
     */
    StatusCode CreateInputTrackList();

    /**
     *  @brief  Get the current track list name
     * 
     *  @param  trackListName to receive the current track list name
     */
    StatusCode GetCurrentListName(std::string &trackListName) const;

    /**
     *  @brief  Get the algorithm input track list name
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  trackListName to receive the algorithm input track list name
     */
    StatusCode GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &trackListName) const;

    /**
     *  @brief  Get the current track list
     * 
     *  @param  pTrackList to receive the current track list
     *  @param  trackListName to receive the name of the current track list
     */
    StatusCode GetCurrentList(const TrackList *&pTrackList, std::string &trackListName) const;

    /**
     *  @brief  Get the algorithm input track list
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  pTrackList to receive the algorithm input track list
     *  @param  trackListName to receive the name of the algorithm input track list
     */
    StatusCode GetAlgorithmInputList(const Algorithm *const pAlgorithm, const TrackList *&pTrackList, std::string &trackListName) const;

    /**
     *  @brief  Get a track list
     * 
     *  @param  listName the name of the list
     *  @param  pTrackList to receive the track list
     */
    StatusCode GetList(const std::string &listName, const TrackList *&pTrackList) const;

    /**
     *  @brief  Replace the current and algorithm input lists with a pre-existing list
     *
     *  @param  pAlgorithm address of the algorithm changing the current track list
     *  @param  trackListName the name of the new current (and algorithm input) track list
     */
    StatusCode ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &trackListName);

    /**
     *  @brief  Reset the current list to the algorithm input list
     *
     *  @param  pAlgorithm address of the algorithm changing the current track list
     */
    StatusCode ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Drop the current list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentList();

    /**
     *  @brief  Change the current track list to a specified temporary list of tracks
     *
     *  @param  pAlgorithm address of the algorithm changing the current track list
     *  @param  trackList the specified temporary list of tracks
     *  @param  temporaryListName to receive the name of the temporary list
     */
    StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const TrackList &trackList, 
        std::string &temporaryListName);

    /**
     *  @brief  Save a list of tracks as a new list with a specified name
     * 
     *  @param  pTrackList the list of tracks
     *  @param  newListName the new list name
     */
    StatusCode SaveList(const TrackList &trackList, const std::string &newListName);

    /**
     *  @brief  Add tracks to a saved list with a specified name
     *
     *  @param  listName the list to add the tracks to
     *  @param  trackList the list of tracks to be added
     */
    StatusCode AddTracksToList(const std::string &listName, const TrackList &trackList);

    /**
     *  @brief  Remove tracks from a saved track list
     *
     *  @param  listName the list to remove the tracks from
     *  @param  trackList the lsit of tracks to be removed
     */
    StatusCode RemoveTracksFromList(const std::string &listName, const TrackList &trackList);

    /**
     *  @brief  Match tracks to their correct mc particles for particle flow
     *
     *  @param  trackToPfoTargetMap the track uid to mc pfo target map
     */
    StatusCode MatchTracksToMCPfoTargets(const UidToMCParticleMap &trackToPfoTargetMap);

    /**
     *  @brief  Register an algorithm with the track manager
     * 
     *  @param  pAlgorithm address of the algorithm
     */
    StatusCode RegisterAlgorithm(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Remove temporary lists and reset the current track list to that when algorithm was initialized
     * 
     *  @param  pAlgorithm address of the algorithm altering the lists
     *  @param  isAlgorithmFinished whether the algorithm has completely finished and the algorithm info should be entirely removed
     */
    StatusCode ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished);

    /**
     *  @brief  Reset the track manager
     */
    StatusCode ResetForNextEvent();

    /**
     *  @brief  Set a track parent-daughter relationship
     * 
     *  @param  parentUid the parent track unique identifier
     *  @param  daughterUid the daughter track unique identifier
     */
    StatusCode SetTrackParentDaughterRelationship(const Uid parentUid, const Uid daughterUid);

    /**
     *  @brief  Set a track sibling relationship
     * 
     *  @param  firstSiblingUid the first sibling track unique identifier
     *  @param  secondSiblingUid the second sibling track unique identifier
     */
    StatusCode SetTrackSiblingRelationship(const Uid firstSiblingUid, const Uid secondSiblingUid);

    /**
     *  @brief  Apply track associations (parent-daughter and sibling) that have been registered with the track manager
     */
    StatusCode AssociateTracks() const;

    /**
     *  @brief  Add parent-daughter associations to tracks
     */
    StatusCode AddParentDaughterAssociations() const;

    /**
     *  @brief  Add sibling associations to tracks
     */
    StatusCode AddSiblingAssociations() const;

    /**
     *  @brief  Remove all track to cluster associations
     */
    StatusCode RemoveAllClusterAssociations() const;

    /**
     *  @brief  Remove track to cluster associations from all tracks in the current list
     * 
     *  @param  danglingClusters to receive the list of "dangling" associations
     */
    StatusCode RemoveCurrentClusterAssociations(TrackToClusterMap &danglingClusters) const;

    /**
     *  @brief  Remove track to cluster associations from a specified list of tracks
     * 
     *  @param  trackList the specified track list
     */
    StatusCode RemoveClusterAssociations(const TrackList &trackList) const;

    /**
     *  @brief  AlgorithmInfo class
     */
    class AlgorithmInfo
    {
    public:
        std::string                 m_parentListName;                   ///< The current track list when algorithm was initialized
        StringSet                   m_temporaryListNames;               ///< The temporary track list names
        unsigned int                m_numberOfListsCreated;             ///< The number of track lists created by the algorithm
    };

    typedef std::map<std::string, TrackList *> NameToTrackListMap;
    typedef std::map<const Algorithm *, AlgorithmInfo> AlgorithmInfoMap;

    TrackVector                     m_inputTrackVector;                 ///< The input track vector
    NameToTrackListMap              m_nameToTrackListMap;               ///< The name to track list map
    AlgorithmInfoMap                m_algorithmInfoMap;                 ///< The algorithm info map

    std::string                     m_currentListName;                  ///< The name of the current track list
    StringSet                       m_savedLists;                       ///< The set of saved track lists
    static const std::string        NULL_LIST_NAME;                     ///< The name of the default empty (NULL) track list
    static const std::string        INPUT_LIST_NAME;                    ///< The name of the input track list

    typedef std::map<Uid, Track *> UidToTrackMap;
    typedef std::multimap<Uid, Uid> TrackRelationMap;

    UidToTrackMap                   m_uidToTrackMap;                    ///< The uid to track map
    TrackRelationMap                m_parentDaughterRelationMap;        ///< The track parent-daughter relation map
    TrackRelationMap                m_siblingRelationMap;               ///< The track sibling relation map

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
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

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode TrackManager::ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm)
{
    return this->GetAlgorithmInputListName(pAlgorithm, m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode TrackManager::DropCurrentList()
{
    m_currentListName = NULL_LIST_NAME;
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef TRACK_MANAGER_H
