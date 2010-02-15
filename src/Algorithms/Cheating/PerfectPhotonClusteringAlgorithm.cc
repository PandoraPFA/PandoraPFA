/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/PerfectPhotonClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating photon clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/PerfectPhotonClusteringAlgorithm.h"

#include "Objects/MCParticle.h"

using namespace pandora;


//------------------------------------------------------------------------------------------------------------------------------------------

bool PerfectPhotonClusteringAlgorithm::SelectCaloHitsOfMcParticleForClustering( MCParticle* pMcParticle ) const
{
    #define PHOTON 22

    if( pMcParticle->GetParticleId() == PHOTON )
	return true;
    return false;
}

