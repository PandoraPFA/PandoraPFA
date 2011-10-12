/**
 *  @file   PandoraPFANew/FineGranularityContent/src/TopologicalAssociation/MuonClusterAssociationAlgorithm.cc
 * 
 *  @brief  Implementation of the muon cluster association algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "TopologicalAssociation/MuonClusterAssociationAlgorithm.h"

using namespace pandora;

StatusCode MuonClusterAssociationAlgorithm::Run()
{
    // Get the muon cluster list, which will only exist if there are muon hits to cluster
    const ClusterList *pMuonClusterList = NULL;
    StatusCode statusCode = PandoraContentApi::GetClusterList(*this, m_muonClusterListName, pMuonClusterList);

    if (STATUS_CODE_NOT_INITIALIZED == statusCode)
        return STATUS_CODE_SUCCESS;

    if (STATUS_CODE_SUCCESS != statusCode)
        return statusCode;

    ClusterVector muonClusterVector(pMuonClusterList->begin(), pMuonClusterList->end());
    std::sort(muonClusterVector.begin(), muonClusterVector.end(), Cluster::SortByInnerLayer);

    // Get the current cluster list, with which muon clusters will be associated
    std::string inputClusterListName;
    const ClusterList *pInputClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pInputClusterList, inputClusterListName));

    ClusterList standaloneMuonClusters;

    // Loop over muon cluster list, looking for muon clusters containing sufficient hits
    for (ClusterVector::iterator iterI = muonClusterVector.begin(), iterIEnd = muonClusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pMuonCluster = *iterI;

        if (NULL == pMuonCluster)
            continue;

        if (pMuonCluster->GetNCaloHits() < m_minHitsInMuonCluster)
            continue;

        Cluster *pBestHadron(NULL), *pBestLeavingTrack(NULL), *pBestNonLeavingTrack(NULL);
        float bestDCosHadron(m_dCosCut), bestDCosLeavingTrack(m_dCosCut), bestDCosNonLeavingTrack(m_dCosCut);
        float bestEnergyHadron(0.), bestEnergyLeavingTrack(0.), bestEnergyNonLeavingTrack(0.);

        // Calculate muon cluster properties
        const PseudoLayer muonClusterInnerLayer(pMuonCluster->GetInnerPseudoLayer());
        const CartesianVector muonInnerCentroid(pMuonCluster->GetCentroid(muonClusterInnerLayer));
        const CartesianVector &muonDirection(pMuonCluster->GetInitialDirection());

        float energyLostInCoil(0.);

        if (m_shouldEstimateEnergyLostInCoil)
        {
            const float muonInnerX(muonInnerCentroid.GetX()), muonInnerY(muonInnerCentroid.GetY());
            const float innerRadius(std::sqrt(muonInnerX * muonInnerX + muonInnerY * muonInnerY));

            const unsigned int nHitsInInnerLayer(pMuonCluster->GetOrderedCaloHitList().GetNCaloHitsInPseudoLayer(muonClusterInnerLayer));

            if ((innerRadius > m_coilCorrectionMinInnerRadius) && (nHitsInInnerLayer >= m_coilCorrectionMinInnerLayerHits))
                energyLostInCoil += m_coilEnergyLossCorrection;
        }

        // For each muon cluster, examine suitable clusters in the input cluster list, looking for merging possibilities
        for (ClusterList::const_iterator iterJ = pInputClusterList->begin(), iterJEnd = pInputClusterList->end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pCluster = *iterJ;

            // Apply basic threshold cuts to cluster candidate
            const float clusterEnergy(pCluster->GetHadronicEnergy());

            if (clusterEnergy < m_minClusterHadronicEnergy)
                continue;

            if (pCluster->GetNCaloHits() < m_minHitsInCluster)
                continue;

            // Calculate properties of cluster candidate
            const CartesianVector clusterDirection(pCluster->GetCentroid(pCluster->GetOuterPseudoLayer()).GetUnitVector());
            const float dCos(muonDirection.GetDotProduct(clusterDirection));

            const TrackList &trackList(pCluster->GetAssociatedTrackList());

            float trackEnergySum(0.);

            for (TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
            {
                trackEnergySum += (*trackIter)->GetEnergyAtDca();
            }

            const bool hasAssociatedTrack(!trackList.empty());
            const bool isLeavingDetector(ClusterHelper::IsClusterLeavingDetector(pCluster));

            // Identify best association with a leaving, non-track-associated cluster
            if (isLeavingDetector && !hasAssociatedTrack)
            {
                if ((dCos > bestDCosHadron) || ((dCos == bestDCosHadron) && (clusterEnergy > bestEnergyHadron)))
                {
                    bestDCosHadron = dCos;
                    pBestHadron = pCluster;
                    bestEnergyHadron = clusterEnergy;
                }
            }

            // Identify best association with a leaving, track-associated cluster
            if (isLeavingDetector && hasAssociatedTrack)
            {
                if ((dCos > bestDCosLeavingTrack) || ((dCos == bestDCosLeavingTrack) && (clusterEnergy > bestEnergyLeavingTrack)))
                {
                    const float chi(ReclusterHelper::GetTrackClusterCompatibility(pCluster->GetTrackComparisonEnergy() + energyLostInCoil,
                        trackEnergySum));

                    if (chi < m_clusterAssociationChi)
                    {
                        bestDCosLeavingTrack = dCos;
                        pBestLeavingTrack = pCluster;
                        bestEnergyLeavingTrack = clusterEnergy;
                    }
                }
            }

            // Identify best association with a non-leaving, track-associated cluster
            if (!isLeavingDetector && hasAssociatedTrack)
            {
                if ((dCos > bestDCosNonLeavingTrack) || ((dCos == bestDCosNonLeavingTrack) && (clusterEnergy > bestEnergyNonLeavingTrack)))
                {
                    const float oldChi(ReclusterHelper::GetTrackClusterCompatibility(pCluster->GetTrackComparisonEnergy(), trackEnergySum));
                    const float newChi(ReclusterHelper::GetTrackClusterCompatibility(pCluster->GetTrackComparisonEnergy() + energyLostInCoil,
                        trackEnergySum));

                    if ((oldChi < -m_clusterAssociationChi) && (newChi < m_clusterAssociationChi))
                    {
                        bestDCosNonLeavingTrack = dCos;
                        pBestNonLeavingTrack = pCluster;
                        bestEnergyNonLeavingTrack = clusterEnergy;
                    }
                }
            }
        }

        // Select best merging candidate from those identified above
        Cluster *pBestInputCluster(NULL);

        if (NULL != pBestLeavingTrack)
        {
            pBestInputCluster = pBestLeavingTrack;
        }
        else if (NULL != pBestHadron)
        {
            pBestInputCluster = pBestHadron;
        }
        else if (NULL != pBestNonLeavingTrack)
        {
            pBestInputCluster = pBestNonLeavingTrack;
        }
        else
        {
            standaloneMuonClusters.insert(pMuonCluster);
        }

        // Merge the clusters
        if (NULL != pBestInputCluster)
        {
            *iterI = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestInputCluster,
                pMuonCluster, inputClusterListName, m_muonClusterListName));
        }
    }

    // Merge any identified standalone muon clusters into the input cluster list
    if (!standaloneMuonClusters.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_muonClusterListName,
            inputClusterListName, standaloneMuonClusters));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonClusterAssociationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_muonClusterListName = "MuonClusterList";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MuonClusterListName", m_muonClusterListName));

    m_dCosCut = 0.95f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DCosCut", m_dCosCut));

    m_minHitsInMuonCluster = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInMuonCluster", m_minHitsInMuonCluster));

    m_shouldEstimateEnergyLostInCoil = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldEstimateEnergyLostInCoil", m_shouldEstimateEnergyLostInCoil));

    m_coilCorrectionMinInnerRadius = 4000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CoilCorrectionMinInnerRadius", m_coilCorrectionMinInnerRadius));

    m_coilCorrectionMinInnerLayerHits = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CoilCorrectionMinInnerLayerHits", m_coilCorrectionMinInnerLayerHits));

    m_coilEnergyLossCorrection = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CoilEnergyLossCorrection", m_coilEnergyLossCorrection));

    m_minClusterHadronicEnergy = 0.25f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterHadronicEnergy", m_minClusterHadronicEnergy));

    m_minHitsInCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    m_clusterAssociationChi = 3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterAssociationChi", m_clusterAssociationChi));

    return STATUS_CODE_SUCCESS;
}
