/**
 *	@file	PandoraPFANew/src/Objects/Track.cc
 * 
 *	@brief	Implementation of the track class.
 * 
 *	$Log: $
 */

#include "Objects/Track.h"

namespace pandora
{

bool Track::operator< (const Track &rhs) const
{
	return m_momentum > rhs.m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

Track::Track(const PandoraApi::TrackParameters &trackParameters) :
	m_momentum(trackParameters.m_momentum),
	m_pParentAddress(trackParameters.m_pParentAddress)
{
}
	
} // namespace pandora
