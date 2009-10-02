/**
 *  @file   PandoraPFANew/src/Algorithms/ReclusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the reclustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/ReclusteringAlgorithm.h"

using namespace pandora;

StatusCode ReclusteringAlgorithm::Run()
{
    const TrackList *pInputTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pInputTrackList));

    for (TrackList::const_iterator trackIter = pInputTrackList->begin(); trackIter != pInputTrackList->end(); ++trackIter)
    {
        // Select clusters and tracks to use for reclustering
        TrackList reclusterTrackList;
        reclusterTrackList.insert(*trackIter);

        ClusterList reclusterClusterList;

        const ClusterList *pInputClusterList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pInputClusterList));

        for (ClusterList::const_iterator clusterIter = pInputClusterList->begin(); clusterIter != pInputClusterList->end(); ++clusterIter)
        {
            // For now, by way of example, just pair each track with the first cluster
            reclusterClusterList.insert(*clusterIter);
            break;
        }

        // Initialize reclustering with these local lists
        std::string originalClustersListName;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeReclustering(*this, reclusterTrackList, 
            reclusterClusterList, originalClustersListName));

        // Run multiple clustering algorithms, using returned cluster candidate list to calculate a figure of merit and 
        // identify the best recluster candidates.
        std::string bestReclusterCandidateListName = originalClustersListName;

        for (StringVector::const_iterator clusteringIter = m_clusteringAlgorithms.begin(),
            clusteringIterEnd = m_clusteringAlgorithms.end(); clusteringIter != clusteringIterEnd; ++clusteringIter)
        {
            std::string reclusterCandidatesListName;
            const ClusterList *pReclusterCandidatesList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, *clusteringIter, 
                pReclusterCandidatesList, reclusterCandidatesListName));

            // Run topological association algorithm
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));

            // Calculate figure of merit for recluster candidates here. Label as best recluster candidates if applicable.
            // For now, just take the last populated list.
            if (!pReclusterCandidatesList->empty())
                bestReclusterCandidateListName = reclusterCandidatesListName;
        }

        // Choose best clusters, which may be the originals
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndReclustering(*this, bestReclusterCandidateListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "clusteringAlgorithms",
        m_clusteringAlgorithms));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ClusterAssociation",
        m_associationAlgorithmName));

    return STATUS_CODE_SUCCESS;
}
