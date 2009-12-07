/**
 *  @file   PandoraPFANew/src/Helpers/FragmentRemovalHelper.cc
 * 
 *  @brief  Implementation of the fragment removal helper class.
 * 
 *  $Log: $
 */

#include "Helpers/FragmentRemovalHelper.h"
#include "Helpers/GeometryHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/Helix.h"
#include "Objects/Track.h"

namespace pandora
{

float FragmentRemovalHelper::GetFractionOfCloseHits(const Cluster *const pClusterI, const Cluster *const pClusterJ, const float distanceThreshold)
{
    const unsigned int nCaloHitsI(pClusterI->GetNCaloHits());

    if (0 == nCaloHitsI)
        return 0.;

    CaloHitVector caloHitVectorI, caloHitVectorJ;
    pClusterI->GetOrderedCaloHitList().GetCaloHitVector(caloHitVectorI);
    pClusterJ->GetOrderedCaloHitList().GetCaloHitVector(caloHitVectorJ);

    unsigned int nCloseHits(0);

    for (CaloHitVector::const_iterator iterI = caloHitVectorI.begin(), iterIEnd = caloHitVectorI.end(); iterI != iterIEnd; ++iterI)
    {
        const CartesianVector &positionVectorI((*iterI)->GetPositionVector());

        for (CaloHitVector::const_iterator iterJ = caloHitVectorJ.begin(), iterJEnd = caloHitVectorJ.end(); iterJ != iterJEnd; ++iterJ)
        {
            const float distance((positionVectorI - (*iterJ)->GetPositionVector()).GetMagnitude());

            if (distance < distanceThreshold)
            {
                nCloseHits++;
                break;
            }
        }
    }

    return static_cast<float>(nCloseHits) / static_cast<float>(nCaloHitsI);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float FragmentRemovalHelper::GetFractionOfHitsInCone(const Cluster *const pClusterI, const Cluster *const pClusterJ, const float coneCosineHalfAngle)
{
    const unsigned int nCaloHitsI(pClusterI->GetNCaloHits());

    if (0 == nCaloHitsI)
        return 0.;

    CartesianVector coneApex, coneDirection;
    const TrackList &associatedTrackList(pClusterJ->GetAssociatedTrackList());

    if (associatedTrackList.empty())
    {
        const PseudoLayer showerMaxLayer(pClusterJ->GetShowerMaxLayer());
        const CartesianVector showerMaxCentroid(pClusterJ->GetCentroid(showerMaxLayer));

        coneApex = showerMaxCentroid;
        coneDirection = showerMaxCentroid.GetUnitVector();
    }
    else
    {
        const Track *const pTrack(*(associatedTrackList.begin()));

        coneApex = pTrack->GetTrackStateAtECal().GetPosition();
        coneDirection = pTrack->GetTrackStateAtECal().GetMomentum().GetUnitVector();
    }

    unsigned int nHitsInCone(0);
    const OrderedCaloHitList &orderedCaloHitList(pClusterI->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const CaloHitList *const pCaloHitList(iter->second);

        for (CaloHitList::const_iterator hitIter = pCaloHitList->begin(), hitIterEnd = pCaloHitList->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector &hitPosition((*hitIter)->GetPositionVector());
            const CartesianVector positionDifference(hitPosition - coneApex);

            const float cosTheta(coneDirection.GetDotProduct(positionDifference.GetUnitVector()));

            if (cosTheta > coneCosineHalfAngle)
                nHitsInCone++;
        }
    }

    return static_cast<float>(nHitsInCone) / static_cast<float>(nCaloHitsI);
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer FragmentRemovalHelper::GetNLayersCrossed(const Helix *const pHelix, const float zStart, const float zEnd,
    const unsigned int nSamplingPoints)
{
    if ((0 == nSamplingPoints) || (1000 < nSamplingPoints))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    CartesianVector intersectionPoint;
    const CartesianVector &referencePoint(pHelix->GetReferencePoint());
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, != , pHelix->GetPointInZ(zStart, referencePoint, intersectionPoint));

    static const GeometryHelper *const pGeometryHelper = GeometryHelper::GetInstance();
    const PseudoLayer startLayer(pGeometryHelper->GetPseudoLayer(intersectionPoint));

    PseudoLayer currentLayer(startLayer);
    PseudoLayer layerCount(0);

    const float deltaZ((zEnd - zStart) / static_cast<float>(nSamplingPoints));

    for (float z = zStart; std::fabs(z) < std::fabs(zEnd + 0.5 * deltaZ); z += deltaZ)
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, != , pHelix->GetPointInZ(z, referencePoint, intersectionPoint));
        const PseudoLayer iLayer(pGeometryHelper->GetPseudoLayer(intersectionPoint));
        const bool isInECalGapRegion(pGeometryHelper->IsInECalGapRegion(intersectionPoint));

        if (iLayer != currentLayer)
        {
            if (!isInECalGapRegion)
                layerCount += ((iLayer > currentLayer) ? iLayer - currentLayer : currentLayer - iLayer);

            currentLayer = iLayer;
        }
    }

    return layerCount;
}

} // namespace pandora
