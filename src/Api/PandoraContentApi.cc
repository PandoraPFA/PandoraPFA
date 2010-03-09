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
StatusCode PandoraContentApi::Cluster::Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters)
{
    pandora::Cluster *pCluster = NULL;
    return algorithm.GetPandoraContentApiImpl()->CreateCluster(pClusterParameters, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename CLUSTER_PARAMETERS>
StatusCode PandoraContentApi::Cluster::Create(const pandora::Algorithm &algorithm, CLUSTER_PARAMETERS *pClusterParameters,
    pandora::Cluster *&pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->CreateCluster(pClusterParameters, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::ParticleFlowObject::Create(const pandora::Algorithm &algorithm, const Parameters &parameters)
{
    return algorithm.GetPandoraContentApiImpl()->CreateParticleFlowObject(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::CreateDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, TiXmlElement *const pXmlElement,
    std::string &daughterAlgorithmName)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->CreateDaughterAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RunDaughterAlgorithm(const pandora::Algorithm &parentAlgorithm, const std::string &daughterAlgorithmName)
{
    return parentAlgorithm.GetPandoraContentApiImpl()->RunAlgorithm(daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList)
{
    std::string clusterListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentClusterList(const pandora::Algorithm &algorithm, const pandora::ClusterList *&pClusterList,
    std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterList(pClusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentClusterListName(const pandora::Algorithm &algorithm, std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentClusterListName(clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetClusterList(const pandora::Algorithm &algorithm, const std::string &clusterListName,
    const pandora::ClusterList *&pClusterList)
{
    return algorithm.GetPandoraContentApiImpl()->GetClusterList(clusterListName, pClusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm,
    const pandora::OrderedCaloHitList *&pOrderedCaloHitList)
{
    std::string orderedCaloHitListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentOrderedCaloHitList(pOrderedCaloHitList, orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm,
    const pandora::OrderedCaloHitList *&pOrderedCaloHitList, std::string &orderedCaloHitListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentOrderedCaloHitList(pOrderedCaloHitList, orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentOrderedCaloHitListName(const pandora::Algorithm &algorithm, std::string &orderedCaloHitListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentOrderedCaloHitListName(orderedCaloHitListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &orderedCaloHitListName,
    const pandora::OrderedCaloHitList *&pOrderedCaloHitList)
{
    return algorithm.GetPandoraContentApiImpl()->GetOrderedCaloHitList(orderedCaloHitListName, pOrderedCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList)
{
    std::string trackListName;
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList *&pTrackList,
    std::string &trackListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackList(pTrackList, trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetCurrentTrackListName(const pandora::Algorithm &algorithm, std::string &trackListName)
{
    return algorithm.GetPandoraContentApiImpl()->GetCurrentTrackListName(trackListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetTrackList(const pandora::Algorithm &algorithm, const std::string &trackListName,
    const pandora::TrackList *&pTrackList)
{
    return algorithm.GetPandoraContentApiImpl()->GetTrackList(trackListName, pTrackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::InitializeReclustering(const pandora::Algorithm &algorithm, const pandora::TrackList &inputTrackList,
    const pandora::ClusterList &inputClusterList, std::string &originalClustersListName)
{
    return algorithm.GetPandoraContentApiImpl()->InitializeReclustering(algorithm, inputTrackList, inputClusterList, originalClustersListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::EndReclustering(const pandora::Algorithm &algorithm, const std::string &selectedClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->EndReclustering(algorithm, selectedClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
    const pandora::ClusterList *&pNewClusterList)
{
    std::string newClusterListName;
    return algorithm.GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RunClusteringAlgorithm(const pandora::Algorithm &algorithm, const std::string &clusteringAlgorithmName, 
    const pandora::ClusterList *&pNewClusterList, std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->RunClusteringAlgorithm(algorithm, clusteringAlgorithmName, pNewClusterList, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::AddCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->AddCaloHitToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RemoveCaloHitFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveCaloHitFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::AddIsolatedCaloHitToCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->AddIsolatedCaloHitToCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RemoveIsolatedCaloHitFromCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, pandora::CaloHit *pCaloHit)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveIsolatedCaloHitFromCluster(pCluster, pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteCluster(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::DeleteCluster(const pandora::Algorithm &algorithm, pandora::Cluster *pCluster, const std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteCluster(pCluster, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::DeleteClusters(const pandora::Algorithm &algorithm, const pandora::ClusterList &clusterList)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteClusters(clusterList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::DeleteClusters(const pandora::Algorithm &algorithm, const pandora::ClusterList &clusterList, const std::string &clusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->DeleteClusters(clusterList, clusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterToEnlarge,
    pandora::Cluster *pClusterToDelete, const std::string &enlargeListName, const std::string &deleteListName)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete, enlargeListName, deleteListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::AddTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->AddTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RemoveTrackClusterAssociation(const pandora::Algorithm &algorithm, pandora::Track *const pTrack,
    pandora::Cluster *const pCluster)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveTrackClusterAssociation(pTrack, pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RemoveCurrentTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveCurrentTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::RemoveAllTrackClusterAssociations(const pandora::Algorithm &algorithm)
{
    return algorithm.GetPandoraContentApiImpl()->RemoveAllTrackClusterAssociations();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
    const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, newClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &oldClusterListName,
    const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, oldClusterListName, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &oldClusterListName,
    const std::string &newClusterListName, const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, oldClusterListName, newClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::ReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentClusterList(algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndReplaceCurrent(algorithm, newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterListAndReplaceCurrent(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
    const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndReplaceCurrent(algorithm, newClusterListName, clustersToSave);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::TemporarilyReplaceCurrentClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->TemporarilyReplaceCurrentClusterList(newClusterListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveOrderedCaloHitList(const pandora::Algorithm &algorithm,
    const pandora::OrderedCaloHitList &orderedCaloHitList, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveOrderedCaloHitList(algorithm, orderedCaloHitList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::ReplaceCurrentOrderedCaloHitList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentOrderedCaloHitList(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveOrderedCaloHitListAndReplaceCurrent(const pandora::Algorithm &algorithm,
    const pandora::OrderedCaloHitList &orderedCaloHitList, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveOrderedCaloHitListAndReplaceCurrent(algorithm, orderedCaloHitList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveTrackList(const pandora::Algorithm &algorithm, const pandora::TrackList &trackList,
    const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveTrackList(algorithm, trackList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::ReplaceCurrentTrackList(const pandora::Algorithm &algorithm, const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->ReplaceCurrentTrackList(algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveTrackListAndReplaceCurrent(const pandora::Algorithm &algorithm, const pandora::TrackList &trackList,
    const std::string &newListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveTrackListAndReplaceCurrent(algorithm, trackList, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::GetMCParticleList(const pandora::Algorithm &algorithm, pandora::MCParticleList &mcParticleList)
{
    return algorithm.GetPandoraContentApiImpl()->GetMCParticleList(mcParticleList);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit);
template StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHitList>(const pandora::Algorithm &algorithm, pandora::CaloHitList *pCaloHitList);
template StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack);

template StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit, pandora::Cluster *&pCluster);
template StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHitList>(const pandora::Algorithm &algorithm, pandora::CaloHitList *pCaloHitList, pandora::Cluster *&pCluster);
template StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack, pandora::Cluster *&pCluster);
