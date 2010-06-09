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

bool PerfectPhotonClusteringAlgorithm::SelectMCParticlesForClustering(const MCParticle *pMCParticle) const
{
    if (pMCParticle->GetParticleId() == PHOTON)
        return true;

    return false;
}
