/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Monitoring/EnergyMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the energy monitoring algorithm
 * 
 *  $Log: $
 */
#ifndef ENERGY_MONITORING_ALGORITHM_H
#define ENERGY_MONITORING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#define FORMATTED_OUTPUT_SHORT(TITLE, E1, E2, E3)                                                           \
    std::cout << std::left << std::setw(width) << TITLE                                                     \
    << std::right << std::setw(widthNum) << " " << std::setw(width) << E1                                   \
    << std::right << std::setw(widthNum) << " " << std::setw(width) << E2                                   \
    << std::right << std::setw(widthNum) << " " << std::setw(width) << E3 << std::endl
                
#define FORMATTED_OUTPUT_LONG(TITLE, E1, N1, E2, N2, E3, N3)                                                \
    std::cout << std::setw(width) << std::left << TITLE                                                     \
    << std::right << std::setw(width) << E1 << "/" << std::left << std::setw(widthNum) << N1                \
    << std::right << std::setw(width) << E2 << "/" << std::left << std::setw(widthNum) << N2                \
    << std::right << std::setw(width) << E3 << "/" << std::left << std::setw(widthNum) << N3 << std::endl

#define FORMATTED_OUTPUT(TITLE, E1, E2, E3)                                                                 \
    std::cout << std::left << std::setw(width) << TITLE                                                     \
    << std::right << std::setw(width) << E1                                                                 \
    << std::right << std::setw(width) << E2                                                                 \
    << std::right << std::setw(width) << E3 << std::endl

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief EnergyMonitoringAlgorithm class
 */
class EnergyMonitoringAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  EnergyMixing class
     */
    class EnergyMixing
    {
    public:
        /**
         *  @brief  Default constructor
         */
        EnergyMixing();

        /**
         *  @brief  AddChargedClusterCalorimetricEnergy
         * 
         *  @param  energy
         */
        void AddChargedClusterCalorimetricEnergy(float energy);

        /**
         *  @brief  AddNeutralClusterCalorimetricEnergy
         * 
         *  @param  energy
         */
        void AddNeutralClusterCalorimetricEnergy(float energy);

        /**
         *  @brief  AddChargedClusterTracksEnergy
         * 
         *  @param  energy
         */
         void AddChargedClusterTracksEnergy(float energy);

        /**
         *  @brief  AddPhotonClusterCalorimetricEnergy
         * 
         *  @param  energy
         */
        void AddPhotonClusterCalorimetricEnergy (float energy);

        /**
         *  @brief  AddChargedCluster
         */
        void AddChargedCluster();

        /**
         *  @brief  AddNeutralCluster
         */
        void AddNeutralCluster();

        /**
         *  @brief  AddTrack
         */
        void AddTrack();

        /**
         *  @brief  AddPhotonCluster
         */
        void AddPhotonCluster();

        /**
         *  @brief  GetChargedClusterCalorimetricEnergy
         * 
         *  @return 
         */
        float GetChargedClusterCalorimetricEnergy() const;

        /**
         *  @brief  GetNeutralClusterCalorimetricEnergy
         * 
         *  @return 
         */
        float GetNeutralClusterCalorimetricEnergy() const;

        /**
         *  @brief  GetChargedClusterTracksEnergy
         * 
         *  @return 
         */
        float GetChargedClusterTracksEnergy() const;

        /**
         *  @brief  GetPhotonClusterCalorimetricEnergy
         * 
         *  @return 
         */
        float GetPhotonClusterCalorimetricEnergy() const;

        /**
         *  @brief  GetChargedClusterCalorimetricClusters
         * 
         *  @return 
         */
        int GetChargedClusterCalorimetricClusters() const;

        /**
         *  @brief  GetNeutralClusterCalorimetricClusters
         * 
         *  @return 
         */
        int GetNeutralClusterCalorimetricClusters() const;

        /**
         *  @brief  GetChargedClusterTracks
         * 
         *  @return 
         */
        int GetChargedClusterTracks() const;

        /**
         *  @brief  GetPhotonClusterCalorimetricClusters
         * 
         *  @return 
         */
        int GetPhotonClusterCalorimetricClusters() const;

        /**
         *  @brief  AddMCParticle
         * 
         *  @param  pMCParticle
         */
        void AddMCParticle(const pandora::MCParticle* pMCParticle);

        typedef std::set<const pandora::MCParticle*> MCParticleSet;

        /**
         *  @brief  GetPhotonClusterCalorimetricClusters
         * 
         *  @return the MCParticleSet
         */
        const MCParticleSet &GetMCParticleSet() const;

        /**
         *  @brief  GetMCParticleSetEnergy
         * 
         *  @return MCParticleSetEnergy
         */
        float GetMCParticleSetEnergy() const;

        /**
         *  @brief  GetMCParticleNumber
         * 
         *  @return MCParticleNumber
         */
        int GetMCParticleNumber() const;

    private:
        MCParticleSet       m_mcParticleSet;                ///< The mc particle set

        float               m_chargedCalorimetric;          ///< Charged cluster calorimetric energy
        float               m_chargedTracks;                ///< Charged cluster track energy
        float               m_neutralCalorimetric;          ///< Neutral cluster calorimetric energy
        float               m_photonCalorimetric;           ///< Photon cluster calorimetric energy

        int                 m_chargedCalorimetricClusters;  ///< Number of charged calorimetric clusters
        int                 m_chargedTracksNumber;          ///< Number of charged cluster tracks
        int                 m_neutralCalorimetricClusters;  ///< Number of neutral clusters
        int                 m_photonCalorimetricClusters;   ///< Number of photon clusters
    };

    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief  Destructor
     */
    virtual ~EnergyMonitoringAlgorithm();

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    /**
     *  @brief  MonitoringOutput
     * 
     *  @param  trueChargedHadrons
     *  @param  trueNeutral
     *  @param  truePhotons
     *  @param  numberClusters
     *  @param  numberTracks
     */
    pandora::StatusCode MonitoringOutput(const EnergyMixing &trueCharged, const EnergyMixing &trueNeutral, const EnergyMixing &truePhotons,
        const int numberClusters, const int numberTracks);

    pandora::StringVector   m_clusterListNames;             ///< list of strings denoting clusternames 
    bool                    m_clusters;                     ///< show current clusters
    std::string             m_monitoringFileName;           ///< filename for storing the monitoring information (ROOT)
    std::string             m_treeName;                     ///< tree name for the monitoring output
    bool                    m_print;                        ///< print the monitoring info
    bool                    m_quantity;                     ///< show the number of clusters/tracks
    pandora::FloatVector    m_clusterEnergies;              ///< Energies of the clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddChargedClusterCalorimetricEnergy(float energy)
{
    m_chargedCalorimetric += energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddNeutralClusterCalorimetricEnergy(float energy)
{
    m_neutralCalorimetric += energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddChargedClusterTracksEnergy(float energy)
{
    m_chargedTracks += energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddPhotonClusterCalorimetricEnergy(float energy)
{
    m_photonCalorimetric += energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddChargedCluster()
{
    m_chargedCalorimetricClusters++;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddNeutralCluster()
{
    m_neutralCalorimetricClusters++;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddTrack()
{
    m_chargedTracksNumber++;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddPhotonCluster()
{
    m_photonCalorimetricClusters++;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float EnergyMonitoringAlgorithm::EnergyMixing::GetChargedClusterCalorimetricEnergy() const
{
    return m_chargedCalorimetric;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float EnergyMonitoringAlgorithm::EnergyMixing::GetNeutralClusterCalorimetricEnergy() const
{
    return m_neutralCalorimetric;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float EnergyMonitoringAlgorithm::EnergyMixing::GetChargedClusterTracksEnergy() const
{
    return m_chargedTracks;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float EnergyMonitoringAlgorithm::EnergyMixing::GetPhotonClusterCalorimetricEnergy() const
{
    return m_photonCalorimetric;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int EnergyMonitoringAlgorithm::EnergyMixing::GetChargedClusterCalorimetricClusters() const
{
    return m_chargedCalorimetricClusters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int EnergyMonitoringAlgorithm::EnergyMixing::GetNeutralClusterCalorimetricClusters() const
{
    return m_neutralCalorimetricClusters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int EnergyMonitoringAlgorithm::EnergyMixing::GetChargedClusterTracks() const
{
    return m_chargedTracksNumber;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int EnergyMonitoringAlgorithm::EnergyMixing::GetPhotonClusterCalorimetricClusters() const
{
    return m_photonCalorimetricClusters;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void EnergyMonitoringAlgorithm::EnergyMixing::AddMCParticle(const pandora::MCParticle *const pMCParticle)
{
    m_mcParticleSet.insert(pMCParticle);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const EnergyMonitoringAlgorithm::EnergyMixing::MCParticleSet &EnergyMonitoringAlgorithm::EnergyMixing::GetMCParticleSet() const
{
    return m_mcParticleSet;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int EnergyMonitoringAlgorithm::EnergyMixing::GetMCParticleNumber() const
{
    return m_mcParticleSet.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EnergyMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new EnergyMonitoringAlgorithm();
}

#endif // #ifndef ENERGY_MONITORING_ALGORITHM_H
