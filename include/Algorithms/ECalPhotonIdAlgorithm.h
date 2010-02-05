/**
 *  @file   PandoraPFANew/include/Algorithms/ECalPhotonIdAlgorithm.h
 * 
 *  @brief  Header file for the ecal photon id algorithm class.
 * 
 *  $Log: $
 */
#ifndef ECAL_PHOTON_ID_ALGORITHM_H
#define ECAL_PHOTON_ID_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief ECalPhotonIdAlgorithm class
 */
class ECalPhotonIdAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief  Perform any operations that must occur after reading settings, but before running the algorithm
     */
    virtual StatusCode Initialize();

private:

    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    std::string     m_clusteringAlgorithmName;          ///< The name of the clustering algorithm to run
    std::string     m_photonExtractionAlgorithmName;    ///< Algorithm for extracting photon clusters

    std::string     m_clusterListName;                  ///< name for storing the clusteres of initial clustering
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ECalPhotonIdAlgorithm::Factory::CreateAlgorithm() const
{
    return new ECalPhotonIdAlgorithm();
}

#endif // #ifndef ECAL_PHOTON_ID_ALGORITHM_H
