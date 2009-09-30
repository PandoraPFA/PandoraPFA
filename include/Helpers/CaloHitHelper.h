/**
 *  @file   PandoraPFANew/include/Helpers/CaloHitHelper.h
 * 
 *  @brief  Header file for the calo hit helper class.
 * 
 *  $Log: $
 */
#ifndef CALO_HIT_HELPER_H
#define CALO_HIT_HELPER_H 1

#include "Api/PandoraApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *  @brief  CaloHitHelper class
 */
class CaloHitHelper
{
public:
    /**
     *  @brief  Is calo hit available to add to a cluster
     * 
     *  @param  pCaloHit address of the calo hit
     * 
     *  @return boolean
     */
    static bool IsCaloHitAvailable(CaloHit *const pCaloHit);

    /**
     *  @brief  Are all calo hits in list available to add to a cluster
     * 
     *  @param  pInputCaloHitList address of the calo hit list
     * 
     *  @return boolean
     */
    static bool AreCaloHitsAvailable(InputCaloHitList *const pInputCaloHitList);

private:
    /**
     *  @brief  Set availability of a calo hit to be added to a cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     *  @param  isAvailable the calo hit availability
     */
    static StatusCode SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable);

    /**
     *  @brief  Set availability of all calo hits in list
     * 
     *  @param  pInputCaloHitList the address of the calo hit list
     *  @param  isAvailable the calo hit availability
     */
    static StatusCode SetCaloHitAvailability(InputCaloHitList *const pInputCaloHitList, bool isAvailable);

    /**
     *  @brief  Create a calo hit usage map containing an entry for each calo hit in the ordered calo hit list
     *          specifying whether the calo hit is available to be added to a cluster.
     * 
     *  @param  usageMapName 
     *  @param  pOrderedCaloHitList 
     */
    static StatusCode CreateInitialCaloHitUsageMap(const std::string &usageMapName, const OrderedCaloHitList *pOrderedCaloHitList);

    /**
     *  @brief  Create a new calo hit usage map containing the same calo hits as the current usage map
     *          and with all isAvailable flags set to true
     * 
     *  @param  usageMapName the name of the usage map to create
     */
    static StatusCode CreateAdditionalCaloHitUsageMap(const std::string &usageMapName);

    /**
     *  @brief  Apply the details in a calo hit usage map to the calo hits
     * 
     *  @param  usageMapName the name of the usage map to apply
     */
    static StatusCode ApplyCaloHitUsageMap(const std::string &usageMapName);

    /**
     *  @brief  Clear the calo hit usage maps
     */
    static StatusCode ClearCaloHitUsageMaps();

    typedef std::map<CaloHit *, bool> CaloHitUsageMap;
    typedef std::map<std::string, CaloHitUsageMap *> NameToCaloHitUsageMap;

    static bool                         m_isReclustering;           ///< Whether the current algorithms are performing reclustering
    static CaloHitUsageMap             *m_pCurrentUsageMap;         ///< Address of the current calo hit usage map
    static NameToCaloHitUsageMap        m_nameToCaloHitUsageMap;    ///< The name to calo hit availability map

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
};

} // namespace pandora

#endif // #ifndef CALO_HIT_HELPER_H
