/**
 *  @file   PandoraPFANew/Framework/include/Managers/MCManager.h
 * 
 *  @brief  Header file for the mc particle manager class.
 * 
 *  $Log: $
 */
#ifndef MC_MANAGER_H
#define MC_MANAGER_H 1

#include "Api/PandoraApi.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class MCParticle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief MCManager class
 */
class MCManager
{
public:
    /**
     *  @brief  Destructor
     */
    ~MCManager();

private:
    /**
     *  @brief  UidAndWeight class
     */
    class UidAndWeight
    {
    public:
        /**
         *  @brief  UidAndWeight class
         * 
         *  @param  uid the unique identifier
         *  @param  weight the weight
         */
        UidAndWeight(const Uid uid, const float weight);

        Uid                 m_uid;                      ///< The unique identifier
        float               m_weight;                   ///< The weight
    };

    typedef std::map<Uid, UidAndWeight> UidRelationMap;

    /**
     *  @brief  Create a mc particle
     * 
     *  @param  mcParticleParameters the mc particle parameters
     */
    StatusCode CreateMCParticle(const PandoraApi::MCParticleParameters &mcParticleParameters);

    /**
     *  @brief  Retrieve a mc particle, creating an empty mc particle if it does not already exist
     * 
     *  @param  mcParticleUid the unique identifier of the mc particle
     *  @param  pMCParticle to receive the address of the mc particle
     */
    StatusCode RetrieveExistingOrCreateEmptyMCParticle(const Uid mcParticleUid, MCParticle *&pMCParticle);

    /**
     *  @brief  Set mc particle relationship
     * 
     *  @param  parentUid the parent unique identifier
     *  @param  daughterUid the daughter unique identifier
     */
    StatusCode SetMCParentDaughterRelationship(const Uid parentUid, const Uid daughterUid);

    /**
     *  @brief  Set calo hit to mc particle relationship
     * 
     *  @param  caloHitUid the calo hit unique identifier
     *  @param  mcParticleUid the mc particle unique identifier
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    StatusCode SetCaloHitToMCParticleRelationship(const Uid caloHitUid, const Uid mcParticleUid, const float mcParticleWeight);

    /**
     *  @brief  Set track to mc particle relationship
     * 
     *  @param  trackUid the track unique identifier
     *  @param  mcParticleUid the mc particle unique identifier
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    StatusCode SetTrackToMCParticleRelationship(const Uid trackUid, const Uid mcParticleUid, const float mcParticleWeight);

    /**
     *  @brief  Set an object (e.g. calo hit or track) to mc particle relationship
     * 
     *  @param  uid the unique identifier of the object
     *  @param  mcParticleUid the mc particle unique identifier
     *  @param  mcParticleWeight weighting to assign to the mc particle
     *  @param  uidRelationMap the uid relation map to populate
     */
    StatusCode SetUidToMCParticleRelationship(const Uid objectUid, const Uid mcParticleUid, const float mcParticleWeight,
        UidRelationMap &uidRelationMap);

    /**
     *  @brief  Select pfo targets
     */
    StatusCode SelectPfoTargets();

   /**
    *  @brief  Apply mc pfo selection rules 
    *
    *  @params mcRootParticle address of the mc root particle
    *  @params mcPfoList reference to list with all MCPFOs which have been selected so far
    */
    StatusCode ApplyPfoSelectionRules(MCParticle *const mcRootParticle, MCParticleList &mcPfoList) const;

   /**
     *  @brief  Create a map relating calo hit uid to mc pfo target
     * 
     *  @param  caloHitToPfoTargetMap to receive the calo hit uid to mc pfo target map
     */
    StatusCode CreateCaloHitToPfoTargetMap(UidToMCParticleMap &caloHitToPfoTargetMap) const;

   /**
     *  @brief  Create a map relating track uid to mc pfo target
     * 
     *  @param  trackToPfoTargetMap to receive the track uid to mc pfo target map
     */
    StatusCode CreateTrackToPfoTargetMap(UidToMCParticleMap &trackToPfoTargetMap) const;

   /**
     *  @brief  Create a map relating an object (calo hit or track) uid to mc pfo target
     * 
     *  @param  caloHitToPfoTargetMap to receive the calo hit uid to mc pfo target map
     *  @param  uidRelationMap the uid relation map containing the information
     */
    StatusCode CreateUidToPfoTargetMap(UidToMCParticleMap &uidToPfoTargetMap, const UidRelationMap &uidRelationMap) const;

    /**
     *  @brief  Get the list of mc pfo targets
     *
     *  @param  mcParticleList to receive the mc particle list
     */
    StatusCode GetMCParticleList(MCParticleList &mcParticleList) const;

    /**
     *  @brief  Delete non pfo targets
     */
    StatusCode DeleteNonPfoTargets();

    /**
     *  @brief  Remove all parent/daughter particle links from a mc particle and from its (previously) linked particles
     * 
     *  @param  pMCParticle address of the mc particle
     */
    StatusCode RemoveMCParticleRelationships(MCParticle *const pMCParticle);

    /**
     *  @brief  Reset the mc manager
     */
    StatusCode ResetForNextEvent();

    UidToMCParticleMap      m_uidToMCParticleMap;       ///< The uid to mc particle map
    UidRelationMap          m_caloHitToMCParticleMap;   ///< The calo hit to mc particle relation map
    UidRelationMap          m_trackToMCParticleMap;     ///< The track to mc particle relation map

    friend class PandoraImpl;
    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCManager::SetCaloHitToMCParticleRelationship(const Uid caloHitUid, const Uid mcParticleUid, const float mcParticleWeight)
{
    return this->SetUidToMCParticleRelationship(caloHitUid, mcParticleUid, mcParticleWeight, m_caloHitToMCParticleMap);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCManager::SetTrackToMCParticleRelationship(const Uid trackUid, const Uid mcParticleUid, const float mcParticleWeight)
{
    return this->SetUidToMCParticleRelationship(trackUid, mcParticleUid, mcParticleWeight, m_trackToMCParticleMap);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCManager::CreateCaloHitToPfoTargetMap(UidToMCParticleMap &caloHitToPfoTargetMap) const
{
    return this->CreateUidToPfoTargetMap(caloHitToPfoTargetMap, m_caloHitToMCParticleMap);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCManager::CreateTrackToPfoTargetMap(UidToMCParticleMap &trackToPfoTargetMap) const
{
    return this->CreateUidToPfoTargetMap(trackToPfoTargetMap, m_trackToMCParticleMap);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline MCManager::UidAndWeight::UidAndWeight(const Uid uid, const float weight) :
    m_uid(uid),
    m_weight(weight)
{
}

} // namespace pandora

#endif // #ifndef MC_MANAGER_H
