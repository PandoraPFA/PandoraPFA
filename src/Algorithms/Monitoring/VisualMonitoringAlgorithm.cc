/**
 *  @file   PandoraPFANew/src/Algorithms/Monitoring/VisualMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the visual monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Algorithms/Monitoring/VisualMonitoringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode VisualMonitoringAlgorithm::Run()
{
    // Show mc particles
    if (m_mcParticles)
        VisualizeMCParticleList();

    // Show current ordered calo hit list
    if (m_hits)
    {
        std::string orderedCaloHitListName;
        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetCurrentOrderedCaloHitListName(*this, orderedCaloHitListName))
            VisualizeOrderedCaloHitsList(orderedCaloHitListName);
        else
            std::cout << "VisualMonitoringAlgorithm/ No current ordered calohitlist with name '" << orderedCaloHitListName << "' found." << std::endl;
        
    }

    if( !m_inputCaloHitListNames.empty() )
        for( StringVector::iterator itInpCaloHitListName = m_inputCaloHitListNames.begin(), itInpCaloHitListNameEnd = m_inputCaloHitListNames.end(); 
             itInpCaloHitListName != itInpCaloHitListNameEnd; ++itInpCaloHitListName )
        {
            std::string inputCaloHitListName = (*itInpCaloHitListName);
            VisualizeOrderedCaloHitsList(inputCaloHitListName);
        }



    // Show current tracks
    if (m_tracks)
        VisualizeCurrentTrackList();

    // Show specified lists of clusters
    for (pandora::StringVector::iterator itClusterListName = m_clusterListNames.begin(), itClusterListNameEnd = m_clusterListNames.end();
        itClusterListName != itClusterListNameEnd; ++itClusterListName)
        VisualizeClusterList( (*itClusterListName) );

    // Show current clusters
    if (m_clusters)
        VisualizeClusterList("");

    // Show current particle flow objects
    if (m_particleFlowObjects)
        VisualizeCurrentParticleFlowList();

    if (m_displayEvent)
    {
        PANDORA_MONITORING_API(ViewEvent() );
    }

    return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeMCParticleList()
{
    MCParticleList mcParticleList;

    if (STATUS_CODE_SUCCESS != PandoraContentApi::GetMCParticleList(*this, mcParticleList))
        return;
    PANDORA_MONITORING_API(VisualizeMCParticles(&mcParticleList, "MCParticles", AUTO, &m_particleSuppressionMap));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeOrderedCaloHitsList(std::string caloHitListName)
{
    OrderedCaloHitList orderedCaloHitList;

    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    if (STATUS_CODE_SUCCESS != PandoraContentApi::GetOrderedCaloHitList(*this, caloHitListName, pOrderedCaloHitList))
        return;

    orderedCaloHitList = (*pOrderedCaloHitList);

    if (m_onlyAvailable)
    {
        if (STATUS_CODE_SUCCESS != CaloHitHelper::RemoveUnavailableCaloHits(orderedCaloHitList))
            return;
    }

    PANDORA_MONITORING_API(VisualizeCaloHits(&orderedCaloHitList, caloHitListName, GRAY));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeCurrentTrackList()
{
    TrackList trackList;

    const TrackList *pTrackList = NULL;
    if (STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentTrackList(*this, pTrackList))
        return;

    for (TrackList::const_iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack)
    {
        Track* pTrack = (*itTrack);

        if (!(pTrack->HasAssociatedCluster() && m_onlyAvailable))
        {
            trackList.insert(pTrack);
        }
    }

    PANDORA_MONITORING_API(VisualizeTracks(&trackList, "currentTracks", GRAY));
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeClusterList(std::string clusterListName)
{
    const ClusterList* pClusterList = NULL;

    if (clusterListName.empty())
    {
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetCurrentClusterList(*this, pClusterList))
        {
            std::cout << "VisualMonitoringAlgorithm: current cluster-list not found." << std::endl;
            return;
        }
        clusterListName = "currentClusters";
    }
    else
        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, clusterListName, pClusterList))
        {
            std::cout << "VisualMonitoringAlgorithm: cluster-list " << clusterListName << " not found." << std::endl;
            return;
        }

   PANDORA_MONITORING_API(VisualizeClusters(pClusterList, clusterListName, AUTO  ) );
}

//------------------------------------------------------------------------------------------------------------------------------------------

void VisualMonitoringAlgorithm::VisualizeCurrentParticleFlowList()
{
    const ParticleFlowObjectList* pPfoList = NULL;

    if (STATUS_CODE_SUCCESS == PandoraContentApi::GetCurrentPfoList(*this, pPfoList))
    {
        PANDORA_MONITORING_API(VisualizeParticleFlowObjects(pPfoList, "currentPfos", AUTO  ) );
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode VisualMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames));

    m_mcParticles = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowMCParticles", m_mcParticles));

    m_particleFlowObjects = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentPfos", m_particleFlowObjects));

    m_clusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentClusters", m_clusters));

    m_hits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentCaloHits", m_hits));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CaloHitListNames", m_inputCaloHitListNames));

    m_tracks = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentTracks", m_tracks));

    m_onlyAvailable = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowOnlyAvailable", m_onlyAvailable));

    m_displayEvent = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DisplayEvent", m_displayEvent));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "SuppressMCParticles", m_suppressMCParticles));

    for (StringVector::iterator it = m_suppressMCParticles.begin(), itEnd = m_suppressMCParticles.end(); it != itEnd; ++it )
    {
        std::string pdgEnergy = (*it);
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

    return STATUS_CODE_SUCCESS;
}
