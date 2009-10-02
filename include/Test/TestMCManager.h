/**
 *	@file	PandoraPFANew/include/Test/TestMCManager.h
 * 
 *	@brief	Header file for the MCManager test-class.
 * 
 *	$Log: $
 */
#ifndef TEST_MC_MANAGER_H
#define TEST_MC_MANAGER_H 1

#include "Managers/MCManager.h"

#include "Pandora/PandoraInternal.h"
#include "Pandora/Pandora.h"

#include "StatusCodes.h"

#include <assert.h>
#include <iostream>
#include <iomanip>

namespace pandora
{

/**
 *	@brief TestMCManager class
 */
class TestMCManager
{
public:
    /**
     *  @brief  test the GetMCParticle method
     */
    StatusCode Test_RetrieveExistingOrCreateEmptyMCParticle();
   
    /**
     *  @brief  test the GetMCParticle method
     */
    StatusCode Test_CreateMCParticle();
   
    /**
     *  @brief  test the GetMCParticle method
     */
    StatusCode Test_SetMCParentDaughterRelationship();

    /**
     *  @brief  test the AssociateCaloHitWithMCParticle method
     */
    StatusCode Test_SetCaloHitToMCParticleRelationship();

    /**
     *  @brief  test the CreateUidToPfoTargetMap method
     */
    StatusCode Test_CreateUidToPfoTargetMap();

    /**
     *  @brief  test the SelectPfoTargets method
     */
    StatusCode Test_SelectPfoTargets();

    /**
     *  @brief  combined tests
     */
    StatusCode Test_Combined();

    /**
     *  @brief  Run all tests
     */
    StatusCode Test_All();

    // helper functions for debugging

    /**
     *  @brief  prints the datea members of the MCManager (sizes, initialization state, ...)
     *
     *  @param  pPandora Pandora object of which the mc particle trees are printed
     *  @param  o the output stream to which everything is printed
     */
    static void PrintMCManagerData(const Pandora &pPandora, std::ostream & o);

    /**
     *  @brief  print the MCParticle trees
     *  @param  pPandora Pandora object of which the mc particle trees are printed
     *  @param  o the output stream to which everything is printed
     *  @param  maxDepthAfterPFOTarget constrains the tree-depth after the PFO-targets
     */
    static void PrintMCParticleTrees(const Pandora &pPandora, std::ostream & o, int maxDepthAfterPFOTarget = 10000000);

    /**
     *  @brief  print the MCParticle trees
     *  @param  mcManager MCManager of which the mc particle trees are printed
     *  @param  o the output stream to which everything is printed
     *  @param  maxDepthAfterPFOTarget constrains the tree-depth after the PFO-targets
     */
    static void PrintMCParticleTrees(MCManager* mcManager, std::ostream & o, int maxDepthAfterPFOTarget = 10000000);

    /**
     *  @brief  Print the MCParticle's parameters
     * 
     *  @param mcParticle mcparticle of which the information is printed
     *  @param o output-stream where the information is written to
     */
    static void PrintMCParticle(MCParticle* mcParticle, std::ostream & o );

    /**
     *  @brief  Print the MCParticle-tree
     * 
     *  @param mcParticle mc particle of which the mc particle tree is printed
     *  @param o output-stream where the information is written to
     *  @param depth current depth of the tree
     *  @param parent parent MCParticle (NULL if no parent)
     *  @param stepSize change the step-size for the display of the tree
     */
    static void PrintMCParticle(MCParticle* mcParticle, std::ostream & o, int depth, int maxDepthAfterPfoTarget = 100000, 
        const MCParticle* parent = NULL, int stepSize = 2);
};

} // namespace pandora

#endif // #ifndef TEST_MC_MANAGER_H
