/**
 *  @file   PandoraPFANew/src/Algorithms/Clustering/ForcedClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the forced clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/Clustering/ForcedClusteringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode ForcedClusteringAlgorithm::Run()
{
    // Read current track list, which should have exactly one entry
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    if (pTrackList->size() != 1)
        return STATUS_CODE_INVALID_PARAMETER;

    Track *pTrack = *(pTrackList->begin());
    const Helix *const pHelix(pTrack->GetHelixFitAtECal());
    const float trackEnergy(pTrack->GetEnergyAtDca());

    // Read current ordered calo hit list
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    if (pOrderedCaloHitList->empty())
        return STATUS_CODE_INVALID_PARAMETER;

    CaloHitList inputCaloHitList;
    pOrderedCaloHitList->GetCaloHitList(inputCaloHitList);

    // Order all available calo hits by distance to track seed
    CaloHitDistanceVector caloHitDistanceVector;

    for (CaloHitList::const_iterator iter = inputCaloHitList.begin(), iterEnd = inputCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (CaloHitHelper::IsCaloHitAvailable(*iter) && (m_shouldClusterIsolatedHits || !(*iter)->IsIsolated()))
        {
            CartesianVector helixSeparation;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetDistanceToPoint((*iter)->GetPositionVector(), helixSeparation));
            caloHitDistanceVector.push_back(std::make_pair(*iter, helixSeparation.GetMagnitudeSquared()));
        }
    }

    std::sort(caloHitDistanceVector.begin(), caloHitDistanceVector.end(), ForcedClusteringAlgorithm::SortByDistanceToTrack);

    // Return if there are no suitable calo hits to cluster
    if (caloHitDistanceVector.empty())
        return STATUS_CODE_SUCCESS;

    // Create a single track seeded cluster
    Cluster *pCluster = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pTrack, pCluster));

    // Work along ordered list of calo hits, adding to the cluster until cluster energy matches track energy.
    for (CaloHitDistanceVector::const_iterator iter = caloHitDistanceVector.begin(), iterEnd = caloHitDistanceVector.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pCluster, iter->first));

        if (pCluster->GetHadronicEnergy() >= trackEnergy)
            break;
    }

    // Deal with remaining hits. Either run standard clustering algorithm, or crudely collect together into one cluster
    if (m_shouldRunStandardClusteringAlgorithm)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_standardClusteringAlgorithmName));
    }
    else
    {
        CaloHitList remnantCaloHitList;

        for (CaloHitList::const_iterator iter = inputCaloHitList.begin(), iterEnd = inputCaloHitList.end(); iter != iterEnd; ++iter)
        {
            if (CaloHitHelper::IsCaloHitAvailable(*iter) && (m_shouldClusterIsolatedHits || !(*iter)->IsIsolated()))
                remnantCaloHitList.insert(*iter);
        }

        if (!remnantCaloHitList.empty())
        {
            Cluster *pRemnantCluster = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, &remnantCaloHitList, pRemnantCluster));
        }
    }

    // If specified, associate isolated hits with the newly formed clusters
    if (m_shouldAssociateIsolatedHits)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_isolatedHitAssociationAlgorithmName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ForcedClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_shouldRunStandardClusteringAlgorithm = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldRunStandardClusteringAlgorithm", m_shouldRunStandardClusteringAlgorithm));

    if (m_shouldRunStandardClusteringAlgorithm)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "StandardClustering",
            m_standardClusteringAlgorithmName));
    }

    m_shouldClusterIsolatedHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldClusterIsolatedHits", m_shouldClusterIsolatedHits));

    m_shouldAssociateIsolatedHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldAssociateIsolatedHits", m_shouldAssociateIsolatedHits));

    if (m_shouldAssociateIsolatedHits)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "IsolatedHitAssociation",
            m_isolatedHitAssociationAlgorithmName));
    }

    return STATUS_CODE_SUCCESS;
}
