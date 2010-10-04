/**
 *  @file   PandoraPFANew/include/Api/PandoraApiImpl.h
 *
 *  @brief  Header file for the pandora api implementation class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_API_IMPL_H
#define PANDORA_API_IMPL_H 1

#include "Api/PandoraApi.h"

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief PandoraApiImpl class
 */
class PandoraApiImpl
{
public:
    /**
     *  @brief  Create an object for pandora
     * 
     *  @param  parameters the object parameters
     */
    template <typename PARAMETERS>
    StatusCode CreateObject(const PARAMETERS &parameters) const;

    /**
     *  @brief  Process event
     */
    StatusCode ProcessEvent() const;

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  xmlFileName the name of the xml file containing the settings
     */
    StatusCode ReadSettings(const std::string &xmlFileName) const;

    /**
     *  @brief  Register an algorithm factory with pandora
     * 
     *  @param  algorithmType the type of algorithm that the factory will create
     *  @param  pAlgorithmFactory the address of an algorithm factory instance
     */
    StatusCode RegisterAlgorithmFactory(const std::string &algorithmType, AlgorithmFactory *const pAlgorithmFactory) const;

    /**
     *  @brief  Set parent-daughter mc particle relationship
     * 
     *  @param  pParentAddress address of parent mc particle in the user framework
     *  @param  pDaughterAddress address of daughter mc particle in the user framework
     */
    StatusCode SetMCParentDaughterRelationship(const void *pParentAddress, const void *pDaughterAddress) const;

    /**
     *  @brief  Set parent-daughter track relationship
     * 
     *  @param  pParentAddress address of parent track in the user framework
     *  @param  pDaughterAddress address of daughter track in the user framework
     */
    StatusCode SetTrackParentDaughterRelationship(const void *pParentAddress, const void *pDaughterAddress) const;

    /**
     *  @brief  Set sibling track relationship
     * 
     *  @param  pFirstSiblingAddress address of first sibling track in the user framework
     *  @param  pSecondSiblingAddress address of second sibling track in the user framework
     */
    StatusCode SetTrackSiblingRelationship(const void *pFirstSiblingAddress, const void *pSecondSiblingAddress) const;

    /**
     *  @brief  Set calo hit to mc particle relationship
     * 
     *  @param  pCaloHitParentAddress address of calo hit in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    StatusCode SetCaloHitToMCParticleRelationship(const void *pCaloHitParentAddress, const void *pMCParticleParentAddress,
        const float mcParticleWeight) const;

    /**
     *  @brief  Set track to mc particle relationship
     * 
     *  @param  pTrackParentAddress address of track in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    StatusCode SetTrackToMCParticleRelationship(const void *pTrackParentAddress, const void *pMCParticleParentAddress,
        const float mcParticleWeight) const;

    /**
     *  @brief  Get the particle flow objects
     * 
     *   @param  particleFlowObjectList container to receive the particle flow objects
     */
    StatusCode GetParticleFlowObjects(ParticleFlowObjectList &particleFlowObjectList) const;

    /**
     *  @brief  Set the pseudo layer calculator used by pandora
     * 
     *  @param  pPseudoLayerCalculator address of the pseudo layer calculator
     */
    StatusCode SetPseudoLayerCalculator(PseudoLayerCalculator *pPseudoLayerCalculator) const;

    /**
     *  @brief  Register an energy correction function
     * 
     *  @param  functionName the name/label associated with the energy correction function
     *  @param  energyCorrectionType the energy correction type
     *  @param  energyCorrectionFunction pointer to an energy correction function
     */
    StatusCode RegisterEnergyCorrectionFunction(const std::string &functionName, const EnergyCorrectionType energyCorrectionType,
        EnergyCorrectionFunction *pEnergyCorrectionFunction) const;

    /**
     *  @brief  Register a particle id function
     * 
     *  @param  functionName the name/label associated with the particle id function
     *  @param  particleIdFunction pointer to a particle id function
     */
    StatusCode RegisterParticleIdFunction(const std::string &functionName, ParticleIdFunction *pParticleIdFunction) const;

    /**
     *  @brief  Reset pandora to process another event
     */
    StatusCode ResetForNextEvent() const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the pandora object to interface
     */
    PandoraApiImpl(Pandora *pPandora);

    Pandora    *m_pPandora;    ///< The pandora object to provide an interface to

    friend class Pandora;
};

} // namespace pandora

#endif // #ifndef PANDORA_API_IMPL_H
