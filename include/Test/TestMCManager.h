/**
 *	@file	PandoraPFANew/include/Test/TestMCManager.h
 * 
 *	@brief	Header file for the MCManager test-class.
 * 
 *	$Log: $
 */
#ifndef TEST_MC_MANAGER_H
#define TEST_MC_MANAGER_H 1

#include <assert.h>

#include "BasicTypes.h"
#include "StatusCodes.h"


namespace pandora
{
	
/**
 *	@brief TestMCManager class
 */
class TestMCManager
{
public:
	/**
	 *	@brief	test the GetMCParticle method
	 */
	 StatusCode Test_RetrieveExistingOrCreateEmptyMCParticle();
   
	/**
	 *	@brief	test the GetMCParticle method
	 */
	 StatusCode Test_CreateMCParticle();
   
	/**
	 *	@brief	test the GetMCParticle method
	 */
	 StatusCode Test_SetMCParentDaughterRelationship();

	/**
	 *	@brief	test the AssociateCaloHitWithMCParticle method
	 */
	 StatusCode Test_SetCaloHitToMCParticleRelationship();

	/**
	 *	@brief	test the y method
	 */
	 StatusCode Test_SelectPfoTargets();


	/**
	 *	@brief	combined tests
	 */
	 StatusCode Test_Combined();

	/**
	 *	@brief	Run all tests
	 */
	 StatusCode Test_All();
   



};

} // namespace pandora

#endif
