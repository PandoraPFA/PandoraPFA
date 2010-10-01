/**
 *  @file   PandoraPFANew/src/Utilities/PseudoLayerCalculator.cc
 * 
 *  @brief  Implementation of the pseudo layer calculator class.
 * 
 *  $Log: $
 */

#include "Utilities/PseudoLayerCalculator.h"

#include <algorithm>

namespace pandora
{

void HighGranularityPseudoLayerCalculator::Initialize(const GeometryHelper *const pGeometryHelper)
{
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
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer HighGranularityPseudoLayerCalculator::GetPseudoLayer(const CartesianVector &positionVector) const
{
    static const GeometryHelper *const pGeometryHelper = GeometryHelper::GetInstance();
    static const GeometryHelper::SubDetectorParameters &eCalBarrelParameters(pGeometryHelper->GetECalBarrelParameters());
    static const GeometryHelper::SubDetectorParameters &eCalEndCapParameters(pGeometryHelper->GetECalEndCapParameters());
    static const GeometryHelper::SubDetectorParameters &muonBarrelParameters(pGeometryHelper->GetMuonBarrelParameters());
    static const GeometryHelper::SubDetectorParameters &muonEndCapParameters(pGeometryHelper->GetMuonEndCapParameters());

    const float zCoordinate(std::fabs(positionVector.GetZ()));
    const float rCoordinate(pGeometryHelper->GetMaximumECalBarrelRadius(positionVector.GetX(), positionVector.GetY()));
    const float rCoordinateMuon(pGeometryHelper->GetMaximumMuonBarrelRadius(positionVector.GetX(), positionVector.GetY()));

    PseudoLayer pseudoLayer;

    if ((zCoordinate < muonEndCapParameters.GetInnerZCoordinate()) && (rCoordinateMuon < muonBarrelParameters.GetInnerRCoordinate()))
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetPseudoLayer(eCalBarrelParameters, eCalEndCapParameters, rCoordinate,
            zCoordinate, pseudoLayer));
    }
    else
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetPseudoLayer(muonBarrelParameters, muonEndCapParameters, rCoordinateMuon,
            zCoordinate, pseudoLayer));
    }

    // Reserve pseudo layer(s) for track projections
    return (1 + TRACK_PROJECTION_LAYER + pseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode HighGranularityPseudoLayerCalculator::GetPseudoLayer(const GeometryHelper::SubDetectorParameters &barrelParameters,
    const GeometryHelper::SubDetectorParameters &endCapParameters, const float rCoordinate, const float zCoordinate, PseudoLayer &pseudoLayer) const
{
    if (zCoordinate < endCapParameters.GetInnerZCoordinate())
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindMatchingLayer(rCoordinate, m_barrelLayerPositions, pseudoLayer));
    }
    else if (rCoordinate < barrelParameters.GetInnerRCoordinate())
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->FindMatchingLayer(zCoordinate, m_endCapLayerPositions, pseudoLayer));
    }
    else
    {
        static const GeometryType geometryType(GeometryHelper::GetInstance()->GetGeometryType());

        const float rCorrection((ENCLOSING_ENDCAP != geometryType) ? 0.f : barrelParameters.GetInnerRCoordinate() *
            ((endCapParameters.GetInnerZCoordinate() / barrelParameters.GetOuterZCoordinate()) - 1.f));
        const float zCorrection((ENCLOSING_BARREL != geometryType) ? 0.f : endCapParameters.GetInnerZCoordinate() *
            ((barrelParameters.GetInnerRCoordinate() / endCapParameters.GetOuterRCoordinate()) - 1.f));

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

StatusCode HighGranularityPseudoLayerCalculator::FindMatchingLayer(const float position, const LayerPositionList &layerPositionList, PseudoLayer &layer) const
{
    LayerPositionList::const_iterator upperIter = std::upper_bound(layerPositionList.begin(), layerPositionList.end(), position);

    if (layerPositionList.end() == upperIter)
    {
LayerPositionList::const_iterator lowerIter = upperIter - 1;
std::cout << " position " << position << " last " << *lowerIter << std::endl;
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
