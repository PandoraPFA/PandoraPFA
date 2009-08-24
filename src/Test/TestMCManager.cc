/**
 *	@file	PandoraPFANew/src/Test/TestMCManager.cc
 * 
 * 	@brief	Implementation of the tester for the mc manager class.
 * 
 *	$Log: $
 */

#include <assert.h>
#include <iostream>

#include "Test/TestMCManager.h"
#include "Managers/MCManager.h"


namespace pandora
{


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_GetMCParticle()
{
        std::cout << "--- --- GetMCParticle | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager
	
        std::cout << "        get MCParticle" << std::endl;
	MCParticle* mcParticle = pMcManager->GetMCParticle( (void*)123 ); 
	assert( mcParticle != 0 ); // MCParticle could not be created

        std::cout << "        get the same MCParticle again" << std::endl;
	MCParticle* sameMcParticle = pMcManager->GetMCParticle( (void*)123 ); 
	assert( sameMcParticle != 0 );          // i should get again an address != NULL
	assert( mcParticle == sameMcParticle ); // the same particle should have been given back again

        std::cout << "        get another MCParticle" << std::endl;
	MCParticle* differentMcParticle = pMcManager->GetMCParticle( (void*)567 ); 
	assert( differentMcParticle != NULL );       // should be a != NULL
	assert( mcParticle != differentMcParticle ); // should be different pointers
	

        std::cout << "        delete MCManager" << std::endl;
	delete pMcManager;
        std::cout << "--- --- GetMCParticle | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_CreateMCParticle()
{
        std::cout << "--- --- CreateMCParticle | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager

        std::cout << "        create MCParticleParameters" << std::endl;
	PandoraApi::MCParticleParameters mcParticleParameters;
	mcParticleParameters.m_energy = 10;
	mcParticleParameters.m_momentum = 12;
	mcParticleParameters.m_innerRadius = 0.1;
	mcParticleParameters.m_outerRadius = 40.5;
	mcParticleParameters.m_particleId = 13;
	mcParticleParameters.m_pParentAddress = (void*)100;
	
        std::cout << "        create MCParticle with parameters" << std::endl;
	assert( pMcManager->CreateMCParticle( mcParticleParameters ) == STATUS_CODE_SUCCESS ); 

        std::cout << "        get the created MCParticle" << std::endl;
	MCParticle* mcParticle = pMcManager->GetMCParticle( (void*)100 ); 
	assert( mcParticle != 0 ); // MCParticle has not been created
	assert( mcParticle->GetUid() == (void*)100 ); // check if the Uid of the created MCParticle coincides with the one asked for
	assert( mcParticle->IsRootParticle() == true ); // after creation, it MCParticle should be a root-particle (of a MC-tree with exactly one node)
	assert( mcParticle->IsPfoTarget() == true ); // after creation, the pfo-target should be "this"
	assert( mcParticle->GetPfoTarget() == mcParticle ); // pfo-target should be "this"
	assert( mcParticle->IsPfoTargetSet() == false ); // the MCParticle should know, that the pfo-target is still the default ("this")
	

        std::cout << "        delete MCManager" << std::endl;
	delete pMcManager;
        std::cout << "--- --- CreateMCParticle | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_SetMCParticleRelationship()
{
        std::cout << "--- --- SetMCParticleRelationship | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager
	
        std::cout << "        create relationship between non-yet-existing MCParticles" << std::endl;
	assert( pMcManager->SetMCParticleRelationship( (void*)100, (void*)200 ) == STATUS_CODE_SUCCESS );

        std::cout << "        check if the MCParticles have been created implicitly" << std::endl;
	MCParticle* firstMcParticle = pMcManager->GetMCParticle( (void*)100 ); 
	assert( firstMcParticle != NULL ); // this particle should have been created
	MCParticle* secondMcParticle = pMcManager->GetMCParticle( (void*)200 ); 
	assert( secondMcParticle != NULL ); // this particle should have been created as well
	assert( firstMcParticle != secondMcParticle ); // these two should be different

        std::cout << "        create another relationship between non-yet-existing MCParticles" << std::endl;
	assert( pMcManager->SetMCParticleRelationship( (void*)300, (void*)400 ) == STATUS_CODE_SUCCESS );
	MCParticle* thirdMcParticle = pMcManager->GetMCParticle( (void*)300 ); 
	assert( thirdMcParticle != NULL ); // this particle should have been created
	MCParticle* fourthMcParticle = pMcManager->GetMCParticle( (void*)400 ); 
	assert( fourthMcParticle != NULL ); // this particle should have been created as well
	assert( thirdMcParticle != fourthMcParticle ); // these two should be different

        std::cout << "        create a relationship between two of the existing MCParticles" << std::endl;
	assert( pMcManager->SetMCParticleRelationship( (void*)100, (void*)300 ) == STATUS_CODE_SUCCESS );
	MCParticle* copyOfFirstMcParticle = pMcManager->GetMCParticle( (void*)100 ); 
	assert( copyOfFirstMcParticle != NULL ); // this particle should be there already
	MCParticle* copyOfThirdMcParticle = pMcManager->GetMCParticle( (void*)300 ); 
	assert( copyOfThirdMcParticle != NULL ); // this particle should be there already
	assert( copyOfFirstMcParticle == firstMcParticle ); // these should be the same
	assert( copyOfThirdMcParticle == thirdMcParticle ); // these should be the same

        std::cout << "        delete MCManager" << std::endl;
	delete pMcManager;

        std::cout << "--- --- SetMCParticleRelationship | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_AssociateCaloHitWithMCParticle()
{
        std::cout << "--- --- AssociateCaloHitWithMCParticle | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager
	

        std::cout << "        make association of two calohit-uids with MCParticle-uid (without existing MCParticle)" << std::endl;
	assert( pMcManager->AssociateCaloHitWithMCParticle( (void*)100,(void*)200 ) == STATUS_CODE_SUCCESS ); 
	MCParticle* mcParticle = pMcManager->GetMCParticle( (void*)200 ); 
	assert( pMcManager->AssociateCaloHitWithMCParticle( (void*)101,(void*)200 ) == STATUS_CODE_SUCCESS ); 
	MCParticle* sameMcParticle = pMcManager->GetMCParticle( (void*)200 ); 
	assert( mcParticle == sameMcParticle );

        std::cout << "        check if it fails to associate the same calohit two times (it should fail) " << std::endl;
	assert( pMcManager->AssociateCaloHitWithMCParticle( (void*)101,(void*)200 ) == STATUS_CODE_FAILURE ); // for one CaloHit only one MCParticle can be associated --> error when tried to associate a second time to the same calohit

        std::cout << "        check if association with different MCParticle-Uid produces a different MCParticle" << std::endl;
	assert( pMcManager->AssociateCaloHitWithMCParticle( (void*)102,(void*)201 ) == STATUS_CODE_SUCCESS ); 
	MCParticle* differentMcParticle = pMcManager->GetMCParticle( (void*)201 ); 
	assert( mcParticle != differentMcParticle );
	

        std::cout << "        delete MCManager" << std::endl;
	delete pMcManager;

        std::cout << "--- --- AssociateCaloHitWithMCParticle | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_SelectPfoTargets()
{
        std::cout << "--- --- SelectPfoTargets | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager

        std::cout << "        get MCParticle" << std::endl;
	MCParticle* root0Particle = pMcManager->GetMCParticle( (void*)100 ); 
	assert( root0Particle != 0 ); // MCParticle could not be created
	
        std::cout << "        get MCParticle" << std::endl;
	MCParticle* root1Particle = pMcManager->GetMCParticle( (void*)101 ); 
	assert( root1Particle != 0 ); // MCParticle could not be created

        std::cout << "        build a MCParticle-tree by adding relationships" << std::endl;
	/*
	 *  
	 * 100 
	 *  |--200 <=== will be the Pfo-target
	 *  |   |--300
	 *  |   |--301 ...............
	 *  |       |--400           |
	 *  |       |--401           |
	 *  |                        |
	 *  |--201                   |
	 *                           |
	 * 101                       |
	 *  |--210 ...................
	 *  |   |--310
	 *  |   |--311
	 *  |
	 *  |--211
	 *
	 *  102 <== isolated particle
	 *
	 */
        std::cout << "        create relationships" << std::endl;
	pMcManager->GetMCParticle( (void*)102 );

	assert( pMcManager->SetMCParticleRelationship( (void*)100, (void*)200 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParticleRelationship( (void*)100, (void*)201 ) == STATUS_CODE_SUCCESS );

	assert( pMcManager->SetMCParticleRelationship( (void*)101, (void*)210 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParticleRelationship( (void*)101, (void*)211 ) == STATUS_CODE_SUCCESS );

	assert( pMcManager->SetMCParticleRelationship( (void*)200, (void*)300 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParticleRelationship( (void*)200, (void*)301 ) == STATUS_CODE_SUCCESS );

	assert( pMcManager->SetMCParticleRelationship( (void*)301, (void*)400 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParticleRelationship( (void*)301, (void*)401 ) == STATUS_CODE_SUCCESS );

	assert( pMcManager->SetMCParticleRelationship( (void*)210, (void*)310 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParticleRelationship( (void*)210, (void*)311 ) == STATUS_CODE_SUCCESS );

        std::cout << "        create cross-parental relationship" << std::endl;
	assert( pMcManager->SetMCParticleRelationship( (void*)210, (void*)301 ) == STATUS_CODE_SUCCESS );

        std::cout << "        get one MCParticle which will be the PfoTarget" << std::endl;
	MCParticle* pfoTarget = pMcManager->GetMCParticle( (void*)200 );

        std::cout << "        set the PfoTarget in the whole MCParticle tree" << std::endl;
	pfoTarget->SetPfoTargetInTree( pfoTarget );

        std::cout << "        check all MCParticles in the tree if their pfo-target is set correctly" << std::endl;
	assert( pMcManager->GetMCParticle( (void*)100 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in parent
	assert( pMcManager->GetMCParticle( (void*)201 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in sister
	assert( pMcManager->GetMCParticle( (void*)200 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in self
	assert( pMcManager->GetMCParticle( (void*)300 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in daughter
	assert( pMcManager->GetMCParticle( (void*)301 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in daughter
	assert( pMcManager->GetMCParticle( (void*)400 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in grand-daughter
	assert( pMcManager->GetMCParticle( (void*)401 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in grand-daughter
	assert( pMcManager->GetMCParticle( (void*)210 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in other-family connection
	assert( pMcManager->GetMCParticle( (void*)101 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in other-family parent
	assert( pMcManager->GetMCParticle( (void*)211 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in other-family sister
	assert( pMcManager->GetMCParticle( (void*)310 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in other-family daughter
	assert( pMcManager->GetMCParticle( (void*)311 )->GetPfoTarget() == pfoTarget ); // check if pfo target is set right in other-family daughter

	assert( pMcManager->GetMCParticle( (void*)102 )->GetPfoTarget() != pfoTarget ); // check if pfo target NOT set in isolated MCParticle


        std::cout << "        delete MCManager" << std::endl;
	delete pMcManager;

        std::cout << "--- --- SelectPfoTargets | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_Combined()
{
        std::cout << "--- --- Combined | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager
	

        std::cout << "        delete MCManager" << std::endl;
	delete pMcManager;

        std::cout << "--- --- Combined | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_All()
{
        std::cout << "--- --- ALL | START ------------------------------" << std::endl;
        assert( Test_GetMCParticle() == STATUS_CODE_SUCCESS );
        assert( Test_CreateMCParticle() == STATUS_CODE_SUCCESS );
	assert( Test_SetMCParticleRelationship() == STATUS_CODE_SUCCESS );
	assert( Test_AssociateCaloHitWithMCParticle() == STATUS_CODE_SUCCESS );
	assert( Test_SelectPfoTargets() == STATUS_CODE_SUCCESS );
	assert( Test_Combined() == STATUS_CODE_SUCCESS );

        std::cout << "--- --- ALL | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}



} // namespace pandora
