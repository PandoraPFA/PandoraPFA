/**
 *	@file	PandoraPFANew/include/GeometryHelper.h
 *
 * 	@brief	Header file for the geometry helper class.
 *
 *	$Log: $
 */
#ifndef GEOMETRY_HELPER_H
#define GEOMETRY_HELPER_H 1

#include "Api/PandoraApi.h"

namespace pandora
{

/**
 *	@brief	GeometryHelper class
 */
class GeometryHelper
{
private:
	/**
	 *	@brief	Constructor
	 */
	 GeometryHelper();

	/**
	 *	@brief	Constructor
	 * 
	 *	@param	
	 */
	 StatusCode Initialize(const PandoraApi::GeometryParameters &geometryParameters);

	bool	m_isInitialized;		///< Whether the geometry helper is initialized
	float	m_tpcInnerRadius;		///< The tpc inner radius
	
	friend class Pandora;
	friend class PandoraApiImpl;
};

} // namespace pandora

#endif // #ifndef GEOMETRY_HELPER_H
