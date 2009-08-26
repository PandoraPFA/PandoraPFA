/**
 *	@file	PandoraPFANew/include/Objects/Track.h
 * 
 *	@brief	Header file for the track class.
 * 
 *	$Log: $
 */
#ifndef TRACK_H
#define TRACK_H 1

#include "Api/PandoraApi.h"

namespace pandora
{
	
/**
 *	@brief	Track class
 */
class Track 
{
public:
	/**
	 *	@brief	Operator< now orders by track momentum
	 * 
	 *	@param	rhs track to compare with
	 */
	bool operator< (const Track &rhs) const;

	/**
	 *	@brief	Get the track momentum
	 */
	float GetMomentum() const;

	/**
	 *	@brief	Get the address of the parent track in the user framework
	 */
	void *GetParentTrackAddress() const;
	 
private:
	/**
	 *	@brief	Constructor
	 * 
	 *	@param	parameters the calo hit parameters
	 */
	Track(const PandoraApi::TrackParameters &trackParameters);

	/**
	 *	@brief	Destructor
	 */
	~Track();

	float	m_momentum;				///< The track momentum
	void	*m_pParentAddress;		///< The address of the parent track in the user framework
	
	friend class TrackManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Track::GetMomentum() const
{
	return m_momentum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void *Track::GetParentTrackAddress() const
{
	return m_pParentAddress;
}

} // namespace pandora

#endif // #ifndef TRACK_H
