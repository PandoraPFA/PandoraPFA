#include <iostream>

#include "Test/TestMCManager.h"
#include "Test/TestCaloHitManager.h"

int main(){
   std::cout << "--- TEST : MCManager | START ----------------------------------" << std::endl;
   pandora::TestMCManager* testMcManager = new pandora::TestMCManager();
   testMcManager->Test_All();

   delete testMcManager;
   std::cout << "--- TEST : MCManager | END ----------------------------------" << std::endl;

   std::cout << "--- TEST : CaloHitManager | START ----------------------------------" << std::endl;
   pandora::TestCaloHitManager* testCaloHitManager = new pandora::TestCaloHitManager();
   testCaloHitManager->Test_All();

   delete testCaloHitManager;
   std::cout << "--- TEST : CaloHitManager | END ----------------------------------" << std::endl;
} 
