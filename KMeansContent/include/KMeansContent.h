/**
 *  @file   PandoraPFANew/KMeansContent/include/KMeansContent.h
 * 
 *  @brief  Header file detailing content related to k means clustering algorithms
 * 
 *  $Log: $
 */
#ifndef K_MEANS_CONTENT_H
#define K_MEANS_CONTENT_H 1

#include "KMeansClusteringAlgorithm.h"

/**
 *  @brief  KMeansContent class
 */
class KMeansContent
{
public:
    #define K_MEANS_CLUSTERING_ALGORITHM_LIST(d)                                                                                \
        d("KMeansClustering",                       KMeansClusteringAlgorithm::Factory)

    /**
     *  @brief  Register all the k means clustering algorithms with pandora
     * 
     *  @param  pandora the pandora instance with which to register algorithms
     */
    static pandora::StatusCode RegisterAlgorithms(pandora::Pandora &pandora);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode KMeansContent::RegisterAlgorithms(pandora::Pandora &pandora)
{
    K_MEANS_CLUSTERING_ALGORITHM_LIST(PANDORA_REGISTER_ALGORITHM);

    return pandora::STATUS_CODE_SUCCESS;
}

#endif // #ifndef K_MEANS_CONTENT_H
