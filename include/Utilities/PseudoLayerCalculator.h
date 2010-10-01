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
public:
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
};

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  HighGranularityPseudoLayerCalculator class
 */
class HighGranularityPseudoLayerCalculator : public PseudoLayerCalculator
{
public:
    void Initialize(const GeometryHelper *const pGeometryHelper);
    PseudoLayer GetPseudoLayer(const CartesianVector &positionVector) const;

private:
    typedef std::vector<float> LayerPositionList;

    /**
     *  @brief  Store barrel and layer positions upon initialization
     * 
     *  @param  layerParametersList the layer parameters list
     */
    void StoreLayerPositions(const GeometryHelper::LayerParametersList &layerParametersList, LayerPositionList &LayerPositionList);

    /**
     *  @brief  Get the appropriate pseudolayer for a specified position, with reference to given barrel and endcap parameters
     * 
     *  @param  barrelParameters the relevant barrel parameters
     *  @param  endCapParameters the relevant endcap parameters
     *  @param  radius the radial coordinate
     *  @param  zPosition the z coordinate
     *  @param  pseudoLayer to receive the appropriate pseudolayer
     */
    StatusCode GetPseudoLayer(const GeometryHelper::SubDetectorParameters &barrelParameters,
    const GeometryHelper::SubDetectorParameters &endCapParameters, const float rCoordinate, const float zCoordinate, PseudoLayer &pseudoLayer) const;

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
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayerCalculator::~PseudoLayerCalculator()
{
}

} // namespace pandora

#endif // #ifndef PSEUDO_LAYER_CALCULATOR_H
