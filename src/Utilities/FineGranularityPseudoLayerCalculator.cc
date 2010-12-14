/**
 *  @file   PandoraPFANew/src/Utilities/FineGranularityPseudoLayerCalculator.cc
 * 
 *  @brief  Implementation of the fine granularity pseudo layer calculator class.
 * 
 *  $Log: $
 */

#include "Utilities/FineGranularityPseudoLayerCalculator.h"

#include <algorithm>

namespace pandora
{

FineGranularityPseudoLayerCalculator::FineGranularityPseudoLayerCalculator() :
    PseudoLayerCalculator(),
    m_barrelInnerR(),
    m_endCapInnerZ(),
    m_barrelInnerRMuon(),
    m_endCapInnerZMuon(),
    m_rCorrection(0.f),
    m_zCorrection(0.f),
    m_rCorrectionMuon(0.f),
    m_zCorrectionMuon(0.f),
    m_barrelEdgeR(),
    m_endCapEdgeZ()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::Initialize(const GeometryHelper *const pGeometryHelper)
{
    this->StoreLayerPositions(pGeometryHelper);
    this->StoreOverlapCorrectionDetails(pGeometryHelper);
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

    // Reserve pseudo layer(s) for track projections
    return (1 + TRACK_PROJECTION_LAYER + pseudoLayer);
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
    // Cache all relevant layer positions in ordered lists, demanding no duplicates
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
        std::cout << "PseudoLayerCalculator: No layer positions specified." << std::endl;
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
    }

    std::sort(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end());
    std::sort(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end());

    LayerPositionList::const_iterator barrelIter = std::unique(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end());
    LayerPositionList::const_iterator endcapIter = std::unique(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end());

    if ((m_barrelLayerPositions.end() != barrelIter) || (m_endCapLayerPositions.end() != endcapIter))
    {
        std::cout << "PseudoLayerCalculator: Duplicate layer position detected." << std::endl;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    this->StoreDetectorOuterEdge(pGeometryHelper);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::StoreLayerPositions(const GeometryHelper::SubDetectorParameters &subDetectorParameters,
    LayerPositionList &LayerPositionList)
{
    if (!subDetectorParameters.IsInitialized())
        return;

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
    // Find extremal barrel and endcap coordinates. Necessary to guard against e.g. user-specified dummy muon coordinates.
    m_barrelEdgeR = (std::max(pGeometryHelper->GetECalBarrelParameters().GetOuterRCoordinate(), std::max(
        pGeometryHelper->GetHCalBarrelParameters().GetOuterRCoordinate(),
        pGeometryHelper->GetMuonBarrelParameters().GetOuterRCoordinate()) ));

    m_endCapEdgeZ = (std::max(pGeometryHelper->GetECalEndCapParameters().GetOuterZCoordinate(), std::max(
        pGeometryHelper->GetHCalEndCapParameters().GetOuterZCoordinate(),
        pGeometryHelper->GetMuonEndCapParameters().GetOuterZCoordinate()) ));

    if ((m_barrelLayerPositions.end() != std::upper_bound(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end(), m_barrelEdgeR)) ||
        (m_endCapLayerPositions.end() != std::upper_bound(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end(), m_endCapEdgeZ)))
    {
        std::cout << "PseudoLayerCalculator: Layers specified outside detector edge." << std::endl;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    m_barrelLayerPositions.push_back(m_barrelEdgeR);
    m_endCapLayerPositions.push_back(m_endCapEdgeZ);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void FineGranularityPseudoLayerCalculator::StoreOverlapCorrectionDetails(const GeometryHelper *const pGeometryHelper)
{
    // TODO tidy, not strictly overlap stuff.
    // Cache sine and cosine of angles used to project hit positions onto polygonal calorimeter surfaces
    GeometryHelper::FillAngleVector(pGeometryHelper->GetECalBarrelParameters().GetInnerSymmetryOrder(),
        pGeometryHelper->GetECalBarrelParameters().GetInnerPhiCoordinate(), m_eCalBarrelAngleVector);
    GeometryHelper::FillAngleVector(pGeometryHelper->GetMuonBarrelParameters().GetInnerSymmetryOrder(),
        pGeometryHelper->GetMuonBarrelParameters().GetInnerPhiCoordinate(), m_muonBarrelAngleVector);

    // Cache information used to identify overlap regions
    m_barrelInnerR = pGeometryHelper->GetECalBarrelParameters().GetInnerRCoordinate();
    m_endCapInnerZ = pGeometryHelper->GetECalEndCapParameters().GetInnerZCoordinate();
    m_barrelInnerRMuon = pGeometryHelper->GetMuonBarrelParameters().GetInnerRCoordinate();
    m_endCapInnerZMuon = pGeometryHelper->GetMuonEndCapParameters().GetInnerZCoordinate();

    const float barrelOuterZ = pGeometryHelper->GetECalBarrelParameters().GetOuterZCoordinate();
    const float endCapOuterR = pGeometryHelper->GetECalEndCapParameters().GetOuterRCoordinate();
    const float barrelOuterZMuon = pGeometryHelper->GetMuonBarrelParameters().GetOuterZCoordinate();
    const float endCapOuterRMuon = pGeometryHelper->GetMuonEndCapParameters().GetOuterRCoordinate();

    // Cache corrections to be applied in barrel/endcap overlap regions
    const bool IsEnclosingEndCap(endCapOuterR > m_barrelInnerR);

    m_rCorrection = ((!IsEnclosingEndCap) ? 0.f : m_barrelInnerR * ((m_endCapInnerZ / barrelOuterZ) - 1.f));
    m_zCorrection = ((IsEnclosingEndCap) ? 0.f : m_endCapInnerZ * ((m_barrelInnerR / endCapOuterR) - 1.f));
    m_rCorrectionMuon = ((!IsEnclosingEndCap) ? 0.f : m_barrelInnerRMuon * ((m_endCapInnerZMuon / barrelOuterZMuon) - 1.f));
    m_zCorrectionMuon = ((IsEnclosingEndCap) ? 0.f : m_endCapInnerZMuon * ((m_barrelInnerRMuon / endCapOuterRMuon) - 1.f));
}

} // namespace pandora
