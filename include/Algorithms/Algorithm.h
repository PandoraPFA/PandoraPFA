/**
 *	@file	PandoraPFANew/include/Algorithms/Algorithm.h
 * 
 *	@brief	Header file for the algorithm class.
 * 
 *	$Log: $
 */
#ifndef ALGORITHM_H
#define ALGORITHM_H 1

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/OrderedCaloHitList.h"
#include "Objects/Track.h"

namespace pandora
{

/**
 *	@brief	Algorithm class
 */
class Algorithm
{
public:
	/**
	 *	@brief	Default constructor
	 */
	Algorithm();

	/**
	 *	@brief	Get the pandora content api impl
	 * 
	 *	@return	Address of the pandora content api impl
	 */
	const PandoraContentApiImpl *const GetPandoraContentApiImpl() const;

	Pandora		*m_pPandora;		///< The pandora object that will run the algorithm

protected:
	/**
	 *	@brief	Run the algorithm
	 */
	virtual StatusCode Run() = 0;

	/**
	 *	@brief	Register the pandora object that will run the algorithm
	 *
	 *	@param	pPandora address of the pandora object that will run the algorithm
	 */
	StatusCode RegisterPandora(Pandora *pPandora);

	friend class PandoraApiImpl;
	friend class PandoraContentApiImpl;	
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline 	Algorithm::Algorithm() :
	m_pPandora(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const PandoraContentApiImpl *const Algorithm::GetPandoraContentApiImpl() const
{
	if (NULL == m_pPandora)
		throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
	
	return m_pPandora->GetPandoraContentApiImpl();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Algorithm::RegisterPandora(Pandora *pPandora)
{
	if (NULL == pPandora)
		return STATUS_CODE_FAILURE;

	m_pPandora = pPandora;
	
	return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef ALGORITHM_H
