/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/PerfectNeutralHadronClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating photon clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/PerfectNeutralHadronClusteringAlgorithm.h"

#include "Objects/MCParticle.h"

using namespace pandora;


//------------------------------------------------------------------------------------------------------------------------------------------

bool PerfectNeutralHadronClusteringAlgorithm::SelectCaloHitsOfMcParticleForClustering( MCParticle* pMcParticle ) const
{
    #define K0L 130
    #define K0S 310
    #define NEUTRON 2112


    if( pMcParticle->GetParticleId() == K0L ||
        pMcParticle->GetParticleId() == K0S ||
        pMcParticle->GetParticleId() == NEUTRON )
	return true;
    return false;
}

