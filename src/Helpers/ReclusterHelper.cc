/**
 *  @file   PandoraPFANew/src/Helpers/ReclusterHelper.cc
 * 
 *  @brief  Implementation of the recluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ReclusterHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"
#include "Objects/Track.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>
#include <limits>

namespace pandora
{

float ReclusterHelper::GetTrackClusterCompatibility(const Cluster *const pCluster, const TrackList &trackList)
{
    float trackEnergySum(0.);

    for (TrackList::const_iterator trackIter = trackList.begin(), trackIterEnd = trackList.end(); trackIter != trackIterEnd; ++trackIter)
        trackEnergySum += (*trackIter)->GetEnergyAtDca();

    static const float hadronicEnergyResolution(PandoraSettings::GetInstance()->GetHadronicEnergyResolution());

    if ((0. == trackEnergySum) || (0. == hadronicEnergyResolution))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergySum / std::sqrt(trackEnergySum));
    const float chi((pCluster->GetTrackComparisonEnergy() - trackEnergySum) / sigmaE);

    return chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ReclusterHelper::GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy)
{
    static const float hadronicEnergyResolution(PandoraSettings::GetInstance()->GetHadronicEnergyResolution());

    if ((0. == trackEnergy) || (0. == hadronicEnergyResolution))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergy / std::sqrt(trackEnergy));
    const float chi((clusterEnergy - trackEnergy) / sigmaE);

    return chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::ExtractReclusterResults(const ClusterList *const pReclusterCandidatesList, ReclusterResult &reclusterResult)
{
    unsigned int nExcessTrackAssociations(0);
    float chi(0.), chi2(0.), dof(0.), unassociatedEnergy(0.), minTrackAssociationEnergy(std::numeric_limits<float>::max());

    for (ClusterList::const_iterator iter = pReclusterCandidatesList->begin(), iterEnd = pReclusterCandidatesList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        const TrackList &trackList(pCluster->GetAssociatedTrackList());
        const unsigned int nTrackAssociations(trackList.size());
        const float clusterEnergy(pCluster->GetHadronicEnergy());

        if (0 == nTrackAssociations)
        {
            unassociatedEnergy += clusterEnergy;
            continue;
        }

        if (clusterEnergy < minTrackAssociationEnergy)
            minTrackAssociationEnergy = clusterEnergy;

        nExcessTrackAssociations += nTrackAssociations - 1;

        const float newChi(ReclusterHelper::GetTrackClusterCompatibility(pCluster, trackList));

        chi2 += newChi * newChi;
        chi += newChi;
        dof += 1.;
    }

    if (0. == dof)
        return STATUS_CODE_FAILURE;

    reclusterResult.SetChi(chi);
    reclusterResult.SetChi2(chi2);
    reclusterResult.SetChiPerDof(chi /= dof);
    reclusterResult.SetChi2PerDof(chi2 /= dof);
    reclusterResult.SetUnassociatedEnergy(unassociatedEnergy);
    reclusterResult.SetMinTrackAssociationEnergy(minTrackAssociationEnergy);
    reclusterResult.SetNExcessTrackAssociations(nExcessTrackAssociations);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusterHelper::ReadSettings(const TiXmlHandle xmlHandle)
{
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
