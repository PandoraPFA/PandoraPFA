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
    StatusCode ReadSettings(TiXmlHandle xmlHandle);

    std::string     m_clusteringAlgorithmName;      ///< The name of the clustering algorithm to run
    std::string     m_cheatingListName;             ///< The name under which to save the new cheated cluster list

    std::string     m_energyFrom;                   ///< tells which energy is taken
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingAlgorithm();
}

#endif // #ifndef CHEATING_ALGORITHM_H
