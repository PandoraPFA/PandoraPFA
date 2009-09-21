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

StatusCode PandoraContentApi::MergeAndDeleteClusters(const pandora::Algorithm &algorithm, pandora::Cluster *pClusterLhs,
    pandora::Cluster *pClusterRhs)
{
    return algorithm.GetPandoraContentApiImpl()->MergeAndDeleteClusters(pClusterLhs, pClusterRhs);
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

StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string newClusterListName)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, newClusterListName);
}
        
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApi::SaveClusterList(const pandora::Algorithm &algorithm, const std::string &newClusterListName,
    const pandora::ClusterList &clustersToSave)
{
    return algorithm.GetPandoraContentApiImpl()->SaveClusterList(algorithm, newClusterListName, &clustersToSave);
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
    return algorithm.GetPandoraContentApiImpl()->SaveClusterListAndReplaceCurrent(algorithm, newClusterListName, &clustersToSave);
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
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit);
template StatusCode PandoraContentApi::Cluster::Create<pandora::InputCaloHitList>(const pandora::Algorithm &algorithm, pandora::InputCaloHitList *pInputCaloHitList);
template StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack);

template StatusCode PandoraContentApi::Cluster::Create<pandora::CaloHit>(const pandora::Algorithm &algorithm, pandora::CaloHit *pCaloHit, pandora::Cluster *&pCluster);
template StatusCode PandoraContentApi::Cluster::Create<pandora::InputCaloHitList>(const pandora::Algorithm &algorithm, pandora::InputCaloHitList *pInputCaloHitList, pandora::Cluster *&pCluster);
template StatusCode PandoraContentApi::Cluster::Create<pandora::Track>(const pandora::Algorithm &algorithm, pandora::Track *pTrack, pandora::Cluster *&pCluster);
