/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstructionAlgorithm.h
 * 
 *  @brief  Header file for the pfo construction algorithm class.
 * 
 *  $Log: $
 */
#ifndef PFO_CONSTRUCTION_ALGORITHM_H
#define PFO_CONSTRUCTION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  PfoConstructionAlgorithm class
 */
class PfoConstructionAlgorithm : public pandora::Algorithm
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

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    std::string     m_clusterPreparationAlgorithmName;      ///< The name of the cluster preparation algorithm to run
    std::string     m_trackPreparationAlgorithmName;        ///< The name of the track preparation algorithm to run
    std::string     m_fragmentRemovalAlgorithmName;         ///< The name of the fragment removal algorithm to run
    std::string     m_pfoCreationAlgorithmName;             ///< The name of the pfo creation algorithm to run
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PfoConstructionAlgorithm::Factory::CreateAlgorithm() const
{
    return new PfoConstructionAlgorithm();
}

#endif // #ifndef PFO_CONSTRUCTION_ALGORITHM_H
