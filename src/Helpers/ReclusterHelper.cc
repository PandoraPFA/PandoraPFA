/**
 *  @file   PandoraPFANew/src/Helpers/ReclusterHelper.cc
 * 
 *  @brief  Implementation of the recluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ReclusterHelper.h"

#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>
#include <limits>

namespace pandora
{

StatusCode ReclusterHelper::EvaluateTrackClusterCompatibility(const Cluster *const pCluster, const TrackList &trackList, float &chi)
{
    float trackEnergySum(0.);

    for (TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
        trackEnergySum += (*trackIter)->GetEnergyAtDca();

    static const float hadronicEnergyResolution(PandoraSettings::GetInstance()->GetHadronicEnergyResolution());

    if ((0. == trackEnergySum) || (0. == hadronicEnergyResolution))
        return STATUS_CODE_FAILURE;

    const float sigmaE(hadronicEnergyResolution * trackEnergySum / std::sqrt(trackEnergySum));
    chi = ((pCluster->GetHadronicEnergy() - trackEnergySum) / sigmaE);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::ExtractReclusterResults(const ClusterList *const pReclusterCandidatesList, ReclusterResult &reclusterResult)
{
    unsigned int nExcessTrackAssociations(0);
    float chi(0.), chi2(0.), dof(0.), minTrackAssociationEnergy(std::numeric_limits<float>::max());

    for (ClusterList::const_iterator iter = pReclusterCandidatesList->begin(), iterEnd = pReclusterCandidatesList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        const TrackList &trackList(pCluster->GetAssociatedTrackList());
        const unsigned int nTrackAssociations(trackList.size());

        if (0 == nTrackAssociations)
            continue;

        const float clusterEnergy(pCluster->GetHadronicEnergy());

        if (clusterEnergy < minTrackAssociationEnergy)
            minTrackAssociationEnergy = clusterEnergy;

        nExcessTrackAssociations += nTrackAssociations - 1;

        float newChi(0.);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ReclusterHelper::EvaluateTrackClusterCompatibility(pCluster, trackList, newChi));

        chi2 += newChi * newChi;
        chi += newChi;
        dof += 1.;
    }

    if (0. == dof)
        return STATUS_CODE_FAILURE;

    reclusterResult.SetChi(chi);
    reclusterResult.SetChi2PerDof(chi2 /= dof);
    reclusterResult.SetMinTrackAssociationEnergy(minTrackAssociationEnergy);
    reclusterResult.SetNExcessTrackAssociations(nExcessTrackAssociations);

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
