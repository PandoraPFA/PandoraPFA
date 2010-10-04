/**
 *  @file   PandoraPFANew/include/Utilities/PseudoLayerCalculator.h
 * 
 *  @brief  Header file for the pseudo layer calculator class.
 * 
 *  $Log: $
 */
#ifndef PSEUDO_LAYER_CALCULATOR_H
#define PSEUDO_LAYER_CALCULATOR_H 1

#include "Helpers/GeometryHelper.h"

namespace pandora
{

/**
 *  @brief  PseudoLayerCalculator class
 */
class PseudoLayerCalculator
{
protected:
    /**
     *  @brief  Destructor
     */
    virtual ~PseudoLayerCalculator();

    /**
     *  @brief  Initialize the pseudo layer calculator
     * 
     *  @param  pGeometryHelper
     */
    virtual void Initialize(const GeometryHelper *const pGeometryHelper) = 0;

    /**
     *  @brief  Get the appropriate pseudolayer for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the appropriate pseudolayer
     */
    virtual PseudoLayer GetPseudoLayer(const CartesianVector &positionVector) const = 0;

    friend class GeometryHelper;
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  HighGranularityPseudoLayerCalculator class
 */
class HighGranularityPseudoLayerCalculator : public PseudoLayerCalculator
{
private:
    void Initialize(const GeometryHelper *const pGeometryHelper);
    PseudoLayer GetPseudoLayer(const CartesianVector &positionVector) const;

    typedef std::vector<float> LayerPositionList;

    /**
     *  @brief  Store barrel and layer positions upon initialization
     * 
     *  @param  layerParametersList the layer parameters list
     */
    void StoreLayerPositions(const GeometryHelper::LayerParametersList &layerParametersList, LayerPositionList &LayerPositionList);

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
    StatusCode GetPseudoLayer(const float rCoordinate, const float zCoordinate, const float rCorrection, const float zCorrection, 
        const float barrelInnerR, const float endCapInnerZ, PseudoLayer &pseudoLayer) const;

    /**
     *  @brief  Find the layer number corresponding to a specified position, via reference to a specified layer position list
     * 
     *  @param  position the specified position
     *  @param  layerPositionList the specified layer position list
     *  @param  layer to receive the layer number
     */
    StatusCode FindMatchingLayer(const float position, const LayerPositionList &layerPositionList, unsigned int &layer) const;

    LayerPositionList   m_barrelLayerPositions;     ///< List of barrel layer positions
    LayerPositionList   m_endCapLayerPositions;     ///< List of endcap layer positions

    float               m_barrelInnerR;             ///< Barrel inner radius
    float               m_endCapInnerZ;             ///< Endcap inner z position
    float               m_barrelInnerRMuon;         ///< Muon barrel inner radius
    float               m_endCapInnerZMuon;         ///< Muon endcap inner z position

    float               m_rCorrection;              ///< Barrel/endcap overlap r correction
    float               m_zCorrection;              ///< Barrel/endcap overlap z correction
    float               m_rCorrectionMuon;          ///< Muon barrel/endcap overlap r correction
    float               m_zCorrectionMuon;          ///< Muon barrel/endcap overlap z correction
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayerCalculator::~PseudoLayerCalculator()
{
}

} // namespace pandora

#endif // #ifndef PSEUDO_LAYER_CALCULATOR_H
