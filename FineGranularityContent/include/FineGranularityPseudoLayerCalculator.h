/**
 *  @file   PandoraPFANew/FineGranularityContent/include/FineGranularityPseudoLayerCalculator.h
 * 
 *  @brief  Header file for the fine granularity pseudo layer calculator class.
 * 
 *  $Log: $
 */
#ifndef FINE_GRANULARITY_PSEUDO_LAYER_CALCULATOR_H
#define FINE_GRANULARITY_PSEUDO_LAYER_CALCULATOR_H 1

#include "Utilities/PseudoLayerCalculator.h"

/**
 *  @brief  FineGranularityPseudoLayerCalculator class
 */
class FineGranularityPseudoLayerCalculator : public pandora::PseudoLayerCalculator
{
public:
    /**
     *  @brief  Default constructor
     */
    FineGranularityPseudoLayerCalculator();

private:
    void InitializeGeometry();
    pandora::PseudoLayer GetPseudoLayer(const pandora::CartesianVector &positionVector) const;
    pandora::PseudoLayer GetPseudoLayerAtIp() const;

    /**
     *  @brief  Get the appropriate pseudolayer for a specified parameters
     * 
     *  @param  rCoordinate the radial coordinate
     *  @param  zCoordinate the z coordinate
     *  @param  rCorrection the barrel/endcap overlap r correction
     *  @param  zCorrection the barrel/endcap overlap z correction
     *  @param  barrelInnerR the barrel inner r coordinate
     *  @param  endCapInnerZ the endcap inner z coordinate
     *  @param  pseudoLayer to receive the appropriate pseudolayer
     */
    pandora::StatusCode GetPseudoLayer(const float rCoordinate, const float zCoordinate, const float rCorrection, const float zCorrection, 
        const float barrelInnerR, const float endCapInnerZ, pandora::PseudoLayer &pseudoLayer) const;

    typedef std::vector<float> LayerPositionList;

    /**
     *  @brief  Find the layer number corresponding to a specified position, via reference to a specified layer position list
     * 
     *  @param  position the specified position
     *  @param  layerPositionList the specified layer position list
     *  @param  layer to receive the layer number
     */
    pandora::StatusCode FindMatchingLayer(const float position, const LayerPositionList &layerPositionList, unsigned int &layer) const;

    /**
     *  @brief  Store all revelevant barrel and endcap layer positions upon initialization
     */
    void StoreLayerPositions();

    /**
     *  @brief  Store subdetector layer positions upon initialization
     * 
     *  @param  subDetectorParameters the sub detector parameters
     *  @param  layerParametersList the layer parameters list
     */
    void StoreLayerPositions(const pandora::GeometryHelper::SubDetectorParameters &subDetectorParameters, LayerPositionList &LayerPositionList);

    /**
     *  @brief  Store positions of barrel and endcap outer edges upon initialization
     */
    void StoreDetectorOuterEdge();

    /**
     *  @brief  Store sine and cosine of angles used to project hit positions onto polygonal calorimeter surfaces upon initialization
     */
    void StorePolygonAngles();

    /**
     *  @brief  Store all details revelevant to barrel/endcap overlap corrections upon initialization
     */
    void StoreOverlapCorrectionDetails();

    LayerPositionList                       m_barrelLayerPositions;     ///< List of barrel layer positions
    LayerPositionList                       m_endCapLayerPositions;     ///< List of endcap layer positions

    pandora::GeometryHelper::AngleVector    m_eCalBarrelAngleVector;    ///< The ecal barrel angle vector
    pandora::GeometryHelper::AngleVector    m_muonBarrelAngleVector;    ///< The muon barrel angle vector

    float                                   m_barrelInnerR;             ///< Barrel inner radius
    float                                   m_endCapInnerZ;             ///< Endcap inner z position
    float                                   m_barrelInnerRMuon;         ///< Muon barrel inner radius
    float                                   m_endCapInnerZMuon;         ///< Muon endcap inner z position

    float                                   m_rCorrection;              ///< Barrel/endcap overlap r correction
    float                                   m_zCorrection;              ///< Barrel/endcap overlap z correction
    float                                   m_rCorrectionMuon;          ///< Muon barrel/endcap overlap r correction
    float                                   m_zCorrectionMuon;          ///< Muon barrel/endcap overlap z correction

    float                                   m_barrelEdgeR;              ///< Extremal barrel r coordinate
    float                                   m_endCapEdgeZ;              ///< Extremal endcap z coordinate
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::PseudoLayer FineGranularityPseudoLayerCalculator::GetPseudoLayerAtIp() const
{
    static const pandora::PseudoLayer pseudoLayerAtIp(this->GetPseudoLayer(pandora::CartesianVector(0.f, 0.f, 0.f)));
    return pseudoLayerAtIp;
}

#endif // #ifndef FINE_GRANULARITY_PSEUDO_LAYER_CALCULATOR_H
