/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Cheating/PerfectNeutralHadronClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating photon clustering algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Cheating/PerfectNeutralHadronClusteringAlgorithm.h"

using namespace pandora;

bool PerfectNeutralHadronClusteringAlgorithm::SelectMCParticlesForClustering(const MCParticle *const pMCParticle) const
{
    if ((pMCParticle->GetParticleId() == K_LONG) || (pMCParticle->GetParticleId() == NEUTRON))
        return true;

    return false;
}
