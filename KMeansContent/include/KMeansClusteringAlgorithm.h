/**
 *  @file   PandoraPFANew/KMeansContent/include/KMeansClusteringAlgorithm.h
 * 
 *  @brief  Header file for the k-means clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef K_MEANS_CLUSTERING_ALGORITHM_H
#define K_MEANS_CLUSTERING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  KMeansClusteringAlgorithm class
 */
class KMeansClusteringAlgorithm : public pandora::Algorithm
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

    typedef std::map<int, pandora::Cluster *> IndexToClusterMap;

    bool            m_shouldUseIsolatedHits;        ///< Whether to use isolated hits in the clustering algorithm
    unsigned int    m_nMeans;                       ///< The number of means
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *KMeansClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new KMeansClusteringAlgorithm();
}

#endif // #ifndef K_MEANS_CLUSTERING_ALGORITHM_H
