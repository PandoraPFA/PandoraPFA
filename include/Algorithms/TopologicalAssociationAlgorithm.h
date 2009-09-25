/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociationAlgorithm.h
 * 
 *  @brief  Header file for the topological association algorithm class.
 * 
 *  $Log: $
 */
#ifndef TOPOLOGICAL_ASSOCIATION_ALGORITHM_H
#define TOPOLOGICAL_ASSOCIATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  TemplateAlgorithm class
 */
class TopologicalAssociationAlgorithm : public pandora::Algorithm
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
    StatusCode ReadSettings(TiXmlHandle xmlHandle);

    // Member variables here
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TopologicalAssociationAlgorithm::Factory::CreateAlgorithm() const
{
    return new TopologicalAssociationAlgorithm();
}

#endif // #ifndef TOPOLOGICAL_ASSOCIATION_ALGORITHM_H
