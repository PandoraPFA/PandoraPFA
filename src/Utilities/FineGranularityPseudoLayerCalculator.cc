/**
 *  @file   PandoraPFANew/src/Utilities/FineGranularityPseudoLayerCalculator.cc
 * 
 *  @brief  Implementation of the fine granularity pseudo layer calculator class.
 * 
 *  $Log: $
 */

#include "Utilities/FineGranularityPseudoLayerCalculator.h"

#include <algorithm>

using namespace pandora;

FineGranularityPseudoLayerCalculator::FineGranularityPseudoLayerCalculator() :
    PseudoLayerCalculator(),
    m_barrelInnerR(0.f),
    m_endCapInnerZ(0.f),
    m_barrelInnerRMuon(0.f),
    m_endCapInnerZMuon(0.f),
    m_rCorrection(0.f),
    m_zCorrection(0.f),
    m_rCorrectionMuon(0.f),
    m_zCorrectionMuon(0.f),
    m_barrelEdgeR(0.f),
    m_endCapEdgeZ(0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::Initialize(const GeometryHelper *const pGeometryHelper)
{
    try
    {
        this->StoreLayerPositions(pGeometryHelper);
        this->StoreDetectorOuterEdge(pGeometryHelper);
        this->StorePolygonAngles(pGeometryHelper);
        this->StoreOverlapCorrectionDetails(pGeometryHelper);
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "FineGranularityPseudoLayerCalculator: Incomplete geometry - consider using a custom PseudoLayerCalculator." << std::endl;
        throw statusCodeException;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer FineGranularityPseudoLayerCalculator::GetPseudoLayer(const CartesianVector &positionVector) const
{
    const float zCoordinate(std::fabs(positionVector.GetZ()));

    if (zCoordinate > m_endCapEdgeZ)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    const float rCoordinate(GeometryHelper::GetMaximumRadius(m_eCalBarrelAngleVector, positionVector.GetX(), positionVector.GetY()));
    const float rCoordinateMuon(GeometryHelper::GetMaximumRadius(m_muonBarrelAngleVector, positionVector.GetX(), positionVector.GetY()));

    if ((rCoordinateMuon > m_barrelEdgeR) || (rCoordinate > m_barrelEdgeR))
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    PseudoLayer pseudoLayer;

    if ((zCoordinate < m_endCapInnerZMuon) && (rCoordinateMuon < m_barrelInnerRMuon))
    {
        const StatusCode statusCode(this->GetPseudoLayer(rCoordinate, zCoordinate, m_rCorrection, m_zCorrection, m_barrelInnerR,
            m_endCapInnerZ, pseudoLayer));

        if (STATUS_CODE_SUCCESS != statusCode)
            throw StatusCodeException(statusCode);
    }
    else
    {
        const StatusCode statusCode(this->GetPseudoLayer(rCoordinateMuon, zCoordinate, m_rCorrectionMuon, m_zCorrectionMuon,
            m_barrelInnerRMuon, m_endCapInnerZMuon, pseudoLayer));

        if (STATUS_CODE_SUCCESS != statusCode)
            throw StatusCodeException(statusCode);
    }

    // Reserve a pseudo layer for track projections, etc.
    return (1 + pseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FineGranularityPseudoLayerCalculator::GetPseudoLayer(const float rCoordinate, const float zCoordinate, const float rCorrection,
    const float zCorrection, const float barrelInnerR, const float endCapInnerZ, PseudoLayer &pseudoLayer) const
{
    if (zCoordinate < endCapInnerZ)
    {
        return this->FindMatchingLayer(rCoordinate, m_barrelLayerPositions, pseudoLayer);
    }
    else if (rCoordinate < barrelInnerR)
    {
        return this->FindMatchingLayer(zCoordinate, m_endCapLayerPositions, pseudoLayer);
    }
    else
    {
        PseudoLayer bestBarrelLayer(0);
        const StatusCode barrelStatusCode(this->FindMatchingLayer(rCoordinate - rCorrection, m_barrelLayerPositions, bestBarrelLayer));

        PseudoLayer bestEndCapLayer(0);
        const StatusCode endCapStatusCode(this->FindMatchingLayer(zCoordinate - zCorrection, m_endCapLayerPositions, bestEndCapLayer));

        if ((STATUS_CODE_SUCCESS != barrelStatusCode) && (STATUS_CODE_SUCCESS != endCapStatusCode))
            return STATUS_CODE_NOT_FOUND;

        pseudoLayer = std::max(bestBarrelLayer, bestEndCapLayer);
        return STATUS_CODE_SUCCESS;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FineGranularityPseudoLayerCalculator::FindMatchingLayer(const float position, const LayerPositionList &layerPositionList,
    PseudoLayer &layer) const
{
    LayerPositionList::const_iterator upperIter = std::upper_bound(layerPositionList.begin(), layerPositionList.end(), position);

    if (layerPositionList.end() == upperIter)
    {
        return STATUS_CODE_NOT_FOUND;
    }

    if (layerPositionList.begin() == upperIter)
    {
        layer = 0;
        return STATUS_CODE_SUCCESS;
    }

    LayerPositionList::const_iterator lowerIter = upperIter - 1;

    if (std::fabs(position - *lowerIter) < std::fabs(position - *upperIter))
    {
        layer = std::distance(layerPositionList.begin(), lowerIter);
    }
    else
    {
        layer = std::distance(layerPositionList.begin(), upperIter);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::StoreLayerPositions(const GeometryHelper *const pGeometryHelper)
{
    this->StoreLayerPositions(pGeometryHelper->GetInDetBarrelParameters(), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetInDetEndCapParameters(), m_endCapLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetECalBarrelParameters(), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetECalEndCapParameters(), m_endCapLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetHCalBarrelParameters(), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetHCalEndCapParameters(), m_endCapLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetMuonBarrelParameters(), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetMuonEndCapParameters(), m_endCapLayerPositions);

    if (m_barrelLayerPositions.empty() || m_endCapLayerPositions.empty())
    {
        std::cout << "FineGranularityPseudoLayerCalculator: No layer positions specified." << std::endl;
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
    }

    std::sort(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end());
    std::sort(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end());

    LayerPositionList::const_iterator barrelIter = std::unique(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end());
    LayerPositionList::const_iterator endcapIter = std::unique(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end());

    if ((m_barrelLayerPositions.end() != barrelIter) || (m_endCapLayerPositions.end() != endcapIter))
    {
        std::cout << "FineGranularityPseudoLayerCalculator: Duplicate layer position detected." << std::endl;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::StoreLayerPositions(const GeometryHelper::SubDetectorParameters &subDetectorParameters,
    LayerPositionList &LayerPositionList)
{
    if (!subDetectorParameters.IsInitialized())
        return;

    if (!subDetectorParameters.IsMirroredInZ())
    {
        std::cout << "FineGranularityPseudoLayerCalculator: Error, detector must be symmetrical about z=0 plane." << std::endl;
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }

    const GeometryHelper::LayerParametersList &layerParametersList(subDetectorParameters.GetLayerParametersList());

    for (GeometryHelper::LayerParametersList::const_iterator iter = layerParametersList.begin(), iterEnd = layerParametersList.end();
        iter != iterEnd; ++iter)
    {
        LayerPositionList.push_back(iter->m_closestDistanceToIp);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::StoreDetectorOuterEdge(const GeometryHelper *const pGeometryHelper)
{
    m_barrelEdgeR = (std::max(pGeometryHelper->GetECalBarrelParameters().GetOuterRCoordinate(), std::max(
        pGeometryHelper->GetHCalBarrelParameters().GetOuterRCoordinate(),
        pGeometryHelper->GetMuonBarrelParameters().GetOuterRCoordinate()) ));

    m_endCapEdgeZ = (std::max(std::fabs(pGeometryHelper->GetECalEndCapParameters().GetOuterZCoordinate()), std::max(
        std::fabs(pGeometryHelper->GetHCalEndCapParameters().GetOuterZCoordinate()),
        std::fabs(pGeometryHelper->GetMuonEndCapParameters().GetOuterZCoordinate())) ));

    if ((m_barrelLayerPositions.end() != std::upper_bound(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end(), m_barrelEdgeR)) ||
        (m_endCapLayerPositions.end() != std::upper_bound(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end(), m_endCapEdgeZ)))
    {
        std::cout << "FineGranularityPseudoLayerCalculator: Layers specified outside detector edge." << std::endl;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    m_barrelLayerPositions.push_back(m_barrelEdgeR);
    m_endCapLayerPositions.push_back(m_endCapEdgeZ);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::StorePolygonAngles(const GeometryHelper *const pGeometryHelper)
{
    GeometryHelper::FillAngleVector(pGeometryHelper->GetECalBarrelParameters().GetInnerSymmetryOrder(),
        pGeometryHelper->GetECalBarrelParameters().GetInnerPhiCoordinate(), m_eCalBarrelAngleVector);

    GeometryHelper::FillAngleVector(pGeometryHelper->GetMuonBarrelParameters().GetInnerSymmetryOrder(),
        pGeometryHelper->GetMuonBarrelParameters().GetInnerPhiCoordinate(), m_muonBarrelAngleVector);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::StoreOverlapCorrectionDetails(const GeometryHelper *const pGeometryHelper)
{
    m_barrelInnerR = pGeometryHelper->GetECalBarrelParameters().GetInnerRCoordinate();
    m_endCapInnerZ = std::fabs(pGeometryHelper->GetECalEndCapParameters().GetInnerZCoordinate());
    m_barrelInnerRMuon = pGeometryHelper->GetMuonBarrelParameters().GetInnerRCoordinate();
    m_endCapInnerZMuon = std::fabs(pGeometryHelper->GetMuonEndCapParameters().GetInnerZCoordinate());

    const float barrelOuterZ = std::fabs(pGeometryHelper->GetECalBarrelParameters().GetOuterZCoordinate());
    const float endCapOuterR = pGeometryHelper->GetECalEndCapParameters().GetOuterRCoordinate();
    const float barrelOuterZMuon = std::fabs(pGeometryHelper->GetMuonBarrelParameters().GetOuterZCoordinate());
    const float endCapOuterRMuon = pGeometryHelper->GetMuonEndCapParameters().GetOuterRCoordinate();

    const bool IsEnclosingEndCap(endCapOuterR > m_barrelInnerR);
    m_rCorrection = ((!IsEnclosingEndCap) ? 0.f : m_barrelInnerR * ((m_endCapInnerZ / barrelOuterZ) - 1.f));
    m_zCorrection = ((IsEnclosingEndCap) ? 0.f : m_endCapInnerZ * ((m_barrelInnerR / endCapOuterR) - 1.f));
    m_rCorrectionMuon = ((!IsEnclosingEndCap) ? 0.f : m_barrelInnerRMuon * ((m_endCapInnerZMuon / barrelOuterZMuon) - 1.f));
    m_zCorrectionMuon = ((IsEnclosingEndCap) ? 0.f : m_endCapInnerZMuon * ((m_barrelInnerRMuon / endCapOuterRMuon) - 1.f));
}
