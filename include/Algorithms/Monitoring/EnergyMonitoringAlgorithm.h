/**
 *  @file   PandoraPFANew/include/Algorithms/Monitoring/EnergyMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the energy monitoring algorithm
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

	typedef std::set<const pandora::MCParticle*> McParticleSet;


	void AddChargedClusterCalorimetricEnergy( float energy ) { m_chargedCalorimetric += energy; }
	void AddNeutralClusterCalorimetricEnergy( float energy ) { m_neutralCalorimetric += energy; }
	void AddChargedClusterTracksEnergy      ( float energy ) { m_chargedTracks       += energy; }
	void AddPhotonClusterCalorimetricEnergy ( float energy ) { m_photonCalorimetric  += energy; }

	void AddChargedCluster() { m_chargedCalorimetricClusters += 1; }
	void AddNeutralCluster() { m_neutralCalorimetricClusters += 1; }
	void AddTrack         () { m_chargedTracksNumber         += 1; }
	void AddPhotonCluster () { m_photonCalorimetricClusters  += 1; }


	float GetChargedClusterCalorimetricEnergy() { return m_chargedCalorimetric; }
	float GetNeutralClusterCalorimetricEnergy() { return m_neutralCalorimetric; }
	float GetChargedClusterTracksEnergy      () { return m_chargedTracks;       }
	float GetPhotonClusterCalorimetricEnergy () { return m_photonCalorimetric;  }

	int GetChargedClusterCalorimetricClusters() { return m_chargedCalorimetricClusters; }
	int GetNeutralClusterCalorimetricClusters() { return m_neutralCalorimetricClusters; }
	int GetChargedClusterTracks              () { return m_chargedTracksNumber;         }
	int GetPhotonClusterCalorimetricClusters () { return m_photonCalorimetricClusters;  }

	void AddMcParticle( const pandora::MCParticle* mcParticle ) { m_mcParticleSet.insert( mcParticle ); }
	McParticleSet& GetMcParticleSet() { return m_mcParticleSet; }
	float GetMcParticleSetEnergy();
	int   GetMcParticleNumber();

	EnergyMixing();

    private:

	McParticleSet m_mcParticleSet;

	float m_chargedCalorimetric;
	float m_chargedTracks;
	float m_neutralCalorimetric;
	float m_photonCalorimetric;

	int m_chargedCalorimetricClusters;
	int m_chargedTracksNumber;
	int m_neutralCalorimetricClusters;
	int m_photonCalorimetricClusters;

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

    StatusCode MonitoringOutput( EnergyMixing& trueChargedHadrons, EnergyMixing& trueNeutral, EnergyMixing& truePhotons, int numberClusters, int numberTracks );

    pandora::StringVector m_clusterListNames;            ///< list of strings denoting clusternames 
    bool         m_clusters;                             ///< show current clusters
    std::string  m_monitoringFileName;                   ///< filename for storing the monitoring information (ROOT)
    std::string  m_treeName;                             ///< tree name for the monitoring output
    bool         m_print;                                ///< print the monitoring info
    bool         m_quantity;                             ///< show the number of clusters/tracks
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EnergyMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new EnergyMonitoringAlgorithm();
}

#endif // #ifndef ENERGY_MONITORING_ALGORITHM_H
