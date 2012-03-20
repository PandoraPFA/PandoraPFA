/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Monitoring/VisualMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the visual monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Monitoring/VisualMonitoringAlgorithm.h"

#include <algorithm>
#include <string>

using namespace pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode VisualMonitoringAlgorithm::Run()
{
    PANDORA_MONITORING_API(SetEveDisplayParameters(m_blackBackground, m_showDetector, m_transparencyThresholdE, m_energyScaleThresholdE));

    // Show mc particles
    if (m_showMCParticles)
    {
        this->VisualizeMCParticleList();
    }

    // Show current calo hit list
    if (m_showCurrentCaloHits)
    {
        this->VisualizeCaloHitList(std::string());
    }

    // Show specified lists of calo hits
    for (StringVector::const_iterator iter = m_caloHitListNames.begin(), iterEnd = m_caloHitListNames.end(); iter != iterEnd; ++iter)
    {
        this->VisualizeCaloHitList(*iter);
    }

    // Show current cluster list
    if (m_showCurrentClusters)
    {
        this->VisualizeClusterList(std::string());
    }

    // Show specified lists of clusters
    for (StringVector::const_iterator iter = m_clusterListNames.begin(), iterEnd = m_clusterListNames.end(); iter != iterEnd; ++iter)
    {
        this->VisualizeClusterList(*iter);
    }

    // Show current track list
    if (m_showCurrentTracks)
    {
        this->VisualizeTrackList(std::string());
    }

    // Show specified lists of tracks
    for (StringVector::const_iterator iter = m_trackListNames.begin(), iterEnd = m_trackListNames.end(); iter != iterEnd; ++iter)
    {
        this->VisualizeTrackList(*iter);
    }

    // Show current particle flow objects
    if (m_showCurrentPfos)
    {
        this->VisualizeParticleFlowList(std::string());
    }

    // Show specified lists of pfo
    for (StringVector::const_iterator iter = m_pfoListNames.begin(), iterEnd = m_pfoListNames.end(); iter != iterEnd; ++iter)
    {
        this->VisualizeParticleFlowList(*iter);
    }

    // Finally, display the event and pause application
    if (m_displayEvent)
    {
        PANDORA_MONITORING_API(ViewEvent());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeMCParticleList() const
{
    MCParticleList mcParticleList;

    if (STATUS_CODE_SUCCESS != PandoraContentApi::GetMCParticleList(*this, mcParticleList))
    {
        std::cout << "VisualMonitoringAlgorithm: mc particle list unavailable." << std::endl;
        return;
    }

    PANDORA_MONITORING_API(VisualizeMCParticles(&mcParticleList, "currentMCParticles", AUTO, &m_particleSuppressionMap));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeCaloHitList(const std::string &listName) const
{
    const CaloHitList *pCaloHitList = NULL;

    if (listName.empty())
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentCaloHitList(*this, pCaloHitList))
        {
            std::cout << "VisualMonitoringAlgorithm: current calo hit list unavailable." << std::endl;
            return;
        }
    }
    else
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetCaloHitList(*this, listName, pCaloHitList))
        {
            std::cout << "VisualMonitoringAlgorithm: calo hit list " << listName << " unavailable." << std::endl;
            return;
        }
    }

    CaloHitList caloHitList(*pCaloHitList);

    // Filter calo hit list
    for (CaloHitList::const_iterator hitIter = caloHitList.begin(), hitIterEnd = caloHitList.end(); hitIter != hitIterEnd; )
    {
        if (((*hitIter)->GetElectromagneticEnergy() < m_thresholdEnergy))
        {
            caloHitList.erase(hitIter++);
        }
        else if (m_showOnlyAvailable && !PandoraContentApi::IsCaloHitAvailable(*this, *hitIter))
        {
            caloHitList.erase(hitIter++);
        }
        else
        {
            hitIter++;
        }
    }

    PANDORA_MONITORING_API(VisualizeCaloHits(&caloHitList, listName.empty() ? "currentCaloHits" : listName.c_str(),
        (m_hitColors.find("energy") != std::string::npos ? AUTOENERGY : GRAY)));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeTrackList(const std::string &listName) const
{
    const TrackList *pTrackList = NULL;

    if (listName.empty())
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentTrackList(*this, pTrackList))
        {
            std::cout << "VisualMonitoringAlgorithm: current track list unavailable." << std::endl;
            return;
        }
    }
    else
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetTrackList(*this, listName, pTrackList))
        {
            std::cout << "VisualMonitoringAlgorithm: track list " << listName << " unavailable." << std::endl;
            return;
        }
    }

    // Filter track list
    TrackList trackList;

    for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        if (!m_showOnlyAvailable || !pTrack->HasAssociatedCluster())
        {
            trackList.insert(pTrack);
        }
    }

    PANDORA_MONITORING_API(VisualizeTracks(&trackList, listName.empty() ? "currentTracks" : listName.c_str(), GRAY));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeClusterList(const std::string &listName) const
{
    const ClusterList *pClusterList = NULL;

    if (listName.empty())
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentClusterList(*this, pClusterList))
        {
            std::cout << "VisualMonitoringAlgorithm: current cluster list unavailable." << std::endl;
            return;
        }
    }
    else
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, listName, pClusterList))
        {
            std::cout << "VisualMonitoringAlgorithm: cluster list " << listName << " unavailable." << std::endl;
            return;
        }
    }

    PANDORA_MONITORING_API(VisualizeClusters(pClusterList, listName.empty() ? "currentClusters" : listName.c_str(),
        (m_hitColors.find("particleid") != std::string::npos) ? AUTOID :
        (m_hitColors.find("particletype") != std::string::npos) ? AUTOTYPE :
        (m_hitColors.find("iterate") != std::string::npos ? AUTOITER :
        (m_hitColors.find("energy") != std::string::npos ? AUTOENERGY :
        AUTO))));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeParticleFlowList(const std::string &listName) const
{
    const PfoList *pPfoList = NULL;

    if (listName.empty())
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentPfoList(*this, pPfoList))
        {
            std::cout << "VisualMonitoringAlgorithm: current pfo list unavailable." << std::endl;
            return;
        }
    }
    else
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetPfoList(*this, listName, pPfoList))
        {
            std::cout << "VisualMonitoringAlgorithm: pfo list " << listName << " unavailable." << std::endl;
            return;
        }
    }

    PANDORA_MONITORING_API(VisualizeParticleFlowObjects(pPfoList, listName.empty() ? "currentPfos" : listName.c_str(),
        (m_hitColors.find("particleid") != std::string::npos) ? AUTOID :
        (m_hitColors.find("particletype") != std::string::npos) ? AUTOTYPE :
        (m_hitColors.find("iterate") != std::string::npos ? AUTOITER :
        (m_hitColors.find("energy") != std::string::npos ? AUTOENERGY :
        AUTO))));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode VisualMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_showMCParticles = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowMCParticles", m_showMCParticles));

    m_showCurrentCaloHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentCaloHits", m_showCurrentCaloHits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CaloHitListNames", m_caloHitListNames));

    m_showCurrentTracks = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentTracks", m_showCurrentTracks));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "TrackListNames", m_trackListNames));

    m_showCurrentClusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentClusters", m_showCurrentClusters));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames));

    m_showCurrentPfos = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentPfos", m_showCurrentPfos));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "PfoListNames", m_pfoListNames));

    m_hitColors = "pfo";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HitColors", m_hitColors));
    std::transform(m_hitColors.begin(), m_hitColors.end(), m_hitColors.begin(), ::tolower);

    m_thresholdEnergy = -1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ThresholdEnergy", m_thresholdEnergy));

    m_showOnlyAvailable = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowOnlyAvailable", m_showOnlyAvailable));

    m_displayEvent = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DisplayEvent", m_displayEvent));

    m_transparencyThresholdE = -1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TransparencyThresholdE", m_transparencyThresholdE));

    m_energyScaleThresholdE = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyScaleThresholdE", m_energyScaleThresholdE));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "SuppressMCParticles", m_suppressMCParticles));

    for (StringVector::iterator iter = m_suppressMCParticles.begin(), iterEnd = m_suppressMCParticles.end(); iter != iterEnd; ++iter)
    {
        const std::string pdgEnergy(*iter);
        StringVector pdgEnergySeparated;
        const std::string delimiter = ":";
        XmlHelper::TokenizeString(pdgEnergy, pdgEnergySeparated, delimiter);

        if (pdgEnergySeparated.size() != 2)
            return STATUS_CODE_INVALID_PARAMETER;

        int pdgCode(0);
        float energy(0.f);

        if (!StringToType(pdgEnergySeparated.at(0), pdgCode) || !StringToType(pdgEnergySeparated.at(1), energy))
            return STATUS_CODE_INVALID_PARAMETER;

        m_particleSuppressionMap.insert(PdgCodeToEnergyMap::value_type(pdgCode, energy));
    }

    m_blackBackground = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "BlackBackground", m_blackBackground));

    m_showDetector = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowDetector", m_showDetector));

    return STATUS_CODE_SUCCESS;
}
