/**
 *  @file   PandoraPFANew/include/Algorithms/Cheating/EnergyMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef ENERGY_MONITORING_ALGORITHM_H
#define ENERGY_MONITORING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"


/**
 *  @brief EnergyMonitoringAlgorithm class
 */
class EnergyMonitoringAlgorithm : public pandora::Algorithm
{
private:
public:


    class EnergyMixing
    {
    public:

	class pandora::MCParticle;

	typedef std::set<const pandora::MCParticle*> MCPARTICLESET;


	void AddChargedClusterCalorimetricEnergy( float energy ) { m_chargedCalorimetric += energy; }
	void AddNeutralClusterCalorimetricEnergy( float energy ) { m_neutralCalorimetric += energy; }
	void AddChargedClusterTracksEnergy      ( float energy ) { m_chargedTracks       += energy; }
	void AddPhotonClusterCalorimetricEnergy ( float energy ) { m_photonCalorimetric  += energy; }

	float GetChargedClusterCalorimetricEnergy() { return m_chargedCalorimetric; }
	float GetNeutralClusterCalorimetricEnergy() { return m_neutralCalorimetric; }
	float GetChargedClusterTracksEnergy      () { return m_chargedTracks;       }
	float GetPhotonClusterCalorimetricEnergy () { return m_photonCalorimetric;  }

	void AddMcParticle( const pandora::MCParticle* mcParticle ) { m_mcParticleSet.insert( mcParticle ); }
	MCPARTICLESET& GetMcParticleSet() { return m_mcParticleSet; }
	float GetMcParticleSetEnergy();

	EnergyMixing();

    private:

	MCPARTICLESET m_mcParticleSet;

	float m_chargedCalorimetric;
	float m_chargedTracks;
	float m_neutralCalorimetric;
	float m_photonCalorimetric;
    };



    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

    virtual ~EnergyMonitoringAlgorithm();


private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    StatusCode MonitoringOutput( EnergyMixing& trueChargedHadrons, EnergyMixing& trueNeutral, EnergyMixing& truePhotons );

    typedef std::vector<std::string> STRINGVECTOR;
    STRINGVECTOR m_clusterListNames;                     ///< list of strings denoting clusternames 
    std::string  m_monitoringFileName;                   ///< filename for storing the monitoring information (ROOT)
    std::string  m_treeName;                             ///< tree name for the monitoring output
    bool         m_print;                                ///< print the monitoring info
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EnergyMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new EnergyMonitoringAlgorithm();
}

#endif // #ifndef ENERGY_MONITORING_ALGORITHM_H
