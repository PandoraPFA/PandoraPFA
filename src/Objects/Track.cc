/**
 *  @file   PandoraPFANew/src/Objects/Track.cc
 * 
 *  @brief  Implementation of the track class.
 * 
 *  $Log: $
 */

#include "Objects/Track.h"

namespace pandora
{

bool Track::operator< (const Track &rhs) const
{
    return m_momentumMagnitude > rhs.m_momentumMagnitude;
}

//------------------------------------------------------------------------------------------------------------------------------------------

Track::Track(const PandoraApi::TrackParameters &trackParameters) :
    m_d0(trackParameters.m_d0.Get()),
    m_z0(trackParameters.m_z0.Get()),
    m_momentum(trackParameters.m_momentum.Get()),
    m_momentumMagnitude(m_momentum.GetMagnitude()),
    m_startTrackState(trackParameters.m_startTrackState.Get()),
    m_endTrackState(trackParameters.m_endTrackState.Get()),
    m_eCalTrackState(trackParameters.m_eCalTrackState.Get()),
    m_reachesECal(trackParameters.m_reachesECal.Get()),
    m_pAssociatedCluster(NULL),
    m_pMCParticle(NULL),
    m_pParentAddress(trackParameters.m_pParentAddress.Get())
{
    for (InputTrackStateList::const_iterator iter = trackParameters.m_calorimeterProjections.begin(),
        iterEnd = trackParameters.m_calorimeterProjections.end(); iter != iterEnd; ++iter)
    {
        m_calorimeterProjections.push_back(new TrackState(*iter));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

Track::~Track()
{
    for (TrackStateList::iterator iter = m_calorimeterProjections.begin(),
        iterEnd = m_calorimeterProjections.end(); iter != iterEnd; ++iter)
    {
        delete *iter;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::GetAssociatedCluster(Cluster *pCluster) const
{
    if (NULL == m_pAssociatedCluster)
        return STATUS_CODE_NOT_INITIALIZED;

    pCluster = m_pAssociatedCluster;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::GetMCParticle(MCParticle *pMCParticle) const
{
    if (NULL == m_pMCParticle)
        return STATUS_CODE_NOT_INITIALIZED;

    pMCParticle = m_pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::SetMCParticle(MCParticle *const pMCParticle)
{
    if (NULL == pMCParticle)
        return STATUS_CODE_FAILURE;

    m_pMCParticle = pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &stream, const Track &track)
{
    stream  << " Track: " << std::endl
            << " d0     " << track.GetD0() << std::endl
            << " z0     " << track.GetZ0() << std::endl
            << " p0     " << track.GetMomentum() << std::endl;

    return stream;
}

} // namespace pandora
