/**
 *  @file   PandoraPFANew/Algorithms/include/Monitoring/EfficiencyMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the energy monitoring algorithm
 * 
 *  $Log: $
 */
#ifndef EFFICIENCY_MONITORING_ALGORITHM_H
#define EFFICIENCY_MONITORING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief EfficiencyMonitoringAlgorithm class
 */
class EfficiencyMonitoringAlgorithm : public pandora::Algorithm
{
public:
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
    virtual ~EfficiencyMonitoringAlgorithm();

    /**
     *  @brief  Perform any operations that must occur after reading settings, but before running the algorithm
     */
    virtual pandora::StatusCode Initialize();

private:
    /**
     *  @brief  CaloHitMCType class
     */
    class CaloHitMCType
    {
    public:
        /**
         *  @brief  Default constructor
         */
        CaloHitMCType();

        float       m_truePositiveEnergy;                 ///< The true positive energy
        float       m_falsePositiveEnergy;                ///< The false positive energy
        float       m_trueNegativeEnergy;                 ///< The true negative energy
        float       m_falseNegativeEnergy;                ///< The false negative energy
    };

    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    int             m_particleId;                         ///< particle Id of which the data is collected
    bool            m_print;                              ///< if true, print out the efficiency and purity per event
    std::string     m_monitoringFileName;                 ///< filename for storing the monitoring information (ROOT)
    std::string     m_foundTreeName;                      ///< tree name for the monitoring output for the found MCParticles
    std::string     m_notFoundTreeName;                   ///< tree name for the monitoring output for the not found MCParticles
    std::string     m_fakesTreeName;                      ///< tree name for the monitoring output for the falsely found Pfos
    std::string     m_otherTreeName;                      ///< tree name for the monitoring output for the other Pfos

    std::string     m_pfoCaloHitETreeName;                ///< tree name for the monitoring output for the rightly and wrongly assigned calorimeter hit energy from PFOs
    std::string     m_mcCaloHitETreeName;                 ///< tree name for the monitoring output for the rightly and wrongly assigned calorimeter hit energy from MCs
    std::string     m_controlTreeName;                    ///< tree name for the control output
    std::string     m_eventTreeName;                      ///< tree name for the event information

    float           m_minCaloHitEnergyFraction;           ///< cut on the fraction (percentage) of energy deposited in the calorimeter by the chosen particle type
    float           m_calorimeterResolutionStochasticCut; ///< cut on the energy of the Pfo based on the stochastic term of the calorimetric energy resolution
    float           m_calorimeterResolutionConstantCut;   ///< cut on the energy of the Pfo based on the constant term of the calorimetric energy resolution
    int             m_thetaBins;                          ///< number of bins in theta

    float           m_thetaMin;                           ///< minimum theta angle for the plots as a function of theta
    float           m_thetaMax;                           ///< maximum theta angle for the plots as a function of theta
    int             m_energyBins;                         ///< number of bins in energy
    float           m_energyMin;                          ///< minimum energy for the plots as a function of energy
    float           m_energyMax;                          ///< maximum energy for the plots as a function of energy

    int             m_numberFindableMCs;                  ///< number of findable MC particles (to compute the global MCParticle efficiency and purity)
    int             m_numberFoundMCs;                     ///< number of found MC particles (to compute the global MCParticle efficiency and purity)
    int             m_numberNotFoundMCs;                  ///< number of not found MC particles (to compute the global MCParticle efficiency and purity)
    int             m_numberFakes;                        ///< number of found fakes (to compute the global MCParticle efficiency and purity)
    int             m_numberEvents;                       ///< number of events
    float           m_eventEfficiency;                    ///< average MCParticle efficiency per event
    float           m_eventPurity;                        ///< average MCParticle purity per event

    std::string     m_histEntriesForEffPerThetaBin;       ///< histogram name
    std::string     m_histEntriesForEffPerEnergyBin;      ///< histogram name
    std::string     m_histEntriesForPurityPerThetaBin;    ///< histogram name
    std::string     m_histEntriesForPurityPerEnergyBin;   ///< histogram name

    std::string     m_histMCParticleEffVsTheta;           ///< histogram name
    std::string     m_histMCParticleEffVsEnergy;          ///< histogram name
    std::string     m_histMCParticlePurityVsTheta;        ///< histogram name
    std::string     m_histMCParticlePurityVsEnergy;       ///< histogram name

    std::string     m_histCaloHitMCEffVsTheta;              ///< histogram name
    std::string     m_histCaloHitMCEffVsEnergy;             ///< histogram name
    std::string     m_histCaloHitMCPurityVsTheta;           ///< histogram name
    std::string     m_histCaloHitMCPurityVsEnergy;          ///< histogram name

    std::string     m_histPFO_MC_EDiffMulByEMC;           ///< histogram name
    std::string     m_histMCFractionLargestContribution;  ///< histogram name


};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline EfficiencyMonitoringAlgorithm::CaloHitMCType::CaloHitMCType() :
    m_truePositiveEnergy(0.f),
    m_falsePositiveEnergy(0.f),
    m_trueNegativeEnergy(0.f),
    m_falseNegativeEnergy(0.f)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *EfficiencyMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new EfficiencyMonitoringAlgorithm();
}

#endif // #ifndef EFFICIENCY_MONITORING_ALGORITHM_H
