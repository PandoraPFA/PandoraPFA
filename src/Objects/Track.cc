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

    m_parentTrackList.clear();
    m_siblingTrackList.clear();
    m_daughterTrackList.clear();
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

StatusCode Track::SetAssociatedCluster(Cluster *const pCluster)
{
    if (NULL == pCluster)
        return STATUS_CODE_INVALID_PARAMETER;

    if (NULL != m_pAssociatedCluster)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_pAssociatedCluster = pCluster;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::RemoveAssociatedCluster(Cluster *const pCluster)
{
    if (pCluster != m_pAssociatedCluster)
        return STATUS_CODE_NOT_FOUND;

    m_pAssociatedCluster = NULL;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddParent(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_parentTrackList.insert(pTrack).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddDaughter(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_daughterTrackList.insert(pTrack).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Track::AddSibling(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_siblingTrackList.insert(pTrack).second)
        return STATUS_CODE_ALREADY_PRESENT;

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

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SortByMomentum(const TrackList &trackList, MomentumSortedTrackList &momentumSortedTrackList)
{
    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        if (!momentumSortedTrackList.insert(*iter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
