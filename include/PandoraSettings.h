/**
 *	@file	PandoraPFANew/include/PandoraSettings.h
 * 
 *	@brief	Header file for the pandora settings class.
 * 
 *	$Log: $
 */
#ifndef PANDORA_SETTINGS_H
#define PANDORA_SETTINGS_H 1

#include <vector>

typedef std::vector<std::string> StringVector;

/**
 *	@brief PandoraSettings class
 */
class PandoraSettings
{
  public:
	unsigned int 	m_numberOfPseudoLayers;	///< The number of pseudolayers

	StringVector	m_trackCollections;		///< The reconstructed track collections
	StringVector	m_v0VertexCollections;	///< The v0 vertex collections
	StringVector	m_hCalCollections;		///< The hcal calorimeter hit collections
	StringVector	m_eCalCollections;		///< The ecal calorimeter hit collections	
	
	StringVector	m_lCalCollections;		///< The lcal calorimeter hit collections	
	StringVector	m_bCalCollections;		///< The bcal calorimeter hit collections
	StringVector	m_lHCalCollections;		///< The lhcal calorimeter hit collections
	StringVector	m_muonCollections;		///< The muon hit collections
};

#endif // #ifndef PANDORA_SETTINGS_H
