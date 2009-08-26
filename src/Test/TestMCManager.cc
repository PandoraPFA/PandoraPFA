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

StatusCode TestMCManager::Test_RetrieveExistingOrCreateEmptyMCParticle()
{
        std::cout << "--- --- GetMCParticle | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager
	
        std::cout << "        get MCParticle" << std::endl;
	MCParticle* mcParticle = NULL;
	std::cout << "        mcParticle address before " << mcParticle << std::endl;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)123, mcParticle ); 
	std::cout << "        mcParticle address afterwards " << mcParticle << std::endl;
	assert( mcParticle != NULL ); // MCParticle could not be created

        std::cout << "        get the same MCParticle again" << std::endl;
	MCParticle* sameMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)123, sameMcParticle ); 
	assert( sameMcParticle != NULL );          // i should get again an address != NULL
	assert( mcParticle == sameMcParticle ); // the same particle should have been given back again

        std::cout << "        get another MCParticle" << std::endl;
	MCParticle* differentMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)567, differentMcParticle ); 
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
	MCParticle* mcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)100, mcParticle ); 
	assert( mcParticle != 0 ); // MCParticle has not been created
	assert( mcParticle->GetUid() == (void*)100 ); // check if the Uid of the created MCParticle coincides with the one asked for
	assert( mcParticle->IsRootParticle() == true ); // after creation, it MCParticle should be a root-particle (of a MC-tree with exactly one node)
	assert( mcParticle->IsPfoTarget() == false ); // after creation, the pfo-target should be NULL (not set)
	MCParticle *pfo = NULL;
	mcParticle->GetPfoTarget( pfo );
	assert( pfo == NULL ); // pfo-target should be NULL
	assert( mcParticle->IsPfoTargetSet() == false ); // the MCParticle should know, that the pfo-target is still the default ("this")
	

        std::cout << "        delete MCManager" << std::endl;
	delete pMcManager;
        std::cout << "--- --- CreateMCParticle | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_SetMCParentDaughterRelationship()
{
        std::cout << "--- --- SetMCParentDaughterRelationship | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager
	
        std::cout << "        create relationship between non-yet-existing MCParticles" << std::endl;
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)100, (void*)200 ) == STATUS_CODE_SUCCESS );

        std::cout << "        check if the MCParticles have been created implicitly" << std::endl;
	MCParticle* firstMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)100, firstMcParticle ); 
	assert( firstMcParticle != NULL ); // this particle should have been created
	MCParticle* secondMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)200, secondMcParticle ); 
	assert( secondMcParticle != NULL ); // this particle should have been created as well
	assert( firstMcParticle != secondMcParticle ); // these two should be different

        std::cout << "        create another relationship between non-yet-existing MCParticles" << std::endl;
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)300, (void*)400 ) == STATUS_CODE_SUCCESS );
	MCParticle* thirdMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)300, thirdMcParticle ); 
	assert( thirdMcParticle != NULL ); // this particle should have been created
	MCParticle* fourthMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)400, fourthMcParticle ); 
	assert( fourthMcParticle != NULL ); // this particle should have been created as well
	assert( thirdMcParticle != fourthMcParticle ); // these two should be different

        std::cout << "        create a relationship between two of the existing MCParticles" << std::endl;
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)100, (void*)300 ) == STATUS_CODE_SUCCESS );
	MCParticle* copyOfFirstMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)100, copyOfFirstMcParticle ); 
	assert( copyOfFirstMcParticle != NULL ); // this particle should be there already
	MCParticle* copyOfThirdMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)300, copyOfThirdMcParticle ); 
	assert( copyOfThirdMcParticle != NULL ); // this particle should be there already
	assert( copyOfFirstMcParticle == firstMcParticle ); // these should be the same
	assert( copyOfThirdMcParticle == thirdMcParticle ); // these should be the same

        std::cout << "        delete MCManager" << std::endl;
	delete pMcManager;

        std::cout << "--- --- SetMCParentDaughterRelationship | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TestMCManager::Test_SetCaloHitToMCParticleRelationship()
{
        std::cout << "--- --- SetCaloHitToMCParticleRelationship | START ------------------------------" << std::endl;
   
        std::cout << "        create MCManager" << std::endl;
        MCManager* pMcManager = new MCManager();
	assert( pMcManager != 0 ); // problem at creating a MCManager
	

        std::cout << "        make association of two calohit-uids with MCParticle-uid (without existing MCParticle)" << std::endl;
	assert( pMcManager->SetCaloHitToMCParticleRelationship( (void*)100,(void*)200, 1.0 ) == STATUS_CODE_SUCCESS ); 
	MCParticle* mcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)200, mcParticle ); 
	assert( pMcManager->SetCaloHitToMCParticleRelationship( (void*)101,(void*)200, 1.0 ) == STATUS_CODE_SUCCESS ); 

	MCParticle* sameMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)200, sameMcParticle ); 
	assert( mcParticle == sameMcParticle );

        std::cout << "        associate a second mcparticle to the calohit " << std::endl;
	assert( pMcManager->SetCaloHitToMCParticleRelationship( (void*)190,(void*)200, 0.8 ) == STATUS_CODE_SUCCESS ); // for one CaloHit only one MCParticle can be associated --> when the second one is associated, only the one with the larger weight is taken

        std::cout << "        associate a third mcparticle to the calohit " << std::endl;
	assert( pMcManager->SetCaloHitToMCParticleRelationship( (void*)191,(void*)200, 0.7 ) == STATUS_CODE_SUCCESS ); // for one CaloHit only one MCParticle can be associated --> when the second one is associated, only the one with the larger weight is taken

        std::cout << "        check if association with different MCParticle-Uid produces a different MCParticle" << std::endl;
	assert( pMcManager->SetCaloHitToMCParticleRelationship( (void*)102,(void*)201, 1.0 ) == STATUS_CODE_SUCCESS ); 

