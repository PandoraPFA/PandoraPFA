/**
 *  @file   PandoraPFANew/src/Algorithms/Reclustering/TrackDrivenAssociationAlg.cc
 * 
 *  @brief  Implementation of the track driven association algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/Reclustering/TrackDrivenAssociationAlg.h"

using namespace pandora;

StatusCode TrackDrivenAssociationAlg::Run()
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
        const TrackList &parentTrackList(pParentCluster->GetAssociatedTrackList());
        const unsigned int nTrackAssociations(parentTrackList.size());

        if ((nTrackAssociations < m_minTrackAssociations) || (nTrackAssociations > m_maxTrackAssociations))
            continue;

        const float chi(ReclusterHelper::GetTrackClusterCompatibility(pParentCluster, parentTrackList));

        if (chi > m_chiToAttemptReclustering)
            continue;

        if (ClusterHelper::IsClusterLeavingDetector(pParentCluster))
            continue;

        // Specify tracks and clusters to be used in reclustering
        TrackList reclusterTrackList(parentTrackList.begin(), parentTrackList.end());

        ClusterList reclusterClusterList;
        reclusterClusterList.insert(pParentCluster);

        UIntVector originalClusterIndices(1, i);

        float excessEnergy(0.);

        // Look for clusters in the nearby region with an excess of energy compared to the track
        for (unsigned int j = 0; j < nClusters; ++j)
        {
            Cluster *pDaughterCluster = clusterVector[j];

            if ((NULL == pDaughterCluster) || (pParentCluster == pDaughterCluster))
                continue;

            float daughterTrackEnergy(0.);
            float bestFraction(FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, m_coneCosineHalfAngle));

            const TrackList &daughterTrackList(pDaughterCluster->GetAssociatedTrackList());

            for (TrackList::const_iterator iter = daughterTrackList.begin(), iterEnd = daughterTrackList.end(); iter != iterEnd; ++iter)
            {
                daughterTrackEnergy += (*iter)->GetEnergyAtDca();

                const float fraction(FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, *iter, m_coneCosineHalfAngle));

                if (fraction > bestFraction)
                    bestFraction = fraction;
            }

            if (bestFraction > m_minFractionOfHitsInCone)
            {
                if (daughterTrackList.empty())
                {
                    reclusterClusterList.insert(pDaughterCluster);
                    originalClusterIndices.push_back(j);
                }
                else
                {
                    excessEnergy += pDaughterCluster->GetHadronicEnergy() - daughterTrackEnergy;
                }
            }
            else
            {
                float contactFraction(0.);
                unsigned int nContactLayers(0);

                if (STATUS_CODE_SUCCESS != FragmentRemovalHelper::GetClusterContactDetails(pDaughterCluster, pParentCluster,
                    m_contactDistanceThreshold, nContactLayers, contactFraction))
                {
                    continue;
                }

                if (nContactLayers < m_minContactLayers)
                    continue;

                if (daughterTrackList.empty())
                {
                    reclusterClusterList.insert(pDaughterCluster);
                    originalClusterIndices.push_back(j);
                }
                else
                {
                    excessEnergy += pDaughterCluster->GetHadronicEnergy() - daughterTrackEnergy;
                }
            }
        }

        // Initialize reclustering with these local lists
        std::string originalClustersListName;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeReclustering(*this, reclusterTrackList,
            reclusterClusterList, originalClustersListName));

        // Run multiple clustering algorithms and identify the best cluster candidates produced
        std::string bestReclusterListName(originalClustersListName);
        float bestReclusterChi2(std::numeric_limits<float>::max());

        for (StringVector::const_iterator clusteringIter = m_clusteringAlgorithms.begin(),
            clusteringIterEnd = m_clusteringAlgorithms.end(); clusteringIter != clusteringIterEnd; ++clusteringIter)
        {
            std::string reclusterListName;
            const ClusterList *pReclusterList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, *clusteringIter, 
                pReclusterList, reclusterListName));

            // Run topological association algorithm
            if (!pReclusterList->empty())
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_associationAlgorithmName));

            // Remove any track projection clusters remaining at this stage
            for (ClusterList::const_iterator reclusterIter = pReclusterList->begin(); reclusterIter != pReclusterList->end();)
            {
                Cluster *pReclusterCandidate = *(reclusterIter++);

                if (0 == pReclusterCandidate->GetNCaloHits())
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteCluster(*this, pReclusterCandidate));
            }

            // Calculate final track-cluster associations for these recluster candidates
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

            // Calculate figure of merit for recluster candidates. Label as best recluster candidates if applicable
            ReclusterHelper::ReclusterResult reclusterResult;
            if (STATUS_CODE_SUCCESS != ReclusterHelper::ExtractReclusterResults(pReclusterList, reclusterResult))
                continue;

            if (reclusterResult.GetMinTrackAssociationEnergy() < m_minClusterEnergyForTrackAssociation)
                continue;

            // Are recluster candidates good enough to justify replacing original clusters?
            const float reclusterChi2(reclusterResult.GetChi2PerDof());

            if (reclusterChi2 < bestReclusterChi2)
            {
                bestReclusterChi2 = reclusterChi2;
                bestReclusterListName = reclusterListName;

                // If chi2 is very good, stop the reclustering attempts
                if(bestReclusterChi2 < m_chi2ForAutomaticClusterSelection)
                    break;
            }
        }

        if (bestReclusterListName != originalClustersListName)
        {
            bool shouldRecluster(bestReclusterChi2 < m_chiToAttemptReclustering * m_chiToAttemptReclustering);

            // Check to see if some energy has been clustered into adjacent track cluster
            if (!shouldRecluster && (excessEnergy > m_minExcessEnergy))
            {
                float clusterEnergy(0.);
                const ClusterList *pBestReclusterList = NULL;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, bestReclusterListName, pBestReclusterList));

                for (ClusterList::const_iterator iter = pBestReclusterList->begin(), iterEnd = pBestReclusterList->end(); iter != iterEnd; ++iter)
                {
                    if (!(*iter)->GetAssociatedTrackList().empty())
                        clusterEnergy += (*iter)->GetHadronicEnergy();
                }

                float trackEnergy(0.);
                for (TrackList::const_iterator trackIter = parentTrackList.begin(), trackIterEnd = parentTrackList.end(); trackIter != trackIterEnd; ++trackIter)
                {
                    trackEnergy += (*trackIter)->GetEnergyAtDca();
                }

                static const float hadronicEnergyResolution(PandoraSettings::GetInstance()->GetHadronicEnergyResolution());
                const float sigmaE(hadronicEnergyResolution * trackEnergy / std::sqrt(trackEnergy));

                float alpha((trackEnergy - clusterEnergy) / excessEnergy);
                alpha = std::max(0.f, alpha);
                alpha = std::min(1.f, alpha);

                const float deltaEWithExcess(clusterEnergy - trackEnergy + alpha * excessEnergy);
                const float chiWithExcess(deltaEWithExcess / sigmaE);

                if (std::fabs(chiWithExcess) < std::fabs(m_chiToAttemptReclustering))
                    shouldRecluster = true;
            }

            if (!shouldRecluster)
                bestReclusterListName = originalClustersListName;
        }

        // Tidy cluster vector, to remove addresses of deleted clusters
        if (bestReclusterListName != originalClustersListName)
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

StatusCode TrackDrivenAssociationAlg::ReadSettings(const TiXmlHandle xmlHandle)
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

    m_minFractionOfHitsInCone = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinFractionOfHitsInCone", m_minFractionOfHitsInCone));

    m_contactDistanceThreshold = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactDistanceThreshold", m_contactDistanceThreshold));

    m_minContactLayers = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinContactLayers", m_minContactLayers));

    m_minClusterEnergyForTrackAssociation = 0.1f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEnergyForTrackAssociation", m_minClusterEnergyForTrackAssociation));

    m_chi2ForAutomaticClusterSelection = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Chi2ForAutomaticClusterSelection", m_chi2ForAutomaticClusterSelection));

    m_minExcessEnergy = 0.1f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinExcessEnergy", m_minExcessEnergy));

    return STATUS_CODE_SUCCESS;
}