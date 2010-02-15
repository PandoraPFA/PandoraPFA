/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstruction/PfoCreationAlgorithm.cc
 * 
 *  @brief  Implementation of the pfo creation algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/PfoCreationAlgorithm.h"

using namespace pandora;

StatusCode PfoCreationAlgorithm::Run()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateTrackBasedPfos());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateNeutralPfos());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PfoCreationAlgorithm::CreateTrackBasedPfos() const
{
    // Current track list should contain those tracks selected as "good" by the track preparation algorithm
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        // Specify the pfo parameters
        PandoraContentApi::ParticleFlowObject::Parameters pfoParameters;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetTrackBasedPfoParameters(pTrack, pfoParameters));

        // Walk along list of associated daughter/sibling tracks and their cluster associations
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PopulateTrackBasedPfo(pTrack, pfoParameters));

        // Create the pfo
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PfoCreationAlgorithm::SetTrackBasedPfoParameters(Track *const pTrack, PfoParameters &pfoParameters) const
{
    // TODO More sophisticated particle id - currently assumes pion or photon conversion

    const TrackList &siblingTrackList(pTrack->GetSiblingTrackList());

    // Single parent track as pfo target
    if (siblingTrackList.empty())
    {
        pfoParameters.m_charge = pTrack->GetChargeSign();
        pfoParameters.m_particleId = ((pTrack->GetChargeSign() > 0) ? 211 : -211);
        pfoParameters.m_mass = pTrack->GetMass();
        pfoParameters.m_energy = pTrack->GetEnergyAtDca();
        pfoParameters.m_momentum = pTrack->GetMomentumAtDca();

        return STATUS_CODE_SUCCESS;
    }

    // Sibling tracks as first evidence of pfo target
    int charge(0);
    float energy(0.f);
    CartesianVector momentum(0.f, 0.f, 0.f);

    for (TrackList::const_iterator iter = siblingTrackList.begin(), iterEnd = siblingTrackList.end(); iter != iterEnd; ++iter)
    {
        Track *pSiblingTrack = *iter;

        charge += pSiblingTrack->GetChargeSign();
        energy += pSiblingTrack->GetEnergyAtDca();
        momentum += pSiblingTrack->GetMomentumAtDca();
    }

    pfoParameters.m_charge = charge;
    pfoParameters.m_energy = energy;
    pfoParameters.m_momentum = momentum;
    pfoParameters.m_mass = std::sqrt(std::max(energy * energy - momentum.GetDotProduct(momentum), 0.f));
    pfoParameters.m_particleId = 22;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PfoCreationAlgorithm::PopulateTrackBasedPfo(Track *const pTrack, PfoParameters &pfoParameters, const bool readSiblingInfo) const
{
    // Add track to the pfo
    pfoParameters.m_trackList.insert(pTrack);

    // Add any cluster associated with this track to the pfo
    Cluster *pAssociatedCluster = NULL;

    if (STATUS_CODE_SUCCESS == pTrack->GetAssociatedCluster(pAssociatedCluster))
    {
        if (NULL == pAssociatedCluster)
            return STATUS_CODE_FAILURE;

        pfoParameters.m_clusterList.insert(pAssociatedCluster);
    }

    // Consider any sibling tracks
    if (readSiblingInfo)
    {
        const TrackList &siblingTrackList(pTrack->GetSiblingTrackList());

        for (TrackList::const_iterator iter = siblingTrackList.begin(), iterEnd = siblingTrackList.end(); iter != iterEnd; ++iter)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PopulateTrackBasedPfo(*iter, pfoParameters, false));
        }
    }

    // Consider any daughter tracks
    const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());

    for (TrackList::const_iterator iter = daughterTrackList.begin(), iterEnd = daughterTrackList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PopulateTrackBasedPfo(*iter, pfoParameters));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PfoCreationAlgorithm::CreateNeutralPfos() const
{
    // Create a list containing both input and photon clusters
    const ClusterList *pInputClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pInputClusterList));

    ClusterList combinedClusterList(pInputClusterList->begin(), pInputClusterList->end());

    if (m_shouldUsePhotonClusters)
    {
        const ClusterList *pPhotonClusterList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, m_photonClusterListName, pPhotonClusterList));
        combinedClusterList.insert(pPhotonClusterList->begin(), pPhotonClusterList->end());
    }

    // Examine clusters with no associated tracks to form neutral pfos
    for (ClusterList::const_iterator iter = combinedClusterList.begin(), iterEnd = combinedClusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!pCluster->GetAssociatedTrackList().empty())
            continue;

        if (pCluster->GetNCaloHits() < m_minHitsInCluster)
            continue;

        // TODO use BestEnergyEstimate, if energy corrections are made
        const bool isPhoton(pCluster->IsPhoton());
        float clusterEnergy(isPhoton ? pCluster->GetElectromagneticEnergy() : pCluster->GetHadronicEnergy());

        // Veto non-photon clusters below hadronic energy threshold and those occupying a single layer
        if (!isPhoton)
        {
            if (clusterEnergy < m_minClusterHadronicEnergy)
                continue;

            if (pCluster->GetInnerPseudoLayer() == pCluster->GetOuterPseudoLayer())
                continue;
        }

        // Specify the pfo parameters
        PandoraContentApi::ParticleFlowObject::Parameters pfoParameters;
        pfoParameters.m_particleId = (isPhoton ? 22 : 2112);
        pfoParameters.m_charge = 0;
        pfoParameters.m_mass = 0;
        pfoParameters.m_energy = clusterEnergy;
        pfoParameters.m_clusterList.insert(pCluster);

        // TODO check position matches lcio::ClusterImpl::GetPosition()
        const CartesianVector positionWeight(pCluster->GetCentroid(pCluster->GetInnerPseudoLayer()).GetUnitVector());

        pfoParameters.m_momentum = CartesianVector(clusterEnergy * positionWeight.GetX(),
            clusterEnergy * positionWeight.GetY(),
            clusterEnergy * positionWeight.GetZ());

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PfoCreationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_shouldUsePhotonClusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUsePhotonClusters", m_shouldUsePhotonClusters));

    if (m_shouldUsePhotonClusters)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
            "PhotonClusterListName", m_photonClusterListName));
    }

    m_minClusterHadronicEnergy = 0.25f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterHadronicEnergy", m_minClusterHadronicEnergy));

    m_minHitsInCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    return STATUS_CODE_SUCCESS;
}
