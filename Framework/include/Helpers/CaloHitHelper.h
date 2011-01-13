/**
 *  @file   PandoraPFANew/Framework/include/Helpers/CaloHitHelper.h
 * 
 *  @brief  Header file for the calo hit helper class.
 * 
 *  $Log: $
 */
#ifndef CALO_HIT_HELPER_H
#define CALO_HIT_HELPER_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

class TiXmlHandle;

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
     *  @param  caloHitList the list of calo hits
     * 
     *  @return boolean
     */
    static bool AreCaloHitsAvailable(const CaloHitList &caloHitList);

    /**
     *  @brief  Remove all calo hits unavailable for clustering from a calo hit list
     * 
     *  @param  caloHitList the list of calo hits from which to remove unavailable hits
     */
    static void RemoveUnavailableCaloHits(CaloHitList &caloHitList);

    /**
     *  @brief  Remove all calo hits unavailable for clustering from an ordered calo hit list
     * 
     *  @param  orderedCaloHitList the ordered list of calo hits from which to remove unavailable hits
     */
    static StatusCode RemoveUnavailableCaloHits(OrderedCaloHitList &orderedCaloHitList);

    /**
     *  @brief  Get contribution to a hit's density weight from a list of other hits
     * 
     *  @param  pCaloHit the calo hit
     *  @param  CaloHitList the calo hit list
     * 
     *  @return the density weight contribution
     */
    static float GetDensityWeightContribution(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList);

    /**
     *  @brief  Get contribution to a hit's surrounding energy measure from a list of other hits
     * 
     *  @param  pCaloHit the calo hit
     *  @param  pCaloHitList the calo hit list
     * 
     *  @return the surrounding energy contribution
     */
    static float GetSurroundingEnergyContribution(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList);

    /**
     *  @brief  Count number of "nearby" hits using the isolation scheme
     * 
     *  @param  pCaloHit the calo hit
     *  @param  pCaloHitList the calo hit list
     * 
     *  @return the number of nearby hits
     */
    static unsigned int IsolationCountNearbyHits(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList);

    /**
     *  @brief  Whether calo hit helper has been configured to use the simple (density weight cut) isolation scheme
     * 
     *  @return boolean
     */
    static bool ShouldUseSimpleIsolationScheme();

    /**
     *  @brief  Count number of "nearby" hits using the mip identification scheme
     * 
     *  @param  pCaloHit the calo hit
     *  @param  pCaloHitList the calo hit list
     * 
     *  @return the number of nearby hits
     */
    static unsigned int MipCountNearbyHits(const CaloHit *const pCaloHit, const CaloHitList *const pCaloHitList);

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
     *  @param  caloHitList the list of calo hits
     *  @param  isAvailable the calo hit availability
     */
    static StatusCode SetCaloHitAvailability(CaloHitList &caloHitList, bool isAvailable);

    /**
     *  @brief  Create a calo hit usage map containing an entry for each calo hit in a cluster list
     *          specifying whether the calo hit is available to be added to a cluster
     * 
     *  @param  usageMapName the usage map name
     *  @param  clusterList the cluster list
     */
    static StatusCode CreateInitialCaloHitUsageMap(const std::string &usageMapName, const ClusterList &clusterList);

    /**
     *  @brief  Create a calo hit usage map containing an entry for each calo hit in an ordered calo hit list
     *          specifying whether the calo hit is available to be added to a cluster
     * 
     *  @param  usageMapName the usage map name
     *  @param  pOrderedCaloHitList the address of the ordered calo hit list
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

    /**
     *  @brief  Clear the calo hit usage maps used by the most recent reclustering process
     */
    static StatusCode ClearMostRecentCaloHitUsageMaps();

    /**
     *  @brief  Calculate calo hit properties for a particular calo hit, through comparison with an ordered list of other hits.
     *          Calculates density weights, isolation flags, possible mip flags and surrounding energy
     * 
     *  @param  pCaloHit the calo hit
     *  @param  pOrderedCaloHitList the ordered calo hit list
     */
    static void CalculateCaloHitProperties(CaloHit *const pCaloHit, const OrderedCaloHitList *const pOrderedCaloHitList);

    /**
     *  @brief  Identify isolated hits by applying a simple density weight cut to all hits in a calo hit vector
     *
     *  @param  caloHitVector the calo hit vector
     */
    static void ApplySimpleIsolationScheme(const CaloHitVector &caloHitVector);

    /**
     *  @brief  Read the calo hit helper settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle *const pXmlHandle);

    typedef std::map<CaloHit *, bool> CaloHitUsageMap;
    typedef std::map<std::string, CaloHitUsageMap *> NameToCaloHitUsageMap;
    typedef std::vector<CaloHitUsageMap *> UsageMapVector;
    typedef std::vector<StringVector *> NestedUsageMapNames;

    static unsigned int             m_nReclusteringProcesses;           ///< The number of reclustering algorithms currently in use
    static CaloHitUsageMap         *m_pCurrentUsageMap;                 ///< Address of the current calo hit usage map
    static UsageMapVector           m_parentCaloHitUsageMaps;           ///< List of current usage maps for all reclustering algorithms in use
    static NestedUsageMapNames      m_nestedUsageMapNames;              ///< List of usage maps names, ordered by recluster process
    static NameToCaloHitUsageMap    m_nameToCaloHitUsageMap;            ///< The name to calo hit availability map

    static float                    m_caloHitMaxSeparation;             ///< Max separation to consider associations between hits, units mm
    static float                    m_isolationCaloHitMaxSeparation;    ///< Max separation considered when identifying isolated hits, units mm

    static float                    m_densityWeightContribution;        ///< The basic density weight contribution
    static unsigned int             m_densityWeightPower;               ///< The density weighting power
    static unsigned int             m_densityWeightNLayers;             ///< Number of adjacent layers to use in density weight calculation

    static bool                     m_shouldUseSimpleIsolationScheme;   ///< Whether to use the simple (density weight cut) isolation scheme
    static float                    m_isolationDensityWeightCutFine;    ///< Fine granularity isolation density weight cut
    static float                    m_isolationDensityWeightCutCoarse;  ///< Coarse granularity isolation density weight cut

    static unsigned int             m_isolationNLayers;                 ///< Number of adjacent layers to use in isolation calculation
    static float                    m_isolationCutDistanceFine;         ///< Fine granularity isolation cut distance, units mm
    static float                    m_isolationCutDistanceCoarse;       ///< Coarse granularity isolation cut distance, units mm
    static unsigned int             m_isolationMaxNearbyHits;           ///< Max number of "nearby" hits for a hit to be considered isolated

    static float                    m_mipLikeMipCut;                    ///< Mip equivalent energy cut for hit to be flagged as possible mip
    static unsigned int             m_mipNCellsForNearbyHit;            ///< Separation (in calo cells) for hits to be declared "nearby"
    static unsigned int             m_mipMaxNearbyHits;                 ///< Max number of "nearby" hits for hit to be flagged as possible mip

    friend class CaloHitManager;
    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraSettings;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHitHelper::ShouldUseSimpleIsolationScheme()
{
    return m_shouldUseSimpleIsolationScheme;
}

} // namespace pandora

#endif // #ifndef CALO_HIT_HELPER_H
