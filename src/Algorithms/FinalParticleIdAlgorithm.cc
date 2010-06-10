/**
 *  @file   PandoraPFANew/src/Algorithms/FinalParticleIdAlgorithm.cc
 * 
 *  @brief  Implementation of the final particle id algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/FinalParticleIdAlgorithm.h"

#include "Helpers/ParticleIdHelper.h"

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

        // Consider only pfos with a single track and a single cluster
        if ((trackList.size() != 1) || (clusterList.size() != 1))
            continue;

        const int charge(pParticleFlowObject->GetCharge());

        if (0 == charge)
            return STATUS_CODE_FAILURE;

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

StatusCode FinalParticleIdAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    return STATUS_CODE_SUCCESS;
}
