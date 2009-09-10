/**
 *  @file   PandoraPFANew/include/Objects/CartesianSpacePoint.h
 * 
 *  @brief  Header file for the cartesian space point class.
 * 
 *  $Log: $
 */
#ifndef CARTESIAN_SPACE_POINT_H
#define CARTESIAN_SPACE_POINT_H 1

#include <iostream>

namespace pandora
{

/**
 *  @brief  CartesianSpacePoint class
 */
class CartesianSpacePoint
{
public:
    /**
     *  @brief  Constructor, create a point given its cartesian coordinates x, y, z 
     *
     *  @param  x the x coordinate
     *  @param  y the y coordinate
     *  @param  z the z coordinate
     */
    CartesianSpacePoint(float x, float y, float z);

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
     *  @brief  Get the distance to a second cartesian space point
     * 
     *  @param  rhs the second cartesian space point
     * 
     *  @return The distance to the second cartesian space point
     */
    float GetDistanceToSpacePoint(const CartesianSpacePoint &rhs);

private:
    const float   m_x;    ///< The x coordinate
    const float   m_y;    ///< The y coordinate
    const float   m_z;    ///< The z coordinate
};

/**
 *  @brief  Operator to dump cartesian space point properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  cartesianSpacePoint the cartesian space point
 */
std::ostream &operator<<(std::ostream &stream, const CartesianSpacePoint &cartesianSpacePoint);

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianSpacePoint::GetX() const
{
    return m_x;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianSpacePoint::GetY() const
{
    return m_y;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CartesianSpacePoint::GetZ() const
{
    return m_z;
}

} // namespace pandora

#endif // #ifndef CARTESIAN_SPACE_POINT_H
