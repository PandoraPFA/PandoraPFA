/**
 *  @file   PandoraPFANew/src/Api/PandoraContentApiImpl.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Api/PandoraContentApiImpl.h"

#include "Helpers/CaloHitHelper.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/TrackManager.h"
#include "Managers/ParticleFlowObjectManager.h"

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
    if (!CaloHitHelper::IsCaloHitAvailable(pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateCluster(pCaloHit, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::SetCaloHitAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::CreateCluster(CaloHitVector *pCaloHitVector, Cluster *&pCluster) const
{
    if (!CaloHitHelper::AreCaloHitsAvailable(*pCaloHitVector))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateCluster(pCaloHitVector, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::SetCaloHitAvailability(*pCaloHitVector, false));

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
    return m_pPandora->m_pParticleFlowObjectManager->CreateParticleFlowObject(particleFlowObjectParameters);
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
        for (unsigned int i = 1; i < m_pPandora->m_pCaloHitManager->m_algorithmInfoMap.size(); ++i)
            std::cout << "----";

        std::cout << "> Running Algorithm: " << iter->first << ", " << iter->second->GetAlgorithmType() << std::endl;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->Run());
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetAlgorithmType() << ", " << statusCodeException.ToString() << std::endl;
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

StatusCode PandoraContentApiImpl::GetCurrentOrderedCaloHitList(const OrderedCaloHitList *&pOrderedCaloHitList,
    std::string &orderedCaloHitListName) const
{
    return m_pPandora->m_pCaloHitManager->GetCurrentList(pOrderedCaloHitList, orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetCurrentOrderedCaloHitListName(std::string &orderedCaloHitListName) const
{
    return m_pPandora->m_pCaloHitManager->GetCurrentListName(orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::GetOrderedCaloHitList(const std::string &orderedCaloHitListName, const OrderedCaloHitList *&pOrderedCaloHitList) const
{
    return m_pPandora->m_pCaloHitManager->GetList(orderedCaloHitListName, pOrderedCaloHitList);
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

StatusCode PandoraContentApiImpl::InitializeReclustering(const Algorithm &algorithm, const TrackList &inputTrackList,
    const ClusterList &inputClusterList, std::string &originalClustersListName) const
{
    std::string temporaryListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->CreateTemporaryListAndSetCurrent(&algorithm, inputTrackList, temporaryListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->CreateTemporaryListAndSetCurrent(&algorithm, inputClusterList, temporaryListName));

    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MoveClustersToTemporaryListAndSetCurrent(&algorithm, inputClusterListName, originalClustersListName, inputClusterList));

    std::string orderedCaloHitListName;
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->GetCurrentList(pOrderedCaloHitList, orderedCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::CreateInitialCaloHitUsageMap(originalClustersListName, pOrderedCaloHitList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const
{
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveClusters(&algorithm, inputClusterListName, selectedClusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAlgorithmInfo(&algorithm, false));

    ClusterList clustersToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetClustersToBeDeleted(&algorithm, clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareReclusterCandidatesForDeletion(clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(&algorithm, false));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ApplyCaloHitUsageMap(selectedClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RunClusteringAlgorithm(const Algorithm &algorithm, const std::string &clusteringAlgorithmName,
    const ClusterList *&pNewClusterList, std::string &newClusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MakeTemporaryListAndSetCurrent(&algorithm, newClusterListName));

    if (0 < CaloHitHelper::m_nReclusteringProcesses)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::CreateAdditionalCaloHitUsageMap(newClusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RunAlgorithm(clusteringAlgorithmName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetCurrentList(pNewClusterList, newClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddCaloHitToCluster(Cluster *pCluster, CaloHit *pCaloHit) const
{
    if (!CaloHitHelper::IsCaloHitAvailable(pCaloHit))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->AddCaloHitToCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::SetCaloHitAvailability(pCaloHit, false));

    return STATUS_CODE_SUCCESS;
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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete, const std::string &enlargeListName,
    const std::string &deleteListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete,
        enlargeListName, deleteListName));

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

    const ClusterList *pNewClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetList(newClusterListName, pNewClusterList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentClusterList(const Algorithm &algorithm, const std::string &newClusterListName) const
{
    return m_pPandora->m_pClusterManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterListAndReplaceCurrent(const Algorithm &algorithm, const std::string &newClusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SaveClusterList(algorithm, newClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentClusterList(const std::string &newClusterListName) const
{
    return m_pPandora->m_pClusterManager->TemporarilyReplaceCurrentList(newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveClusterListAndReplaceCurrent(const Algorithm &algorithm, const std::string &newClusterListName,
    const ClusterList &clustersToSave) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SaveClusterList(algorithm, newClusterListName, clustersToSave));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveOrderedCaloHitList(const Algorithm &algorithm, const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName) const
{
    return m_pPandora->m_pCaloHitManager->SaveList(orderedCaloHitList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentOrderedCaloHitList(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pCaloHitManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveOrderedCaloHitListAndReplaceCurrent(const Algorithm &algorithm, const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SaveOrderedCaloHitList(algorithm, orderedCaloHitList, newListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveTrackList(const Algorithm &algorithm, const TrackList &trackList, const std::string &newListName) const
{
    return m_pPandora->m_pTrackManager->SaveList(trackList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::ReplaceCurrentTrackList(const Algorithm &algorithm, const std::string &newListName) const
{
    return m_pPandora->m_pTrackManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SaveTrackListAndReplaceCurrent(const Algorithm &algorithm, const TrackList &trackList, const std::string &newListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SaveTrackList(algorithm, trackList, newListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraContentApiImpl::PandoraContentApiImpl(Pandora *pPandora) :
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareClusterForDeletion(const Cluster *const pCluster) const
{
    CaloHitVector caloHitVector;
    pCluster->GetOrderedCaloHitList().GetCaloHitVector(caloHitVector);

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::SetCaloHitAvailability(caloHitVector, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(pCluster->GetAssociatedTrackList()));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareClustersForDeletion(const ClusterList &clusterList) const
{
    TrackList trackList;
    CaloHitVector caloHitVector;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        (*iter)->GetOrderedCaloHitList().GetCaloHitVector(caloHitVector);
        trackList.insert((*iter)->GetAssociatedTrackList().begin(), (*iter)->GetAssociatedTrackList().end());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::SetCaloHitAvailability(caloHitVector, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->RemoveClusterAssociations(trackList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PrepareReclusterCandidatesForDeletion(const ClusterList &clusterList) const
{
    TrackList trackList;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        trackList.insert((*iter)->GetAssociatedTrackList().begin(), (*iter)->GetAssociatedTrackList().end());
    }

    return m_pPandora->m_pTrackManager->RemoveClusterAssociations(trackList);
}

} // namespace pandora
