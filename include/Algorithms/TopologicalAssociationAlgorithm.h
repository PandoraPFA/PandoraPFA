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
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    pandora::StringVector   m_associationAlgorithms;    ///< The ordered list of topological association algorithms to be used
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TopologicalAssociationAlgorithm::Factory::CreateAlgorithm() const
{
    return new TopologicalAssociationAlgorithm();
}

#endif // #ifndef TOPOLOGICAL_ASSOCIATION_ALGORITHM_H
