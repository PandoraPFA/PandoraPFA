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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateChargedPfos());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateNeutralPfos());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PfoCreationAlgorithm::CreateChargedPfos() const
{
    // Current track list should contain those tracks selected as "good" by the track preparation algorithm
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    for (TrackList::const_iterator iter = pTrackList->begin(), iterEnd = pTrackList->end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        // Specify the pfo parameters
        PandoraContentApi::ParticleFlowObject::Parameters pfoParameters;
        pfoParameters.m_chargeSign = pTrack->GetChargeSign();
        pfoParameters.m_particleId = ((pTrack->GetChargeSign() > 0) ? 211 : -211);
        pfoParameters.m_mass = pTrack->GetMass();
        pfoParameters.m_energy = pTrack->GetEnergyAtDca();
        pfoParameters.m_momentum = pTrack->GetMomentumAtDca();
        pfoParameters.m_trackList.insert(pTrack);

        // TODO Add any back scatter daughters

        // Add any cluster associated with this track to the pfo
        Cluster *pAssociatedCluster = NULL;

        if (STATUS_CODE_SUCCESS == pTrack->GetAssociatedCluster(pAssociatedCluster))
        {
            if (NULL == pAssociatedCluster)
                return STATUS_CODE_FAILURE;

            pfoParameters.m_clusterList.insert(pAssociatedCluster);

            // TODO check to see if cluster is leaving detector

            // TODO If the cluster energy is larger than reasonable for track, use cluster energy
        }

        // TODO Electron ID

        // TODO Split tracks

        // TODO Kinks

        // TODO Prongs

        // TODO V0s

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters));
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

        // TODO use BestEnergyEstimate for photon clusters too, if photon cluster energy corrections are made
        const bool isPhoton(pCluster->IsPhoton());
        float clusterEnergy(isPhoton ? pCluster->GetElectromagneticEnergy() : pCluster->GetBestEnergyEstimate());

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
        pfoParameters.m_chargeSign = 0;
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

    m_minClusterHadronicEnergy = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterHadronicEnergy", m_minClusterHadronicEnergy));

    return STATUS_CODE_SUCCESS;
}
