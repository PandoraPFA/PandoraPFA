/**
 *  @file   PandoraPFANew/src/Algorithms/FinalParticleIdAlgorithm.cc
 * 
 *  @brief  Implementation of the final particle id algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/FinalParticleIdAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode FinalParticleIdAlgorithm::Run()
{
    const ParticleFlowObjectList *pParticleFlowObjectList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pParticleFlowObjectList));

    for (ParticleFlowObjectList::const_iterator iter = pParticleFlowObjectList->begin(), iterEnd = pParticleFlowObjectList->end();
        iter != iterEnd; ++iter)
    {
        ParticleFlowObject *pParticleFlowObject = *iter;

        const TrackList &trackList(pParticleFlowObject->GetTrackList());
        const ClusterList &clusterList(pParticleFlowObject->GetClusterList());

        // Consider only pfos with a single cluster and no track sibling relationships
        if ((clusterList.size() != 1) || (trackList.empty()) || this->ContainsSiblingTrack(trackList))
            continue;

        const int charge(pParticleFlowObject->GetCharge());

        if (0 == charge)
            return STATUS_CODE_FAILURE;

        // Ignore particle flow objects already tagged as electrons or muons
        if ((std::abs(pParticleFlowObject->GetParticleId()) == E_MINUS) || (std::abs(pParticleFlowObject->GetParticleId()) == MU_MINUS))
            continue;

        Cluster *pCluster = *(clusterList.begin());

        // Run fast electron id, followed by fast muon id
        if (ParticleIdHelper::IsElectronFast(pCluster))
        {
            pParticleFlowObject->SetParticleId((charge < 0) ? E_MINUS : E_PLUS);
        }

        if (ParticleIdHelper::IsMuonFast(pCluster))
        {
            pParticleFlowObject->SetParticleId((charge < 0) ? MU_MINUS : MU_PLUS);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool FinalParticleIdAlgorithm::ContainsSiblingTrack(const TrackList &trackList) const
{
    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        if (!(*iter)->GetSiblingTrackList().empty())
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FinalParticleIdAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    return STATUS_CODE_SUCCESS;
}
