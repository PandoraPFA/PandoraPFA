/**
 *  @file   PandoraPFANew/FineGranularityContent/src/ParticleId/PhotonReconstructionAlgorithm.cc
 * 
 *  @brief  Implementation of the photon reconstruction algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ParticleId/PhotonReconstructionAlgorithm.h"

using namespace pandora;

StatusCode PhotonReconstructionAlgorithm::Run()
{
    // Create list of candidate photon clusters
    const ClusterList *pPhotonClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_photonClusteringAlgName,
        pPhotonClusterList));

    // Fragmentation can only proceed with reference to a saved cluster list, so save these temporary clusters
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_clusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentClusterList(*this, m_clusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pPhotonClusterList));

    // Sort the clusters, then examine each cluster profile to obtain best-guess photon clusters. Delete non-photon clusters.
    ClusterVector clusterVector(pPhotonClusterList->begin(), pPhotonClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), Cluster::SortByInnerLayer);

    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        *iter = NULL;

        bool usedCluster(false);

        if (pCluster->GetElectromagneticEnergy() > m_minClusterEnergy)
        {
            // Look for peaks in cluster transverse shower profile
            ParticleIdHelper::ShowerPeakList showerPeakList;
            ParticleIdHelper::CalculateTransverseProfile(pCluster, m_transProfileMaxLayer, showerPeakList);

            if (!showerPeakList.empty())
            {
                ClusterList clusterList;
                clusterList.insert(pCluster);

                std::string originalClusterListName, peakClusterListName;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList,
                    originalClusterListName, peakClusterListName));

                bool useOriginalCluster(false);

                for (unsigned int iPeak = 0, iPeakEnd = showerPeakList.size(); iPeak < iPeakEnd; ++iPeak)
                {
                    // Extract cluster corresponding to identified shower peak
                    const ParticleIdHelper::ShowerPeak &showerPeak(showerPeakList[iPeak]);
                    CaloHitList peakCaloHitList(showerPeak.GetPeakCaloHitList());

                    if (showerPeak.GetPeakEnergy() < m_minPeakEnergy)
                        continue;

                    if (showerPeak.GetPeakRms() > m_maxPeakRms)
                        continue;

                    if (peakCaloHitList.size() < m_minPeakCaloHits)
                        continue;

                    Cluster *pPeakCluster = NULL;
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, &peakCaloHitList, pPeakCluster));

                    // TODO get pid here, for now cheat photon identification step
                    bool isPeakPhoton(false);
                    const MCParticle *pPeakMCParticle(MCParticleHelper::GetMainMCParticle(pPeakCluster));

                    if ((NULL != pPeakMCParticle) && (PHOTON == pPeakMCParticle->GetParticleId()))
                        isPeakPhoton = true;

                    // Decide whether to use cluster from shower peak region
                    bool acceptPhotonCandidate(false);

                    const float longProfileStart(pPeakCluster->GetShowerProfileStart());
                    const float longProfileDiscrepancy(pPeakCluster->GetShowerProfileDiscrepancy());

                    if (isPeakPhoton && (longProfileStart < m_maxLongProfileStart) && (longProfileDiscrepancy < m_maxLongProfileDiscrepancy))
                    {
                        // TODO examine distances to nearby tracks here
                        const bool closeToTrack(false);

                        if (!closeToTrack)
                        {
                            acceptPhotonCandidate = true;

                            if (0 == iPeak)
                            {
                                const float peakEnergyFraction(pPeakCluster->GetElectromagneticEnergy() / pCluster->GetElectromagneticEnergy());
                                const float peakEnergyDifference(pCluster->GetElectromagneticEnergy() - pPeakCluster->GetElectromagneticEnergy());

                                if ((peakEnergyFraction > m_oldClusterEnergyFraction0) || (showerPeakList.size() == m_oldClusterNPeaks))
                                {
                                    useOriginalCluster = true;
                                }
                                else if ((peakEnergyFraction > m_oldClusterEnergyFraction1) && (peakEnergyDifference < m_oldClusterEnergyDifference1))
                                {
                                    useOriginalCluster = true;
                                }
                                else if ((peakEnergyFraction > m_oldClusterEnergyFraction2) && (peakEnergyDifference < m_oldClusterEnergyDifference2))
                                {
                                    useOriginalCluster = true;
                                }
                                else if ((peakEnergyFraction > m_oldClusterEnergyFraction3) && (peakEnergyDifference < m_oldClusterEnergyDifference3))
                                {
                                    useOriginalCluster = true;
                                }
                            }
                        }
                        else
                        {
                            // TODO demand higher pid values in event of close proximity to a track
                        }
                    }

                    if (acceptPhotonCandidate)
                    {
                        usedCluster = true;
                        useOriginalCluster ? pCluster->SetIsFixedPhotonFlag(true) : pPeakCluster->SetIsFixedPhotonFlag(true);
                    }
                    else
                    {
                        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteCluster(*this, pPeakCluster));
                    }

                    if (useOriginalCluster)
                        break;
                }

                const std::string clusterListToSaveName((usedCluster && !useOriginalCluster) ? peakClusterListName : originalClusterListName);
                const std::string clusterListToDeleteName((usedCluster && !useOriginalCluster) ? originalClusterListName : peakClusterListName);
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,
                    clusterListToDeleteName));
            }
        }

        // Delete clusters from which no components have been used
        if (!usedCluster)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteCluster(*this, pCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "PhotonClusterFormation", m_photonClusteringAlgName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterListName", m_clusterListName));

    m_minClusterEnergy = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterEnergy", m_minClusterEnergy));

    m_transProfileMaxLayer = 30;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TransProfileMaxLayer", m_transProfileMaxLayer));

    m_minPeakEnergy = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinPeakEnergy", m_minPeakEnergy));

    m_maxPeakRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxPeakRms", m_maxPeakRms));

    m_minPeakCaloHits = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinPeakCaloHits", m_minPeakCaloHits));

    m_maxLongProfileStart = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLongProfileStart", m_maxLongProfileStart));

    m_maxLongProfileDiscrepancy = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLongProfileDiscrepancy", m_maxLongProfileDiscrepancy));

    m_oldClusterEnergyFraction0 = 0.95f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction0", m_oldClusterEnergyFraction0));

    m_oldClusterNPeaks = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterNPeaks", m_oldClusterNPeaks));

    m_oldClusterEnergyFraction1 = 0.9f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction1", m_oldClusterEnergyFraction1));

    m_oldClusterEnergyDifference1 = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyDifference1", m_oldClusterEnergyDifference1));

    m_oldClusterEnergyFraction2 = 0.8f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction2", m_oldClusterEnergyFraction2));

    m_oldClusterEnergyDifference2 = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyDifference2", m_oldClusterEnergyDifference2));

    m_oldClusterEnergyFraction3 = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction3", m_oldClusterEnergyFraction3));

    m_oldClusterEnergyDifference3 = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyDifference3", m_oldClusterEnergyDifference3));

    return STATUS_CODE_SUCCESS;
}
