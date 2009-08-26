/**
 *	@file	PandoraPFANew/src/Objects/OrderedCaloHitList.cc
 * 
 *	@brief	Implementation of the ordered calo hit list class.
 * 
 *	$Log: $
 */

#include "Objects/CaloHit.h"
#include "Objects/OrderedCaloHitList.h"

namespace pandora
{

StatusCode OrderedCaloHitList::Add(const OrderedCaloHitList &rhs)
{
	for (OrderedCaloHitList::const_iterator iter = rhs.begin(), iterEnd = rhs.end(); iter != iterEnd; ++iter)
	{
		for (CaloHitList::const_iterator caloHitIter = iter->second->begin(), caloHitIterEnd = iter->second->end();
			caloHitIter != caloHitIterEnd; ++caloHitIter)
		{
			PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(*caloHitIter, iter->first));
		}
	}

	return STATUS_CODE_SUCCESS;	
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::Remove(const OrderedCaloHitList &rhs)
{
	for (OrderedCaloHitList::const_iterator iter = rhs.begin(), iterEnd = rhs.end(); iter != iterEnd; ++iter)
	{
		for (CaloHitList::const_iterator caloHitIter = iter->second->begin(), caloHitIterEnd = iter->second->end();
			caloHitIter != caloHitIterEnd; ++caloHitIter)
		{
			PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, this->RemoveCaloHit(*caloHitIter, iter->first));
		}
	}

	return STATUS_CODE_SUCCESS;		
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::AddCaloHit(CaloHit *const pCaloHit)
{
	PseudoLayer pseudoLayer;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCaloHit->GetPseudoLayer(pseudoLayer));
	
	return this->AddCaloHit(pCaloHit, pseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::RemoveCaloHit(CaloHit *const pCaloHit)
{
	PseudoLayer pseudoLayer;
	PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCaloHit->GetPseudoLayer(pseudoLayer));
	
	return this->RemoveCaloHit(pCaloHit, pseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *&pCaloHitList) const
{
	OrderedCaloHitList::const_iterator iter = this->find(pseudoLayer);
	
	if (this->end() == iter)
		return STATUS_CODE_NOT_FOUND;

	pCaloHitList = iter->second;

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::AddCaloHit(CaloHit *const pCaloHit, const PseudoLayer pseudoLayer)
{
	OrderedCaloHitList::iterator iter = this->find(pseudoLayer);

	if (this->end() == iter)
	{
		CaloHitList *pCaloHitList = new CaloHitList;
		pCaloHitList->insert(pCaloHit);
		
		if (!(this->insert(OrderedCaloHitList::value_type(pseudoLayer, pCaloHitList)).second))
			return STATUS_CODE_FAILURE;
	}
	else
	{
		iter->second->insert(pCaloHit);	
	}

	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode OrderedCaloHitList::RemoveCaloHit(CaloHit *const pCaloHit, const PseudoLayer pseudoLayer)
{
	OrderedCaloHitList::iterator listIter = this->find(pseudoLayer);

	if (this->end() == listIter)
		return STATUS_CODE_NOT_FOUND;
	
	std::pair<CaloHitList::const_iterator, CaloHitList::const_iterator> range;
	range = listIter->second->equal_range(pCaloHit);

	if (listIter->second->end() == range.first)
		return STATUS_CODE_NOT_FOUND;

	for (CaloHitList::iterator caloHitIter = range.first, caloHitIterEnd = range.second; caloHitIter != caloHitIterEnd; ++caloHitIter)
	{
		if (pCaloHit == *caloHitIter)
			listIter->second->erase(caloHitIter);
	}
	
	if (listIter->second->empty())
		this->erase(listIter);

	return STATUS_CODE_SUCCESS;
}

} // namespace pandora
