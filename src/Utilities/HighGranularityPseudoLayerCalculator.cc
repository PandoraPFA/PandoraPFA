/**
 *  @file   PandoraPFANew/src/Utilities/HighGranularityPseudoLayerCalculator.cc
 * 
 *  @brief  Implementation of the high granularity pseudo layer calculator class.
 * 
 *  $Log: $
 */

#include "Utilities/HighGranularityPseudoLayerCalculator.h"

#include <algorithm>

namespace pandora
{

void HighGranularityPseudoLayerCalculator::Initialize(const GeometryHelper *const pGeometryHelper)
{
    // Cache all relevant layer positions in ordered lists, demanding no duplicates
    this->StoreLayerPositions(pGeometryHelper->GetECalBarrelParameters().GetLayerParametersList(), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetECalEndCapParameters().GetLayerParametersList(), m_endCapLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetHCalBarrelParameters().GetLayerParametersList(), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetHCalEndCapParameters().GetLayerParametersList(), m_endCapLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetMuonBarrelParameters().GetLayerParametersList(), m_barrelLayerPositions);
    this->StoreLayerPositions(pGeometryHelper->GetMuonEndCapParameters().GetLayerParametersList(), m_endCapLayerPositions);

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
    const GeometryType geometryType(GeometryHelper::GetInstance()->GetGeometryType());

    m_rCorrection = ((ENCLOSING_ENDCAP != geometryType) ? 0.f : m_barrelInnerR * ((m_endCapInnerZ / barrelOuterZ) - 1.f));
    m_zCorrection = ((ENCLOSING_BARREL != geometryType) ? 0.f : m_endCapInnerZ * ((m_barrelInnerR / endCapOuterR) - 1.f));
    m_rCorrectionMuon = ((ENCLOSING_ENDCAP != geometryType) ? 0.f : m_barrelInnerRMuon * ((m_endCapInnerZMuon / barrelOuterZMuon) - 1.f));
    m_zCorrectionMuon = ((ENCLOSING_BARREL != geometryType) ? 0.f : m_endCapInnerZMuon * ((m_barrelInnerRMuon / endCapOuterRMuon) - 1.f));
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer HighGranularityPseudoLayerCalculator::GetPseudoLayer(const CartesianVector &positionVector) const
{
    static const GeometryHelper *const pGeometryHelper = GeometryHelper::GetInstance();

    const float zCoordinate(std::fabs(positionVector.GetZ()));
    const float rCoordinate(pGeometryHelper->GetMaximumECalBarrelRadius(positionVector.GetX(), positionVector.GetY()));
    const float rCoordinateMuon(pGeometryHelper->GetMaximumMuonBarrelRadius(positionVector.GetX(), positionVector.GetY()));

    PseudoLayer pseudoLayer;

    if ((zCoordinate < m_endCapInnerZMuon) && (rCoordinateMuon < m_barrelInnerRMuon))
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetPseudoLayer(rCoordinate, zCoordinate, m_rCorrection,
            m_zCorrection, m_barrelInnerR, m_endCapInnerZ, pseudoLayer));
    }
    else
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetPseudoLayer(rCoordinateMuon, zCoordinate, m_rCorrectionMuon,
            m_zCorrectionMuon, m_barrelInnerRMuon, m_endCapInnerZMuon, pseudoLayer));
    }

    // Reserve pseudo layer(s) for track projections
    return (1 + TRACK_PROJECTION_LAYER + pseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void HighGranularityPseudoLayerCalculator::StoreLayerPositions(const GeometryHelper::LayerParametersList &layerParametersList,
    LayerPositionList &LayerPositionList)
{
    for (GeometryHelper::LayerParametersList::const_iterator iter = layerParametersList.begin(), iterEnd = layerParametersList.end();
        iter != iterEnd; ++iter)
    {
        LayerPositionList.push_back(iter->m_closestDistanceToIp);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void HighGranularityPseudoLayerCalculator::StoreDetectorOuterEdge(const GeometryHelper *const pGeometryHelper)
{
    // Find extremal barrel and endcap coordinates. Necessary to guard against e.g. user-specified dummy muon coordinates.
    const float barrelEdgeR(std::max(pGeometryHelper->GetECalBarrelParameters().GetOuterRCoordinate(), std::max(
        pGeometryHelper->GetHCalBarrelParameters().GetOuterRCoordinate(),
        pGeometryHelper->GetMuonBarrelParameters().GetOuterRCoordinate()) ));

    const float endCapEdgeZ(std::max(pGeometryHelper->GetECalEndCapParameters().GetOuterZCoordinate(), std::max(
        pGeometryHelper->GetHCalEndCapParameters().GetOuterZCoordinate(),
        pGeometryHelper->GetMuonEndCapParameters().GetOuterZCoordinate()) ));

    if ((m_barrelLayerPositions.end() != std::upper_bound(m_barrelLayerPositions.begin(), m_barrelLayerPositions.end(), barrelEdgeR)) ||
        (m_endCapLayerPositions.end() != std::upper_bound(m_endCapLayerPositions.begin(), m_endCapLayerPositions.end(), endCapEdgeZ)))
    {
        std::cout << "PseudoLayerCalculator: Layers specified outside detector edge." << std::endl;
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    m_barrelLayerPositions.push_back(barrelEdgeR);
    m_endCapLayerPositions.push_back(endCapEdgeZ);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighGranularityPseudoLayerCalculator::GetPseudoLayer(const float rCoordinate, const float zCoordinate, const float rCorrection,
    const float zCorrection, const float barrelInnerR, const float endCapInnerZ, PseudoLayer &pseudoLayer) const
{
    if (zCoordinate < endCapInnerZ)
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindMatchingLayer(rCoordinate, m_barrelLayerPositions, pseudoLayer));
    }
    else if (rCoordinate < barrelInnerR)
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindMatchingLayer(zCoordinate, m_endCapLayerPositions, pseudoLayer));
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
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighGranularityPseudoLayerCalculator::FindMatchingLayer(const float position, const LayerPositionList &layerPositionList,
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

} // namespace pandora
