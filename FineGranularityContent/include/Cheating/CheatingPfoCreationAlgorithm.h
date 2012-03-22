/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Cheating/CheatingPfoCreationAlgorithm.h
 * 
 *  @brief  Header file for the cheating pfo creation algorithm class.
 * 
 *  $Log: $
 */
#ifndef CHEATING_PFO_CREATION_ALGORITHM_H
#define CHEATING_PFO_CREATION_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Objects/CartesianVector.h"

/**
 *  @brief CheatingPfoCreationAlgorithm class
 */
class CheatingPfoCreationAlgorithm : public pandora::Algorithm
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
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  ComputeFromCalorimeter
     * 
     *  @param  pCluster
     *  @param  energy
     *  @param  momentum
     *  @param  mass
     *  @param  particleId
     *  @param  charge
     */
    static void ComputeFromCalorimeter(pandora::Cluster *pCluster, float &energy, pandora::CartesianVector &momentum, float &mass,
        int &particleId, int &charge);

    /**
     *  @brief  ComputeFromMC
     * 
     *  @param  pCluster
     *  @param  energy
     *  @param  momentum
     *  @param  mass
     *  @param  particleId
     *  @param  charge
     */
    static void ComputeFromMC(pandora::Cluster *pCluster, float &energy, pandora::CartesianVector &momentum, float &mass,
        int &particleId, int &charge);

    /**
     *  @brief  ComputeFromTracks
     * 
     *  @param  pCluster
     *  @param  energy
     *  @param  momentum
     *  @param  mass
     *  @param  particleId
     *  @param  charge
     */
    static void ComputeFromTracks(pandora::Cluster *pCluster, float &energy, pandora::CartesianVector &momentum, float &mass,
        int &particleId, int &charge);

    /**
     *  @brief  ComputeFromCalorimeterAndTracks
     * 
     *  @param  pCluster
     *  @param  energy
     *  @param  momentum
     *  @param  mass
     *  @param  particleId
     *  @param  charge
     */
    static void ComputeFromCalorimeterAndTracks(pandora::Cluster *pCluster, float &energy, pandora::CartesianVector &momentum, float &mass,
        int &particleId, int &charge);

    /**
     *  @brief  ComputeEnergyWeightedClusterPosition
     * 
     *  @param  pCluster
     *  @param  energyWeightedClusterPosition
     */
    static void ComputeEnergyWeightedClusterPosition(pandora::Cluster *pCluster, pandora::CartesianVector &energyWeightedClusterPosition);

    std::string     m_outputPfoListName;            ///< The output pfo list name
    std::string     m_inputClusterListName;         ///< The input cluster list name; if not specified, use current list
    std::string     m_pfoParameterDetermination;    ///< Determines how to calculate pfo parameters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingPfoCreationAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingPfoCreationAlgorithm();
}

#endif // #ifndef CHEATING_PFO_CREATION_ALGORITHM_H
