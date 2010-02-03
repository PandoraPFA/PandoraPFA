/**
 *  @file   PandoraPFANew/include/Helpers/PhotonIdHelper.h
 * 
 *  @brief  Header file for the photon id helper class.
 * 
 *  $Log: $
 */
#ifndef PHOTON_ID_HELPER_H
#define PHOTON_ID_HELPER_H 1

#include "Pandora/PandoraInternal.h"

#include "StatusCodes.h"

namespace pandora
{

/**
 *  @brief  PhotonIdHelper class
 */
class PhotonIdHelper
{
public:
    /**
     *  @brief  Calculate shower profile for a cluster and compare it with the expected profile for a photon
     * 
     *  @param  pCluster address of the cluster to investigate
     *  @param  profileShowerStart to receive the profile shower start, in radiation lengths
     *  @param  profileGammaFraction to receive the profile gamma fraction
     */
    static StatusCode CalculateShowerProfile(Cluster *const pCluster, float &profileShowerStart, float &profileGammaFraction);
};

} // namespace pandora

#endif // #ifndef PHOTON_ID_HELPER_H