//	assert( pMcManager->CreateCaloHitToMCParticleRelationships() ); // create the relationships now

	MCParticle* differentMcParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)201, differentMcParticle ); 
	assert( mcParticle != NULL );
	assert( differentMcParticle != NULL );
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
	MCParticle* root0Particle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)100, root0Particle ); 
	assert( root0Particle != 0 ); // MCParticle could not be created
	
        std::cout << "        get MCParticle" << std::endl;
	MCParticle* root1Particle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)101, root1Particle ); 
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
	MCParticle *isolatedParticle = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)102, isolatedParticle );

	assert( pMcManager->SetMCParentDaughterRelationship( (void*)100, (void*)200 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)100, (void*)201 ) == STATUS_CODE_SUCCESS );

	assert( pMcManager->SetMCParentDaughterRelationship( (void*)101, (void*)210 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)101, (void*)211 ) == STATUS_CODE_SUCCESS );

	assert( pMcManager->SetMCParentDaughterRelationship( (void*)200, (void*)300 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)200, (void*)301 ) == STATUS_CODE_SUCCESS );

	assert( pMcManager->SetMCParentDaughterRelationship( (void*)301, (void*)400 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)301, (void*)401 ) == STATUS_CODE_SUCCESS );

	assert( pMcManager->SetMCParentDaughterRelationship( (void*)210, (void*)310 ) == STATUS_CODE_SUCCESS );
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)210, (void*)311 ) == STATUS_CODE_SUCCESS );

        std::cout << "        create cross-parental relationship" << std::endl;
	assert( pMcManager->SetMCParentDaughterRelationship( (void*)210, (void*)301 ) == STATUS_CODE_SUCCESS );

        std::cout << "        get one MCParticle which will be the PfoTarget" << std::endl;
	MCParticle* pfoTarget = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)200, pfoTarget );

        std::cout << "        set the PfoTarget in the whole MCParticle tree" << std::endl;
	pfoTarget->SetPfoTargetInTree( pfoTarget );

        std::cout << "        check all MCParticles in the tree if their pfo-target is set correctly" << std::endl;
	MCParticle *mcP = NULL;
	MCParticle *pfo = NULL;
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)100, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo != NULL );
	assert( pfo == pfoTarget ); // check if pfo target is set right in parent
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)201, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in sister
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)200, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in self
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)300, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in daughter
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)301, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in daughter
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)400, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in grand-daughter
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)401, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in grand-daughter
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)210, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in other-family connection
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)101, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in other-family parent
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)211, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in other-family sister
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)310, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in other-family daughter
	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)311, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) == STATUS_CODE_SUCCESS );
	assert( pfo == pfoTarget ); // check if pfo target is set right in other-family daughter

	pMcManager->RetrieveExistingOrCreateEmptyMCParticle( (void*)102, mcP );
	pfo = NULL;
	assert( mcP->GetPfoTarget(pfo) != STATUS_CODE_SUCCESS );
	assert( pfo == NULL );
	assert( pfo != pfoTarget ); // check if pfo target NOT set in isolated MCParticle


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
        assert( Test_RetrieveExistingOrCreateEmptyMCParticle() == STATUS_CODE_SUCCESS );
        assert( Test_CreateMCParticle() == STATUS_CODE_SUCCESS );
	assert( Test_SetMCParentDaughterRelationship() == STATUS_CODE_SUCCESS );
	assert( Test_SetCaloHitToMCParticleRelationship() == STATUS_CODE_SUCCESS );
	assert( Test_SelectPfoTargets() == STATUS_CODE_SUCCESS );
	assert( Test_Combined() == STATUS_CODE_SUCCESS );

        std::cout << "--- --- ALL | END ------------------------------" << std::endl;
	return STATUS_CODE_SUCCESS;
}



} // namespace pandora
