/**
 *  @file   PandoraPFANew/Framework/include/Utilities/BFieldCalculator.h
 * 
 *  @brief  Header file for the bfield calculator interface class.
 * 
 *  $Log: $
 */
#ifndef BFIELD_CALCULATOR_H
#define BFIELD_CALCULATOR_H 1

#include "Helpers/GeometryHelper.h"

#include "Objects/CartesianVector.h"

class TiXmlHandle;

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
     *  @brief  Initialize the bfield calculator using information from geometry helper
     * 
     *  @param  pGeometryHelper address of the geometry helper
     */
    virtual void Initialize(const GeometryHelper *const pGeometryHelper);

    /**
     *  @brief  Initialize the bfield calculator using information from xml file
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    virtual void Initialize(const TiXmlHandle *const pXmlHandle);

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

//------------------------------------------------------------------------------------------------------------------------------------------

inline void BFieldCalculator::Initialize(const TiXmlHandle *const /*pXmlHandle*/)
{
}

} // namespace pandora

#endif // #ifndef BFIELD_CALCULATOR_H
