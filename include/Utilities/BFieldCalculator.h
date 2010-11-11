/**
 *  @file   PandoraPFANew/include/Utilities/BFieldCalculator.h
 * 
 *  @brief  Header file for the bfield calculator class.
 * 
 *  $Log: $
 */
#ifndef BFIELD_CALCULATOR_H
#define BFIELD_CALCULATOR_H 1

#include "Helpers/GeometryHelper.h"

namespace pandora
{

/**
 *  @brief  BFieldCalculator class
 */
class BFieldCalculator
{
protected:
    /**
     *  @brief  Destructor
     */
    virtual ~BFieldCalculator();

    /**
     *  @brief  Initialize the bfield calculator
     * 
     *  @param  pGeometryHelper
     */
    virtual void Initialize(const GeometryHelper *const pGeometryHelper);

    /**
     *  @brief  Get the bfield value for a specified position vector
     * 
     *  @param  positionVector the specified position
     * 
     *  @return the bfield, units Tesla
     */
    virtual float GetBField(const CartesianVector &positionVector) const = 0;

    friend class GeometryHelper;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline BFieldCalculator::~BFieldCalculator()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void BFieldCalculator::Initialize(const GeometryHelper *const /*pGeometryHelper*/)
{
}

} // namespace pandora

#endif // #ifndef BFIELD_CALCULATOR_H
