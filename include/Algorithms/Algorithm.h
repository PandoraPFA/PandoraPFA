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

#include "Xml/tinyxml.h"

namespace pandora
{

/**
 *	@brief	Factory class for instantiating algorithms
 */
class AlgorithmFactory
{
public:
	/**
	 *	@brief	Create an instance of an algorithm
	 * 
	 *	@return	the address of the algorithm instance
	 */
	virtual Algorithm *CreateAlgorithm() const = 0;
};

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
	 *	@brief	Default constructor
	 */
	virtual ~Algorithm();
	
	/**
	 *	@brief	Get the pandora content api impl
	 * 
	 *	@return	Address of the pandora content api impl
	 */
	const PandoraContentApiImpl *const GetPandoraContentApiImpl() const;

	/**
	 *	@brief	Get the algorithm type
	 * 
	 *	@return	The algorithm type name
	 */
	const std::string GetAlgorithmType() const;
	
protected:
	/**
	 *	@brief	Run the algorithm
	 */
	virtual StatusCode Run() = 0;

	/**
	 *	@brief	Read the algorithm settings
	 * 
	 *	@param	xmlHandle the relevant xmlHandle
	 */
	virtual StatusCode ReadSettings(TiXmlHandle xmlHandle) = 0;

	/**
	 *	@brief	Register the pandora object that will run the algorithm
	 *
	 *	@param	pPandora address of the pandora object that will run the algorithm
	 */
	StatusCode RegisterPandora(Pandora *pPandora);

	Pandora			*m_pPandora;			///< The pandora object that will run the algorithm
	std::string		m_algorithmType;		///< The type of algorithm

	friend class AlgorithmManager;
	friend class PandoraContentApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline 	Algorithm::Algorithm() :
	m_pPandora(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline 	Algorithm::~Algorithm()
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

inline const std::string Algorithm::GetAlgorithmType() const
{
	return m_algorithmType;
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
