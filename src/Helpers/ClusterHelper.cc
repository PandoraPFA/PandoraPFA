/**
 *  @file   PandoraPFANew/src/Helpers/ClusterHelper.cc
 * 
 *  @brief  Implementation of the cluster helper class.
 * 
 *  $Log: $
 */

#include "Helpers/ClusterHelper.h"
#include "Helpers/GeometryHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/OrderedCaloHitList.h"

#include <cmath>

namespace pandora
{

StatusCode ClusterHelper::FitPoints(const OrderedCaloHitList &orderedCaloHitList, ClusterFitResult &clusterFitResult)
{
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
        const float meanY(sumX / static_cast<float>(clusterFitPointList.size()));
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
    float sumU(0.), sumV(0.), sumZ(0.);
    float sumVU(0.), sumZU(0.), sumUU(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const float u(cosTheta * iter->GetPosition().GetX() + sinTheta * iter->GetPosition().GetY());
        const float v(cosTheta * iter->GetPosition().GetY() - sinTheta * iter->GetPosition().GetX());
        const float z(iter->GetPosition().GetZ());

        sumU += u; sumV += v; sumZ += z;
        sumVU += v * u; sumZU += z * u; sumUU += u * u;
    }

    // Perform the fit
    const float nPoints(static_cast<float>(clusterFitPointList.size()));
    const float denominatorU(sumU * sumU - nPoints * sumUU);

    if (0 == denominatorU)
        return STATUS_CODE_FAILURE;

    const float aV((sumU * sumV - nPoints * sumVU) / denominatorU);
    const float bV((sumV - aV * sumU) / nPoints);
    const float aZ((sumU * sumZ - nPoints * sumZU) / denominatorU);
    const float bZ((sumZ - aZ * sumU) / nPoints);

    if (0 == aV)
        return STATUS_CODE_FAILURE;

    const float bU(-bV / aV);
    const float r(std::sqrt(1. + aV * aV + aZ * aZ));
    const float dirU(1. / r), dirV(aV / r), dirZ(aZ / r);
    CartesianVector direction(cosTheta * dirU - sinTheta * dirV, sinTheta * dirU + cosTheta * dirV, dirZ);
    CartesianVector intercept(cosTheta * bU, sinTheta * bU, aZ * bU + bZ);

    float dirCosR(direction.GetDotProduct(intercept) / intercept.GetMagnitude());
    if (0 > dirCosR)
    {
        dirCosR = -dirCosR;
        direction = CartesianVector(0, 0, 0) - direction;
    }

    // Now calculate something like a chi2
    float chi2_V(0.), chi2_Z(0.), rms(0.);
    float sumA(0.), sumL(0.), sumAL(0.), sumLL(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const float u(cosTheta * iter->GetPosition().GetX() + sinTheta * iter->GetPosition().GetY());
        const float v(cosTheta * iter->GetPosition().GetY() - sinTheta * iter->GetPosition().GetX());
        const float z(iter->GetPosition().GetZ());

        const float error(iter->GetCellSize() / 3.46);
        const float chiV((v - aV * u - bV) / error);
        const float chiZ((z - aZ * u - bZ) / error);

        chi2_V += chiV * chiV;
        chi2_Z += chiZ * chiZ;

        const CartesianVector difference(iter->GetPosition() - intercept);
        rms += (direction.GetCrossProduct(difference)).GetMagnitudeSquared();

        const float a(direction.GetDotProduct(difference));
        const float l(static_cast<float>(iter->GetPseudoLayer()));
        sumA += a; sumL += l; sumAL += a * l; sumLL += l * l;
    }

    const float denominatorL(sumL * sumL - nPoints * sumLL);
    if ((0 != denominatorL) && (0 > ((sumL * sumA - nPoints * sumAL) / denominatorL)));
    {
        direction = CartesianVector(0, 0, 0) - direction;
    }

    const float chi2((chi2_V + chi2_Z) / nPoints);
    rms /= nPoints;

    clusterFitResult.SetDirection(direction);
    clusterFitResult.SetIntercept(intercept);
    clusterFitResult.SetChi2(chi2);
    clusterFitResult.SetRms(rms);
    clusterFitResult.SetRadialDirectionCosine(dirCosR);
    clusterFitResult.SetSuccessFlag(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterHelper::FitEndCapPoints(const ClusterFitPointList &clusterFitPointList, bool isPositiveZ, ClusterFitResult &clusterFitResult)
{
    // Extract the data
    float sumX(0.), sumY(0.), sumZ(0.);
    float sumXZ(0.), sumYZ(0.), sumZZ(0.);

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
    const float nPoints(static_cast<float>(clusterFitPointList.size()));
    const float denominatorZ(sumZ * sumZ - nPoints * sumZZ);

    if (0 == denominatorZ)
        return STATUS_CODE_FAILURE;

    const float aX((sumZ * sumX - nPoints * sumXZ) / denominatorZ);
    const float bX((sumX - aX * sumZ) / nPoints);
    const float aY((sumZ * sumY - nPoints * sumYZ) / denominatorZ);
    const float bY((sumY - aY * sumZ) / nPoints);

    const float r(std::sqrt(1. + aX * aX + aY * aY));
    CartesianVector direction(aX / r, aY / r, 1. / r);
    CartesianVector intercept(bX, bY, (isPositiveZ) ? eCalEndCapZCoordinate : -eCalEndCapZCoordinate);

    float dirCosR(direction.GetDotProduct(intercept) / intercept.GetMagnitude());
    if (0 > dirCosR)
    {
        dirCosR = -dirCosR;
        direction = CartesianVector(0, 0, 0) - direction;
    }

    // Now calculate something like a chi2
    float chi2_X(0.), chi2_Y(0.), rms(0.);
    float sumA(0.), sumL(0.), sumAL(0.), sumLL(0.);

    for (ClusterFitPointList::const_iterator iter = clusterFitPointList.begin(), iterEnd = clusterFitPointList.end(); iter != iterEnd; ++iter)
    {
        const CartesianVector difference(iter->GetPosition() - intercept);
        const float error(iter->GetCellSize() / 3.46);
        const float chiX((difference.GetX() - aX * difference.GetZ()) / error);
        const float chiY((difference.GetY() - aY * difference.GetZ()) / error);

        chi2_X += chiX * chiX;
        chi2_Y += chiY * chiY;
        rms += (direction.GetCrossProduct(difference)).GetMagnitudeSquared();

        const float a(direction.GetDotProduct(difference));
        const float l(static_cast<float>(iter->GetPseudoLayer()));
        sumA += a; sumL += l; sumAL += a * l; sumLL += l * l;
    }

    const float denominatorL(sumL * sumL - nPoints * sumLL);
    if ((0 != denominatorL) && (0 > ((sumL * sumA - nPoints * sumAL) / denominatorL)));
    {
        direction = CartesianVector(0, 0, 0) - direction;
    }

    const float chi2((chi2_X + chi2_Y) / nPoints);
    rms /= nPoints;

    clusterFitResult.SetDirection(direction);
    clusterFitResult.SetIntercept(intercept);
    clusterFitResult.SetChi2(chi2);
    clusterFitResult.SetRms(rms);
    clusterFitResult.SetRadialDirectionCosine(dirCosR);
    clusterFitResult.SetSuccessFlag(true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ClusterHelper::ClusterFitPoint::ClusterFitPoint(const CaloHit *const pCaloHit) :
    m_position(pCaloHit->GetPositionVector()),
    m_cellSize(std::sqrt(pCaloHit->GetCellSizeU() * pCaloHit->GetCellSizeV())),
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

} // namespace pandora
