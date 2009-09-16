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

#include "PandoraInternal.h"

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
     *  @brief  Get the 2D impact parameter wrt (0,0)
     * 
     *  @return the 2D impact parameter wrt (0,0)
     */
    float GetD0() const;

    /**
     *  @brief  Get the z coordinate at the 2D distance of closest approach
     * 
     *  @return the z coordinate at the 2D distance of closest approach
     */
    float GetZ0() const;

    /**
     *  @brief  Get the track momentum at the 2D distance of closest approach
     * 
     *  @return the track momentum at the 2D distance of closest approach
     */
    const CartesianVector &GetMomentum() const;

    /**
     *  @brief  Get the track state at the start of the track
     * 
     *  @return the track state at the start of the track
     */
    const TrackState &GetStartTrackState() const;

    /**
     *  @brief  Get the track state at the end of the track
     * 
     *  @return the track state at the end of the track
     */
    const TrackState &GetEndTrackState() const;

    /**
     *  @brief  Get the (sometimes projected) track state at the ecal
     * 
     *  @return the track state at the ecal
     */
    const TrackState &GetECalTrackState() const;

    /**
     *  @brief  Whether the track reaches the ecal
     * 
     *  @return boolean
     */
    bool ReachesECal() const;

    /**
     *  @brief  Get the list of calorimeter track state projections
     * 
     *  @return address of the list of calorimeter track state projections
     */
    const TrackStateList &GetCalorimeterProjections() const;

    /**
     *  @brief  Get address of the cluster associated with the track
     * 
     *  @param  pCluster to receive the address of the cluster
     */
    StatusCode GetAssociatedCluster(Cluster *pCluster) const;

    /**
     *  @brief  Get address of the mc particle associated with the track
     * 
     *  @param  pMCParticle to receive the address of the mc particle
     */
    StatusCode GetMCParticle(MCParticle *pMCParticle) const;    

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

    /**
     *  @brief  Set the mc particle associated with the track
     * 
     *  @param  pMCParticle to receive the address of the mc particle
     */
    StatusCode SetMCParticle(MCParticle *const pMCParticle);

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

inline float Track::GetD0() const
{
    return m_d0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Track::GetZ0() const
{
    return m_z0;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Track::GetMomentum() const
{
    return m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackState &Track::GetStartTrackState() const
{
    return m_startTrackState;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackState &Track::GetEndTrackState() const
{
    return m_endTrackState;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackState &Track::GetECalTrackState() const
{
    return m_eCalTrackState;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Track::ReachesECal() const
{
    return m_reachesECal;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackStateList &Track::GetCalorimeterProjections() const
{
    return m_calorimeterProjections;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const void *Track::GetParentTrackAddress() const
{
    return m_pParentAddress;
}

} // namespace pandora

#endif // #ifndef TRACK_H
