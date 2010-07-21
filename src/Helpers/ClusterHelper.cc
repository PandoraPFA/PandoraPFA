/**
 *  @file   PandoraPFANew/src/Helpers/ClusterHelper.cc
 * 
 *  @brief  Implementation of the cluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ClusterHelper.h"
#include "Helpers/GeometryHelper.h"
#include "Helpers/XmlHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"
#include "Objects/Track.h"

#include <cmath>
#include <limits>

namespace pandora
{

StatusCode ClusterHelper::FitStart(const Cluster *const pCluster, unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult)
{
    if (maxOccupiedLayers < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    unsigned int occupiedLayerCount(0);

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (++occupiedLayerCount > maxOccupiedLayers)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitEnd(const Cluster *const pCluster, unsigned int maxOccupiedLayers, ClusterFitResult &clusterFitResult)
{
    if (maxOccupiedLayers < 2)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    unsigned int occupiedLayerCount(0);

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_reverse_iterator iter = orderedCaloHitList.rbegin(), iterEnd = orderedCaloHitList.rend(); iter != iterEnd; ++iter)
    {
        if (++occupiedLayerCount > maxOccupiedLayers)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitLayers(const Cluster *const pCluster, PseudoLayer startLayer, PseudoLayer endLayer, ClusterFitResult &clusterFitResult)
{
    if (startLayer >= endLayer)
        return STATUS_CODE_INVALID_PARAMETER;

    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer pseudoLayer(iter->first);

        if (startLayer > pseudoLayer)
            continue;

        if (endLayer < pseudoLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitPoints(const Cluster *const pCluster, ClusterFitResult &clusterFitResult)
{
    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    const unsigned int listSize(orderedCaloHitList.size());

    if (0 == listSize)
        return STATUS_CODE_NOT_INITIALIZED;

    if (listSize < 2)
        return STATUS_CODE_OUT_OF_RANGE;

    ClusterFitPointList clusterFitPointList;
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            clusterFitPointList.push_back(ClusterFitPoint(*hitIter));
        }
    }

    return FitPoints(clusterFitPointList, clusterFitResult);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitPoints(const ClusterFitPointList &clusterFitPointList, ClusterFitResult &clusterFitResult)
{
    if (clusterFitPointList.size() <= 1)
        return STATUS_CODE_INVALID_PARAMETER;

    float sumX(0.), sumY(0.), sumZ(0.);
    clusterFitResult.Reset();

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        sumX += iter->GetPosition().GetX();
        sumY += iter->GetPosition().GetY();
        sumZ += iter->GetPosition().GetZ();
    }

    const float meanZ(sumZ / static_cast<float>(clusterFitPointList.size()));
    static const float eCalEndCapZCoordinate(GeometryHelper::GetInstance()->GetECalEndCapParameters().GetInnerZCoordinate());
    const bool isInBarrelRegion(std::fabs(meanZ) < eCalEndCapZCoordinate);

    if (isInBarrelRegion)
    {
        const float meanX(sumX / static_cast<float>(clusterFitPointList.size()));
        const float meanY(sumY / static_cast<float>(clusterFitPointList.size()));
        const float rXY(std::sqrt(meanX * meanX + meanY * meanY));

        if (0 == rXY)
            return STATUS_CODE_FAILURE;

        return FitBarrelPoints(clusterFitPointList, meanX / rXY, meanY / rXY, clusterFitResult);
    }
    else
    {
        const bool isPositiveZ(meanZ > 0);
        return FitEndCapPoints(clusterFitPointList, isPositiveZ, clusterFitResult);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitBarrelPoints(const ClusterFitPointList &clusterFitPointList, float cosTheta, float sinTheta,
    ClusterFitResult &clusterFitResult)
{
    // Extract the data
    double sumU(0.), sumV(0.), sumZ(0.);
    double sumVU(0.), sumZU(0.), sumUU(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const double u(cosTheta * iter->GetPosition().GetX() + sinTheta * iter->GetPosition().GetY());
        const double v(cosTheta * iter->GetPosition().GetY() - sinTheta * iter->GetPosition().GetX());
        const double z(iter->GetPosition().GetZ());

        sumU += u; sumV += v; sumZ += z;
        sumVU += v * u; sumZU += z * u; sumUU += u * u;
    }

    // Perform the fit
    const double nPoints(static_cast<double>(clusterFitPointList.size()));
    const double denominatorU(sumU * sumU - nPoints * sumUU);

    if (0 == denominatorU)
        return STATUS_CODE_FAILURE;

    const double aV((sumU * sumV - nPoints * sumVU) / denominatorU);
    const double bV((sumV - aV * sumU) / nPoints);
    const double aZ((sumU * sumZ - nPoints * sumZU) / denominatorU);
    const double bZ((sumZ - aZ * sumU) / nPoints);

    if (0 == aV)
        return STATUS_CODE_FAILURE;

    const double bU(-bV / aV);
    const double r(std::sqrt(1. + aV * aV + aZ * aZ));
    const double dirU(1. / r), dirV(aV / r), dirZ(aZ / r);
    CartesianVector direction(static_cast<float>(cosTheta * dirU - sinTheta * dirV), static_cast<float>(sinTheta * dirU + cosTheta * dirV), static_cast<float>(dirZ));
    CartesianVector intercept(static_cast<float>(cosTheta * bU), static_cast<float>(sinTheta * bU), static_cast<float>(aZ * bU + bZ));

    float dirCosR(direction.GetDotProduct(intercept) / intercept.GetMagnitude());

    if (0 > dirCosR)
    {
        dirCosR = -dirCosR;
        direction = CartesianVector(0, 0, 0) - direction;
    }

    // Now calculate something like a chi2
    double chi2_V(0.), chi2_Z(0.), rms(0.);
    double sumA(0.), sumL(0.), sumAL(0.), sumLL(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const double u(cosTheta * iter->GetPosition().GetX() + sinTheta * iter->GetPosition().GetY());
        const double v(cosTheta * iter->GetPosition().GetY() - sinTheta * iter->GetPosition().GetX());
        const double z(iter->GetPosition().GetZ());

        const double error(iter->GetCellSize() / 3.46);
        const double chiV((v - aV * u - bV) / error);
        const double chiZ((z - aZ * u - bZ) / error);

        chi2_V += chiV * chiV;
        chi2_Z += chiZ * chiZ;

        const CartesianVector difference(iter->GetPosition() - intercept);
        rms += (direction.GetCrossProduct(difference)).GetMagnitudeSquared();

        const float a(direction.GetDotProduct(difference));
        const float l(static_cast<float>(iter->GetPseudoLayer()));

        sumA += a; sumL += l; sumAL += a * l; sumLL += l * l;
    }

    const double denominatorL(sumL * sumL - nPoints * sumLL);
    if (0 != denominatorL)
    {
        if (((sumL * sumA - nPoints * sumAL) / denominatorL) < 0)
            direction = CartesianVector(0, 0, 0) - direction;
    }

    clusterFitResult.SetDirection(direction);
    clusterFitResult.SetIntercept(intercept);
    clusterFitResult.SetChi2(static_cast<float>((chi2_V + chi2_Z) / nPoints));
    clusterFitResult.SetRms(static_cast<float>(std::sqrt(rms / nPoints)));
    clusterFitResult.SetRadialDirectionCosine(dirCosR);
    clusterFitResult.SetSuccessFlag(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitEndCapPoints(const ClusterFitPointList &clusterFitPointList, bool isPositiveZ, ClusterFitResult &clusterFitResult)
{
    // Extract the data
    double sumX(0.), sumY(0.), sumZ(0.);
    double sumXZ(0.), sumYZ(0.), sumZZ(0.);

    static const float eCalEndCapZCoordinate(GeometryHelper::GetInstance()->GetECalEndCapParameters().GetInnerZCoordinate());

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const float x(iter->GetPosition().GetX());
        const float y(iter->GetPosition().GetY());
        const float z((isPositiveZ) ? iter->GetPosition().GetZ() - eCalEndCapZCoordinate : iter->GetPosition().GetZ() + eCalEndCapZCoordinate);

        sumX += x; sumY += y; sumZ += z;
        sumXZ += x * z; sumYZ += y * z; sumZZ += z * z;
    }

    // Perform the fit
    const double nPoints(static_cast<double>(clusterFitPointList.size()));
    const double denominatorZ(sumZ * sumZ - nPoints * sumZZ);

    if (0 == denominatorZ)
        return STATUS_CODE_FAILURE;

    const double aX((sumZ * sumX - nPoints * sumXZ) / denominatorZ);
    const double bX((sumX - aX * sumZ) / nPoints);
    const double aY((sumZ * sumY - nPoints * sumYZ) / denominatorZ);
    const double bY((sumY - aY * sumZ) / nPoints);

    const double r(std::sqrt(1. + aX * aX + aY * aY));
    CartesianVector direction(static_cast<float>(aX / r), static_cast<float>(aY / r), static_cast<float>(1. / r));
    CartesianVector intercept(static_cast<float>(bX), static_cast<float>(bY), (isPositiveZ) ? eCalEndCapZCoordinate : -eCalEndCapZCoordinate);

    float dirCosR(direction.GetDotProduct(intercept) / intercept.GetMagnitude());
    if (0 > dirCosR)
    {
        dirCosR = -dirCosR;
        direction = CartesianVector(0, 0, 0) - direction;
    }

    // Now calculate something like a chi2
    double chi2_X(0.), chi2_Y(0.), rms(0.);
    double sumA(0.), sumL(0.), sumAL(0.), sumLL(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const CartesianVector difference(iter->GetPosition() - intercept);
        const double error(iter->GetCellSize() / 3.46);
        const double chiX((difference.GetX() - aX * difference.GetZ()) / error);
        const double chiY((difference.GetY() - aY * difference.GetZ()) / error);

        chi2_X += chiX * chiX;
        chi2_Y += chiY * chiY;
        rms += (direction.GetCrossProduct(difference)).GetMagnitudeSquared();

        const float a(direction.GetDotProduct(difference));
        const float l(static_cast<float>(iter->GetPseudoLayer()));
        sumA += a; sumL += l; sumAL += a * l; sumLL += l * l;
    }

    const double denominatorL(sumL * sumL - nPoints * sumLL);
    if (0 != denominatorL)
    {
        if (((sumL * sumA - nPoints * sumAL) / denominatorL) < 0)
            direction = CartesianVector(0, 0, 0) - direction;
    }

    clusterFitResult.SetDirection(direction);
    clusterFitResult.SetIntercept(intercept);
    clusterFitResult.SetChi2(static_cast<float>((chi2_X + chi2_Y) / nPoints));
    clusterFitResult.SetRms(static_cast<float>(std::sqrt(rms / nPoints)));
    clusterFitResult.SetRadialDirectionCosine(dirCosR);
    clusterFitResult.SetSuccessFlag(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetFitResultsClosestApproach(const ClusterHelper::ClusterFitResult &lhs, const ClusterHelper::ClusterFitResult &rhs,
    float &closestApproach)
{
    try
    {
        if (!lhs.IsFitSuccessful() || !rhs.IsFitSuccessful())
            return STATUS_CODE_INVALID_PARAMETER;

        const CartesianVector directionNormal((lhs.GetDirection().GetCrossProduct(rhs.GetDirection())).GetUnitVector());
        const CartesianVector interceptDifference(lhs.GetIntercept() - rhs.GetIntercept());
        closestApproach = std::fabs(directionNormal.GetDotProduct(interceptDifference));
    }
    catch (StatusCodeException &statusCodeException)
    {
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestHit(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
    PseudoLayer startLayer, PseudoLayer endLayer)
{
    if (startLayer > endLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const CartesianVector interceptDifference(pCaloHit->GetPositionVector() - clusterFitResult.GetIntercept());
            const float distanceSquared(interceptDifference.GetCrossProduct(clusterFitResult.GetDirection()).GetMagnitudeSquared());

            if (distanceSquared < minDistanceSquared)
            {
                minDistanceSquared = distanceSquared;
            }
        }
    }

    return std::sqrt(minDistanceSquared);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestHit(const Cluster *const pClusterI, const Cluster *const pClusterJ)
{
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitListI(pClusterI->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pClusterJ->GetOrderedCaloHitList());

    // Loop over hits in cluster I
    for (OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.begin(), iterIEnd = orderedCaloHitListI.end(); iterI != iterIEnd; ++iterI)
    {
        for (CaloHitList::const_iterator hitIterI = iterI->second->begin(), hitIterIEnd = iterI->second->end(); hitIterI != hitIterIEnd; ++hitIterI)
        {
            const CartesianVector &positionVectorI((*hitIterI)->GetPositionVector());

            // For each hit in cluster I, find closest distance to a hit in cluster J
            for (OrderedCaloHitList::const_iterator iterJ = orderedCaloHitListJ.begin(), iterJEnd = orderedCaloHitListJ.end(); iterJ != iterJEnd; ++iterJ)
            {
                for (CaloHitList::const_iterator hitIterJ = iterJ->second->begin(), hitIterJEnd = iterJ->second->end(); hitIterJ != hitIterJEnd; ++hitIterJ)
                {
                    const float distanceSquared((positionVectorI - (*hitIterJ)->GetPositionVector()).GetMagnitudeSquared());

                    if (distanceSquared < minDistanceSquared)
                        minDistanceSquared = distanceSquared;
                }
            }
        }
    }

    return std::sqrt(minDistanceSquared);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::GetDistanceToClosestCentroid(const ClusterFitResult &clusterFitResult, const Cluster *const pCluster,
    PseudoLayer startLayer, PseudoLayer endLayer)
{
    if (startLayer > endLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (PseudoLayer iLayer = startLayer; iLayer <= endLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() == iter)
            continue;

        const CartesianVector interceptDifference(pCluster->GetCentroid(iLayer) - clusterFitResult.GetIntercept());
        const float distanceSquared(interceptDifference.GetCrossProduct(clusterFitResult.GetDirection()).GetMagnitudeSquared());

        if (distanceSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceSquared;
        }
    }

    return std::sqrt(minDistanceSquared);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetDistanceToClosestCentroid(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &centroidDistance)
{
    // Return if clusters do not overlap
    if ((pClusterI->GetOuterPseudoLayer() < pClusterJ->GetInnerPseudoLayer()) || (pClusterJ->GetOuterPseudoLayer() < pClusterI->GetInnerPseudoLayer()))
        return STATUS_CODE_NOT_FOUND;

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitListI(pClusterI->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pClusterJ->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.begin(), iterIEnd = orderedCaloHitListI.end(); iterI != iterIEnd; ++iterI)
    {
        const CartesianVector centroidI(pClusterI->GetCentroid(iterI->first));

        for (OrderedCaloHitList::const_iterator iterJ = orderedCaloHitListJ.begin(), iterJEnd = orderedCaloHitListJ.end(); iterJ != iterJEnd; ++iterJ)
        {
            if (orderedCaloHitListI.end() == orderedCaloHitListI.find(iterJ->first))
                continue;

            const CartesianVector centroidJ(pClusterJ->GetCentroid(iterJ->first));

            const float distanceSquared((centroidI - centroidJ).GetMagnitudeSquared());

            if (distanceSquared < minDistanceSquared)
            {
                minDistanceSquared = distanceSquared;
                distanceFound = true;
            }
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    centroidDistance = std::sqrt(minDistanceSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetClosestIntraLayerDistance(const Cluster *const pClusterI, const Cluster *const pClusterJ, float &intraLayerDistance)
{
    // Return if clusters do not overlap
    if ((pClusterI->GetOuterPseudoLayer() < pClusterJ->GetInnerPseudoLayer()) || (pClusterJ->GetOuterPseudoLayer() < pClusterI->GetInnerPseudoLayer()))
        return STATUS_CODE_NOT_FOUND;

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());
    const OrderedCaloHitList &orderedCaloHitListI(pClusterI->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pClusterJ->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.begin(), iterIEnd = orderedCaloHitListI.end(); iterI != iterIEnd; ++iterI)
    {
        const PseudoLayer pseudoLayer(iterI->first);
        OrderedCaloHitList::const_iterator iterJ = orderedCaloHitListJ.find(pseudoLayer);

        if (orderedCaloHitListJ.end() == iterJ)
            continue;

        const CartesianVector centroidI(pClusterI->GetCentroid(pseudoLayer));
        const CartesianVector centroidJ(pClusterJ->GetCentroid(pseudoLayer));

        const float distanceSquared((centroidI - centroidJ).GetMagnitudeSquared());

        if (distanceSquared < minDistanceSquared)
        {
            minDistanceSquared = distanceSquared;
            distanceFound = true;
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    intraLayerDistance = std::sqrt(minDistanceSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetTrackClusterDistance(const Track *const pTrack, const Cluster *const pCluster,
    const PseudoLayer maxSearchLayer, float parallelDistanceCut, float &trackClusterDistance)
{
    if ((0 == pCluster->GetNCaloHits()) || (pCluster->GetInnerPseudoLayer() > maxSearchLayer))
        return STATUS_CODE_NOT_FOUND;

    bool distanceFound(false);
    float minDistance(std::numeric_limits<float>::max());

    if (STATUS_CODE_SUCCESS == ClusterHelper::GetTrackClusterDistance(pTrack->GetTrackStateAtECal(), pCluster, maxSearchLayer, parallelDistanceCut, minDistance))
        distanceFound = true;

    const TrackStateList &trackStateList(pTrack->GetCalorimeterProjections());
    for (TrackStateList::const_iterator iter = trackStateList.begin(), iterEnd = trackStateList.end(); iter != iterEnd; ++iter)
    {
        const TrackState *const pTrackState = *iter;
        float altProjectionDistance(std::numeric_limits<float>::max());

        if (STATUS_CODE_SUCCESS == ClusterHelper::GetTrackClusterDistance(*pTrackState, pCluster, maxSearchLayer, parallelDistanceCut, altProjectionDistance))
        {
            distanceFound = true;

            if (altProjectionDistance < minDistance)
                minDistance = altProjectionDistance;
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    trackClusterDistance = minDistance;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::GetTrackClusterDistance(const TrackState &trackState, const Cluster *const pCluster,
    const PseudoLayer maxSearchLayer, float parallelDistanceCut, float &trackClusterDistance)
{
    if ((0 == pCluster->GetNCaloHits()) || (pCluster->GetInnerPseudoLayer() > maxSearchLayer))
        return STATUS_CODE_NOT_FOUND;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    const CartesianVector &trackPosition(trackState.GetPosition());
    const CartesianVector trackDirection(trackState.GetMomentum().GetUnitVector());

    bool distanceFound(false);
    float minDistanceSquared(std::numeric_limits<float>::max());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (iter->first > maxSearchLayer)
            break;

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector positionDifference((*hitIter)->GetPositionVector() - trackPosition);
            const float parallelDistance(trackDirection.GetDotProduct(positionDifference));

            if (std::fabs(parallelDistance) > parallelDistanceCut)
                continue;

            const float perpendicularDistanceSquared((trackDirection.GetCrossProduct(positionDifference)).GetMagnitudeSquared());

            if (perpendicularDistanceSquared < minDistanceSquared)
            {
                minDistanceSquared = perpendicularDistanceSquared;
                distanceFound = true;
            }
        }
    }

    if (!distanceFound)
        return STATUS_CODE_NOT_FOUND;

    trackClusterDistance = std::sqrt(minDistanceSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::CanMergeCluster(const Cluster *const pCluster, float minMipFraction, float maxAllHitsFitRms)
{
    if (0 == pCluster->GetNCaloHits())
        return false;

    if (!(pCluster->IsPhotonFast()))
        return true;

    if (pCluster->GetMipFraction() - minMipFraction > std::numeric_limits<float>::epsilon())
        return true;

    return (pCluster->GetFitToAllHitsResult().IsFitSuccessful() && (pCluster->GetFitToAllHitsResult().GetRms() < maxAllHitsFitRms));
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ClusterHelper::IsClusterLeavingDetector(const Cluster *const pCluster)
{
    if (!pCluster->ContainsHitInOuterSamplingLayer())
        return false;

    if (pCluster->ContainsHitType(MUON))
        return true;

    // Examine occupancy and energy content of outer layers
    const PseudoLayer outerLayer(pCluster->GetOuterPseudoLayer());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    if (m_leavingNOuterLayersToExamine > outerLayer)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    unsigned int nOccupiedOuterLayers(0);
    float hadronicEnergyInOuterLayers(0.f);

    for (PseudoLayer iLayer = outerLayer - m_leavingNOuterLayersToExamine; iLayer <= outerLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);

        if (orderedCaloHitList.end() != iter)
        {
            nOccupiedOuterLayers++;

            for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                hadronicEnergyInOuterLayers += (*hitIter)->GetHadronicEnergy();
            }
        }
    }

    if ((nOccupiedOuterLayers >= m_leavingMipLikeNOccupiedLayers) ||
        ((nOccupiedOuterLayers == m_leavingShowerLikeNOccupiedLayers) && (hadronicEnergyInOuterLayers > m_leavingShowerLikeEnergyInOuterLayers)))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer ClusterHelper::GetShowerStartLayer(const Cluster *const pCluster)
{
    const PseudoLayer innerLayer(pCluster->GetInnerPseudoLayer());
    const PseudoLayer outerLayer(pCluster->GetOuterPseudoLayer());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    PseudoLayer currentShowerLayers(0);
    PseudoLayer lastForwardLayer(outerLayer);
    bool foundLastForwardLayer(false);

    // Find two consecutive shower layers
    for (PseudoLayer iLayer = innerLayer; iLayer <= outerLayer; ++iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);
        const bool isLayerPopulated((orderedCaloHitList.end() != iter) && !iter->second->empty());
        float mipFraction(0.f);

        if (isLayerPopulated)
        {
            unsigned int nMipHits(0);

            for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                if ((*hitIter)->IsPossibleMip())
                    nMipHits++;
            }

            mipFraction = static_cast<float>(nMipHits) / static_cast<float>(iter->second->size());
        }

        if (mipFraction - m_showerStartMipFraction > std::numeric_limits<float>::epsilon())
        {
            currentShowerLayers = 0;
        }
        else if (++currentShowerLayers >= m_showerStartNonMipLayers)
        {
            if (isLayerPopulated)
                lastForwardLayer = iLayer;

            foundLastForwardLayer = true;
            break;
        }
    }

    if (!foundLastForwardLayer)
        return outerLayer;

    PseudoLayer currentMipLayers(0);
    PseudoLayer showerStartLayer(lastForwardLayer);

    // Now go backwards to find two consecutive mip layers
    for (PseudoLayer iLayer = lastForwardLayer; iLayer >= innerLayer; --iLayer)
    {
        OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(iLayer);
        const bool isLayerPopulated((orderedCaloHitList.end() != iter) && !iter->second->empty());

        if (!isLayerPopulated)
            continue;

        unsigned int nMipHits(0);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if ((*hitIter)->IsPossibleMip())
                nMipHits++;
        }

        const float mipFraction(static_cast<float>(nMipHits) / static_cast<float>(iter->second->size()));

        if (mipFraction - m_showerStartMipFraction < std::numeric_limits<float>::epsilon())
        {
            currentMipLayers = 0;
            showerStartLayer = iLayer;
        }
        else if (++currentMipLayers >= m_showerStartNonMipLayers)
        {
            return showerStartLayer;
        }
    }

    return showerStartLayer;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CaloHit *const pCaloHit) :
    m_position(pCaloHit->GetPositionVector()),
    m_cellSize(pCaloHit->GetCellLengthScale()),
    m_pseudoLayer(pCaloHit->GetPseudoLayer())
{
    if (!m_position.IsInitialized() || (0 == m_cellSize))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CartesianVector &position, float cellSize, PseudoLayer pseudoLayer) :
    m_position(position),
    m_cellSize(cellSize),
    m_pseudoLayer(pseudoLayer)
{
    if (!m_position.IsInitialized() || (0 == m_cellSize))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterHelper::m_showerStartMipFraction = 0.8f;
unsigned int ClusterHelper::m_showerStartNonMipLayers = 2;
unsigned int ClusterHelper::m_leavingNOuterLayersToExamine = 4;
unsigned int ClusterHelper::m_leavingMipLikeNOccupiedLayers = 4;
unsigned int ClusterHelper::m_leavingShowerLikeNOccupiedLayers = 3;
float ClusterHelper::m_leavingShowerLikeEnergyInOuterLayers = 1.f;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerStartMipFraction", m_showerStartMipFraction));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowerStartNonMipLayers", m_showerStartNonMipLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LeavingNOuterLayersToExamine", m_leavingNOuterLayersToExamine));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LeavingMipLikeNOccupiedLayers", m_leavingMipLikeNOccupiedLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LeavingShowerLikeNOccupiedLayers", m_leavingShowerLikeNOccupiedLayers));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LeavingShowerLikeEnergyInOuterLayers", m_leavingShowerLikeEnergyInOuterLayers));

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
