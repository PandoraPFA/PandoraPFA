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
    static const int K0L(130);
    static const int NEUTRON(2112);

    if ((pMCParticle->GetParticleId() == K0L) || (pMCParticle->GetParticleId() == NEUTRON))
        return true;

    return false;
}
