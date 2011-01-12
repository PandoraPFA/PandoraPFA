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

    #define CREATE_K_MEANS_CLUSTERING_ALGORITHM(a, b)                                                                           \
        {                                                                                                                       \
            const pandora::StatusCode statusCode(PandoraApi::RegisterAlgorithmFactory(pandora, a, new b));                      \
                                                                                                                                \
            if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                                     \
                return statusCode;                                                                                              \
        }

    #define REGISTER_ALL_K_MEANS_CLUSTERING_ALGORITHMS()                                                                        \
        K_MEANS_CLUSTERING_ALGORITHM_LIST(CREATE_K_MEANS_CLUSTERING_ALGORITHM)

    /**
     *  @brief  Register the k means clustering algorithms with pandora
     * 
     *  @param  pandora the pandora instance with which to register algorithms
     */
    static pandora::StatusCode Register(pandora::Pandora &pandora);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode KMeansContent::Register(pandora::Pandora &pandora)
{
    REGISTER_ALL_K_MEANS_CLUSTERING_ALGORITHMS();
    return pandora::STATUS_CODE_SUCCESS;
}

#endif // #ifndef K_MEANS_CONTENT_H
