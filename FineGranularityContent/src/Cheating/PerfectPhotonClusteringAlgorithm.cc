/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Cheating/PerfectPhotonClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating photon clustering algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Cheating/PerfectPhotonClusteringAlgorithm.h"

using namespace pandora;

bool PerfectPhotonClusteringAlgorithm::SelectMCParticlesForClustering(const MCParticle *pMCParticle) const
{
    if (pMCParticle->GetParticleId() == PHOTON)
        return true;

    return false;
}
