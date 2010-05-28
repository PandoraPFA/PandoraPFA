/**
 *  @file   PandoraPFANew/src/Algorithms/Monitoring/VisualMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the energy monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Algorithms/Monitoring/VisualMonitoringAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Helpers/CaloHitHelper.h"

using namespace pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode VisualMonitoringAlgorithm::Run()
{
    if( m_hits )
    {
        const OrderedCaloHitList *pOrderedCaloHitList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));
        
        OrderedCaloHitList caloHitList( *pOrderedCaloHitList ); 
        if( m_onlyAvailable )
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::RemoveUnavailableCaloHits(caloHitList));
        }

        if( m_eve )
        {
            PANDORA_MONITORING_API(VisualizeCaloHits(&caloHitList, "currentHits", GRAY  ) );
        }
        else
            if( m_detectorView == "XZ" )
            {
                PANDORA_MONITORING_API(AddCaloHitList(DETECTOR_VIEW_XZ,&caloHitList, GRAY  ) );
            }
            else
            {
                PANDORA_MONITORING_API(AddCaloHitList(DETECTOR_VIEW_XY,&caloHitList, GRAY  ) );
            }
    }

    if( m_tracks )
    {
        const TrackList *pTrackList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

        
        TrackList trackList; 
        for( TrackList::iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack )
        {
            Track* pTrack = (*itTrack);
            if( !(pTrack->HasAssociatedCluster() && m_onlyAvailable ) )
                trackList.insert(pTrack);
        }
        
        if( m_eve )
        {
            PANDORA_MONITORING_API(VisualizeTracks(&trackList, "currentTracks", GRAY  ) );
        }
        else
            if( m_detectorView == "XZ" )
            {
                PANDORA_MONITORING_API(AddTrackList(DETECTOR_VIEW_XZ,&trackList, GRAY  ) );
            }
            else
            {
                PANDORA_MONITORING_API(AddTrackList(DETECTOR_VIEW_XY,&trackList, GRAY  ) );
            }
    }

    for( pandora::StringVector::iterator itClusterName = m_clusterListNames.begin(), itClusterNameEnd = m_clusterListNames.end(); itClusterName != itClusterNameEnd; ++itClusterName )
    {
        const ClusterList* pClusterList = NULL;
        std::string clusterListName = (*itClusterName);
        std::cout << "visualize: " << clusterListName << std::flush;
        if( STATUS_CODE_SUCCESS == PandoraContentApi::GetClusterList(*this, clusterListName, pClusterList))
        {
            if( m_eve )
            {
                PANDORA_MONITORING_API(VisualizeClusters(pClusterList, clusterListName, AUTO  ) );
            }
            else
                if( m_detectorView == "XZ" )
                {
                    PANDORA_MONITORING_API(AddClusterList(DETECTOR_VIEW_XZ,pClusterList, AUTO  ) );
                }
                else
                {
                    PANDORA_MONITORING_API(AddClusterList(DETECTOR_VIEW_XY,pClusterList, AUTO  ) );
                }
            std::cout << " ... done" << std::endl;
        }
        else
            std::cout << " ... cluster-list not found" << std::endl;
    }

    // show current clusters
    if(m_clusters)
    {
        const ClusterList* pClusterList = NULL;
        if( STATUS_CODE_SUCCESS == PandoraContentApi::GetCurrentClusterList(*this, pClusterList))
            if( m_eve )
            {
                PANDORA_MONITORING_API(VisualizeClusters(pClusterList, "current", AUTO  ) );
            }
            else
                if( m_detectorView == "XZ" )
                {
                    PANDORA_MONITORING_API(AddClusterList(DETECTOR_VIEW_XZ,pClusterList, AUTO  ) );
                }
                else
                {
                    PANDORA_MONITORING_API(AddClusterList(DETECTOR_VIEW_XY,pClusterList, AUTO  ) );
                }
    }

    if( m_show )
    {
        if( m_eve )
        {
            PANDORA_MONITORING_API(View() );
        }
        else
        {
            PANDORA_MONITORING_API(ViewEvent() );
        }   
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode VisualMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "ClusterListNames", m_clusterListNames));

    m_eve = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "UseROOTEve", m_eve));

    m_clusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowCurrentClusters", m_clusters));

    m_hits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowCurrentCaloHits", m_hits));

    m_tracks = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowCurrentTracks", m_tracks));

    m_onlyAvailable = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowOnlyAvailable", m_onlyAvailable));
    

    m_detectorView = "XZ";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "DetectorView", m_detectorView));

    m_show = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "Show", m_show));

    return STATUS_CODE_SUCCESS;
}
