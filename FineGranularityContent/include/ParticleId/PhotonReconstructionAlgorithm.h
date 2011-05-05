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
    std::string     m_clusterListName;                  ///< The name of the output cluster list 

    float           m_minClusterEnergy;                 ///< The minimum energy to consider a cluster
    unsigned int    m_transProfileMaxLayer;             ///< Maximum layer to consider in calculation of shower transverse profiles
    float           m_minPeakEnergy;                    ///< The minimum energy to consider a transverse profile peak
    float           m_maxPeakRms;                       ///< The maximum rms value to consider a transverse profile peak
    unsigned int    m_minPeakCaloHits;                  ///< The minimum number of calo hits associated with a transverse profile peak

    float           m_maxLongProfileStart;              ///< The maximum longitudinal shower profile start
    float           m_maxLongProfileDiscrepancy;        ///< The maximum longitudinal shower profile discrepancy

    float           m_oldClusterEnergyFraction0;        ///< The cluster energy fraction above which original cluster will be used
    unsigned int    m_oldClusterNPeaks;                 ///< The number of peaks identified leading to use of original cluster
    float           m_oldClusterEnergyFraction1;        ///< Decision to use original cluster: energy fraction 1
    float           m_oldClusterEnergyDifference1;      ///< Decision to use original cluster: energy difference 1
    float           m_oldClusterEnergyFraction2;        ///< Decision to use original cluster: energy fraction 2
    float           m_oldClusterEnergyDifference2;      ///< Decision to use original cluster: energy difference 2
    float           m_oldClusterEnergyFraction3;        ///< Decision to use original cluster: energy fraction 3
    float           m_oldClusterEnergyDifference3;      ///< Decision to use original cluster: energy difference 3
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonReconstructionAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonReconstructionAlgorithm();
}

#endif // #ifndef PHOTON_RECONSTRUCTION_ALGORITHM_H
