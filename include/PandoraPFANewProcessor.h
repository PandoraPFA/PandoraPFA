/**
 *	@file	PandoraPFANew/include/PandoraPFANewProcessor.h
 * 
 *	@brief	Header file for the pandora pfa new processor class.
 * 
 *	$Log: $
 */
#ifndef PANDORA_PFA_NEW_PROCESSOR_H
#define PANDORA_PFA_NEW_PROCESSOR_H 1

#include "marlin/Processor.h"

#include "Pandora.h"

/**
 *	@brief	PandoraPFANewProcessor class
 */
class PandoraPFANewProcessor : public marlin::Processor
{
public:
	typedef std::vector<std::string> StringVector;

	/**
	 *	@brief Settings class
	 */
	class Settings
	{
	  public:
		std::string		m_pandoraSettingsXmlFile;	///< The pandora settings xml file
		
		StringVector	m_trackCollections;			///< The reconstructed track collections
		StringVector	m_v0VertexCollections;		///< The v0 vertex collections
		StringVector	m_hCalCollections;			///< The hcal calorimeter hit collections
		StringVector	m_eCalCollections;			///< The ecal calorimeter hit collections	
	};		

	/**
	 *	@brief	Default constructor
	 */
	PandoraPFANewProcessor();

	/**
	 *	@brief	Create new processor
	 */
	virtual Processor *newProcessor();

	/*
	 * @brief	Initialize, called at startup
	 */
	virtual void init();

	/**
	 *	@brief	Process run header
	 *
	 *	@param	pLCRunHeader the lc run header
	 */
	virtual void processRunHeader(lcio::LCRunHeader *pLCRunHeader);

	/**
	 *	@brief	Process event, main entry point
	 *
	 *	@param	pLCEvent the lc event
	 */
	virtual void processEvent(lcio::LCEvent *pLCEvent);

	/**
	 *	@brief	Checks for event
	 *
	 *	@param	pLCEvent the lc event
	 */
	virtual void check(lcio::LCEvent *pLCEvent);

	/**
	 *	@brief	End, called at shutdown
	 */
	virtual void end();

private:
	/**
	 *	@brief	Create geometry, insert user code here
	 */
	StatusCode CreateGeometry();

	/**
	 *	@brief	Register user algorithm factories, insert user code here
	 */
	StatusCode RegisterUserAlgorithmFactories();

	/**
	 *	@brief	Create tracks, insert user code here
	 * 
	 *	@param	pLCEvent the lcio event
	 */	
	StatusCode CreateTracks(const LCEvent *const pLCEvent);
	
	/**
	 *	@brief	Create calo hits, insert user code here
	 * 
	 *	@param	pLCEvent the lcio event
	 */	
	StatusCode CreateCaloHits(const LCEvent *const pLCEvent);
	
	/**
	 *	@brief	Create MC trees from MC particles and their relationships, insert user code here
	 * 
	 *	@param	pLCEvent the lcio event
	 */	
	StatusCode CreateMCTrees(const LCEvent *const pLCEvent);
	
	/**
	 *	@brief	Process particle flow objects, insert user code here
	 * 
	 *	@param	pLCEvent the lcio event
	 */	
	StatusCode ProcessParticleFlowObjects(const LCEvent *const pLCEvent);
	
  	/**
	 *	@brief	Process steering file parameters, insert user code here
	 */
	void ProcessSteeringFile();

	pandora::Pandora	m_pandora;			///< The pandora instance
	Settings			m_settings;			///< The settings for the pandora pfa new processor
	std::string			m_detectorName;		///< The detector name
	unsigned int		m_nRun;				///< The run number
	unsigned int		m_nEvent;			///< The event number
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline marlin::Processor *PandoraPFANewProcessor::newProcessor()
{
	return new PandoraPFANewProcessor;
}

#endif // #ifndef PANDORA_PFA_NEW_PROCESSOR_H
