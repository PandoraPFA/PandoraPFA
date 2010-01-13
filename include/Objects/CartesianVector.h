/**
 *  @file   PandoraPFANew/include/Objects/CartesianVector.h
 * 
 *  @brief  Header file for the cartesian vector class.
 * 
 *  $Log: $
 */
#ifndef CARTESIAN_VECTOR_H
#define CARTESIAN_VECTOR_H 1

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
     *  @brief  Default constructor
     */
    CartesianVector();

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
     *  @brief  Copy constructor
     * 
     *  @param  rhs the cartesian vector to copy
     */
    CartesianVector(const CartesianVector &rhs);

    /**
     *  @brief  Query whether the cartesian vector has been initialized
     * 
     *  @return boolean
     */
    bool IsInitialized() const;

    /**
     *  @brief  Reset the cartesian vector
     */
    void Reset();

    /**
     *  @brief  Set the values of cartesian vector components
     * 
     *  @param  x the x coordinate
     *  @param  y the y coordinate
     *  @param  z the z coordinate
     */
    void SetValues(float x, float y, float z);

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

    /**
     *  @brief  Get the spherical coordinates of the cartesian vector
     * 
     *  @param  radius the magnitude of the vector
     *  @param  phi the azimuth of the vector
     *  @param  theta the inclination of the vector
     */
    void GetSphericalCoordinates(float &radius, float &phi, float &theta) const;

    /**
     *  @brief  Get a unit vector in the direction of the cartesian vector
     * 
     *  @return The unit vector
     */
    CartesianVector GetUnitVector() const;

    /**
     *  @brief  Cartesian vector assignment operator
     * 
     *  @param  rhs the cartesian vector to assign
     */
    bool operator=(const CartesianVector &rhs);

    /**
     *  @brief  Cartesian vector += operator
     * 
     *  @param  rhs the cartesian vector to add
     */
    bool operator+=(const CartesianVector &rhs);

    /**
     *  @brief  Cartesian vector -= operator
     * 
     *  @param  rhs the cartesian vector to subtract
     */
    bool operator-=(const CartesianVector &rhs);

    /**
     *  @brief  Cartesian vector *= operator
     * 
     *  @param  scalar the scalar to multiply
     */
    bool operator*=(const double scalar);

private:
    float   m_x;                ///< The x coordinate
    float   m_y;                ///< The y coordinate
    float   m_z;                ///< The z coordinate

    bool    m_isInitialized;    ///< Whether the cartesian vector has been initialized
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
 *  @brief  Cartesian vector multiplication with scalar operator
 * 
 *  @param  lhs the cartesian vector to be multiplied by the scalar
 *  @param  scalar the value of the scalar
 */
CartesianVector operator*(const CartesianVector &lhs, const double scalar);

/**
 *  @brief  Operator to dump cartesian vector properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  cartesianVector the cartesian vector
 */
std::ostream &operator<<(std::ostream & stream, const CartesianVector& cartesianVector);

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CartesianVector::IsInitialized() const
{
    return m_isInitialized;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void CartesianVector::Reset()
{
    m_isInitialized = false;
}

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
