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
    const ClusterList *pPhotonClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_photonClusteringAlgName,
        pPhotonClusterList));

    // TODO explain these lines
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, "PhotonClusters"));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentClusterList(*this, "PhotonClusters"));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pPhotonClusterList));

    ClusterVector clusterVector(pPhotonClusterList->begin(), pPhotonClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), Cluster::SortByInnerLayer);

    for (ClusterVector::iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        *iter = NULL;

        bool usedCluster(false);

        if (pCluster->GetElectromagneticEnergy() > 0.2f)
        {
            // Look for peaks in cluster transverse shower profile
            ParticleIdHelper::ShowerPeakList showerPeakList;
            ParticleIdHelper::GetShowerPeaks(pCluster, 30, showerPeakList);

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

                    if (showerPeak.GetPeakEnergy() < 0.2f)
                        continue;

                    if (showerPeak.GetPeakRms() > 5.f)
                        continue;

                    if (peakCaloHitList.size() < 5)
                        continue;

                    Cluster *pPeakCluster = NULL;
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, &peakCaloHitList, pPeakCluster));

                    // TODO get pid here, for now cheat photon identification
                    bool isPeakPhoton(false);
                    const MCParticle *pPeakMCParticle(MCParticleHelper::GetMainMCParticle(pPeakCluster));

                    if ((NULL != pPeakMCParticle) && (PHOTON == pPeakMCParticle->GetParticleId()))
                        isPeakPhoton = true;

                    // Decide whether to use cluster from shower peak region
                    bool acceptPhotonCandidate(false);

                    const float showerProfileStart(pPeakCluster->GetShowerProfileStart());
                    const float showerProfileDiscrepancy(pPeakCluster->GetShowerProfileDiscrepancy());

                    if (isPeakPhoton && (showerProfileStart < 10.f) && (showerProfileDiscrepancy < 1.f))
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

                                if ((peakEnergyFraction > 0.95f) || (showerPeakList.size() == 1))
                                {
                                    useOriginalCluster = true;
                                }
                                else if ((peakEnergyFraction > 0.9f) && (peakEnergyDifference < 2.f))
                                {
                                    useOriginalCluster = true;
                                }
                                else if ((peakEnergyFraction > 0.8f) && (peakEnergyDifference < 1.f))
                                {
                                    useOriginalCluster = true;
                                }
                                else if ((peakEnergyFraction > 0.5f) && (peakEnergyDifference < 0.5f))
                                {
                                    useOriginalCluster = true;
                                }
                            }
                        }
                        else
                        {
                            // TODO demand higher pid values here
                        }
                    }

                    if (acceptPhotonCandidate)
                    {
                        usedCluster = true;
                        useOriginalCluster ? pCluster->SetIsFixedPhotonFlag(true) : pPeakCluster->SetIsFixedPhotonFlag(true);
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

    return STATUS_CODE_SUCCESS;
}
