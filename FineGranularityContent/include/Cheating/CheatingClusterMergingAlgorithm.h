/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Cheating/CheatingclustermergingAlgorithm.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef CHEATING_CLUSTER_MERGING_ALGORITHM_H
#define CHEATING_CLUSTER_MERGING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief CheatingClusterMergingAlgorithm class
 */
class CheatingClusterMergingAlgorithm : public pandora::Algorithm
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

    pandora::StringVector   m_clusterListNames;             ///< use the given cluster lists to search for daughter clusters to merge
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CheatingClusterMergingAlgorithm::Factory::CreateAlgorithm() const
{
    return new CheatingClusterMergingAlgorithm();
}

#endif // #ifndef CHEATING_CLUSTER_MERGING_ALGORITHM_H
