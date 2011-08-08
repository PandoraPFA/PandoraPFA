/**
 *  @file   PandoraPFANew/Framework/src/Api/PandoraContentApiImpl.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Pandora/Algorithm.h"

#include "Api/PandoraContentApiImpl.h"

#include "Helpers/ReclusterHelper.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/TrackManager.h"
#include "Managers/ParticleFlowObjectManager.h"

#include "Objects/Cluster.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraSettings.h"

namespace pandora
{

template <typename CLUSTER_PARAMETERS>
StatusCode PandoraContentApiImpl::CreateCluster(CLUSTER_PARAMETERS *pClusterParameters, Cluster *&pCluster) const
{
    return STATUS_CODE_FAILURE;
}

template <>
StatusCode PandoraContentApiImpl::CreateCluster(CaloHit *pCaloHit, Cluster *&pCluster) const
{
    if (!m_pPandora->m_pCaloHitManager->IsCaloHitAvailable(pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateCluster(pCaloHit, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::CreateCluster(CaloHitList *pCaloHitList, Cluster *&pCluster) const
{
    if (!m_pPandora->m_pCaloHitManager->AreCaloHitsAvailable(*pCaloHitList))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateCluster(pCaloHitList, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(*pCaloHitList, false));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::CreateCluster(Track *pTrack, Cluster *&pCluster) const
{
    return m_pPandora->m_pClusterManager->CreateCluster(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters) const
{
    const TrackList &trackList(particleFlowObjectParameters.m_trackList);
    const ClusterList &clusterList(particleFlowObjectParameters.m_clusterList);

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        if (!(*iter)->IsAvailable())
            return STATUS_CODE_NOT_ALLOWED;
    }

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        if (!(*iter)->IsAvailable())
            return STATUS_CODE_NOT_ALLOWED;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pParticleFlowObjectManager->CreateParticleFlowObject(particleFlowObjectParameters));

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
        (*iter)->SetAvailability(false);

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
        (*iter)->SetAvailability(false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateDaughterAlgorithm(TiXmlElement *const pXmlElement, std::string &daughterAlgorithmName) const
{
    return m_pPandora->m_pAlgorithmManager->CreateAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RunAlgorithm(const std::string &algorithmName) const
{
    AlgorithmManager::AlgorithmMap::const_iterator iter = m_pPandora->m_pAlgorithmManager->m_algorithmMap.find(algorithmName);

    if (m_pPandora->m_pAlgorithmManager->m_algorithmMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->RegisterAlgorithm(iter->second));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RegisterAlgorithm(iter->second));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RegisterAlgorithm(iter->second));

    try
    {
        static const bool shouldDisplayAlgorithmInfo(PandoraSettings::ShouldDisplayAlgorithmInfo());

        if (shouldDisplayAlgorithmInfo)
        {
            for (unsigned int i = 1; i < m_pPandora->m_pCaloHitManager->m_algorithmInfoMap.size(); ++i)
                std::cout << "----";

            std::cout << "> Running Algorithm: " << iter->first << ", " << iter->second->GetAlgorithmType() << std::endl;
        }

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->Run());
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetAlgorithmType() << ", " << statusCodeException.ToString()
                  << statusCodeException.GetBackTrace() << std::endl;
    }
    catch (...)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetAlgorithmType() << ", unrecognized exception" << std::endl;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAlgorithmInfo(iter->second, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAlgorithmInfo(iter->second, true));

    ClusterList clustersToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetClustersToBeDeleted(iter->second, clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareClustersForDeletion(clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(iter->second, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RepeatEventPreparation() const
{
    return m_pPandora->PrepareEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentClusterList(const ClusterList *&pClusterList, std::string &clusterListName) const
{
    return m_pPandora->m_pClusterManager->GetCurrentList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentClusterListName(std::string &clusterListName) const
{
    return m_pPandora->m_pClusterManager->GetCurrentListName(clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetClusterList(const std::string &clusterListName, const ClusterList *&pClusterList) const
{
    return m_pPandora->m_pClusterManager->GetList(clusterListName, pClusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DropCurrentClusterList() const
{
    return m_pPandora->m_pClusterManager->DropCurrentList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentCaloHitList(const CaloHitList *&pCaloHitList, std::string &caloHitListName) const
{
    return m_pPandora->m_pCaloHitManager->GetCurrentList(pCaloHitList, caloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentCaloHitListName(std::string &caloHitListName) const
{
    return m_pPandora->m_pCaloHitManager->GetCurrentListName(caloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCaloHitList(const std::string &caloHitListName, const CaloHitList *&pCaloHitList) const
{
    return m_pPandora->m_pCaloHitManager->GetList(caloHitListName, pCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DropCurrentCaloHitList() const
{
    return m_pPandora->m_pCaloHitManager->DropCurrentList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentTrackList(const TrackList *&pTrackList, std::string &trackListName) const
{
    return m_pPandora->m_pTrackManager->GetCurrentList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentTrackListName(std::string &trackListName) const
{
    return m_pPandora->m_pTrackManager->GetCurrentListName(trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetTrackList(const std::string &trackListName, const TrackList *&pTrackList) const
{
    return m_pPandora->m_pTrackManager->GetList(trackListName, pTrackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DropCurrentTrackList() const
{
    return m_pPandora->m_pTrackManager->DropCurrentList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentPfoList(const ParticleFlowObjectList *&pParticleFlowObjectList) const
{
    return m_pPandora->m_pParticleFlowObjectManager->GetCurrentList(pParticleFlowObjectList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetMCParticleList(MCParticleList &mcParticleList) const
{
    return m_pPandora->m_pMCManager->GetMCParticleList(mcParticleList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::InitializeFragmentation(const Algorithm &algorithm, const ClusterList &inputClusterList,
    std::string &originalClustersListName, std::string &fragmentClustersListName) const
{
    // TODO tidy this
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MoveClustersToTemporaryListAndSetCurrent(&algorithm, inputClusterListName, originalClustersListName, inputClusterList));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->InitializeReclustering(&algorithm, inputClusterList, originalClustersListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MakeTemporaryListAndSetCurrent(&algorithm, fragmentClustersListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->PrepareForClustering(&algorithm, fragmentClustersListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::EndFragmentation(const Algorithm &algorithm, const std::string &clusterListToSaveName,
    const std::string &clusterListToDeleteName) const
{
    // TODO tidy this
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveClusters(&algorithm, inputClusterListName, clusterListToSaveName));

    const ClusterList *pClustersToBeDeleted = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetList(clusterListToDeleteName, pClustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareReclusterCandidatesForDeletion(*pClustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteTemporaryClusterList(&algorithm, clusterListToDeleteName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->EndReclustering(&algorithm, clusterListToSaveName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetCurrentListToAlgorithmInputList(&algorithm));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::InitializeReclustering(const Algorithm &algorithm, const TrackList &inputTrackList,
    const ClusterList &inputClusterList, std::string &originalClustersListName) const
{
    // TODO tidy this
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MoveClustersToTemporaryListAndSetCurrent(&algorithm, inputClusterListName, originalClustersListName, inputClusterList));

    std::string temporaryListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->CreateTemporaryListAndSetCurrent(&algorithm, inputTrackList, temporaryListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->InitializeReclustering(&algorithm, inputClusterList, originalClustersListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ReclusterHelper::InitializeReclusterMonitoring(inputTrackList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const
{
    // TODO tidy this
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveClusters(&algorithm, inputClusterListName, selectedClusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAlgorithmInfo(&algorithm, false));

    ClusterList clustersToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetClustersToBeDeleted(&algorithm, clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareReclusterCandidatesForDeletion(clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(&algorithm, false));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->EndReclustering(&algorithm, selectedClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ReclusterHelper::EndReclusterMonitoring());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RunClusteringAlgorithm(const Algorithm &algorithm, const std::string &clusteringAlgorithmName,
    const ClusterList *&pNewClusterList, std::string &newClusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MakeTemporaryListAndSetCurrent(&algorithm, newClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->PrepareForClustering(&algorithm, newClusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RunAlgorithm(clusteringAlgorithmName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentList(pNewClusterList, newClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraContentApiImpl::IsCaloHitAvailable(pandora::CaloHit *pCaloHit) const
{
    return m_pPandora->m_pCaloHitManager->IsCaloHitAvailable(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraContentApiImpl::AreCaloHitsAvailable(const pandora::CaloHitList &caloHitList) const
{
    return m_pPandora->m_pCaloHitManager->AreCaloHitsAvailable(caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    if (!m_pPandora->m_pCaloHitManager->IsCaloHitAvailable(pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddCaloHitToCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    if (pCluster->GetNCaloHits() <= 1)
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveCaloHitFromCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddIsolatedCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    if (!m_pPandora->m_pCaloHitManager->IsCaloHitAvailable(pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddIsolatedCaloHitToCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveIsolatedCaloHitFromCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveIsolatedCaloHitFromCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(pCaloHit, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::FragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1, CaloHit *&pDaughterCaloHit1,
    CaloHit *&pDaughterCaloHit2) const
{
    return m_pPandora->m_pCaloHitManager->FragmentCaloHit(pOriginalCaloHit, fraction1, pDaughterCaloHit1, pDaughterCaloHit2);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2, CaloHit *&pMergedCaloHit) const
{
    return m_pPandora->m_pCaloHitManager->MergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2, pMergedCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeleteCluster(Cluster *pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareClusterForDeletion(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteCluster(pCluster));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeleteCluster(Cluster *pCluster, const std::string &clusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareClusterForDeletion(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteCluster(pCluster, clusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeleteClusters(const ClusterList &clusterList) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareClustersForDeletion(clusterList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteClusters(clusterList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeleteClusters(const ClusterList &clusterList, const std::string &clusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareClustersForDeletion(clusterList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteClusters(clusterList, clusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete) const
{
    if ((pClusterToEnlarge == pClusterToDelete) || !pClusterToEnlarge->IsAvailable() || !pClusterToDelete->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
    const std::string &deleteListName) const
{
    if ((pClusterToEnlarge == pClusterToDelete) || !pClusterToEnlarge->IsAvailable() || !pClusterToDelete->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete,
        enlargeListName, deleteListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddClusterToPfo(ParticleFlowObject *pParticleFlowObject, Cluster *pCluster) const
{
    if (!pCluster->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pParticleFlowObjectManager->AddClusterToPfo(pParticleFlowObject, pCluster));
    pCluster->SetAvailability(false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddTrackToPfo(ParticleFlowObject *pParticleFlowObject, Track *pTrack) const
{
    if (!pTrack->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pParticleFlowObjectManager->AddTrackToPfo(pParticleFlowObject, pTrack));
    pTrack->SetAvailability(false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveClusterFromPfo(ParticleFlowObject *pParticleFlowObject, Cluster *pCluster) const
{
    if ((pParticleFlowObject->GetNClusters() <= 1) && (pParticleFlowObject->GetNTracks() == 0))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pParticleFlowObjectManager->RemoveClusterFromPfo(pParticleFlowObject, pCluster));
    pCluster->SetAvailability(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveTrackFromPfo(ParticleFlowObject *pParticleFlowObject, Track *pTrack) const
{
    if ((pParticleFlowObject->GetNTracks() <= 1) && (pParticleFlowObject->GetNClusters() == 0))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pParticleFlowObjectManager->RemoveTrackFromPfo(pParticleFlowObject, pTrack));
    pTrack->SetAvailability(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::DeletePfo(ParticleFlowObject *pParticleFlowObject) const
{
    const TrackList trackList(pParticleFlowObject->GetTrackList());
    const ClusterList clusterList(pParticleFlowObject->GetClusterList());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pParticleFlowObjectManager->DeletePfo(pParticleFlowObject));

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
        (*iter)->SetAvailability(true);

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
        (*iter)->SetAvailability(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddTrackClusterAssociation(Track *const pTrack, Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pTrack->SetAssociatedCluster(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCluster->AddTrackAssociation(pTrack));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveTrackClusterAssociation(Track *const pTrack, Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pTrack->RemoveAssociatedCluster(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCluster->RemoveTrackAssociation(pTrack));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveCurrentTrackClusterAssociations() const
{
    TrackList danglingTracks;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveCurrentTrackAssociations(danglingTracks));

    if (!danglingTracks.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(danglingTracks));

    TrackToClusterMap danglingClusters;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveCurrentClusterAssociations(danglingClusters));

    if (!danglingClusters.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveTrackAssociations(danglingClusters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveAllTrackClusterAssociations() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveAllClusterAssociations());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->RemoveAllTrackAssociations());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterList(const Algorithm &algorithm, const std::string &newClusterListName) const
{
    std::string currentClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentListName(currentClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveClusters(&algorithm, newClusterListName, currentClusterListName));

    const ClusterList *pNewClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetList(newClusterListName, pNewClusterList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterList(const Algorithm &algorithm, const std::string &newClusterListName,
    const ClusterList &clustersToSave) const
{
    std::string currentClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentListName(currentClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveClusters(&algorithm, newClusterListName, currentClusterListName, clustersToSave));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterList(const Algorithm &algorithm, const std::string &oldClusterListName,
    const std::string &newClusterListName) const
{
    return m_pPandora->m_pClusterManager->SaveClusters(&algorithm, newClusterListName, oldClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterList(const Algorithm &algorithm, const std::string &oldClusterListName,
    const std::string &newClusterListName, const ClusterList &clustersToSave) const
{
    return m_pPandora->m_pClusterManager->SaveClusters(&algorithm, newClusterListName, oldClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentClusterList(const Algorithm &algorithm, const std::string &newClusterListName) const
{
    return m_pPandora->m_pClusterManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentClusterList(const std::string &newClusterListName) const
{
    return m_pPandora->m_pClusterManager->TemporarilyReplaceCurrentList(newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveCaloHitList(const Algorithm &/*algorithm*/, const CaloHitList &caloHitList, const std::string &newListName) const
{
    return m_pPandora->m_pCaloHitManager->SaveList(caloHitList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentCaloHitList(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pCaloHitManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveTrackList(const Algorithm &/*algorithm*/, const TrackList &trackList, const std::string &newListName) const
{
    return m_pPandora->m_pTrackManager->SaveList(trackList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentTrackList(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pTrackManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraContentApiImpl::PandoraContentApiImpl(Pandora *pPandora) :
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareClusterForDeletion(const Cluster *const pCluster) const
{
    if (!pCluster->IsAvailable())
        return STATUS_CODE_NOT_ALLOWED;

    CaloHitList caloHitList;
    pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

    const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
    caloHitList.insert(isolatedCaloHitList.begin(), isolatedCaloHitList.end());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(caloHitList, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pCluster->GetAssociatedTrackList()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareClustersForDeletion(const ClusterList &clusterList) const
{
    TrackList trackList;
    CaloHitList caloHitList;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!pCluster->IsAvailable())
            return STATUS_CODE_NOT_ALLOWED;

        pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

        const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
        caloHitList.insert(isolatedCaloHitList.begin(), isolatedCaloHitList.end());

        const TrackList &associatedTrackList(pCluster->GetAssociatedTrackList());
        trackList.insert(associatedTrackList.begin(), associatedTrackList.end());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->SetCaloHitAvailability(caloHitList, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(trackList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareReclusterCandidatesForDeletion(const ClusterList &clusterList) const
{
    TrackList trackList;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!pCluster->IsAvailable())
            return STATUS_CODE_NOT_ALLOWED;

        trackList.insert(pCluster->GetAssociatedTrackList().begin(), pCluster->GetAssociatedTrackList().end());
    }

    return m_pPandora->m_pTrackManager->RemoveClusterAssociations(trackList);
}

} // namespace pandora
