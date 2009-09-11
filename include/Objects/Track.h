/**
 *  @file   PandoraPFANew/include/Objects/Track.h
 * 
 *  @brief  Header file for the track class.
 * 
 *  $Log: $
 */
#ifndef TRACK_H
#define TRACK_H 1

#include "Api/PandoraApi.h"

namespace pandora
{
    
/**
 *  @brief  Track class
 */
class Track 
{
public:
    /**
     *  @brief  Operator< now orders by track momentum
     * 
     *  @param  rhs track to compare with
     */
    bool operator< (const Track &rhs) const;

    /**
     *  @brief  Get the track momentum at the 2D distance of closest approach
     * 
     *  @return the track momentum at the 2D distance of closest approach
     */
    const CartesianVector &GetMomentum() const;

    /**
     *  @brief  Get the address of the parent track in the user framework
     *
     *  @param  the address of the parent track in the user framework
     */
    const void *GetParentTrackAddress() const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the calo hit parameters
     */
    Track(const PandoraApi::TrackParameters &trackParameters);

    /**
     *  @brief  Destructor
     */
    ~Track();

    const float             m_d0;                       ///< The 2D impact parameter wrt (0,0)
    const float             m_z0;                       ///< The z coordinate at the 2D distance of closest approach
    const CartesianVector   m_momentum;                 ///< The momentum vector at the 2D distance of closest approach
    const float             m_momentumMagnitude;        ///< The magnitude of the momentum at the 2D distance of closest approach

    const TrackState        m_startTrackState;          ///< The track state at the start of the track
    const TrackState        m_endTrackState;            ///< The track state at the end of the track
    const TrackState        m_eCalTrackState;           ///< The (sometimes projected) track state at the ecal

    const bool              m_reachesECal;              ///< Whether the track reaches the ecal
    TrackStateList          m_calorimeterProjections;   ///< A list of alternative track state projections to the calorimeters

    Cluster                 *m_pAssociatedCluster;      ///< The address of an associated cluster
    MCParticle              *m_pMCParticle;             ///< The address of the associated MC particle
    const void              *m_pParentAddress;          ///< The address of the parent track in the user framework

    friend class TrackManager;
};

/**
 *  @brief  Operator to dump track properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  track the track
 */
std::ostream &operator<<(std::ostream &stream, const Track &track);

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Track::GetMomentum() const
{
    return m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const void *Track::GetParentTrackAddress() const
{
    return m_pParentAddress;
}

} // namespace pandora

#endif // #ifndef TRACK_H
