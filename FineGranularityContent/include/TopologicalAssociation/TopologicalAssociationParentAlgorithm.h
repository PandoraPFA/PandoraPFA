/**
 *  @file   PandoraPFANew/FineGranularityContent/include/TopologicalAssociation/TopologicalAssociationParentAlgorithm.h
 * 
 *  @brief  Header file for the topological association parent algorithm class.
 * 
 *  $Log: $
 */
#ifndef TOPOLOGICAL_ASSOCIATION_PARENT_ALGORITHM_H
#define TOPOLOGICAL_ASSOCIATION_PARENT_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  TemplateAlgorithm class
 */
class TopologicalAssociationParentAlgorithm : public pandora::Algorithm
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

    pandora::StringVector   m_associationAlgorithms;    ///< The ordered list of topological association algorithms to be used
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *TopologicalAssociationParentAlgorithm::Factory::CreateAlgorithm() const
{
    return new TopologicalAssociationParentAlgorithm();
}

#endif // #ifndef TOPOLOGICAL_ASSOCIATION_PARENT_ALGORITHM_H
