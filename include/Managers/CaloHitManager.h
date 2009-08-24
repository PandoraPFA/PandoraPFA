/**
 *	@file	PandoraPFANew/include/Managers/CaloHitManager.h
 * 
 *	@brief	Header file for the calo hit manager class.
 * 
 *	$Log: $
 */
#ifndef CALO_HIT_MANAGER_H
#define CALO_HIT_MANAGER_H 1

#include "Api/PandoraApi.h"

namespace pandora
{
	
class OrderedCaloHitList;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *	@brief	CaloHitManager class
 */
class CaloHitManager
{
public:
	 /**
	 *	@brief	Destructor
	 */
	~CaloHitManager();

private:
	/**
	 *	@brief	Create calo hit
	 * 
	 *	@param	caloHitParameters the calo hit parameters
	 */
	StatusCode CreateCaloHit(const PandoraApi::CaloHitParameters &caloHitParameters);

	/**
	 *	@brief	Order input calo hits by pseudo layer
	 */
	StatusCode OrderInputCaloHits();

	/**
	 *	@brief	Get the current ordered calo hit list
	 * 
	 *	@param	pOrderedCaloHitList to receive the current ordered calo hit list
	 *	@param	orderedCaloHitListName to receive the name of the current ordered calo hit list
	 */
	StatusCode GetCurrentList(const OrderedCaloHitList *pOrderedCaloHitList, std::string &orderedCaloHitListName) const;

	/**
	 *	@brief	Get an ordered calo hit list
	 * 
 	 *	@param	listName the name of the list
	 *	@param	pOrderedCaloHitList to receive the ordered calo hit list
	 */
	StatusCode GetList(const std::string &listName, const OrderedCaloHitList *pOrderedCaloHitList) const;
	
	/**
	 *	@brief	Change the current ordered calo hit list to a pre-existing list
	 *
	 *	@param	pAlgorithm address of the algorithm changing the current ordered calo hit list
	 *	@param	orderedCaloHitListName the name of the new ordered calo hit list
	 */	
	StatusCode SetCurrentList(const Algorithm *const pAlgorithm, const std::string &orderedCaloHitListName);

	/**
	 *	@brief	Change the current ordered calo hit list to a specified ordered calo hit list
	 *
 	 *	@param	pAlgorithm address of the algorithm changing the current ordered calo hit list
	 * 	@param	orderedCaloHitList the specified ordered calo hit list
	 * 	@param	temporaryListName to receive the name of the temporary list
	 */	
	StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const OrderedCaloHitList &orderedCaloHitList, 
		std::string &temporaryListName);

	/**
	 *	@brief	Change the current ordered calo hit list to the constituent hits of a cluster list
	 *
 	 *	@param	pAlgorithm address of the algorithm changing the current ordered calo hit list
	 * 	@param	clusterList the cluster list containing the hits
	 * 	@param	temporaryListName to receive the name of the temporary list
	 */	
	StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ClusterList &clusterList,
		std::string &temporaryListName);
	
	/**
	 *	@brief	Save the current ordered calo hit list under a new specified name
	 *
	 *	@param	newListName the new list name
	 */		
	StatusCode SaveCurrentList(const std::string &newListName);

	/**
	 *	@brief	Save an ordered calo hit list as a new list with a specified name
	 * 
	 *	@param	orderedCaloHitList the ordered calo hit list
	 *	@param	newListName the new list name
	 */	
	StatusCode SaveList(const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName);
	
	/**
	 *	@brief	Add hits in a specified ordered calo hit list to the current ordered calo hit list
	 *
 	 *	@param	orderedCaloHitList
	 */
	StatusCode AddCaloHitsToCurrentList(const OrderedCaloHitList &orderedCaloHitList);
	
	/**
	 *	@brief	Add constituent hits of a cluster list to the current ordered calo hit list
	 *
 	 *	@param	clusterList the cluster list containing the hits
	 */	
	StatusCode AddCaloHitsToCurrentList(const ClusterList &clusterList);
	
	/**
	 *	@brief	Remove hits in a specified ordered calo hit list from the current ordered calo hit list
	 *
 	 *	@param	orderedCaloHitList
	 */
	StatusCode RemoveCaloHitsFromCurrentList(const OrderedCaloHitList &orderedCaloHitList);
	
	/**
	 *	@brief	Remove constituent hits of a cluster list from the current ordered calo hit list
	 *
 	 *	@param	clusterList the cluster list containing the hits
	 */	
	StatusCode RemoveCaloHitsFromCurrentList(const ClusterList &clusterList);
	
	/**
	 *	@brief	Match calo hits to their correct mc particles for particle flow
	 * 
	 *	@param	caloHitToPfoTargetMap the calo hit uid to mc pfo target map
	 */
	StatusCode MatchCaloHitsToMCPfoTargets(const UidToMCParticleMap &caloHitToPfoTargetMap);

	/**
	 *	@brief	Remove temporary lists and reset the current ordered calo hit list to that when algorithm was initialized
	 * 
	 *	@param	pAlgorithm address of the algorithm altering the lists
	 */
	StatusCode ResetAfterAlgorithmCompletion(const Algorithm *const pAlgorithm);
	
	/**
	 *	@brief	Reset the calo hit manager
	 */	
	StatusCode ResetForNextEvent();
	
	/**
	 *	@brief	AlgorithmInfo class
	 */	
	class AlgorithmInfo
	{
	public:
		std::string					m_parentOrderedCaloHitListName;		///< The current ordered calo hit list when algorithm was initialized
		StringSet					m_temporaryOrderedCaloHitListNames;	///< The temporary ordered calo hit list names
	};
	
	typedef std::map<std::string, OrderedCaloHitList *> NameToOrderedCaloHitListMap;
	typedef std::map<const Algorithm *, AlgorithmInfo> AlgorithmInfoMap;
	
	InputCaloHitList				m_inputCaloHitList;					///< The input calo hit list

	NameToOrderedCaloHitListMap		m_nameToOrderedCaloHitListMap;		///< The name to ordered calo hit list map
	AlgorithmInfoMap				m_algorithmInfoMap;					///< The algorithm info map	

	std::string						m_currentListName;					///< The name of the current ordered calo hit list
	StringSet						m_savedLists;						///< The set of saved ordered calo hit lists
	
	static const std::string		INPUT_LIST_NAME;					///< The name of the input ordered calo hit list
	
	friend class PandoraApiImpl;
	friend class PandoraContentApiImpl;	
};

} // namespace pandora

#endif // #ifndef CALO_HIT_MANAGER_H
