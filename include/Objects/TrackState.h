/**
 *  @file   PandoraPFANew/include/Objects/TrackState.h
 * 
 *  @brief  Header file for the track state class.
 * 
 *  $Log: $
 */
#ifndef TRACK_STATE_H
#define TRACK_STATE_H 1

#include "Objects/CartesianSpacePoint.h"
#include "Objects/CartesianVector.h"

namespace pandora
{

/**
 *  @brief  TrackState class
 */
class TrackState
{
public:
    /**
     *  @brief  Constructor, from position coordinates and momentum components
     *
     *  @param  pathLength the path length
     *  @param  time the time
     *  @param  x the x position coordinate
     *  @param  y the y position coordinate
     *  @param  z the z position coordinate
     *  @param  px the momentum x component
     *  @param  py the momentum y component
     *  @param  pz the momentum z component
     */
    TrackState(float pathLength, float time, float x, float y, float z, float px, float py, float pz);

    /**
     *  @brief  Constructor, from position space point and momentum components
     *
     *  @param  pathLength the path length
     *  @param  time the time
     *  @param  position the track position space point
     *  @param  px the momentum x component
     *  @param  py the momentum y component
     *  @param  pz the momentum z component
     */
    TrackState(float pathLength, float time, const CartesianSpacePoint &position, float px, float py, float pz);

    /**
     *  @brief  Constructor, from position coordinates and momentum vector
     *
     *  @param  pathLength the path length
     *  @param  time the time
     *  @param  x the x position coordinate
     *  @param  y the y position coordinate
     *  @param  z the z position coordinate
     *  @param  momentum the momentum vector
     */
    TrackState(float pathLength, float time, float x, float y, float z, const CartesianVector &momentum);

    /**
     *  @brief  Constructor, from position space point and momentum vector
     *
     *  @param  pathLength the path length
     *  @param  time the time
     *  @param  position the track position space pointe
     *  @param  momentum the momentum vector
     */
    TrackState(float pathLength, float time, const CartesianSpacePoint &position, const CartesianVector &momentum);

    /**
     *  @brief  Get the path length
     * 
     *  @return the path length
     */
    float GetPathLength() const;

    /**
     *  @brief  Get the time
     * 
     *  @return the time
     */
    float GetTime() const;

    /**
     *  @brief  Get the track position space point
     * 
     *  @return the track position space point
     */
    const CartesianSpacePoint &GetPosition() const;

    /**
     *  @brief  Get the track momentum vector
     * 
     *  @return the track momentum vector
     */
    const CartesianVector &GetMomentum() const;

private:
    const float                 m_pathLength;   ///< The path length
    const float                 m_time;         ///< The time
    const CartesianSpacePoint   m_position;     ///< The position space point
    const CartesianVector       m_momentum;     ///< The momentum vector
};

/**
 *  @brief  Operator to dump track state properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  trackState the track state
 */
std::ostream &operator<<(std::ostream &stream, const TrackState &trackState);

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TrackState::GetPathLength() const
{
    return m_pathLength;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TrackState::GetTime() const
{
    return m_time;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianSpacePoint &TrackState::GetPosition() const
{
    return m_position;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &TrackState::GetMomentum() const
{
    return m_momentum;
}

} // namespace pandora

#endif // #ifndef TRACK_STATE_H
