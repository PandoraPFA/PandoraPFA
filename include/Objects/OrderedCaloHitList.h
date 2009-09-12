/**
 *  @file   PandoraPFANew/include/Objects/OrderedCaloHitList.h
 * 
 *  @brief  Header file for the ordered calo hit list class.
 * 
 *  $Log: $
 */
#ifndef ORDERED_CALO_HIT_LIST_H
#define ORDERED_CALO_HIT_LIST_H 1

#include "Internal.h"
#include "StatusCodes.h"

#include <map>

namespace pandora
{

class CaloHit;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Calo hit lists arranged by pseudo layer
 */
class OrderedCaloHitList : public std::map<PseudoLayer, CaloHitList *>
{
public:
    /**
     *  @brief  Add the hits from a second ordered calo hit list to this list
     * 
     *  @param  rhs the source ordered calo hit list
     */
    StatusCode Add(const OrderedCaloHitList &rhs);

    /**
     *  @brief  Remove the hits in a second ordered calo hit list from this list
     * 
     *  @param  rhs the source ordered calo hit list
     */
    StatusCode Remove(const OrderedCaloHitList &rhs);

    /**
     *  @brief  Add a calo hit to the ordered calo hit list
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode AddCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Remove a calo hit from the ordered calo hit list
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode RemoveCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Get calo hits in specified pseudo layer
     * 
     *  @param  pseudoLayer the pseudo layer
     *  @param  pCaloHitList to receive the address of the relevant calo hit list
     */
    StatusCode GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *&pCaloHitList) const;

private:
    /**
     *  @brief  Add a calo hit to a specified pseudo layer
     * 
     *  @param  pCaloHit the address of the calo hit
     *  @param  pseudoLayer the pesudo layer
     */
    StatusCode AddCaloHit(CaloHit *const pCaloHit, const PseudoLayer pseudoLayer);

    /**
     *  @brief  Remove a calo hit from a specified pseudo layer
     * 
     *  @param  pCaloHit the address of the calo hit
     *  @param  pseudoLayer the pesudo layer
     */
    StatusCode RemoveCaloHit(CaloHit *const pCaloHit, const PseudoLayer pseudoLayer);
};

} // namespace pandora

#endif // #ifndef ORDERED_CALO_HIT_LIST_H
