/**
 *  @file   PandoraPFANew/src/Algorithms/Monitoring/VisualMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the energy monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Algorithms/Monitoring/VisualMonitoringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode VisualMonitoringAlgorithm::Run()
{
    // Show current ordered calo hit list
    OrderedCaloHitList orderedCaloHitList;

    if (m_hits)
    {
        const OrderedCaloHitList *pOrderedCaloHitList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

        orderedCaloHitList = (*pOrderedCaloHitList);

        if (m_onlyAvailable)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::RemoveUnavailableCaloHits(orderedCaloHitList));
        }

        PANDORA_MONITORING_API(VisualizeCaloHits(&orderedCaloHitList, "currentHits", GRAY));
    }

    // Show current tracks
    TrackList trackList;

    if (m_tracks)
    {
        const TrackList *pTrackList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

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

    // Show specified lists of clusters
    for (pandora::StringVector::iterator itClusterName = m_clusterListNames.begin(), itClusterNameEnd = m_clusterListNames.end();
        itClusterName != itClusterNameEnd; ++itClusterName)
    {
        const ClusterList* pClusterList = NULL;
        std::string clusterListName = (*itClusterName);

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetClusterList(*this, clusterListName, pClusterList))
        {
            PANDORA_MONITORING_API(VisualizeClusters(pClusterList, clusterListName, AUTO));
        }
        else
        {
            std::cout << "VisualMonitoringAlgorithm: cluster-list " << clusterListName << " not found." << std::endl;
        }
    }

    // Show current clusters
    if (m_clusters)
    {
        const ClusterList* pClusterList = NULL;

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetCurrentClusterList(*this, pClusterList))
        {
            PANDORA_MONITORING_API(VisualizeClusters(pClusterList, "currentClusters", AUTO  ) );
        }
    }

    // Show current particle flow objects
    if (m_particleFlowObjects)
    {
        const ParticleFlowObjectList* pPfoList = NULL;

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetCurrentPfoList(*this, pPfoList))
        {
            PANDORA_MONITORING_API(VisualizeParticleFlowObjects(pPfoList, "currentPfos", AUTO  ) );
        }
    }

    if (m_displayEvent)
    {
        PANDORA_MONITORING_API(ViewEvent() );
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode VisualMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "ClusterListNames", m_clusterListNames));

    m_particleFlowObjects = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowCurrentPfos", m_particleFlowObjects));

    m_clusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowCurrentClusters", m_clusters));

    m_hits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowCurrentCaloHits", m_hits));

    m_tracks = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowCurrentTracks", m_tracks));

    m_onlyAvailable = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowOnlyAvailable", m_onlyAvailable));

    m_displayEvent = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "DisplayEvent", m_displayEvent));

    return STATUS_CODE_SUCCESS;
}
