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
    return m_momentumMagnitudeAtDca > rhs.m_momentumMagnitudeAtDca;
}

//------------------------------------------------------------------------------------------------------------------------------------------

Track::Track(const PandoraApi::TrackParameters &trackParameters) :
    m_d0(trackParameters.m_d0.Get()),
    m_z0(trackParameters.m_z0.Get()),
    m_momentumAtDca(trackParameters.m_momentumAtDca.Get()),
    m_momentumMagnitudeAtDca(m_momentumAtDca.GetMagnitude()),
    m_trackStateAtStart(trackParameters.m_trackStateAtStart.Get()),
    m_trackStateAtEnd(trackParameters.m_trackStateAtEnd.Get()),
    m_trackStateAtECal(trackParameters.m_trackStateAtECal.Get()),
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
            << " p0     " << track.GetMomentumAtDca() << std::endl;

    return stream;
}

} // namespace pandora
