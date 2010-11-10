/**
 *  @file   PandoraPFANew/src/Api/PandoraContentApi.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Algorithms/Algorithm.h"

#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

template <typename CLUSTER_PARAMETERS>
pandora::StatusCode PandoraContentApi::Cluster::Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters)
{
    pandora::Cluster *pCluster = NULL;
    return algorithm.GetPandoraContentApiImpl()->CreateCluster(pClusterParameters, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename CLUSTER_PARAMETERS>
pandora::StatusCode PandoraContentApi::Cluster::Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters,
    pandora::Cluster *&pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->CreateCluster(pClusterParameters, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ParticleFlowObject::Create(const pandora::Algorithm &algorithm, const Parameters &parameters)
{
    return algorithm.GetPandoraContentApiImpl()->CreateParticleFlowObject(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::CreateDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, TiXmlElement *const pXmlElement,
    std::string &daughterAlgorithmName)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->CreateDaughterAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, const std::string &daughterAlgorithmName)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->RunAlgorithm(daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList)
{
    std::string clusterListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList,
    std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentClusterListName(const pandora::Algorithm &algorithm, std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterListName(clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetClusterList(const pandora::Algorithm &algorithm, const std::string &clusterListName,
    const pandora::ClusterList *&pClusterList)
{
    return algorithm.GetPandoraContentApiImpl()->GetClusterList(clusterListName, pClusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DropCurrentClusterList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentClusterList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm,
    const pandora::OrderedCaloHitList *&pOrderedCaloHitList)
{
    std::string orderedCaloHitListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentOrderedCaloHitList(pOrderedCaloHitList, orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm,
    const pandora::OrderedCaloHitList *&pOrderedCaloHitList, std::string &orderedCaloHitListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentOrderedCaloHitList(pOrderedCaloHitList, orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentOrderedCaloHitListName(const pandora::Algorithm &algorithm, std::string &orderedCaloHitListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentOrderedCaloHitListName(orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &orderedCaloHitListName,
    const pandora::OrderedCaloHitList *&pOrderedCaloHitList)
{
    return algorithm.GetPandoraContentApiImpl()->GetOrderedCaloHitList(orderedCaloHitListName, pOrderedCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DropCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentOrderedCaloHitList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList)
{
    std::string trackListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList,
    std::string &trackListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentTrackListName(const pandora::Algorithm &algorithm, std::string &trackListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackListName(trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetTrackList(const pandora::Algorithm &algorithm, const std::string &trackListName,
    const pandora::TrackList *&pTrackList)
{
    return algorithm.GetPandoraContentApiImpl()->GetTrackList(trackListName, pTrackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DropCurrentTrackList(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->DropCurrentTrackList();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetCurrentPfoList(const pandora::Algorithm &algorithm, const pandora::ParticleFlowObjectList *&pParticleFlowObjectList)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentPfoList(pParticleFlowObjectList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::InitializeFragmentation(const pandora::Algorithm &algorithm, const pandora::ClusterList &inputClusterList,
    std::string &originalClustersListName, std::string &fragmentClustersListName)
{
    return algorithm.GetPandoraContentApiImpl()->InitializeFragmentation(algorithm, inputClusterList, originalClustersListName,
        fragmentClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::EndFragmentation(const pandora::Algorithm &algorithm, const std::string &clusterListToSaveName,
    const std::string &clusterListToDeleteName)
{
    return algorithm.GetPandoraContentApiImpl()->EndFragmentation(algorithm, clusterListToSaveName, clusterListToDeleteName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::InitializeReclustering(const pandora::Algorithm &algorithm, const pandora::TrackList &inputTrackList,
    const pandora::ClusterList &inputClusterList, std::string &originalClustersListName)
{
    return algorithm.GetPandoraContentApiImpl()->InitializeReclustering(algorithm, inputTrackList, inputClusterList, originalClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->EndReclustering(algorithm, selectedClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
    const pandora::ClusterList *&pNewClusterList)
{
    std::string newClusterListName;
    return algorithm.GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
    const pandora::ClusterList *&pNewClusterList, std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->AddCaloHitToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveCaloHitFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveCaloHitFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddIsolatedCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->AddIsolatedCaloHitToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveIsolatedCaloHitFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster,
    pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveIsolatedCaloHitFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteCluster(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, const std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteCluster(pCluster, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeleteClusters(const pandora::Algorithm &algorithm, const pandora::ClusterList &clusterList)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteClusters(clusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeleteClusters(const pandora::Algorithm &algorithm, const pandora::ClusterList &clusterList,
    const std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteClusters(clusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete, const std::string &enlargeListName, const std::string &deleteListName)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete, enlargeListName, deleteListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddClusterToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParticleFlowObject,
    pandora::Cluster *pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->AddClusterToPfo(pParticleFlowObject, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddTrackToPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParticleFlowObject,
    pandora::Track *pTrack)
{
    return algorithm.GetPandoraContentApiImpl()->AddTrackToPfo(pParticleFlowObject, pTrack);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveClusterFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParticleFlowObject,
    pandora::Cluster *pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveClusterFromPfo(pParticleFlowObject, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveTrackFromPfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParticleFlowObject,
    pandora::Track *pTrack)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveTrackFromPfo(pParticleFlowObject, pTrack);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::DeletePfo(const pandora::Algorithm &algorithm, pandora::ParticleFlowObject *pParticleFlowObject)
{
    return algorithm.GetPandoraContentApiImpl()->DeletePfo(pParticleFlowObject);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::AddTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->AddTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveCurrentTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveCurrentTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::RemoveAllTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveAllTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
    const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, newClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &oldClusterListName,
    const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, oldClusterListName, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &oldClusterListName,
    const std::string &newClusterListName, const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, oldClusterListName, newClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentClusterList(algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndReplaceCurrent(algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
    const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndReplaceCurrent(algorithm, newClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::TemporarilyReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->TemporarilyReplaceCurrentClusterList(newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveOrderedCaloHitList(const pandora::Algorithm &algorithm,
    const pandora::OrderedCaloHitList &orderedCaloHitList, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveOrderedCaloHitList(algorithm, orderedCaloHitList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ReplaceCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentOrderedCaloHitList(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveOrderedCaloHitListAndReplaceCurrent(const pandora::Algorithm &algorithm,
    const pandora::OrderedCaloHitList &orderedCaloHitList, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveOrderedCaloHitListAndReplaceCurrent(algorithm, orderedCaloHitList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList &trackList,
    const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveTrackList(algorithm, trackList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::ReplaceCurrentTrackList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentTrackList(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::SaveTrackListAndReplaceCurrent(const pandora::Algorithm &algorithm, const pandora::TrackList &trackList,
    const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveTrackListAndReplaceCurrent(algorithm, trackList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode PandoraContentApi::GetMCParticleList(const pandora::Algorithm &algorithm, pandora::MCParticleList &mcParticleList)
{
    return algorithm.GetPandoraContentApiImpl()->GetMCParticleList(mcParticleList);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHitList>(const pandora::Algorithm &algorithm, pandora::CaloHitList *pCaloHitList);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack);

template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit, pandora::Cluster *&pCluster);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHitList>(const pandora::Algorithm &algorithm, pandora::CaloHitList *pCaloHitList, pandora::Cluster *&pCluster);
template pandora::StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack, pandora::Cluster *&pCluster);
