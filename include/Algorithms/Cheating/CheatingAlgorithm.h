/**
 *  @file   PandoraPFANew/include/Algorithms/CheatingAlgorithm.h
 * 
 *  @brief  Header file for the cheating algorithm class.
 * 
 *  $Log: $
 */
#ifndef CHEATING_ALGORITHM_H
#define CHEATING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

class pandora::Cluster;
class pandora::CartesianVector;

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

    const bool operator()(const Pair& arg0, Pair& arg1) const
    { 
        return Op()( arg0.second, arg1.second ); 
    }
};

} // namespace pandora


/**
 *  @brief CheatingAlgorithm class
 */
class CheatingAlgorithm : public pandora::Algorithm
{
private:
public:


    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    void       ComputeEnergyWeightedClusterPosition( pandora::Cluster* cluster, pandora::CartesianVector& energyWeightedClusterPosition );

    void       ComputeFromCalorimeter         ( pandora::Cluster* cluster, float& energy, pandora::CartesianVector& momentum, float& mass, int& particleId, int& charge );
    void       ComputeFromMc                  ( pandora::Cluster* cluster, float& energy, pandora::CartesianVector& momentum, float& mass, int& particleId, int& charge );
    void       ComputeFromTracks              ( pandora::Cluster* cluster, float& energy, pandora::CartesianVector& momentum, float& mass, int& particleId, int& charge );
    void       ComputeFromCalorimeterAndTracks( pandora::Cluster* cluster, float& energy, pandora::CartesianVector& momentum, float& mass, int& particleId, int& charge );

    std::string     m_clusteringAlgorithmName;      ///< The name of the clustering algorithm to run
    std::string     m_cheatingListName;             ///< The name under which to save the new cheated cluster list

    std::string     m_energyFrom;                   ///< tells which energy is taken

    bool            m_debug;                        ///< turn on debugging statements
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingAlgorithm();
}

#endif // #ifndef CHEATING_ALGORITHM_H
