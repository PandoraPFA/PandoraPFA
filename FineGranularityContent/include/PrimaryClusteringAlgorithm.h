/**
 *  @file   PandoraPFANew/FineGranularityContent/include/PrimaryClusteringAlgorithm.h
 * 
 *  @brief  Header file for the primary clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PRIMARY_CLUSTERING_ALGORITHM_H
#define PRIMARY_CLUSTERING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  PrimaryClusteringAlgorithm class
 */
class PrimaryClusteringAlgorithm : public pandora::Algorithm
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

    std::string     m_clusteringAlgorithmName;      ///< The name of the clustering algorithm to run
    std::string     m_associationAlgorithmName;     ///< The name of the topological association algorithm to run

    std::string     m_inputCaloHitListName;         ///< The name of the input ordered calo hit list, containing the hits to be clustered
    bool            m_restoreOriginalCaloHitList;   ///< Whether to restore the original calo hit list as the "current" list upon completion

    std::string     m_clusterListName;              ///< The name under which to save the new cluster list
    bool            m_replaceCurrentClusterList;    ///< Whether to subsequently use the new cluster list as the "current" list
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PrimaryClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new PrimaryClusteringAlgorithm();
}

#endif // #ifndef PRIMARY_CLUSTERING_ALGORITHM_H
