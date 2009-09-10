/**
 *  @file   PandoraPFANew/include/Objects/CartesianVector.h
 * 
 *  @brief  Header file for the cartesian vector class.
 * 
 *  $Log: $
 */
#ifndef CARTESIAN_VECTOR_H
#define CARTESIAN_VECTOR_H 1

#include "Objects/CartesianSpacePoint.h"

#include <iostream>

namespace pandora
{

/**
 *  @brief  CartesianVector class
 */
class CartesianVector
{
public:
    /**
     *  @brief  Constructor, create a vector from the cartesian coordinates of the end point,
     *          origin at (0,0,0,)
     * 
     *  @param  x the end point x coordinate
     *  @param  y the end point y coordinate
     *  @param  z the end point z coordinate
     */
    CartesianVector(float x, float y, float z);

    /**
     *  @brief  Constructor, create a vector from a cartesian space point, origin at (0,0,0)
     * 
     *  @param  cartesianSpacePoint the cartesian space point
     */
    CartesianVector(const CartesianSpacePoint &cartesianSpacePoint);

    /**
     *  @brief  Constructor, create a vector from two cartesian space points. The result points
     *          from spacePoint1 to spacePoint2, origin at (0,0,0).
     * 
     *  @param  spacePoint1 cartesian space point 1
     *  @param  spacePoint2 cartesian space point 2
     */
    CartesianVector(const CartesianSpacePoint &spacePoint1, const CartesianSpacePoint &spacePoint2);

    /**
     *  @brief  Get the cartesian x coordinate
     * 
     *  @return The cartesian x coordinate
     */
    float GetX() const;

    /**
     *  @brief  Get the cartesian y coordinate
     * 
     *  @return The cartesian y coordinate
     */
    float GetY() const;

    /**
     *  @brief  Get the cartesian z coordinate
     * 
     *  @return The cartesian z coordinate
     */
    float GetZ() const;

    /**
     *  @brief  Get the magnitude
     * 
     *  @return The magnitude
     */
    float GetMagnitude() const;

    /**
     *  @brief  Get the magnitude squared
     * 
     *  @return The magnitude squared
     */
    float GetMagnitudeSquared() const;

    /**
     *  @brief  Get the dot product of the cartesian vector with a second cartesian vector
     * 
     *  @param  rhs the second cartesian vector
     * 
     *  @return The dot product
     */
    float GetDotProduct(const CartesianVector &rhs) const;

    /**
     *  @brief  Get the cross product of the cartesian vector with a second cartesian vector
     * 
     *  @param  rhs the second cartesian vector
     * 
     *  @return The cross product
     */
    CartesianVector GetCrossProduct(const CartesianVector &rhs) const;

    /**
     *  @brief  Get the opening angle of the cartesian vector with respect to a second cartesian vector
     * 
     *  @param  rhs the second cartesian vector
     * 
     *  @return The opening angle
     */
    float GetOpeningAngle(const CartesianVector &rhs) const;

private:
    float   m_x;    ///< The x coordinate
    float   m_y;    ///< The y coordinate
    float   m_z;    ///< The z coordinate
};

/**
 *  @brief  Cartesian vector addition operator
 * 
 *  @param  lhs first cartesian vector, to which the second is added
 *  @param  rhs second cartesian vector, which is added to the first
 */
CartesianVector operator+(const CartesianVector &lhs, const CartesianVector &rhs);

/**
 *  @brief  Cartesian vector subtraction operator
 * 
 *  @param  lhs first cartesian vector, from which the second is subtracted
 *  @param  rhs second cartesian vector, which is subtracted from the first
 */
CartesianVector operator-(const CartesianVector &lhs, const CartesianVector &rhs);

/**
 *  @brief  Operator to dump cartesian vector properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  cartesianVector the cartesian vector
 */
std::ostream &operator<<(std::ostream & stream, const CartesianVector& cartesianVector);

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetX() const
{
    return m_x;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetY() const
{
    return m_y;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianVector::GetZ() const
{
    return m_z;
}

} // namespace pandora

#endif // #ifndef CARTESIAN_VECTOR_H
