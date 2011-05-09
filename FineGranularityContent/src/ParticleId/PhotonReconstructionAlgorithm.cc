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

PhotonReconstructionAlgorithm::~PhotonReconstructionAlgorithm()
{
    this->TidyHistograms();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::Run()
{
    // Obtain current track list for later reference
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));
    TrackVector trackVector(pTrackList->begin(), pTrackList->end());
    std::sort(trackVector.begin(), trackVector.end(), Track::SortByEnergy);

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

        if ((pCluster->GetElectromagneticEnergy() > m_minClusterEnergy) && (GeometryHelper::GetHitTypeGranularity(pCluster->GetInnerLayerHitType()) <= FINE))
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

                    // Decide whether to accept cluster from shower peak region as a photon candidate
                    bool acceptPhotonCandidate(false);
                    const float longProfileStart(pPeakCluster->GetShowerProfileStart());
                    const float longProfileDiscrepancy(pPeakCluster->GetShowerProfileDiscrepancy());
                    const float peakEnergyFraction(pPeakCluster->GetElectromagneticEnergy() / pCluster->GetElectromagneticEnergy());

                    // Multivariate/pid analysis to identify photon candidate clusters
                    if ((longProfileStart < m_maxLongProfileStart) && (longProfileDiscrepancy < m_maxLongProfileDiscrepancy))
                    {
                        const float minDistanceToTrack(this->GetMinDistanceToTrack(pPeakCluster, trackVector));

                        if (!m_shouldMakePdfHistograms)
                        {
                            const float pid(this->GetPid(showerPeak.GetPeakRms(), longProfileStart, longProfileDiscrepancy,
                                peakEnergyFraction, minDistanceToTrack));

                            if (pid > m_pidCut)
                                acceptPhotonCandidate = true;
                        }
                        else
                        {
                            this->FillPdfHistograms(pPeakCluster, showerPeak.GetPeakRms(), longProfileStart, longProfileDiscrepancy,
                                peakEnergyFraction, minDistanceToTrack);
                        }
                    }

                    // If shower peak represents large fraction of original cluster, revert to original cluster
                    if (acceptPhotonCandidate && (0 == iPeak))
                    {
                        const float peakEnergyDifference(pCluster->GetElectromagneticEnergy() - pPeakCluster->GetElectromagneticEnergy());

                        if ((showerPeakList.size() == 1) || (peakEnergyFraction > m_oldClusterEnergyFraction0))
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

                    // Decide whether or not to keep shower peak fragment
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

float PhotonReconstructionAlgorithm::GetMinDistanceToTrack(const Cluster *const pPeakCluster, const TrackVector &trackVector) const
{
    float minDistance(std::numeric_limits<float>::max());
    float minEnergyDifference(std::numeric_limits<float>::max());

    for (TrackVector::const_iterator trackIter = trackVector.begin(), trackIterEnd = trackVector.end(); trackIter != trackIterEnd; ++trackIter)
    {
        Track *pTrack = *trackIter;

        float trackClusterDistance(std::numeric_limits<float>::max());
        if (STATUS_CODE_SUCCESS == ClusterHelper::GetTrackClusterDistance(pTrack, pPeakCluster, 9, 100.f, trackClusterDistance))
        {
            const float energyDifference(std::fabs(pPeakCluster->GetHadronicEnergy() - pTrack->GetEnergyAtDca()));

            if ((trackClusterDistance < minDistance) || ((trackClusterDistance == minDistance) && (energyDifference < minEnergyDifference)))
            {
                minDistance = trackClusterDistance;
                minEnergyDifference = energyDifference;
            }
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonReconstructionAlgorithm::GetPid(const float peakRms, const float longProfileStart, const float longProfileDiscrepancy,
    const float peakEnergyFraction, const float minDistanceToTrack) const
{
    // TODO tidy this and investigate using overflow bin
    const int peakRmsBin(std::min(m_pSigPeakRms->GetNBinsX() - 1,
        static_cast<int>(peakRms / m_pSigPeakRms->GetXBinWidth())));
    const int longProfileStartBin(std::min(m_pSigLongProfileStart->GetNBinsX() - 1,
        static_cast<int>(longProfileStart / m_pSigLongProfileStart->GetXBinWidth())));
    const int longProfileDiscrepancyBin(std::min(m_pSigLongProfileDiscrepancy->GetNBinsX() - 1,
        static_cast<int>(longProfileDiscrepancy / m_pSigLongProfileDiscrepancy->GetXBinWidth())));
    const int peakEnergyFractionBin(std::min(m_pSigPeakEnergyFraction->GetNBinsX() - 1,
        static_cast<int>(peakEnergyFraction / m_pSigPeakEnergyFraction->GetXBinWidth())));
    const int minDistanceToTrackBin(std::min(m_pSigMinDistanceToTrack->GetNBinsX() - 1,
        static_cast<int>(minDistanceToTrack / m_pSigMinDistanceToTrack->GetXBinWidth())));

    float pid(0.f);

    const float yes(m_pSigPeakRms->GetBinContent(peakRmsBin) *
        m_pSigLongProfileStart->GetBinContent(longProfileStartBin) *
        m_pSigLongProfileDiscrepancy->GetBinContent(longProfileDiscrepancyBin) *
        m_pSigPeakEnergyFraction->GetBinContent(peakEnergyFractionBin) *
        m_pSigMinDistanceToTrack->GetBinContent(minDistanceToTrackBin));

    const float no(m_pBkgPeakRms->GetBinContent(peakRmsBin) *
        m_pBkgLongProfileStart->GetBinContent(longProfileStartBin) *
        m_pBkgLongProfileDiscrepancy->GetBinContent(longProfileDiscrepancyBin) *
        m_pBkgPeakEnergyFraction->GetBinContent(peakEnergyFractionBin) *
        m_pBkgMinDistanceToTrack->GetBinContent(minDistanceToTrackBin));

    if ((yes + no) > std::numeric_limits<float>::epsilon())
        pid = yes / (yes + no);

    return pid;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::FillPdfHistograms(const Cluster *const pPeakCluster, const float peakRms, const float longProfileStart,
    const float longProfileDiscrepancy, const float peakEnergyFraction, const float minDistanceToTrack) const
{
    const MCParticle *const pMCParticle(MCParticleHelper::GetMainMCParticle(pPeakCluster));

    if ((NULL != pMCParticle) && (PHOTON == pMCParticle->GetParticleId()))
    {
        m_pSigPeakRms->Fill(peakRms);
        m_pSigLongProfileStart->Fill(longProfileStart);
        m_pSigLongProfileDiscrepancy->Fill(longProfileDiscrepancy);
        m_pSigPeakEnergyFraction->Fill(peakEnergyFraction);
        m_pSigMinDistanceToTrack->Fill(minDistanceToTrack);
    }
    else
    {
        m_pBkgPeakRms->Fill(peakRms);
        m_pBkgLongProfileStart->Fill(longProfileStart);
        m_pBkgLongProfileDiscrepancy->Fill(longProfileDiscrepancy);
        m_pBkgPeakEnergyFraction->Fill(peakEnergyFraction);
        m_pBkgMinDistanceToTrack->Fill(minDistanceToTrack);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonReconstructionAlgorithm::TidyHistograms()
{
    if (m_shouldMakePdfHistograms)
    {
        // TODO protect against divide by zero here
        m_pSigPeakRms->Scale(1.f / m_pSigPeakRms->GetCumulativeSum());
        m_pBkgPeakRms->Scale(1.f / m_pBkgPeakRms->GetCumulativeSum());
        m_pSigLongProfileStart->Scale(1.f / m_pSigLongProfileStart->GetCumulativeSum());
        m_pBkgLongProfileStart->Scale(1.f / m_pBkgLongProfileStart->GetCumulativeSum());
        m_pSigLongProfileDiscrepancy->Scale(1.f / m_pSigLongProfileDiscrepancy->GetCumulativeSum());
        m_pBkgLongProfileDiscrepancy->Scale(1.f / m_pBkgLongProfileDiscrepancy->GetCumulativeSum());
        m_pSigPeakEnergyFraction->Scale(1.f / m_pSigPeakEnergyFraction->GetCumulativeSum());
        m_pBkgPeakEnergyFraction->Scale(1.f / m_pBkgPeakEnergyFraction->GetCumulativeSum());
        m_pSigMinDistanceToTrack->Scale(1.f / m_pSigMinDistanceToTrack->GetCumulativeSum());
        m_pBkgMinDistanceToTrack->Scale(1.f / m_pBkgMinDistanceToTrack->GetCumulativeSum());

        TiXmlDocument xmlDocument;
        m_pSigPeakRms->WriteToXml(&xmlDocument, "SigPeakRms");
        m_pBkgPeakRms->WriteToXml(&xmlDocument, "BkgPeakRms");
        m_pSigLongProfileStart->WriteToXml(&xmlDocument, "SigLongProfileStart");
        m_pBkgLongProfileStart->WriteToXml(&xmlDocument, "BkgLongProfileStart");
        m_pSigLongProfileDiscrepancy->WriteToXml(&xmlDocument, "SigLongProfileDiscrepancy");
        m_pBkgLongProfileDiscrepancy->WriteToXml(&xmlDocument, "BkgLongProfileDiscrepancy");
        m_pSigPeakEnergyFraction->WriteToXml(&xmlDocument, "SigPeakEnergyFraction");
        m_pBkgPeakEnergyFraction->WriteToXml(&xmlDocument, "BkgPeakEnergyFraction");
        m_pSigMinDistanceToTrack->WriteToXml(&xmlDocument, "SigMinDistanceToTrack");
        m_pBkgMinDistanceToTrack->WriteToXml(&xmlDocument, "BkgMinDistanceToTrack");
        xmlDocument.SaveFile(m_histogramFile);
    }

    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pSigPeakRms));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pBkgPeakRms));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pSigLongProfileStart));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pBkgLongProfileStart));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pSigLongProfileDiscrepancy));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pBkgLongProfileDiscrepancy));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pSigPeakEnergyFraction));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pBkgPeakEnergyFraction));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pSigMinDistanceToTrack));
    PANDORA_MONITORING_API(DrawPandoraHistogram(*m_pBkgMinDistanceToTrack));

    delete m_pSigPeakRms;
    delete m_pBkgPeakRms;
    delete m_pSigLongProfileStart;
    delete m_pBkgLongProfileStart;
    delete m_pSigLongProfileDiscrepancy;
    delete m_pBkgLongProfileDiscrepancy;
    delete m_pSigPeakEnergyFraction;
    delete m_pBkgPeakEnergyFraction;
    delete m_pSigMinDistanceToTrack;
    delete m_pBkgMinDistanceToTrack;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonReconstructionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "PhotonClusterFormation", m_photonClusteringAlgName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ClusterListName", m_clusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "HistogramFile", m_histogramFile));

    m_shouldMakePdfHistograms = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldMakePdfHistograms", m_shouldMakePdfHistograms));

    if (m_shouldMakePdfHistograms)
    {
        m_pSigPeakRms = new Histogram(100, 0., 5.);
        m_pBkgPeakRms = new Histogram(100, 0., 5.);
        m_pSigLongProfileStart = new Histogram(11, -0.5, 10.5);
        m_pBkgLongProfileStart = new Histogram(11, -0.5, 10.5);
        m_pSigLongProfileDiscrepancy = new Histogram(101, 0., 1.01);
        m_pBkgLongProfileDiscrepancy = new Histogram(101, 0., 1.01);
        m_pSigPeakEnergyFraction = new Histogram(101, 0., 1.01);
        m_pBkgPeakEnergyFraction = new Histogram(101, 0., 1.01);
        m_pSigMinDistanceToTrack = new Histogram(100, 0., 500.);
        m_pBkgMinDistanceToTrack = new Histogram(100, 0., 500.);
    }
    else
    {
        TiXmlDocument pdfXmlDocument(m_histogramFile);

        if (!pdfXmlDocument.LoadFile())
        {
            std::cout << "PhotonReconstructionAlgorithm::ReadSettings - Invalid xml file specified for pdf histograms." << std::endl;
            return STATUS_CODE_INVALID_PARAMETER;
        }

        const TiXmlHandle pdfXmlHandle(&pdfXmlDocument);
        m_pSigPeakRms = new Histogram(&pdfXmlHandle, "SigPeakRms");
        m_pBkgPeakRms = new Histogram(&pdfXmlHandle, "BkgPeakRms");
        m_pSigLongProfileStart = new Histogram(&pdfXmlHandle, "SigLongProfileStart");
        m_pBkgLongProfileStart = new Histogram(&pdfXmlHandle, "BkgLongProfileStart");
        m_pSigLongProfileDiscrepancy = new Histogram(&pdfXmlHandle, "SigLongProfileDiscrepancy");
        m_pBkgLongProfileDiscrepancy = new Histogram(&pdfXmlHandle, "BkgLongProfileDiscrepancy");
        m_pSigPeakEnergyFraction = new Histogram(&pdfXmlHandle, "SigPeakEnergyFraction");
        m_pBkgPeakEnergyFraction = new Histogram(&pdfXmlHandle, "BkgPeakEnergyFraction");
        m_pSigMinDistanceToTrack = new Histogram(&pdfXmlHandle, "SigMinDistanceToTrack");
        m_pBkgMinDistanceToTrack = new Histogram(&pdfXmlHandle, "BkgMinDistanceToTrack");
    }

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

    m_pidCut = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PidCut", m_pidCut));

    m_oldClusterEnergyFraction0 = 0.95f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OldClusterEnergyFraction0", m_oldClusterEnergyFraction0));

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
