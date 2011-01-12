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

namespace pandora 
{

template < typename Pair, typename Op >
class Select2nd : public std::binary_function<Pair, Pair, typename Pair::second_type>
{
 public:
    bool operator()(Pair& arg0, Pair& arg1) const
    { 
        return Op()( arg0.second, arg1.second ); 
    }
};

} // namespace pandora

//------------------------------------------------------------------------------------------------------------------------------------------

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
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  ComputeEnergyWeightedClusterPosition
     * 
     *  @param  pCluster
     *  @param  energyWeightedClusterPosition
     */
    void ComputeEnergyWeightedClusterPosition(pandora::Cluster *pCluster, pandora::CartesianVector &energyWeightedClusterPosition) const;

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
    void ComputeFromCalorimeter(pandora::Cluster *pCluster, float &energy, pandora::CartesianVector &momentum, float &mass,
        int &particleId, int &charge) const;

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
    void ComputeFromMC(pandora::Cluster *pCluster, float &energy, pandora::CartesianVector &momentum, float &mass,
        int &particleId, int &charge) const;

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
    void ComputeFromTracks(pandora::Cluster *pCluster, float &energy, pandora::CartesianVector &momentum, float &mass,
        int &particleId, int &charge) const;

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
    void ComputeFromCalorimeterAndTracks(pandora::Cluster *pCluster, float &energy, pandora::CartesianVector &momentum, float &mass,
        int &particleId, int &charge) const;

    std::string     m_clusteringAlgorithmName;      ///< The name of the clustering algorithm to run
    std::string     m_inputClusterListName;         ///< if a clusterlistname is given, take the clusters from there instead of running a clustering algorithm
    std::string     m_outputClusterListName;        ///< The name under which to save the new cheated cluster list

    std::string     m_energyFrom;                   ///< tells which energy is taken

    bool            m_debug;                        ///< turn on debugging statements
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingPfoCreationAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingPfoCreationAlgorithm();
}

#endif // #ifndef CHEATING_PFO_CREATION_ALGORITHM_H
