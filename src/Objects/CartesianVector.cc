/**
 *  @file PandoraPFANew/src/Objects/CartesianVector.cc
 * 
 *  @brief Implementation of the cartesian vector class.
 * 
 *  $Log: $
 */

#include "Objects/CartesianVector.h"

#include <cmath>

namespace pandora
{

CartesianVector::CartesianVector(float x, float y, float z) :
    m_x(x),
    m_y(y),
    m_z(z)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CartesianVector::GetMagnitude() const
{
    return std::sqrt(this->GetMagnitudeSquared());
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CartesianVector::GetMagnitudeSquared() const
{
    return ((m_x * m_x) + (m_y * m_y) + (m_z * m_z));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CartesianVector::GetDotProduct(const CartesianVector &rhs) const
{
    return ((m_x * rhs.GetX()) + (m_y * rhs.GetY()) + (m_z * rhs.GetZ()));
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector CartesianVector::GetCrossProduct(const CartesianVector &rhs) const
{
    return CartesianVector( (m_y * rhs.GetZ()) - (rhs.GetY() * m_z),
                            (m_z * rhs.GetX()) - (rhs.GetZ() * m_x),
                            (m_x * rhs.GetY()) - (rhs.GetX() * m_y));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CartesianVector::GetOpeningAngle(const CartesianVector &rhs) const
{
    float magnitudesSquared = this->GetMagnitudeSquared() * rhs.GetMagnitudeSquared();

    if (magnitudesSquared <= 0)
    {
        return 0.;
    }
    else
    {
        float cosTheta = this->GetDotProduct(rhs) / std::sqrt(magnitudesSquared);

        if (cosTheta > 1.)
        {
            cosTheta = 1.;
        }
        else if (cosTheta < -1.)
        {
            cosTheta = -1.;
        }

        return acos(cosTheta);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector operator+(const CartesianVector &lhs, const CartesianVector &rhs)
{
    return CartesianVector(lhs.GetX() + rhs.GetX(), lhs.GetY() + rhs.GetY(), lhs.GetZ() + rhs.GetZ());
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector operator-(const CartesianVector &lhs, const CartesianVector &rhs)
{
    return CartesianVector(lhs.GetX() - rhs.GetX(), lhs.GetY() - rhs.GetY(), lhs.GetZ() - rhs.GetZ());
}

//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream & stream, const CartesianVector& cartesianVector)
{
    stream  << "  x: " << cartesianVector.GetX()
            << "  y: " << cartesianVector.GetY()
            << "  z: " << cartesianVector.GetZ()
            << " length: " << cartesianVector.GetMagnitude();

    return stream;
}

} // namespace pandora
