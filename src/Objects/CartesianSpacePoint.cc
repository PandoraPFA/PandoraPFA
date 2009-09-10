/**
 *  @file PandoraPFANew/src/Objects/CartesianSpacePoint.cc
 * 
 *  @brief Implementation of the cartesian space point class.
 * 
 *  $Log: $
 */

#include "Objects/CartesianSpacePoint.h"

#include <cmath>

namespace pandora
{

CartesianSpacePoint::CartesianSpacePoint(float x, float y, float z) :
    m_x(x),
    m_y(y),
    m_z(z)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CartesianSpacePoint::GetDistanceToSpacePoint(const CartesianSpacePoint &rhs)
{
    float dx = rhs.GetX() - this->GetX();
    float dy = rhs.GetY() - this->GetY();
    float dz = rhs.GetZ() - this->GetZ();

    return std::sqrt( (dx * dx) + (dy * dy) + (dz * dz));
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &stream, const CartesianSpacePoint &cartesianSpacePoint)
{
    stream  << " CartesianSpacePoint: " << std::endl
            << "    x: " << cartesianSpacePoint.GetX() << std::endl
            << "    y: " << cartesianSpacePoint.GetY() << std::endl
            << "    z: " << cartesianSpacePoint.GetZ() << std::endl;

    return stream;
}

} // namespace pandora
