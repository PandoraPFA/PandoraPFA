/**
 *  @file   PandoraPFANew/src/Algorithms/FragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the fragment removal algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/FragmentRemovalAlgorithm.h"

using namespace pandora;

StatusCode FragmentRemovalAlgorithm::Run()
{
    // Run fragment removal daughter algorithms
    for (StringVector::const_iterator iter = m_fragmentRemovalAlgorithms.begin(), iterEnd = m_fragmentRemovalAlgorithms.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "fragmentRemovalAlgorithms",
        m_fragmentRemovalAlgorithms));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ClusterContact::ClusterContact(Cluster *const pDaughterCluster, Cluster *const pParentCluster) :
    m_pDaughterCluster(pDaughterCluster),
    m_pParentCluster(pParentCluster),
    m_parentClusterEnergy(pParentCluster->GetHadronicEnergy()),
    m_nContactLayers(0),
    m_contactFraction(0.f),
    m_meanDistanceToHelix(std::numeric_limits<float>::max()),
    m_closestDistanceToHelix(std::numeric_limits<float>::max())
{
    m_distanceToClosestHit = ClusterHelper::GetDistanceToClosestHit(pDaughterCluster, pParentCluster);
    m_coneFraction1 = FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, 0.90f);
    m_coneFraction2 = FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, 0.95f);
    m_coneFraction3 = FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, 0.985f);
    m_closeHitFraction1 = FragmentRemovalHelper::GetFractionOfCloseHits(pDaughterCluster, pParentCluster, 100.f);
    m_closeHitFraction2 = FragmentRemovalHelper::GetFractionOfCloseHits(pDaughterCluster, pParentCluster, 50.f);

    (void) FragmentRemovalHelper::GetClusterContactDetails(pDaughterCluster, pParentCluster, 2.f, m_nContactLayers, m_contactFraction);

    // Configure range of layers in which daughter cluster will be compared to helix fits
    const PseudoLayer startLayer(pDaughterCluster->GetInnerPseudoLayer());

    const PseudoLayer endLayer((pParentCluster->GetMipFraction() > 0.8f) ?
        startLayer + 20 : std::max(startLayer + 20, pParentCluster->GetOuterPseudoLayer() + 10));

    const unsigned int maxOccupiedLayers((pParentCluster->GetMipFraction() > 0.8f) ?
        std::numeric_limits<unsigned int>::max() : 9);

    // Calculate closest distance between daughter cluster and helix fits to parent associated tracks
    float trackEnergySum(0.);
    const TrackList &parentTrackList(pParentCluster->GetAssociatedTrackList());

    for (TrackList::const_iterator iter = parentTrackList.begin(), iterEnd = parentTrackList.end(); iter != iterEnd; ++iter)
    {
        trackEnergySum += (*iter)->GetEnergyAtDca();
        float meanDistanceToHelix(std::numeric_limits<float>::max()), closestDistanceToHelix(std::numeric_limits<float>::max());

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, FragmentRemovalHelper::GetClusterHelixDistance(pDaughterCluster,
            (*iter)->GetHelixFitAtECal(), startLayer, endLayer, maxOccupiedLayers, closestDistanceToHelix, meanDistanceToHelix));

        if (closestDistanceToHelix < m_closestDistanceToHelix)
        {
            m_meanDistanceToHelix = meanDistanceToHelix;
            m_closestDistanceToHelix = closestDistanceToHelix;
        }
    }

    m_parentTrackEnergy = trackEnergySum;
}
