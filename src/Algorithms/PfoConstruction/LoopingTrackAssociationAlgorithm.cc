/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstruction/LoopingTrackAssociationAlgorithm.cc
 * 
 *  @brief  Implementation of the looping track association algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/LoopingTrackAssociationAlgorithm.h"

using namespace pandora;

StatusCode LoopingTrackAssociationAlgorithm::Run()
{
    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    static const float endCapZPosition(GeometryHelper::GetInstance()->GetECalEndCapParameters().GetInnerZCoordinate());

    // Loop over all unassociated tracks in the current track list
    for (TrackList::const_iterator iterT = pTrackList->begin(), iterTEnd = pTrackList->end(); iterT != iterTEnd; ++iterT)
    {
        Track *pTrack = *iterT;

        // Use only unassociated tracks that are flagged as reaching ECal
        if (pTrack->HasAssociatedCluster() || !pTrack->ReachesECal())
            continue;

        if (!pTrack->GetDaughterTrackList().empty())
            continue;

        // 
        const float trackECalZPosition(pTrack->GetTrackStateAtECal().GetPosition().GetZ());

        if (endCapZPosition - std::fabs(trackECalZPosition) > 50.f)
            continue;

        // 
        const Helix *const pHelix(pTrack->GetHelixFitAtECal());

        const float helixXCentre(pHelix->GetXCentre());
        const float helixYCentre(pHelix->GetYCentre());
        const float helixRadius(pHelix->GetRadius());
        const float helixTanLambda(pHelix->GetTanLambda());

        const float helixDCosZ(helixTanLambda / std::sqrt(1.f + helixTanLambda * helixTanLambda));
        const float trackEnergy(pTrack->GetEnergyAtDca());

        // 
        Cluster *pBestCluster(NULL);
        float smallestDeltaR(std::numeric_limits<float>::max());

        for (ClusterList::const_iterator iterC = pClusterList->begin(), iterCEnd = pClusterList->end(); iterC != iterCEnd; ++iterC)
        {
            Cluster *pCluster = *iterC;

            if (!pCluster->GetAssociatedTrackList().empty() || (0 == pCluster->GetNCaloHits()))
                continue;

            // 
            const PseudoLayer innerLayer(pCluster->GetInnerPseudoLayer());

            if (innerLayer > 10)
                continue;

            // 
            const float clusterZPosition(pCluster->GetCentroid(innerLayer).GetZ());

            if (endCapZPosition - std::fabs(clusterZPosition) > 50.f)
                continue;

            if (endCapZPosition * trackECalZPosition < 0.f)
                continue;

            // Check consistency of track momentum and cluster energy
            const float chi(ReclusterHelper::GetTrackClusterCompatibility(pCluster->GetHadronicEnergy(), trackEnergy));

            if (std::fabs(chi) > 2.f)
                continue;

            //
            const CartesianVector innerCentroid(pCluster->GetCentroid(innerLayer));

            const float innerLayerDeltaX(innerCentroid.GetX() - helixXCentre);
            const float innerLayerDeltaY(innerCentroid.GetY() - helixYCentre);
            const float innerLayerDeltaR(std::sqrt((innerLayerDeltaX * innerLayerDeltaX) + (innerLayerDeltaY * innerLayerDeltaY)) - std::fabs(helixRadius));

            const float meanDeltaR(this->GetMeanDeltaR(pCluster, helixXCentre, helixYCentre, helixRadius));

            // 
            const bool isInRangeInner((innerLayerDeltaR < 50.f) && (innerLayerDeltaR > -100.f));
            const bool isInRangeMean((meanDeltaR < 50.f) && (meanDeltaR > -100.f));

            if (!isInRangeInner && !isInRangeMean)
                continue;

            // ATTN: Have changed order of std::min and std::fabs here
            const float deltaR(std::min(std::fabs(innerLayerDeltaR), std::fabs(meanDeltaR)));

            // 
            CartesianVector helixDirection;

            if (0.f != innerLayerDeltaY)
            {
                float helixDCosX((1.f - helixDCosZ * helixDCosZ) / (1.f + ((innerLayerDeltaX * innerLayerDeltaX) / (innerLayerDeltaY * innerLayerDeltaY))));
                helixDCosX = std::sqrt(std::max(helixDCosX, 0.f));

                if (innerLayerDeltaY * helixRadius < 0)
                    helixDCosX *= -1.f;

                helixDirection.SetValues(helixDCosX, -(innerLayerDeltaX / innerLayerDeltaY) * helixDCosX, helixDCosZ);
            }
            else
            {
                float helixDCosY(1.f - helixDCosZ * helixDCosZ);
                helixDCosY = std::sqrt(std::max(helixDCosY, 0.f));

                if (innerLayerDeltaX * helixRadius > 0)
                    helixDCosY *= -1.f;

                helixDirection.SetValues(0.f, helixDCosY, helixDCosZ);
            }

            // 
            ClusterHelper::ClusterFitResult clusterFitResult;
            if (STATUS_CODE_SUCCESS != ClusterHelper::FitLayers(pCluster, innerLayer, innerLayer + 10, clusterFitResult))
                continue;

            // 
            const float directionCosine(helixDirection.GetDotProduct(clusterFitResult.GetDirection()));

            if ((pCluster->GetMipFraction() < 0.5) && (directionCosine < 0.975))
                continue;

            // 
            bool isPossibleMatch(false);

            if (directionCosine > 925.f)
            {
                isPossibleMatch = true;
            }
            else if ((directionCosine > 0.85f) && (deltaR < 50.f))
            {
                isPossibleMatch = true;
            }
            else if ((directionCosine > 0.75f) && (deltaR < 25.f))
            {
                isPossibleMatch = true;
            }
            else if ((directionCosine > 0.f) && (deltaR < 10.f))
            {
                isPossibleMatch = true;
            }

            if (isPossibleMatch)
            {
                smallestDeltaR = deltaR;
                pBestCluster = pCluster;
            }
        }

        if (NULL != pBestCluster)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pTrack, pBestCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float LoopingTrackAssociationAlgorithm::GetMeanDeltaR(Cluster *const pCluster, const float helixXCentre, const float helixYCentre,
    const float helixRadius) const
{
    float deltaRSum(0.f);
    unsigned int nContributions(0);

    const PseudoLayer endLayer(pCluster->GetInnerPseudoLayer() + 10);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > endLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector &hitPosition((*hitIter)->GetPositionVector());
            const float hitDeltaX(hitPosition.GetX() - helixXCentre);
            const float hitDeltaY(hitPosition.GetY() - helixYCentre);

            deltaRSum += std::sqrt((hitDeltaX * hitDeltaX) + (hitDeltaY * hitDeltaY));
            nContributions++;
        }
    }

    if (0 == nContributions)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const float meanDeltaR((deltaRSum / static_cast<float>(nContributions)) - std::fabs(helixRadius));

    return meanDeltaR;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LoopingTrackAssociationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}
