/**
 *  @file   PandoraPFANew/FineGranularityContent/include/ParticleId/PhotonReconstructionAlgorithm.h
 * 
 *  @brief  Header file for the photon reconstruction algorithm class.
 * 
 *  $Log: $
 */
#ifndef PHOTON_RECONSTRUCTION_ALGORITHM_H
#define PHOTON_RECONSTRUCTION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  PhotonReconstructionAlgorithm class
 */
class PhotonReconstructionAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    std::string     m_photonClusteringAlgName;          ///< The name of the photon clustering algorithm to run
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonReconstructionAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonReconstructionAlgorithm();
}

#endif // #ifndef PHOTON_RECONSTRUCTION_ALGORITHM_H
