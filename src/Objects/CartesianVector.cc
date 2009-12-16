/**
 *  @file PandoraPFANew/src/Objects/CartesianVector.cc
 * 
 *  @brief Implementation of the cartesian vector class.
 * 
 *  $Log: $
 */

#include "Objects/CartesianVector.h"

#include "StatusCodes.h"

#include <cmath>

namespace pandora
{

CartesianVector::CartesianVector() :
    m_isInitialized(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector::CartesianVector(float x, float y, float z) :
    m_x(x),
    m_y(y),
    m_z(z),
    m_isInitialized(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector::CartesianVector(const CartesianVector &rhs) :
    m_x(rhs.GetX()),
    m_y(rhs.GetY()),
    m_z(rhs.GetZ()),
    m_isInitialized(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CartesianVector::SetValues(float x, float y, float z)
{
    m_x = x;
    m_y = y;
    m_z = z;

    m_isInitialized = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CartesianVector::GetMagnitude() const
{
    return std::sqrt(this->GetMagnitudeSquared());
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CartesianVector::GetMagnitudeSquared() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return ((m_x * m_x) + (m_y * m_y) + (m_z * m_z));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float CartesianVector::GetDotProduct(const CartesianVector &rhs) const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return ((m_x * rhs.GetX()) + (m_y * rhs.GetY()) + (m_z * rhs.GetZ()));
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector CartesianVector::GetCrossProduct(const CartesianVector &rhs) const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

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

void CartesianVector::GetSphericalCoordinates(float &radius, float &phi, float &theta) const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    const float magnitude(this->GetMagnitude());

    if (0 == magnitude)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    radius = magnitude;
    phi    = std::acos(m_z / radius);
    theta  = std::atan2(m_y , m_x);
}

//------------------------------------------------------------------------------------------------------------------------------------------

CartesianVector CartesianVector::GetUnitVector() const
{
    if (!m_isInitialized)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    const float magnitude(this->GetMagnitude());

    if (0 == magnitude)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    return CartesianVector(m_x / magnitude, m_y / magnitude, m_z / magnitude);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CartesianVector::operator=(const CartesianVector &rhs)
{
    this->SetValues(rhs.GetX(), rhs.GetY(), rhs.GetZ());

    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CartesianVector::operator+=(const CartesianVector &rhs)
{
    if (!m_isInitialized)
    {
        this->SetValues(rhs.GetX(), rhs.GetY(), rhs.GetZ());
    }
    else
    {
        this->SetValues(m_x + rhs.GetX(), m_y + rhs.GetY(), m_z + rhs.GetZ());
    }

    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CartesianVector::operator-=(const CartesianVector &rhs)
{
    if (!m_isInitialized)
    {
        this->SetValues(-rhs.GetX(), -rhs.GetY(), -rhs.GetZ());
    }
    else
    {
        this->SetValues(m_x - rhs.GetX(), m_y - rhs.GetY(), m_z - rhs.GetZ());
    }

    return m_isInitialized;
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

CartesianVector operator*(const CartesianVector &lhs, const double scalar)
{
    return CartesianVector(static_cast<float>(lhs.GetX() * scalar), static_cast<float>(lhs.GetY() * scalar), static_cast<float>(lhs.GetZ() * scalar));
}

//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream & stream, const CartesianVector& cartesianVector)
{
    if (!cartesianVector.IsInitialized())
    {
        stream  << "Not initialized.";
    }
    else
    {
        stream  << "  x: " << cartesianVector.GetX()
                << "  y: " << cartesianVector.GetY()
                << "  z: " << cartesianVector.GetZ()
                << " length: " << cartesianVector.GetMagnitude();
    }

    return stream;
}

} // namespace pandora
