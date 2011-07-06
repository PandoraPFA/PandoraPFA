/**
 *  @file   PandoraPFANew/Framework/include/Managers/CaloHitManager.h
 * 
 *  @brief  Header file for the calo hit manager class.
 * 
 *  $Log: $
 */
#ifndef CALO_HIT_MANAGER_H
#define CALO_HIT_MANAGER_H 1

#include "Api/PandoraApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class Algorithm;
class OrderedCaloHitList;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  CaloHitManager class
 */
class CaloHitManager
{
public:
    /**
     *  @brief  Default constructor
     */
    CaloHitManager();

     /**
     *  @brief  Destructor
     */
    ~CaloHitManager();

private:
    /**
     *  @brief  Create calo hit
     * 
     *  @param  parameters the calo hit parameters
     */
    template <typename PARAMETERS>
    StatusCode CreateCaloHit(const PARAMETERS &parameters);

    /**
     *  @brief  Create the null ordered calo hit list
     */
    StatusCode CreateNullList();

    /**
     *  @brief  Delete the null ordered calo hit list
     */
    void DeleteNullList();

    /**
     *  @brief  Order input calo hits by pseudo layer
     */
    StatusCode OrderInputCaloHits();

    /**
     *  @brief  Calculate calo hit properties for input ordered calo hit list. The properties include density weights,
     *          isolation flags, possible mip flags and surrounding energy
     */
    StatusCode CalculateCaloHitProperties() const;

    /**
     *  @brief  Get the current ordered calo hit list name
     * 
     *  @param  orderedCaloHitListName to receive the current ordered calo hit list name
     */
    StatusCode GetCurrentListName(std::string &orderedCaloHitListName) const;

