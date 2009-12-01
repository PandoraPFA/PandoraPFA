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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->CreateCluster(pTrack, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pTrack->SetAssociatedCluster(pCluster));

    return STATUS_CODE_SUCCESS;
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

    CaloHitVector caloHitsInDeletedClusters;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(iter->second, true, &caloHitsInDeletedClusters));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::SetCaloHitAvailability(caloHitsInDeletedClusters, true));

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

    CaloHitHelper::m_isReclustering = true;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::CreateInitialCaloHitUsageMap(originalClustersListName, pOrderedCaloHitList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const
{
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->SaveClusters(&algorithm, inputClusterListName, selectedClusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pCaloHitManager->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pTrackManager->ResetAlgorithmInfo(&algorithm, false));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ApplyCaloHitUsageMap(selectedClusterListName));
    CaloHitHelper::m_isReclustering = false;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::ClearCaloHitUsageMaps());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RunClusteringAlgorithm(const Algorithm &algorithm, const std::string &clusteringAlgorithmName,
    const ClusterList *&pNewClusterList, std::string &newClusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->MakeTemporaryListAndSetCurrent(&algorithm, newClusterListName));

    if (CaloHitHelper::m_isReclustering)
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
    CaloHitVector caloHitVector;
    pCluster->GetOrderedCaloHitList().GetCaloHitVector(caloHitVector);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandora->m_pClusterManager->DeleteCluster(pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CaloHitHelper::SetCaloHitAvailability(caloHitVector, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(Cluster *pClusterToEnlarge, Cluster *pClusterToDelete) const
{
    return m_pPandora->m_pClusterManager->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete);
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

StatusCode PandoraContentApiImpl::ReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName) const
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

StatusCode PandoraContentApiImpl::ReplaceCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &newListName) const
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

StatusCode PandoraContentApiImpl::ReplaceCurrentTrackList(const pandora::Algorithm &algorithm, const std::string &newListName) const
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
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode PandoraContentApiImpl::CreateCluster<CaloHit>(CaloHit *pCaloHit, Cluster *&pCluster) const;
template StatusCode PandoraContentApiImpl::CreateCluster<CaloHitVector>(CaloHitVector *pCaloHitVector, Cluster *&pCluster) const;
template StatusCode PandoraContentApiImpl::CreateCluster<Track>(pandora::Track *pTrack, Cluster *&pCluster) const;

} // namespace pandora
