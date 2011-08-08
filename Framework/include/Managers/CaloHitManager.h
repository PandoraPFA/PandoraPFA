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

#include "Managers/Metadata.h"

#include "Objects/CaloHit.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class Algorithm;

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
     *  @brief  Perform the actual calo hit instantiation
     * 
     *  @param  parameters the calo hit parameters
     * 
     *  @return address of the new calo hit
     */
    template <typename PARAMETERS>
    CaloHit *HitInstantiation(const PARAMETERS &parameters);

    /**
     *  @brief  Create the input calo hit list (accessible to algorithms), using calo hits created by client application
     */
    StatusCode CreateInputCaloHitList();

    /**
     *  @brief  Calculate calo hit properties for input calo hit list. The properties include density weights,
     *          isolation flags, possible mip flags and surrounding energy
     */
    StatusCode CalculateCaloHitProperties() const;

    /**
     *  @brief  Get the current calo hit list
     * 
     *  @param  pCaloHitList to receive the current calo hit list
     *  @param  caloHitListName to receive the name of the current calo hit list
     */
    StatusCode GetCurrentList(const CaloHitList *&pCaloHitList, std::string &caloHitListName) const;

    /**
     *  @brief  Get the algorithm input calo hit list
     * 
     *  @param  pAlgorithm address of the algorithm
     *  @param  pCaloHitList to receive the algorithm input calo hit list
     *  @param  caloHitListName to receive the name of the algorithm input calo hit list
     */
    StatusCode GetAlgorithmInputList(const Algorithm *const pAlgorithm, const CaloHitList *&pCaloHitList, std::string &caloHitListName) const;

    /**
     *  @brief  Get a calo hit list
     * 
     *  @param  listName the name of the list
     *  @param  pCaloHitList to receive the calo hit list
     */
    StatusCode GetList(const std::string &listName, const CaloHitList *&pCaloHitList) const;

    /**
     *  @brief  Replace the current and algorithm input lists with a pre-existing list
     * 
     *  @param  pAlgorithm address of the algorithm changing the current calo hit list
     *  @param  caloHitListName the name of the new current (and algorithm input) calo hit list
     */
    StatusCode ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &caloHitListName);

    /**
     *  @brief  Reset the current list to the algorithm input list
     * 
     *  @param  pAlgorithm address of the algorithm changing the current calo hit list
     */
    StatusCode ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Drop the current list, returning the current list to its default empty/null state
     */
    StatusCode DropCurrentList();

    /**
     *  @brief  Change the current calo hit list to a specified calo hit list
     * 
     *  @param  pAlgorithm address of the algorithm changing the current calo hit list
     *  @param  caloHitList the specified calo hit list
     *  @param  temporaryListName to receive the name of the temporary list
     */
    StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const CaloHitList &caloHitList, std::string &temporaryListName);

    /**
     *  @brief  Change the current calo hit list to the constituent hits of a cluster list
     * 
     *  @param  pAlgorithm address of the algorithm changing the current calo hit list
     *  @param  clusterList the cluster list containing the hits
     *  @param  temporaryListName to receive the name of the temporary list
     */
    StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ClusterList &clusterList, std::string &temporaryListName);

    /**
     *  @brief  Save an calo hit list as a new list with a specified name
     * 
     *  @param  caloHitList the calo hit list
     *  @param  newListName the new list name
     */
    StatusCode SaveList(const CaloHitList &caloHitList, const std::string &newListName);

    /**
     *  @brief  Add hits in a specified calo hit list to an calo hit list
     * 
     *  @param  listName the list to add the hits to
     *  @param  caloHitList the list of calo hits to add
     */
    StatusCode AddCaloHitsToList(const std::string &listName, const CaloHitList &caloHitList);

    /**
     *  @brief  Match calo hits to their correct mc particles for particle flow
     * 
     *  @param  caloHitToPfoTargetMap the calo hit uid to mc pfo target map
     */
    StatusCode MatchCaloHitsToMCPfoTargets(const UidToMCParticleMap &caloHitToPfoTargetMap);

    /**
     *  @brief  Is calo hit available to add to a cluster
     * 
     *  @param  pCaloHit address of the calo hit
     * 
     *  @return boolean
     */
    bool IsCaloHitAvailable(CaloHit *const pCaloHit) const;

    /**
     *  @brief  Are all calo hits in list available to add to a cluster
     * 
     *  @param  caloHitList the list of calo hits
     * 
     *  @return boolean
     */
    bool AreCaloHitsAvailable(const CaloHitList &caloHitList) const;

    /**
     *  @brief  Set availability of a calo hit to be added to a cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     *  @param  isAvailable the calo hit availability
     */
    StatusCode SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable);

    /**
     *  @brief  Set availability of all calo hits in list
     * 
     *  @param  caloHitList the list of calo hits
     *  @param  isAvailable the calo hit availability
     */
    StatusCode SetCaloHitAvailability(CaloHitList &caloHitList, bool isAvailable);

    /**
     *  @brief  Initialize reclustering operations, preparing lists and metadata accordingly
     * 
     *  @param  pAlgorithm address of the algorithm controlling reclustering
     *  @param  clusterList the input cluster list
     *  @param  originalReclusterListName the list name/key for the original recluster candidates
     */
    StatusCode InitializeReclustering(const Algorithm *const pAlgorithm, const ClusterList &clusterList, const std::string &originalReclusterListName);

    /**
     *  @brief  Prepare metadata to allow for construction of new recluster candidates
     * 
     *  @param  pAlgorithm address of the algorithm controlling reclustering
     *  @param  newReclusterListName the list name/key for the new recluster candidates
     */
    StatusCode PrepareForClustering(const Algorithm *const pAlgorithm, const std::string &newReclusterListName);

    /**
     *  @brief  End reclustering operations and update calo hit lists accordingly
     * 
     *  @param  pAlgorithm address of the algorithm controlling reclustering
     *  @param  selectedReclusterListName the list name/key for the chosen recluster candidates
     */
    StatusCode EndReclustering(const Algorithm *const pAlgorithm, const std::string &selectedReclusterListName);

    /**
     *  @brief  Update all calo hit lists to account for changes by daughter recluster processes
     * 
     *  @param  caloHitMetadata description of the changes made by daughter reclustering processes
     */
    StatusCode Update(const CaloHitMetadata &caloHitMetadata);

    /**
     *  @brief  Update all calo hit lists to account for a specific calo hit replacement
     * 
     *  @param  caloHitReplacement the calo hit replacement
     */
    StatusCode Update(const CaloHitReplacement &caloHitReplacement);

    /**
     *  @brief  Update a calo hit list to account for a specific calo hit replacement
     * 
     *  @param  pCaloHitList address of the calo hit list
     *  @param  caloHitReplacement the calo hit replacement
     */
    StatusCode Update(CaloHitList *pCaloHitList, const CaloHitReplacement &caloHitReplacement);

    /**
     *  @brief  Fragment a calo hit into two daughter calo hits, with a specified energy division
     *
     *  @param  pOriginalCaloHit address of the original calo hit, which will be deleted
     *  @param  fraction1 the fraction of energy to be assigned to daughter fragment 1
     *  @param  pDaughterCaloHit1 to receive the address of daughter fragment 1
     *  @param  pDaughterCaloHit2 to receive the address of daughter fragment 2
     */
    StatusCode FragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1, CaloHit *&pDaughterCaloHit1, CaloHit *&pDaughterCaloHit2);

    /**
     *  @brief  Merge two calo hit fragments, originally from the same parent hit, to form a new calo hit
     *
     *  @param  pFragmentCaloHit1 address of calo hit fragment 1, which will be deleted
     *  @param  pFragmentCaloHit2 address of calo hit fragment 2, which will be deleted
     *  @param  pMergedCaloHit to receive the address of the merged calo hit
     */
    StatusCode MergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2, CaloHit *&pMergedCaloHit);

    /**
     *  @brief  Whether a calo hit can be fragmented into two daughter calo hits with the specified energy division
     * 
     *  @param  pOriginalCaloHit address of the original calo hit
     *  @param  fraction1 the fraction of energy to be assigned to daughter fragment 1
     * 
     *  @return boolean
     */
    bool CanFragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1) const;

    /**
     *  @brief  Whether two candidate calo hit fragments can be merged
     * 
     *  @param  pFragmentCaloHit1 address of first candidate calo hit fragment
     *  @param  pFragmentCaloHit2 address of second candidate calo hit fragment
     * 
     *  @return boolean
     */
    bool CanMergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2) const;

    /**
     *  @brief  Register an algorithm with the calo hit manager
     * 
     *  @param  pAlgorithm address of the algorithm
     */
    StatusCode RegisterAlgorithm(const Algorithm *const pAlgorithm);

    /**
     *  @brief  Remove temporary lists and reset the current calo hit list to that when algorithm was initialized
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
     *  @brief  Erase all calo hit manager content
     */
    StatusCode EraseAllContent();

    /**
     *  @brief  Create initial calo hit lists
     */
    StatusCode CreateInitialLists();

    /**
     *  @brief  AlgorithmInfo class
     */
    class AlgorithmInfo
    {
    public:
        std::string                 m_parentListName;                   ///< The current calo hit list when algorithm was initialized
        StringSet                   m_temporaryListNames;               ///< The temporary calo hit list names
        unsigned int                m_numberOfListsCreated;             ///< The number of calo hit lists created by the algorithm
    };

    typedef std::map<std::string, CaloHitList *> NameToCaloHitListMap;
    typedef std::map<const Algorithm *, AlgorithmInfo> AlgorithmInfoMap;

    NameToCaloHitListMap            m_nameToCaloHitListMap;             ///< The name to calo hit list map
    AlgorithmInfoMap                m_algorithmInfoMap;                 ///< The algorithm info map

    std::string                     m_currentListName;                  ///< The name of the current calo hit list
    StringSet                       m_savedLists;                       ///< The set of saved calo hit lists
    static const std::string        NULL_LIST_NAME;                     ///< The name of the default empty (NULL) calo hit list
    static const std::string        INPUT_LIST_NAME;                    ///< The name of the input calo hit list

    unsigned int                    m_nReclusteringProcesses;           ///< The number of reclustering algorithms currently in operation
    ReclusterMetadata              *m_pCurrentReclusterMetadata;        ///< Address of the current recluster metadata
    ReclusterMetadataList           m_reclusterMetadataList;            ///< The recluster metadata list

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::GetCurrentList(const CaloHitList *&pCaloHitList, std::string &caloHitListName) const
{
    caloHitListName = m_currentListName;

    return this->GetList(caloHitListName, pCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::GetAlgorithmInputList(const Algorithm *const pAlgorithm, const CaloHitList *&pCaloHitList,
    std::string &caloHitListName) const
{
    AlgorithmInfoMap::const_iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() != iter)
    {
        caloHitListName = iter->second.m_parentListName;
    }
    else
    {
        caloHitListName = m_currentListName;
    }

    return this->GetList(caloHitListName, pCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::ResetCurrentListToAlgorithmInputList(const Algorithm *const pAlgorithm)
{
    const CaloHitList *pCaloHitList(NULL);
    return this->GetAlgorithmInputList(pAlgorithm, pCaloHitList, m_currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::DropCurrentList()
{
    m_currentListName = NULL_LIST_NAME;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHitManager::IsCaloHitAvailable(CaloHit *const pCaloHit) const
{
    if (0 == m_nReclusteringProcesses)
        return pCaloHit->m_isAvailable;

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->IsCaloHitAvailable(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
    {
        pCaloHit->m_isAvailable = isAvailable;
        return STATUS_CODE_SUCCESS;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->SetCaloHitAvailability(pCaloHit, isAvailable);
}

} // namespace pandora

#endif // #ifndef CALO_HIT_MANAGER_H
