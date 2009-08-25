/**
 *	@file	PandoraPFANew/src/Objects/Cluster.cc
 * 
 *	@brief	Implementation of the cluster class.
 * 
 *	$Log: $
 */

#include "Objects/Cluster.h"

namespace pandora
{

Cluster::Cluster(CaloHit *pCaloHit)
{
	if (NULL == pCaloHit)
		throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
	
	this->AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(InputCaloHitList *pCaloHitList)
{
	if (NULL == pCaloHitList)
		throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
	
	for (InputCaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
		this->AddCaloHit(*iter);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(Track *pTrack)
{
	if (NULL == pTrack)
		throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);	
	
	// TODO, case where track specified
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::~Cluster()
{
	for (OrderedCaloHitList::iterator iter = m_orderedCaloHitList.begin(), iterEnd = m_orderedCaloHitList.end(); iter != iterEnd; ++iter)
	{
		iter->second->clear();
		delete iter->second;
	}
	
	m_orderedCaloHitList.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddHitsFromSecondCluster(Cluster *const pCluster)
{
	return m_orderedCaloHitList.Add(*(pCluster->GetOrderedCaloHitList()));
}

} // namespace pandora
