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

#include <limits>

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

float FragmentRemovalHelper::GetFractionOfHitsInCone(const Cluster *const pClusterI, Cluster *const pClusterJ, const float coneCosineHalfAngle)
{
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

    return FragmentRemovalHelper::GetFractionOfHitsInCone(pClusterI, coneApex, coneDirection, coneCosineHalfAngle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float FragmentRemovalHelper::GetFractionOfHitsInCone(const Cluster *const pCluster, const Track *const pTrack, const float coneCosineHalfAngle)
{
    const CartesianVector coneApex(pTrack->GetTrackStateAtECal().GetPosition());
    const CartesianVector coneDirection(pTrack->GetTrackStateAtECal().GetMomentum().GetUnitVector());

    return FragmentRemovalHelper::GetFractionOfHitsInCone(pCluster, coneApex, coneDirection, coneCosineHalfAngle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float FragmentRemovalHelper::GetFractionOfHitsInCone(const Cluster *const pCluster, const CartesianVector &coneApex,
    const CartesianVector &coneDirection, const float coneCosineHalfAngle)
{
    const unsigned int nCaloHits(pCluster->GetNCaloHits());

    if (0 == nCaloHits)
        return 0.;

    unsigned int nHitsInCone(0);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

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

    return static_cast<float>(nHitsInCone) / static_cast<float>(nCaloHits);
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

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FragmentRemovalHelper::GetClusterHelixDistance(const Cluster *const pCluster, const Helix *const pHelix, const PseudoLayer startLayer,
    const PseudoLayer endLayer, const unsigned int maxOccupiedLayers, float &closestDistanceToHit, float &meanDistanceToHits)
{
    if (startLayer > endLayer)
        return STATUS_CODE_INVALID_PARAMETER;

    unsigned int nHits(0), nOccupiedLayers(0);
    float sumDistanceToHits(0.), minDistanceToHit(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        if (++nOccupiedLayers > maxOccupiedLayers)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CartesianVector distanceVector;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetDistanceToPoint((*hitIter)->GetPositionVector(), distanceVector));

            const float distance(distanceVector.GetZ());

            if (distance < minDistanceToHit)
                minDistanceToHit = distance;

            sumDistanceToHits += distance;
            nHits++;
        }
    }

    if (0 != nHits)
    {
        meanDistanceToHits = sumDistanceToHits / static_cast<float>(nHits);
        closestDistanceToHit = minDistanceToHit;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_NOT_FOUND;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FragmentRemovalHelper::GetClusterContactDetails(const Cluster *const pClusterI, const Cluster *const pClusterJ,
    const float distanceThreshold, unsigned int &nContactLayers, float &contactFraction)
{
    const PseudoLayer startLayer(std::max(pClusterI->GetInnerPseudoLayer(), pClusterJ->GetInnerPseudoLayer()));
    const PseudoLayer endLayer(std::min(pClusterI->GetOuterPseudoLayer(), pClusterJ->GetOuterPseudoLayer()));
    const OrderedCaloHitList &orderedCaloHitListI(pClusterI->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pClusterJ->GetOrderedCaloHitList());

    unsigned int nLayersCompared(0), nLayersInContact(0);

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.find(iLayer), iterJ = orderedCaloHitListJ.find(iLayer);

        if ((orderedCaloHitListI.end() == iterI) || (orderedCaloHitListJ.end() == iterJ))
            continue;

        nLayersCompared++;
        bool isLayerDone(false);

        for (CaloHitList::const_iterator hitIterI = iterI->second->begin(), hitIterIEnd = iterI->second->end(); hitIterI != hitIterIEnd; ++hitIterI)
        {
            const CartesianVector &positionI((*hitIterI)->GetPositionVector());
            const float separationCut(1.5f * (*hitIterI)->GetCellLengthScale() * distanceThreshold);

            for (CaloHitList::const_iterator hitIterJ = iterJ->second->begin(), hitIterJEnd = iterJ->second->end(); hitIterJ != hitIterJEnd; ++hitIterJ)
            {
                const CartesianVector &positionJ((*hitIterJ)->GetPositionVector());
                const CartesianVector positionDifference(positionI - positionJ);
                const float separation(positionDifference.GetMagnitude());

                if (separation < separationCut)
                {
                    nLayersInContact++;
                    isLayerDone = true;
                    break;
                }
            }

            if (isLayerDone)
                break;
        }
    }

    if (nLayersCompared > 0)
    {
        contactFraction = static_cast<float>(nLayersInContact) / static_cast<float>(nLayersCompared);
        nContactLayers = nLayersInContact;
        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_NOT_FOUND;
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

} // namespace pandora
