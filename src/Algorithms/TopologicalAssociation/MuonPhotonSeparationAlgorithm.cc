/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/MuonPhotonSeparationAlgorithm.cc
 * 
 *  @brief  Implementation of the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/MuonPhotonSeparationAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode MuonPhotonSeparationAlgorithm::PerformFragmentation(Cluster *const pOriginalCluster, Track *const pTrack, PseudoLayer showerStartLayer,
    PseudoLayer showerEndLayer) const
{
    ClusterList clusterList;
    clusterList.insert(pOriginalCluster);
    std::string originalClustersListName, fragmentClustersListName;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList, originalClustersListName,
        fragmentClustersListName));

    // Make the cluster fragments
    Cluster *pMipCluster = NULL, *pPhotonCluster = NULL;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->MakeClusterFragments(showerStartLayer, showerEndLayer, pOriginalCluster,
        pMipCluster, pPhotonCluster));

    // Decide whether to keep original cluster or the fragments
    std::string clusterListToSaveName(originalClustersListName), clusterListToDeleteName(fragmentClustersListName);

    if ((NULL != pMipCluster) && (NULL != pPhotonCluster) && (pPhotonCluster->GetNCaloHits() >= m_minHitsInPhotonCluster))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveTrackClusterAssociation(*this, pTrack, pOriginalCluster));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pMipCluster));

        if (ParticleIdHelper::IsMuonFast(pMipCluster) && (pTrack->GetEnergyAtDca() > m_highEnergyMuonCut))
        {
            clusterListToSaveName = fragmentClustersListName;
            clusterListToDeleteName = originalClustersListName;
        }
        else
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveTrackClusterAssociation(*this, pTrack, pMipCluster));
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pOriginalCluster));
        }
    }

    // End cluster fragmentation operations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,
        clusterListToDeleteName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonPhotonSeparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Read base class settings
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, MipPhotonSeparationAlgorithm::ReadSettings(xmlHandle));

    m_highEnergyMuonCut = 0.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HighEnergyMuonCut", m_highEnergyMuonCut));

    return STATUS_CODE_SUCCESS;
}