    /**
     *  @brief  Get the algorithm input ordered calo hit list name
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  orderedCaloHitListName to receive the algorithm input ordered calo hit list name
     */
    StatusCode GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &orderedCaloHitListName) const;

    /**
     *  @brief  Get the current ordered calo hit list
     * 
     *  @param  pOrderedCaloHitList to receive the current ordered calo hit list
     *  @param  orderedCaloHitListName to receive the name of the current ordered calo hit list
     */
    StatusCode GetCurrentList(const OrderedCaloHitList *&pOrderedCaloHitList, std::string &orderedCaloHitListName) const;

    /**
     *  @brief  Get the algorithm input ordered calo hit list
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  pOrderedCaloHitList to receive the algorithm input ordered calo hit list
     *  @param  orderedCaloHitListName to receive the name of the algorithm input ordered calo hit list
     */
    StatusCode GetAlgorithmInputList(const Algorithm *const pAlgorithm, const OrderedCaloHitList *&pOrderedCaloHitList,
        std::string &orderedCaloHitListName) const;

    /**
     *  @brief  Get an ordered calo hit list
     * 
     *  @param  listName the name of the list
     *  @param  pOrderedCaloHitList to receive the ordered calo hit list
     */
    StatusCode GetList(const std::string &listName, const OrderedCaloHitList *&pOrderedCaloHitList) const;

    /**
     *  @brief  Replace the current and algorithm input lists with a pre-existing list
     *
     *  @param  pAlgorithm address of the algorithm changing the current ordered calo hit list
     *  @param  orderedCaloHitListName the name of the new current (and algorithm input) ordered calo hit list
     */
    StatusCode ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &orderedCaloHitListName);

    /**
     *  @brief  Reset the current list to the algorithm input list
     *
     *  @param  pAlgorithm address of the algorithm changing the current ordered calo hit list
     */
    StatusCode ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Drop the current list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentList();

    /**
     *  @brief  Change the current ordered calo hit list to a specified ordered calo hit list
     *
     *  @param  pAlgorithm address of the algorithm changing the current ordered calo hit list
     *   @param  orderedCaloHitList the specified ordered calo hit list
     *   @param  temporaryListName to receive the name of the temporary list
     */
    StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const OrderedCaloHitList &orderedCaloHitList, 
        std::string &temporaryListName);

    /**
     *  @brief  Change the current ordered calo hit list to the constituent hits of a cluster list
     *
     *  @param  pAlgorithm address of the algorithm changing the current ordered calo hit list
     *  @param  clusterList the cluster list containing the hits
     *  @param  temporaryListName to receive the name of the temporary list
     */
    StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ClusterList &clusterList,
        std::string &temporaryListName);

    /**
     *  @brief  Save an ordered calo hit list as a new list with a specified name
     * 
     *  @param  orderedCaloHitList the ordered calo hit list
     *  @param  newListName the new list name
     */
    StatusCode SaveList(const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName);

    /**
     *  @brief  Add hits in a specified ordered calo hit list to an ordered calo hit list
     *
     *  @param  listName the list to add the hits to
     *  @param  orderedCaloHitList the ordered list of calo hits to add
     */
    StatusCode AddCaloHitsToList(const std::string &listName, const OrderedCaloHitList &orderedCaloHitList);

    /**
     *  @brief  Add constituent hits of a cluster list to an ordered calo hit list
     *
     *  @param  listName the list to add the hits to
     *  @param  clusterList the cluster list containing the hits
     */
    StatusCode AddCaloHitsToList(const std::string &listName, const ClusterList &clusterList);

    /**
     *  @brief  Remove hits in a specified ordered calo hit list from an ordered calo hit list
     *
     *  @param  listName the list to remove the hits from
     *  @param  orderedCaloHitList the ordered list of calo hits to remove
     */
    StatusCode RemoveCaloHitsFromList(const std::string &listName, const OrderedCaloHitList &orderedCaloHitList);

    /**
     *  @brief  Remove constituent hits of a cluster list from an ordered calo hit list
     *
     *  @param  listName the list to remove the hits from
     *  @param  clusterList the cluster list containing the hits
     */
    StatusCode RemoveCaloHitsFromList(const std::string &listName, const ClusterList &clusterList);

    /**
     *  @brief  Match calo hits to their correct mc particles for particle flow
     *
     *  @param  caloHitToPfoTargetMap the calo hit uid to mc pfo target map
     */
    StatusCode MatchCaloHitsToMCPfoTargets(const UidToMCParticleMap &caloHitToPfoTargetMap);

    /**
     *  @brief  Register an algorithm with the calo hit manager
     * 
     *  @param  pAlgorithm address of the algorithm
     */
    StatusCode RegisterAlgorithm(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Remove temporary lists and reset the current ordered calo hit list to that when algorithm was initialized
     * 
     *  @param  pAlgorithm address of the algorithm altering the lists
     *  @param  isAlgorithmFinished whether the algorithm has completely finished and the algorithm info should be entirely removed
     */
    StatusCode ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished);

    /**
     *  @brief  Reset the calo hit manager
     */
    StatusCode ResetForNextEvent();

    /**
     *  @brief  AlgorithmInfo class
     */
    class AlgorithmInfo
    {
    public:
        std::string                 m_parentListName;                   ///< The current ordered calo hit list when algorithm was initialized
        StringSet                   m_temporaryListNames;               ///< The temporary ordered calo hit list names
        unsigned int                m_numberOfListsCreated;             ///< The number of ordered calo hit lists created by the algorithm
    };

    typedef std::map<std::string, OrderedCaloHitList *> NameToOrderedCaloHitListMap;
    typedef std::map<const Algorithm *, AlgorithmInfo> AlgorithmInfoMap;

    CaloHitVector                   m_inputCaloHitVector;               ///< The input calo hit vector
    NameToOrderedCaloHitListMap     m_nameToOrderedCaloHitListMap;      ///< The name to ordered calo hit list map
    AlgorithmInfoMap                m_algorithmInfoMap;                 ///< The algorithm info map

    std::string                     m_currentListName;                  ///< The name of the current ordered calo hit list
    StringSet                       m_savedLists;                       ///< The set of saved ordered calo hit lists
    static const std::string        NULL_LIST_NAME;                     ///< The name of the default empty (NULL) ordered calo hit list
    static const std::string        INPUT_LIST_NAME;                    ///< The name of the input ordered calo hit list

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::GetCurrentListName(std::string &orderedCaloHitListName) const
{
    if (m_currentListName.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    orderedCaloHitListName = m_currentListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::GetAlgorithmInputListName(const Algorithm *const pAlgorithm, std::string &orderedCaloHitListName) const
{
    AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return this->GetCurrentListName(orderedCaloHitListName);

    orderedCaloHitListName = iter->second.m_parentListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::GetCurrentList(const OrderedCaloHitList *&pOrderedCaloHitList, std::string &orderedCaloHitListName) const
{
    orderedCaloHitListName = m_currentListName;

    return this->GetList(orderedCaloHitListName, pOrderedCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::GetAlgorithmInputList(const Algorithm *const pAlgorithm, const OrderedCaloHitList *&pOrderedCaloHitList,
    std::string &orderedCaloHitListName) const
{
    AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() != iter)
    {
        orderedCaloHitListName = iter->second.m_parentListName;
    }
    else
    {
        orderedCaloHitListName = m_currentListName;
    }

    return this->GetList(orderedCaloHitListName, pOrderedCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm)
{
    return this->GetAlgorithmInputListName(pAlgorithm, m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::DropCurrentList()
{
    m_currentListName = NULL_LIST_NAME;
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef CALO_HIT_MANAGER_H
