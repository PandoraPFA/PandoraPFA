/**
 *  @file   PandoraPFANew/include/Helpers/ParticleIdHelper.h
 * 
 *  @brief  Header file for the particle id helper class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_ID_HELPER_H
#define PARTICLE_ID_HELPER_H 1

#include "Pandora/PandoraInternal.h"

#include "StatusCodes.h"

namespace pandora
{

/**
 *  @brief  ParticleIdHelper class
 */
class ParticleIdHelper
{
public:
    /**
     *  @brief  Calculate shower profile for a cluster and compare it with the expected profile for a photon
     * 
     *  @param  pCluster address of the cluster to investigate
     *  @param  showerProfileStart to receive the shower profile start, in radiation lengths
     *  @param  showerProfileDiscrepancy to receive the shower profile discrepancy
     */
    static StatusCode CalculateShowerProfile(Cluster *const pCluster, float &showerProfileStart, float &showerProfileDiscrepancy);
};

} // namespace pandora

#endif // #ifndef PARTICLE_ID_HELPER_H
