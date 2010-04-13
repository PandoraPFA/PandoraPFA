/**
 *  @file   PandoraPFANew/src/Algorithms/Clustering/ForcedClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the forced clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/Clustering/ForcedClusteringAlgorithm.h"

#include "Helpers/CaloHitHelper.h"

#include <limits>

using namespace pandora;

StatusCode ForcedClusteringAlgorithm::Run()
{
    // Read current track list, which should have exactly one entry
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    if (pTrackList->size() != 1)
        return STATUS_CODE_INVALID_PARAMETER;

    Track *pTrack = *(pTrackList->begin());
    const float trackEnergy(pTrack->GetEnergyAtDca());

    // Read current ordered calo hit list
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    if (pOrderedCaloHitList->empty())
        return STATUS_CODE_INVALID_PARAMETER;

    CaloHitList inputCaloHitList;
    pOrderedCaloHitList->GetCaloHitList(inputCaloHitList);

    // Create single track seeded cluster
    Cluster *pCluster = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pTrack, pCluster));

    // Order all available calo hits by distance to track seed
    CaloHitDistanceVector caloHitDistanceVector;

    for (CaloHitList::const_iterator iter = inputCaloHitList.begin(), iterEnd = inputCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (CaloHitHelper::IsCaloHitAvailable(*iter) && (m_shouldClusterIsolatedHits || !(*iter)->IsIsolated()))
        {
            float distanceToTrackSeed(std::numeric_limits<float>::max());

            if (STATUS_CODE_SUCCESS == this->GetDistanceToTrackSeed(pCluster, *iter, distanceToTrackSeed))
                caloHitDistanceVector.push_back(std::make_pair(*iter, distanceToTrackSeed));
        }
    }

    std::sort(caloHitDistanceVector.begin(), caloHitDistanceVector.end(), ForcedClusteringAlgorithm::SortByDistanceToTrackSeed);

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

        Cluster *pRemnantCluster = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, &remnantCaloHitList, pRemnantCluster));
    }

    // If specified, associate isolated hits with the newly formed clusters
    if (m_shouldAssociateIsolatedHits)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_isolatedHitAssociationAlgorithmName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ForcedClusteringAlgorithm::GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, float &distance) const
{
    if (0 == m_maxTrackSeedSeparation)
        return STATUS_CODE_FAILURE;

    const CartesianVector hitPosition(pCaloHit->GetPositionVector());

    const CartesianVector &trackSeedPosition(pCluster->GetTrackSeed()->GetTrackStateAtECal().GetPosition());
    const CartesianVector positionDifference(hitPosition - trackSeedPosition);
    const float separation(positionDifference.GetMagnitude());

    if (separation < m_maxTrackSeedSeparation)
    {
        const float dPerp((pCluster->GetInitialDirection().GetCrossProduct(positionDifference)).GetMagnitude());
        const float flexibility(1.f + (m_trackPathWidth * (separation / m_maxTrackSeedSeparation)));

        const float dCut ((ECAL == pCaloHit->GetHitType()) ?
            flexibility * (m_additionalPadWidthsECal * pCaloHit->GetCellLengthScale()) :
            flexibility * (m_additionalPadWidthsHCal * pCaloHit->GetCellLengthScale()) );

        if (0 == dCut)
            return STATUS_CODE_FAILURE;

        distance = dPerp / dCut;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_UNCHANGED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ForcedClusteringAlgorithm::SortByDistanceToTrackSeed(const CaloHitDistancePair &lhs, const CaloHitDistancePair &rhs)
{
    return (lhs.second < rhs.second);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ForcedClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_trackPathWidth = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackPathWidth", m_trackPathWidth));

    m_maxTrackSeedSeparation = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxTrackSeedSeparation", m_maxTrackSeedSeparation));

    m_additionalPadWidthsECal = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsECal", m_additionalPadWidthsECal));

    m_additionalPadWidthsHCal = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AdditionalPadWidthsHCal", m_additionalPadWidthsHCal));

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
