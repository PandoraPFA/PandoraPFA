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
	this->AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(const InputCaloHitList *pCaloHitList)
{
	for (InputCaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
		this->AddCaloHit(*iter);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(const Track *pTrack)
{
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

} // namespace pandora
