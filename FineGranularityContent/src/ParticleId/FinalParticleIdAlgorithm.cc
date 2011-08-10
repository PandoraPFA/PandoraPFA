/**
 *  @file   PandoraPFANew/FineGranularityContent/src/ParticleId/FinalParticleIdAlgorithm.cc
 * 
 *  @brief  Implementation of the final particle id algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ParticleId/FinalParticleIdAlgorithm.h"

using namespace pandora;

StatusCode FinalParticleIdAlgorithm::Run()
{
    const PfoList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pPfoList));

    for (PfoList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end();
        iter != iterEnd; ++iter)
    {
        ParticleFlowObject *pPfo = *iter;

        const TrackList &trackList(pPfo->GetTrackList());
        const ClusterList &clusterList(pPfo->GetClusterList());

        // Consider only pfos with a single cluster and no track sibling relationships
        if ((clusterList.size() != 1) || (trackList.empty()) || this->ContainsSiblingTrack(trackList))
            continue;

        const int charge(pPfo->GetCharge());

        if (0 == charge)
            return STATUS_CODE_FAILURE;

        // Ignore particle flow objects already tagged as electrons or muons
        if ((std::abs(pPfo->GetParticleId()) == E_MINUS) || (std::abs(pPfo->GetParticleId()) == MU_MINUS))
            continue;

        Cluster *pCluster = *(clusterList.begin());

        // Run fast electron id, followed by fast muon id
        if (ParticleIdHelper::IsElectronFast(pCluster))
        {
            pPfo->SetParticleId((charge < 0) ? E_MINUS : E_PLUS);
        }

        if (ParticleIdHelper::IsMuonFast(pCluster))
        {
            pPfo->SetParticleId((charge < 0) ? MU_MINUS : MU_PLUS);
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
