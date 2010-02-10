/**
 *  @file   PandoraPFANew/src/Algorithms/Reclustering/ResolveTrackAssociationsAlg.cc
 * 
 *  @brief  Implementation of the resolve track associations algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/Reclustering/ResolveTrackAssociationsAlg.h"

#include <limits>

using namespace pandora;

StatusCode ResolveTrackAssociationsAlg::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    // Store copy of input cluster list in a vector
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());

    // Examine each cluster in the input list
    const unsigned int nClusters(clusterVector.size());

    for (unsigned int i = 0; i < nClusters; ++i)
    {
        Cluster *pParentCluster = clusterVector[i];

        if (NULL == pParentCluster)
            continue;

        // Check compatibility of cluster with its associated tracks
        const TrackList &trackList(pParentCluster->GetAssociatedTrackList());
        const unsigned int nTrackAssociations(trackList.size());

        if ((nTrackAssociations < m_minTrackAssociations) || (nTrackAssociations > m_maxTrackAssociations))
            continue;

        const float chi(ReclusterHelper::GetTrackClusterCompatibility(pParentCluster, trackList));

        if (chi > m_chiToAttemptReclustering)
            continue;

        // Specify tracks and clusters to be used in reclustering
        TrackList reclusterTrackList(trackList.begin(), trackList.end());

        ClusterList reclusterClusterList;
        reclusterClusterList.insert(pParentCluster);

        UIntVector originalClusterIndices(1, i);

        // Look for potential daughter clusters to combine in the reclustering
        for (unsigned int j = 0; j < nClusters; ++j)
        {
            Cluster *pDaughterCluster = clusterVector[j];

            if ((NULL == pDaughterCluster) || (pParentCluster == pDaughterCluster) || (!pDaughterCluster->GetAssociatedTrackList().empty()))
                continue;

            if (FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, m_coneCosineHalfAngle) > m_minConeFraction)
            {
                reclusterClusterList.insert(pDaughterCluster);
                originalClusterIndices.push_back(j);
            }
        }

        // Initialize reclustering with these local lists
        std::string originalClustersListName;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeReclustering(*this, reclusterTrackList, 
            reclusterClusterList, originalClustersListName));

        // Run multiple clustering algorithms and identify the best cluster candidates produced
        std::string bestReclusterListName, bestGuessListName;
        float bestReclusterChi2(chi * chi), bestGuessChi(std::numeric_limits<float>::max());

        for (StringVector::const_iterator clusteringIter = m_clusteringAlgorithms.begin(),
            clusteringIterEnd = m_clusteringAlgorithms.end(); clusteringIter != clusteringIterEnd; ++clusteringIter)
        {
            std::string reclustersListName;
            const ClusterList *pReclustersList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, *clusteringIter, 
                pReclustersList, reclustersListName));

            // Run topological association algorithm
            if (!pReclustersList->empty())
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));

            // Remove any track projection clusters remaining at this stage
            for (ClusterList::const_iterator reclusterIter = pReclustersList->begin(); reclusterIter != pReclustersList->end();)
            {
                Cluster *pReclusterCandidate = *(reclusterIter++);

                if (0 == pReclusterCandidate->GetNCaloHits())
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteCluster(*this, pReclusterCandidate));
            }

            // Calculate final track-cluster associations for these recluster candidates
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

            // Calculate figure of merit for recluster candidates. Label as best recluster candidates if applicable
            ReclusterHelper::ReclusterResult reclusterResult;
            if (STATUS_CODE_SUCCESS != ReclusterHelper::ExtractReclusterResults(pReclustersList, reclusterResult))
                continue;

            if (reclusterResult.GetMinTrackAssociationEnergy() < m_minClusterEnergyForTrackAssociation)
                continue;

            // Are recluster candidates good enough to justify replacing original clusters?
            const float reclusterChi2(reclusterResult.GetChi2PerDof());

            if ((reclusterChi2 < bestReclusterChi2) && (reclusterChi2 < m_chiToAttemptReclustering * m_chiToAttemptReclustering))
            {
                bestReclusterChi2 = reclusterChi2;
                bestReclusterListName = reclustersListName;

                // If chi2 is very good, stop the reclustering attempts
                if(bestReclusterChi2 < m_chi2ForAutomaticClusterSelection)
                    break;
            }

            // If no ideal candidate is found, store a best guess candidate for future modification
            else if (m_shouldUseBestGuessCandidates)
            {
                if ((reclusterResult.GetNExcessTrackAssociations() > 0) && (reclusterResult.GetChi() > 0) && (reclusterResult.GetChi() < bestGuessChi))
                {
                    bestGuessChi = reclusterResult.GetChi();
                    bestGuessListName = reclustersListName;
                }
            }
        }

        // If no ideal candidate constructed, can choose to use best guess candidates, which could be split by later algorithms
        if (m_shouldUseBestGuessCandidates && bestReclusterListName.empty())
        {
            bestReclusterListName = bestGuessListName;
        }

        // Tidy cluster vector, to remove addresses of deleted clusters
        if (bestReclusterListName.empty())
        {
            bestReclusterListName = originalClustersListName;
        }
        else if (bestReclusterListName != originalClustersListName)
        {
            for (UIntVector::const_iterator iter = originalClusterIndices.begin(), iterEnd = originalClusterIndices.end(); iter != iterEnd; ++iter)
                clusterVector[*iter] = NULL;
        }

        // Choose the best recluster candidates, which may still be the originals
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndReclustering(*this, bestReclusterListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ResolveTrackAssociationsAlg::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "clusteringAlgorithms",
        m_clusteringAlgorithms));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ClusterAssociation",
        m_associationAlgorithmName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "TrackClusterAssociation",
        m_trackClusterAssociationAlgName));

    m_minTrackAssociations = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackAssociations", m_minTrackAssociations));

    if (m_minTrackAssociations < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    m_maxTrackAssociations = std::numeric_limits<unsigned int>::max();
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackAssociations", m_maxTrackAssociations));

    m_chiToAttemptReclustering = -3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChiToAttemptReclustering", m_chiToAttemptReclustering));

    m_coneCosineHalfAngle = 0.9f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeCosineHalfAngle", m_coneCosineHalfAngle));

    m_minConeFraction = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinConeFraction", m_minConeFraction));

    m_minClusterEnergyForTrackAssociation = 0.1f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEnergyForTrackAssociation", m_minClusterEnergyForTrackAssociation));

    m_chi2ForAutomaticClusterSelection = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Chi2ForAutomaticClusterSelection", m_chi2ForAutomaticClusterSelection));

    m_shouldUseBestGuessCandidates = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseBestGuessCandidates", m_shouldUseBestGuessCandidates));

    return STATUS_CODE_SUCCESS;
}
