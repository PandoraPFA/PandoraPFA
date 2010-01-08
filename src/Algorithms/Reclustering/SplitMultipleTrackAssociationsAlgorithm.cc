/**
 *  @file   PandoraPFANew/src/Algorithms/Reclustering/SplitMultipleTrackAssociationsAlgorithm.cc
 * 
 *  @brief  Implementation of the split multiple track associations algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/Reclustering/SplitMultipleTrackAssociationsAlgorithm.h"

#include <limits>

using namespace pandora;

StatusCode SplitMultipleTrackAssociationsAlgorithm::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    // Store copy of input cluster list in a vector
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());

    // Examine each cluster in the input list
    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        const TrackList &trackList(pCluster->GetAssociatedTrackList());
        const unsigned int nTrackAssociations(trackList.size());

        if ((nTrackAssociations < m_minTrackAssociationsToSplit) || (nTrackAssociations > m_maxTrackAssociationsToSplit))
            continue;

        if (this->GetTrackClusterCompatibility(pCluster, trackList) < m_chiToAttemptReclustering)
            continue;

        // Specify clusters and tracks to be used in reclustering
        ClusterList reclusterClusterList;
        reclusterClusterList.insert(pCluster);

        TrackList reclusterTrackList(trackList.begin(), trackList.end());

        // Initialize reclustering with these local lists
        std::string originalClustersListName;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeReclustering(*this, reclusterTrackList, 
            reclusterClusterList, originalClustersListName));

        // Run multiple clustering algorithms and identify the best cluster candidates produced
        std::string bestReclusterCandidateListName(originalClustersListName);
        float bestReclusterCandidateChi2(std::numeric_limits<float>::max());

        for (StringVector::const_iterator clusteringIter = m_clusteringAlgorithms.begin(),
            clusteringIterEnd = m_clusteringAlgorithms.end(); clusteringIter != clusteringIterEnd; ++clusteringIter)
        {
            std::string reclusterCandidatesListName;
            const ClusterList *pReclusterCandidatesList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, *clusteringIter, 
                pReclusterCandidatesList, reclusterCandidatesListName));

            // Run topological association algorithm
            if (!pReclusterCandidatesList->empty())
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));

            // Remove any track projection clusters remaining at this stage
            for (ClusterList::const_iterator reclusterIter = pReclusterCandidatesList->begin(); reclusterIter != pReclusterCandidatesList->end();)
            {
                Cluster *pReclusterCandidate = *(reclusterIter++);

                if (0 == pReclusterCandidate->GetNCaloHits())
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteCluster(*this, pReclusterCandidate));
            }

            // Calculate final track-cluster associations for these recluster candidates
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

            // Calculate figure of merit for recluster candidates. Label as best recluster candidates if applicable
            const float reclusterCandidateChi2(this->GetReclusterFigureOfMerit(pReclusterCandidatesList));

            if ((reclusterCandidateChi2 < bestReclusterCandidateChi2) && (reclusterCandidateChi2 < m_chiToAttemptReclustering * m_chiToAttemptReclustering))
            {
                bestReclusterCandidateChi2 = reclusterCandidateChi2;
                bestReclusterCandidateListName = reclusterCandidatesListName;

                // If chi2 is very good, stop the reclustering attempts
                if(bestReclusterCandidateChi2 < m_chi2ForAutomaticClusterSelection)
                    break;
            }
        }

        // Choose the best recluster candidates, which may still be the originals
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndReclustering(*this, bestReclusterCandidateListName));

        // Original cluster may have been deleted: for safety, remove its address from ClusterVector
        (*iter) = NULL;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float SplitMultipleTrackAssociationsAlgorithm::GetTrackClusterCompatibility(const Cluster *const pCluster, const TrackList &trackList) const
{
    float trackEnergySum(0.);

    for (TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
        trackEnergySum += (*trackIter)->GetEnergyAtDca();

    static const float hadronicEnergyResolution(PandoraSettings::GetInstance()->GetHadronicEnergyResolution());

    if ((0. == trackEnergySum) || (0. == hadronicEnergyResolution))
        return STATUS_CODE_FAILURE;

    const float sigmaE(hadronicEnergyResolution * trackEnergySum / std::sqrt(trackEnergySum));
    const float chi((pCluster->GetHadronicEnergy() - trackEnergySum) / sigmaE);

    return chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float SplitMultipleTrackAssociationsAlgorithm::GetReclusterFigureOfMerit(const ClusterList *const pReclusterCandidatesList) const
{
    float chi2(0.);
    float dof(0.);

    for (ClusterList::const_iterator iter = pReclusterCandidatesList->begin(), iterEnd = pReclusterCandidatesList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        const TrackList &trackList(pCluster->GetAssociatedTrackList());
        const unsigned int nTrackAssociations(trackList.size());

        if (1 < nTrackAssociations)
            return std::numeric_limits<float>::max();

        if (0 == nTrackAssociations)
            continue;

        const float chi(this->GetTrackClusterCompatibility(pCluster, trackList));
        chi2 += chi * chi;
        dof += 1.;

        // Veto case where track is now matched to v. low energy cluster
        if (pCluster->GetHadronicEnergy() < m_minClusterEnergyForTrackAssociation)
            return std::numeric_limits<float>::max();
    }

    if (0. == dof)
        return std::numeric_limits<float>::max();

    return chi2 /= dof;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SplitMultipleTrackAssociationsAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "clusteringAlgorithms",
        m_clusteringAlgorithms));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ClusterAssociation",
        m_associationAlgorithmName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "TrackClusterAssociation",
        m_trackClusterAssociationAlgName));

    m_minTrackAssociationsToSplit = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackAssociationsToSplit", m_minTrackAssociationsToSplit));

    if (m_minTrackAssociationsToSplit < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    m_maxTrackAssociationsToSplit = std::numeric_limits<unsigned int>::max();
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackAssociationsToSplit", m_maxTrackAssociationsToSplit));

    m_chiToAttemptReclustering = -3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ChiToAttemptReclustering", m_chiToAttemptReclustering));

    m_minClusterEnergyForTrackAssociation = 0.1f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEnergyForTrackAssociation", m_minClusterEnergyForTrackAssociation));

    m_chi2ForAutomaticClusterSelection = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Chi2ForAutomaticClusterSelection", m_chi2ForAutomaticClusterSelection));

    return STATUS_CODE_SUCCESS;
}
