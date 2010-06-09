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

bool PerfectNeutralHadronClusteringAlgorithm::SelectMCParticlesForClustering(const MCParticle *pMCParticle) const
{
    if ((pMCParticle->GetParticleId() == K_LONG) || (pMCParticle->GetParticleId() == NEUTRON))
        return true;

    return false;
}
