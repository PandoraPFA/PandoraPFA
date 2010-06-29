/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/SoftClusterMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the soft cluster merging algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/SoftClusterMergingAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode SoftClusterMergingAlgorithm::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    // Create a vector of input clusters, ordered by inner layer
    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), Cluster::SortByInnerLayer);

    // Loop over soft daughter candidate clusters
    for (ClusterVector::iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = *iterI;

        if (NULL == pDaughterCluster)
            continue;

        if(!this->IsSoftCluster(pDaughterCluster))
            continue;

        Cluster *pBestParentCluster(NULL);
        float bestParentClusterEnergy(0.);
        float closestDistance(std::numeric_limits<float>::max());

        const CartesianVector &daughterInitialDirection(pDaughterCluster->GetInitialDirection());

        // Find best candidate parent cluster: that with closest distance between a pair of hits in the daughter and parent
        for (ClusterVector::iterator iterJ = clusterVector.begin(), iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            if ((NULL == pParentCluster) || (pDaughterCluster == pParentCluster))
                continue;

            if (pParentCluster->GetNCaloHits() <= m_maxHitsInSoftCluster)
                continue;

            const float parentClusterEnergy(pParentCluster->GetHadronicEnergy());

            if (parentClusterEnergy < m_minClusterHadEnergy)
                continue;

            // Apply simple preselection using cosine of opening angle between the clusters
            const float cosOpeningAngle(pParentCluster->GetInitialDirection().GetCosOpeningAngle(daughterInitialDirection));

            if (cosOpeningAngle < m_minCosOpeningAngle)
                continue;

            const float distance(ClusterHelper::GetDistanceToClosestHit(pParentCluster, pDaughterCluster));

            // In event of equidistant parent candidates, choose highest energy cluster
            if ((distance < closestDistance) || ((distance == closestDistance) && (parentClusterEnergy > bestParentClusterEnergy)))
            {
                closestDistance = distance;
                pBestParentCluster = pParentCluster;
                bestParentClusterEnergy = parentClusterEnergy;
            }
        }

        if (NULL == pBestParentCluster)
            continue;

        if (this->CanMergeSoftCluster(pDaughterCluster, closestDistance))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
            *iterI = NULL;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SoftClusterMergingAlgorithm::IsSoftCluster(Cluster *const pDaughterCluster) const
{
    // Note the cuts applied here are order-dependent - use the order defined in original version of pandora
    const unsigned int nCaloHits(pDaughterCluster->GetNCaloHits());

    if (0 == nCaloHits)
        return false;

    // ATTN: Added this cut to prevent merging multiple track-seeded clusters
    if (!pDaughterCluster->GetAssociatedTrackList().empty())
        return false;

    bool isSoftCluster(false);

    if (nCaloHits <= m_maxHitsInSoftCluster)
    {
        isSoftCluster = true;
    }
    else if ((pDaughterCluster->GetOuterPseudoLayer() - pDaughterCluster->GetInnerPseudoLayer()) < m_maxLayersSpannedBySoftCluster)
    {
        isSoftCluster = true;
    }
    else if (pDaughterCluster->GetHadronicEnergy() < m_maxHadEnergyForSoftClusterNoTrack)
    {
        isSoftCluster = true;
    }

    if (pDaughterCluster->GetHadronicEnergy() < m_minClusterHadEnergy)
        isSoftCluster = true;

    if (pDaughterCluster->IsPhotonFast() && (pDaughterCluster->GetElectromagneticEnergy() > m_minClusterEMEnergy))
        isSoftCluster = false;

    return isSoftCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool SoftClusterMergingAlgorithm::CanMergeSoftCluster(const Cluster *const pDaughterCluster, const float closestDistance) const
{
    if (closestDistance < m_closestDistanceCut0)
        return true;

    const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

    if ((closestDistance < m_closestDistanceCut1) && (daughterInnerLayer > m_innerLayerCut1))
        return true;

    if ((closestDistance < m_closestDistanceCut2) && (daughterInnerLayer > m_innerLayerCut2))
        return true;

    if (closestDistance < m_maxClusterDistance)
    {
        if ((pDaughterCluster->GetHadronicEnergy() < m_minClusterHadEnergy) || (pDaughterCluster->GetNCaloHits() < m_minHitsInCluster))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SoftClusterMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_trackClusterAssociationAlgName));

    m_maxHitsInSoftCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxHitsInSoftCluster", m_maxHitsInSoftCluster));

    m_maxLayersSpannedBySoftCluster = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayersSpannedBySoftCluster", m_maxLayersSpannedBySoftCluster));

    m_maxHadEnergyForSoftClusterNoTrack = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxHadEnergyForSoftClusterNoTrack", m_maxHadEnergyForSoftClusterNoTrack));

    m_minClusterHadEnergy = 0.25f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterHadEnergy", m_minClusterHadEnergy));

    m_minClusterEMEnergy = 0.025f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEMEnergy", m_minClusterEMEnergy));

    m_minCosOpeningAngle = 0.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCosOpeningAngle", m_minCosOpeningAngle));

    m_closestDistanceCut0 = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceCut0", m_closestDistanceCut0));

    m_closestDistanceCut1 = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceCut1", m_closestDistanceCut1));

    m_innerLayerCut1 = 20;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InnerLayerCut1", m_innerLayerCut1));

    m_closestDistanceCut2 = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestDistanceCut2", m_closestDistanceCut2));

    m_innerLayerCut2 = 40;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InnerLayerCut2", m_innerLayerCut2));

    m_maxClusterDistance = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterDistance", m_maxClusterDistance));

    m_minHitsInCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    return STATUS_CODE_SUCCESS;
}
