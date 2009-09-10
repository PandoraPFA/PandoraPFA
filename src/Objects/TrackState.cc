/**
 *  @file PandoraPFANew/src/Objects/TrackState.cc
 * 
 *  @brief Implementation of the track state class.
 * 
 *  $Log: $
 */

#include "Objects/TrackState.h"

namespace pandora
{

TrackState::TrackState(float pathLength, float time, float x, float y, float z, float px, float py, float pz) :
    m_pathLength(pathLength),
    m_time(time),
    m_position(CartesianSpacePoint(x, y, z)),
    m_momentum(CartesianVector(px, py, pz))
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackState::TrackState(float pathLength, float time, const CartesianSpacePoint &position, float px, float py, float pz) :
    m_pathLength(pathLength),
    m_time(time),
    m_position(position),
    m_momentum(CartesianVector(px, py, pz))
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackState::TrackState(float pathLength, float time, float x, float y, float z, const CartesianVector &momentum) :
    m_pathLength(pathLength),
    m_time(time),
    m_position(CartesianSpacePoint(x, y, z)),
    m_momentum(momentum)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackState::TrackState(float pathLength, float time, const CartesianSpacePoint &position, const CartesianVector &momentum) :
    m_pathLength(pathLength),
    m_time(time),
    m_position(position),
    m_momentum(momentum)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &stream, const TrackState &trackState)
{
    stream  << " TrackState: " << std::endl
            << " PathLength: " << trackState.GetPathLength() << std::endl
            << " Time:       " << trackState.GetTime() << std::endl
            << " Position:   " << trackState.GetPosition() << std::endl
            << " Momentum:   " << trackState.GetMomentum() << std::endl;

    return stream;
}

} // namespace pandora
