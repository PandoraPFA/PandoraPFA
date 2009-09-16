/**
 *    @file    PandoraPFANew/include/Test/TestCaloHitManager.h
 * 
 *    @brief    Header file for the CaloHitManager test-class.
 * 
 *    $Log: $
 */
#ifndef TEST_CALO_HIT_MANAGER_H
#define TEST_CALO_HIT_MANAGER_H 1

#include "Pandora/PandoraInternal.h"

#include "StatusCodes.h"

#include <assert.h>

namespace pandora
{

/**
 *    @brief TestCaloHitManager class
 */
class TestCaloHitManager
{
public:
    /**
     *  @brief  test the GetMCParticle method
     */
     StatusCode Test_CreateCaloHit();
   
    /**
     *  @brief  test the matching of calo hits to their correct mc particles for particle flow
     */
    StatusCode Test_MatchCaloHitsToMCPfoTargets();

    /**
     *  @brief  Test to get the calorimeter hit vector
     */
    StatusCode Test_GetCaloHitVector();

    /**
     *  @brief  Test to get the current ordered calo hit vector
     */
    StatusCode Test_GetCurrentOrderedCaloHitVector();    

    /**
     *  @brief  Reset the calo hit manager
     */
    StatusCode Test_Reset();

    /**
     *  @brief  combined tests
     */
     StatusCode Test_Combined();

    /**
     *  @brief  Run all tests
     */
     StatusCode Test_All();
};

} // namespace pandora

#endif // #ifndef TEST_CALO_HIT_MANAGER_H
