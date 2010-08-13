/**
 *  @file   PandoraPFANew/src/Algorithms/Monitoring/DumpPfosMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the energy monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Algorithms/Monitoring/DumpPfosMonitoringAlgorithm.h"
#include "Pandora/AlgorithmHeaders.h"
#include "Pandora/PdgTable.h"

#include <vector>
#include <iostream>
#include <iomanip>

using namespace pandora;

const int precision = 2;
const int width = 8;
const int widthFloat = 7;
const int widthSmallFloat = 5;
const int widthInt = 5;
const int widthSmallInt = 2;
const int widthInt4 = 4;
const int widthFlag = 2;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::Run()
{
    m_trackMcPfoTargets.clear();
    m_trackToErrorTypeMap.clear();
    m_mcParticleToTrackMap.clear();
    m_trackRecoAsTrackEnergy = 0;
    m_trackRecoAsPhotonEnergy = 0;
    m_trackRecoAsNeutralEnergy = 0;
    m_photonRecoAsTrackEnergy = 0;
    m_photonRecoAsPhotonEnergy = 0;
    m_photonRecoAsNeutralEnergy = 0;
    m_neutralRecoAsTrackEnergy = 0;
    m_neutralRecoAsPhotonEnergy = 0;
    m_neutralRecoAsNeutralEnergy = 0;
    m_firstChargedPfoToPrint = true;
    m_firstNeutralPfoToPrint = true;
    m_firstPhotonPfoToPrint = true;

    std::cout << std::fixed;
    std::cout << std::setprecision(precision);

    const ParticleFlowObjectList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pPfoList));

    ParticleFlowObjectList chargedPfos;
    ParticleFlowObjectList photonPfos;
    ParticleFlowObjectList neutralHadronPfos;

    float totalPfoEnergy(0.);

    // First loop over pfos to make collections and save track mc pfo list
    for (ParticleFlowObjectList::const_iterator pfoIter = pPfoList->begin(); pfoIter != pPfoList->end(); ++pfoIter)
    {
        ParticleFlowObject *pPfo = *pfoIter;
        totalPfoEnergy += pPfo->GetEnergy();

        const int pfoPid(pPfo->GetParticleId());
        const TrackList &trackList(pPfo->GetTrackList());

        if (trackList.size() > 0)
            chargedPfos.insert(pPfo);

        if (trackList.size() == 0)
            (pfoPid == PHOTON) ? photonPfos.insert(pPfo) : neutralHadronPfos.insert(pPfo);

        for (TrackList::const_iterator trackIter = trackList.begin(); trackIter != trackList.end(); ++trackIter)
        {
            const Track *pTrack = *trackIter;

            const MCParticle* pMcParticle(NULL);
            pTrack->GetMCParticle(pMcParticle);

            if(pMcParticle == NULL)
                continue;

            m_trackMcPfoTargets.insert(pMcParticle);

            const TrackList &daughterTracks(pTrack->GetDaughterTrackList());

            if (!daughterTracks.empty())
                continue;

            MCParticleToTrackMap::iterator it = m_mcParticleToTrackMap.find(pMcParticle);

            if (it == m_mcParticleToTrackMap.end())
            {
                m_mcParticleToTrackMap.insert(MCParticleToTrackMap::value_type(pMcParticle,pTrack));
            }
            else
            {
                const TrackList siblingTracks = pTrack->GetSiblingTrackList();

                if (!siblingTracks.empty())
                    continue;

                switch(pMcParticle->GetParticleId())
                {
                case PHOTON :
                    m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(it->second, MISSED_CONVERSION));
                    m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(pTrack, MISSED_CONVERSION));
                    std::cout << " Track appears twice in list - conversion " << pMcParticle->GetEnergy() << std::endl;
                    break;

                case K_SHORT :
                    m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(it->second, MISSED_KSHORT));
                    m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(pTrack, MISSED_KSHORT));
                    std::cout << " Track appears twice in list - ks " << pMcParticle->GetEnergy() << std::endl;
                    break;

                default:
                    m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(it->second, SPLIT_TRACK));
                    m_trackToErrorTypeMap.insert(TrackToErrorTypeMap::value_type(pTrack, SPLIT_TRACK));
                    std::cout << " Track appears twice in list - split " << pMcParticle->GetEnergy() << std::endl;
                    break;
                }
            }
        }
    }

    if ((totalPfoEnergy > m_totalPfoEnergyDisplayLessThan) && (totalPfoEnergy < m_totalPfoEnergyDisplayGreaterThan))
        return STATUS_CODE_SUCCESS;


    std::cout << " <---------------------------------DumpPfosMonitoringAlgorithm----------------------------------------->" <<  std::endl;
    std::cout << " Total RECO PFO Energy = " << totalPfoEnergy << std::endl;

    // Now order the lists
    ParticleFlowObjectVector sortedChargedPfos(chargedPfos.begin(), chargedPfos.end());
    ParticleFlowObjectVector sortedPhotonPfos(photonPfos.begin(), photonPfos.end());
    ParticleFlowObjectVector sortedNeutralHadronPfos(neutralHadronPfos.begin(), neutralHadronPfos.end());
    std::sort(sortedChargedPfos.begin(), sortedChargedPfos.end(), ParticleFlowObject::SortByEnergy);
    std::sort(sortedPhotonPfos.begin(),  sortedPhotonPfos.end(), ParticleFlowObject::SortByEnergy);
    std::sort(sortedNeutralHadronPfos.begin(), sortedNeutralHadronPfos.end(), ParticleFlowObject::SortByEnergy);

    for (ParticleFlowObjectVector::const_iterator pfoIter = sortedChargedPfos.begin(); pfoIter != sortedChargedPfos.end(); ++pfoIter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DumpChargedPfo(*pfoIter));
    }

    for (ParticleFlowObjectVector::const_iterator pfoIter = sortedPhotonPfos.begin(); pfoIter != sortedPhotonPfos.end(); ++pfoIter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DumpPhotonPfo(*pfoIter));
    }

    for (ParticleFlowObjectVector::const_iterator pfoIter = sortedNeutralHadronPfos.begin(); pfoIter != sortedNeutralHadronPfos.end(); ++pfoIter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->DumpNeutralPfo(*pfoIter));
    }

    const float confmat[3][3] =
    {
        {m_trackRecoAsTrackEnergy, m_photonRecoAsTrackEnergy, m_neutralRecoAsTrackEnergy},
        {m_trackRecoAsPhotonEnergy, m_photonRecoAsPhotonEnergy, m_neutralRecoAsPhotonEnergy},
        {m_trackRecoAsNeutralEnergy, m_photonRecoAsNeutralEnergy, m_neutralRecoAsNeutralEnergy}
    };

    std::cout << std::endl;
    FORMATTED_OUTPUT_CONFUSION(confmat[0][0], confmat[0][1], confmat[0][2], confmat[1][0], confmat[1][1], confmat[1][2], confmat[2][0],
        confmat[2][1], confmat[2][2]);

    const float sumCal(m_trackRecoAsTrackEnergy + m_photonRecoAsTrackEnergy + m_neutralRecoAsTrackEnergy +
        m_trackRecoAsPhotonEnergy + m_photonRecoAsPhotonEnergy + m_neutralRecoAsPhotonEnergy + m_trackRecoAsNeutralEnergy +
        m_photonRecoAsNeutralEnergy + m_neutralRecoAsNeutralEnergy);

    const float nSumCal(sumCal / 100.f);

    if (0.f == nSumCal)
        return STATUS_CODE_FAILURE;

    const float econfmat[3][3] =
    {
        {m_trackRecoAsTrackEnergy / nSumCal, m_photonRecoAsTrackEnergy / nSumCal, m_neutralRecoAsTrackEnergy / nSumCal},
        {m_trackRecoAsPhotonEnergy / nSumCal, m_photonRecoAsPhotonEnergy / nSumCal, m_neutralRecoAsPhotonEnergy / nSumCal},
        {m_trackRecoAsNeutralEnergy / nSumCal, m_photonRecoAsNeutralEnergy / nSumCal, m_neutralRecoAsNeutralEnergy / nSumCal}
    };

    FORMATTED_OUTPUT_CONFUSION(econfmat[0][0], econfmat[0][1], econfmat[0][2], econfmat[1][0], econfmat[1][1], econfmat[1][2], econfmat[2][0],
        econfmat[2][1], econfmat[2][2]);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::DumpChargedPfo(const ParticleFlowObject* pPfo)
{
    const TrackList   &trackList(pPfo->GetTrackList());
    const int pfoPid    = pPfo->GetParticleId();
    const float pfoEnergy = pPfo->GetEnergy();
    bool printedHeader(false);
    bool printThisPfo(false);

    for (TrackList::const_iterator trackIter = trackList.begin(); trackIter != trackList.end(); ++trackIter)
    {
        Track *pTrack = *trackIter;
        TrackErrorTypes trackStatus = OK;

        TrackToErrorTypeMap::const_iterator it = m_trackToErrorTypeMap.find(pTrack);

        if (it != m_trackToErrorTypeMap.end())
            trackStatus = it->second;

        const float trackEnergy(pTrack->GetEnergyAtDca());
        const int trackId(pTrack->GetParticleId());

        float clusterEnergy(0);
        Cluster* pCluster(NULL);

        if (pTrack->HasAssociatedCluster())
        {
            pTrack->GetAssociatedCluster(pCluster);
            clusterEnergy += pCluster->GetTrackComparisonEnergy();
        }

        const MCParticle *pMcParticle(NULL);
        pTrack->GetMCParticle(pMcParticle);

        int mcId(0);
        float mcEnergy(0.);

        if (pMcParticle != NULL)
        {
            mcId = pMcParticle->GetParticleId();
            mcEnergy = pMcParticle->GetEnergy();
        }

        std::string leaving(" ");
        const bool isLeaving(pCluster!=NULL && ClusterHelper::IsClusterLeavingDetector(pCluster));

        if (isLeaving)
            leaving= "L";

        const float chi(ReclusterHelper::GetTrackClusterCompatibility(clusterEnergy, trackEnergy));

        const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());
        const bool isParent(!daughterTrackList.empty());
        const bool badChi(chi>m_minAbsChiToDisplay || (chi<-m_minAbsChiToDisplay && !isLeaving && !isParent));

        float fCharged(0.f);
        float fPhoton(0.f);
        float fNeutral(0.f);
        const MCParticle *pBestMcMatch(NULL);

        if (pCluster != NULL)
            this->DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(pCluster, fCharged, fPhoton, fNeutral, pBestMcMatch);

        // Fix for conversions (where both tracks are ided)
        if (trackStatus == MISSED_CONVERSION)
        {
            fCharged+= fPhoton;
            fPhoton  = 0;
        }

        m_trackRecoAsTrackEnergy += clusterEnergy * fCharged; 
        m_photonRecoAsTrackEnergy += clusterEnergy * fPhoton;
        m_neutralRecoAsTrackEnergy += clusterEnergy * fNeutral;

        const bool badConfusion(clusterEnergy*(fPhoton+fNeutral)>m_minConfusionEnergyToDisplay);

        // Decide whether to print
        if (pfoEnergy > m_minPfoEnergyToDisplay)
            printThisPfo = true;

        if (badConfusion || badChi)
            printThisPfo=true;

        if(printThisPfo)
        {
            if(m_firstChargedPfoToPrint)
            {
                // First loop to dump charged pfos
                std::cout << std::endl;
                FORMATTED_OUTPUT_PFO_HEADER_TITLE();
                FORMATTED_OUTPUT_TRACK_TITLE();
                m_firstChargedPfoToPrint = false;
            }

            if(!printedHeader)
            {
                FORMATTED_OUTPUT_PFO_HEADER(pfoPid, pfoEnergy);

                if (trackList.size() > 1)
                    std::cout << std::endl;

                printedHeader = true;
            }

            if(trackList.size() > 1)
            {
                FORMATTED_OUTPUT_PFO_HEADER("", "");
            }

            if (pCluster != NULL)
            {
                FORMATTED_OUTPUT_TRACK(trackId, mcId, pTrack->CanFormPfo(), pTrack->ReachesECal(),trackEnergy, mcEnergy, clusterEnergy, chi, leaving, fCharged, fPhoton, fNeutral);
            }
            else
            {
                FORMATTED_OUTPUT_TRACK(trackId, mcId, pTrack->CanFormPfo(), pTrack->ReachesECal(), trackEnergy, mcEnergy, " ", chi, " ", " ", " ", " ");
            }

            if (badChi)
            {
                std::cout << " <-- Bad E-P consistency : " << chi << std::endl;
                continue;
            }

            if(badConfusion)
            {
                std::cout << " <-- confusion : " << clusterEnergy*(fPhoton+fNeutral) << " GeV " << std::endl;
                continue;
            }

            if (trackStatus == SPLIT_TRACK)
            {
                std::cout << " <-- split track" << std::endl;
                continue;
            }

            if (trackStatus == MISSED_CONVERSION)
            {
                std::cout << " <-- missed conversion" << std::endl;
                continue;
            }

            if (trackStatus == MISSED_KSHORT)
            {
                std::cout << " <-- missed kshort" << std::endl;
                continue;
            }
            std::cout << std::endl;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::DumpNeutralOrPhotonPfo(const ParticleFlowObject *pPfo, bool isPhotonPfo)
{
    float fCharged(0.f);
    float fPhoton(0.f);
    float fNeutral(0.f);
    bool printThisPfo(false);
    bool printedHeader(false);

    const int pfoPid(pPfo->GetParticleId());
    const float pfoEnergy(pPfo->GetEnergy());
    const ClusterList &clusterList(pPfo->GetClusterList());

    for (ClusterList::const_iterator clusterIter = clusterList.begin(); clusterIter != clusterList.end(); ++clusterIter)
    {
        Cluster *pCluster = *clusterIter;
        const float clusterEnergy(pCluster->GetHadronicEnergy());

        const MCParticle *bestMcMatch(NULL);
        this->DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(pCluster, fCharged, fPhoton, fNeutral, bestMcMatch);

        if (isPhotonPfo)
        {
            m_trackRecoAsPhotonEnergy += clusterEnergy * fCharged;
            m_photonRecoAsPhotonEnergy += clusterEnergy * fPhoton;
            m_neutralRecoAsPhotonEnergy += clusterEnergy * fNeutral;
        }
        else
        {
            m_trackRecoAsNeutralEnergy += clusterEnergy * fCharged;
            m_photonRecoAsNeutralEnergy += clusterEnergy * fPhoton;
            m_neutralRecoAsNeutralEnergy += clusterEnergy * fNeutral;
        }

        float showerProfileStart(0.);
        float showerProfileDiscrepancy(0.);
        (void) ParticleIdHelper::CalculateShowerProfile(pCluster, showerProfileStart, showerProfileDiscrepancy);

        const bool badConfusion((clusterEnergy * fCharged) > m_minConfusionEnergyToDisplay);
        const bool badFragment((fCharged > 0.8f) && ((clusterEnergy * fCharged) > m_fragmentEnergyToDisplay));
        bool badTrackMatch((fCharged > 0.95f) && ((clusterEnergy * fCharged) >m_fragmentEnergyToDisplay));

        if (badTrackMatch)
        {
            MCParticleToTrackMap::iterator it = m_mcParticleToTrackMap.find(bestMcMatch);

            if (it != m_mcParticleToTrackMap.end())
            {
                if (it->second->HasAssociatedCluster())
                    badTrackMatch = false;
            }
        }

        const bool badPhotonId( (!isPhotonPfo && (fPhoton > 0.8f) && ((clusterEnergy * fPhoton)  > m_photonIdEnergyToDisplay))
				||( isPhotonPfo && (fNeutral > 0.8f) && ((clusterEnergy * fNeutral)> m_photonIdEnergyToDisplay)) );



        if (pfoEnergy > m_minPfoEnergyToDisplay)
            printThisPfo = true;

        if (badConfusion || badFragment || badPhotonId)
            printThisPfo = true;

        if (printThisPfo)
        {
            if (m_firstNeutralPfoToPrint)
            {
                // First loop to dump charged pfos
                std::cout << std::endl;
                FORMATTED_OUTPUT_PFO_HEADER_TITLE();
                FORMATTED_OUTPUT_NEUTRAL_TITLE();
                m_firstNeutralPfoToPrint = false;
            }

            if (!printedHeader)
            {
                FORMATTED_OUTPUT_PFO_HEADER(pfoPid, pfoEnergy);
                printedHeader = true;
            }

            if (pCluster != NULL)
            {
                FORMATTED_OUTPUT_NEUTRAL(clusterEnergy, fCharged, fPhoton, fNeutral, pCluster->GetInnerPseudoLayer(),
                    pCluster->GetOuterPseudoLayer(), showerProfileStart, showerProfileDiscrepancy);
            }

            if (badTrackMatch)
            {
                std::cout << " <-- bad track match  : " << clusterEnergy * fCharged << " GeV ";

                if (bestMcMatch != NULL)
                    std::cout << "(" << bestMcMatch->GetEnergy() << " ) ";

                std::cout << std::endl;
                continue;
            }

            if (badFragment)
            {
                std::cout << " <-- fragment  : " << clusterEnergy * fCharged << " GeV ";

                if (bestMcMatch != NULL)
                    std::cout << "(" << bestMcMatch->GetEnergy() << " ) ";

                std::cout << std::endl;
                continue;
            }

            if (badConfusion)
            {
                std::cout << " <-- confusion : " << clusterEnergy * fCharged << " GeV " << std::endl;
                continue;
            }

            if (badPhotonId)
            {
                if (isPhotonPfo)
                {
                    std::cout << " <-- neutral hadron : " << clusterEnergy * fNeutral << " GeV " << std::endl;
                }
                else
                {
                    std::cout << " <-- photon    : " << clusterEnergy * fPhoton << " GeV " << std::endl;
                }

                continue;
            }

            std::cout << std::endl;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(const Cluster* pCluster, float &fCharged, float &fPhoton, float &fNeutral,
    const MCParticle* &pMcBest) const
{
    pMcBest = NULL;
    float totEnergy(0.f);
    float neutralEnergy(0.f);
    float photonEnergy(0.f);
    float chargedEnergy(0.f);

    MCParticleToFloatMap mcParticleContributions;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const MCParticle *pMCParticle(NULL);
            pCaloHit->GetMCParticle(pMCParticle);

            if (pMCParticle == NULL)
                continue;

            const MCParticle *pMCPfoTarget(NULL);
            pMCParticle->GetPfoTarget(pMCPfoTarget);

            if (pMCPfoTarget == NULL)
                continue;

            totEnergy += pCaloHit->GetHadronicEnergy();
            MCParticleToFloatMap::iterator it = mcParticleContributions.find(pMCPfoTarget);

            if (it != mcParticleContributions.end())
            {
                it->second += pCaloHit->GetHadronicEnergy();
            }
            else
            {
                mcParticleContributions.insert(MCParticleToFloatMap::value_type(pMCPfoTarget, pCaloHit->GetHadronicEnergy()));
            }

            const int pdgCode(pMCPfoTarget->GetParticleId());

            try
            {
                const int charge(PdgTable::GetParticleCharge(pdgCode));

                if ((charge != 0) || (std::abs(pdgCode) == LAMBDA) || (std::abs(pdgCode) == K_SHORT))
                {
                    if (m_trackMcPfoTargets.count(pMCParticle) == 0)
                    {
                        neutralEnergy += pCaloHit->GetHadronicEnergy();
                    }
                    else
                    {
                        chargedEnergy += pCaloHit->GetHadronicEnergy();
                    }
                }
                else
                {
                    (pMCParticle->GetParticleId() == PHOTON) ? photonEnergy += pCaloHit->GetHadronicEnergy() : neutralEnergy += pCaloHit->GetHadronicEnergy();
                }
            }
            catch (StatusCodeException &statusCodeException)
            {
                std::cout << "Failed to get charge  " << pdgCode << std::endl;
            }
        }
    }

    if (totEnergy > 0.f)
    {
        fCharged = chargedEnergy/totEnergy;
        fPhoton  = photonEnergy/totEnergy;
        fNeutral = neutralEnergy/totEnergy;
    }

    // Find mc particle with largest associated energy
    float maximumEnergy(0.f);

    for (MCParticleToFloatMap::const_iterator iter = mcParticleContributions.begin(), iterEnd = mcParticleContributions.end(); iter != iterEnd; ++iter)
    {
        if (iter->second > maximumEnergy)
        {
            maximumEnergy = iter->second;
            pMcBest = iter->first;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_minPfoEnergyToDisplay = 0.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinPfoEnergyToDisplay", m_minPfoEnergyToDisplay));

    m_minAbsChiToDisplay = 3.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinAbsChiToDisplay", m_minAbsChiToDisplay));

    m_minConfusionEnergyToDisplay = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinConfusionEnergyToDisplay", m_minConfusionEnergyToDisplay));

    m_minFragmentEnergyToDisplay = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinFragmentEnergyToDisplay", m_minFragmentEnergyToDisplay));

    m_totalPfoEnergyDisplayLessThan = 1000000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TotalPfoEnergyDisplayLessThan", m_totalPfoEnergyDisplayLessThan));
 
    m_totalPfoEnergyDisplayGreaterThan = 0.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TotalPfoEnergyDisplayGreaterThan", m_totalPfoEnergyDisplayGreaterThan));
 
    m_fragmentEnergyToDisplay = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FragmentEnergyToDisplay", m_fragmentEnergyToDisplay));
 
    m_photonIdEnergyToDisplay = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdEnergyToDisplay", m_photonIdEnergyToDisplay));

    return STATUS_CODE_SUCCESS;
}
