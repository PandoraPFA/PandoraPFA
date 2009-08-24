/**
 *	@file	PandoraPFANew/include/Objects/Cluster.h
 * 
 *	@brief	Header file for the cluster class.
 * 
 *	$Log: $
 */
#ifndef CLUSTER_H
#define CLUSTER_H 1

#include "Api/PandoraContentApi.h"

#include "Managers/ClusterManager.h"

#include "Objects/OrderedCaloHitList.h"

namespace pandora
{

class CaloHit;

//------------------------------------------------------------------------------------------------------------------------------------------
	
/**
 *	@brief	Cluster class
 */
class Cluster
{
public:
	/**
	 *	@brief	Get the ordered calo hit list
	 * 
	 *	@return	Address of the ordered calo hit list
	 */	
	const OrderedCaloHitList *const GetOrderedCaloHitList() const;
	
	/**
	 *	@brief	Add a calo hit to the cluster
	 * 
	 *	@param	pCaloHit the address of the calo hit
	 */
	StatusCode AddCaloHit(CaloHit *const pCaloHit);

	/**
	 *	@brief	Get calo hits in specified pseudo layer
	 * 
	 *	@param	pseudoLayer the pseudo layer
	 *	@param	pCaloHitList to receive the address of the relevant calo hit list
	 */
	StatusCode GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *pCaloHitList) const;
	
private:
	/**
	 *	@brief	Constructor
	 * 
	 *	@param	pCaloHit address of calo hit with which initialize cluster
	 */
	Cluster(CaloHit *pCaloHit);

	/**
	 *	@brief	Constructor
	 * 
	 *	@param	pCaloHitList calo hits with which to create cluster
	 */
	Cluster(const InputCaloHitList *pCaloHitList);

	/**
	 *	@brief	Constructor
	 * 
	 *	@param	pTrack address of track with which to seed cluster
	 */
	Cluster(const Track *pTrack);

	/**
	 *	@brief	Destructor
	 */
	~Cluster();

	OrderedCaloHitList		m_orderedCaloHitList;	///< The ordered calo hit list

	friend class ClusterManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const OrderedCaloHitList *const Cluster::GetOrderedCaloHitList() const
{
	return &m_orderedCaloHitList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Cluster::AddCaloHit(CaloHit *const pCaloHit)
{
	return m_orderedCaloHitList.AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Cluster::GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *pCaloHitList) const
{
	return m_orderedCaloHitList.GetCaloHitsInPseudoLayer(pseudoLayer, pCaloHitList);
}

} // namespace pandora

#endif // #ifndef CLUSTER_H
