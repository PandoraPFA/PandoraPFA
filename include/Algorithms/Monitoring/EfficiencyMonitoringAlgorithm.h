/**
 *  @file   PandoraPFANew/include/Algorithms/Monitoring/EfficiencyMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the energy monitoring algorithm
 * 
 *  $Log: $
 */
#ifndef EFFICIENCY_MONITORING_ALGORITHM_H
#define EFFICIENCY_MONITORING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

//------------------------------------------------------------------------------------------------------------------------------------------

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
        Algorithm *CreateAlgorithm() const;
    };

    /**
     *  @brief  Destructor
     */
    virtual ~EfficiencyMonitoringAlgorithm();

    /**
     *  @brief  Perform any operations that must occur after reading settings, but before running the algorithm
     */
    virtual StatusCode Initialize();


private:

    class CaloHitMCType {
    public:
        CaloHitMCType() : m_truePositiveEnergy(0.f), m_falsePositiveEnergy(0.f), m_trueNegativeEnergy(0.f), m_falseNegativeEnergy(0.f) {}

        float m_truePositiveEnergy;
        float m_falsePositiveEnergy;
        float m_trueNegativeEnergy;
        float m_falseNegativeEnergy;
    };

    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    

    int                     m_particleId;                         ///< particle Id of which the data is collected
    std::string             m_monitoringFileName;                 ///< filename for storing the monitoring information (ROOT)
    std::string             m_foundTreeName;                      ///< tree name for the monitoring output for the found MCParticles
    std::string             m_notFoundTreeName;                   ///< tree name for the monitoring output for the not found MCParticles
    std::string             m_fakesTreeName;                      ///< tree name for the monitoring output for the falsely found Pfos
    std::string             m_otherTreeName;                      ///< tree name for the monitoring output for the other Pfos

    std::string             m_pfoCaloHitETreeName;                ///< tree name for the monitoring output for the rightly and wrongly assigned calorimeter hit energy from PFOs
    std::string             m_mcCaloHitETreeName;                 ///< tree name for the monitoring output for the rightly and wrongly assigned calorimeter hit energy from MCs
    std::string             m_controlTreeName;                    ///< tree name for the control output
    std::string             m_eventTreeName;                      ///< tree name for the event information


    float                   m_minCaloHitEnergyFraction;           ///< cut on the fraction (percentage) of energy deposited in the calorimeter by the chosen particle type
    float                   m_calorimeterResolutionStochasticCut; ///< cut on the energy of the Pfo based on the stochastic term of the calorimetric energy resolution
    float                   m_calorimeterResolutionConstantCut;   ///< cut on the energy of the Pfo based on the constant term of the calorimetric energy resolution
    int                     m_thetaBins;                          ///< number of bins in theta

    float                   m_thetaMin;                           ///< minimum theta angle for the plots as a function of theta
    float                   m_thetaMax;                           ///< maximum theta angle for the plots as a function of theta
    int                     m_energyBins;                         ///< number of bins in energy
    float                   m_energyMin;                          ///< minimum energy for the plots as a function of energy
    float                   m_energyMax;                          ///< maximum energy for the plots as a function of energy

    int                     m_numberFindableMCs;                  ///< number of findable MC particles (to compute the global MCParticle efficiency and purity)
    int                     m_numberFoundMCs;                     ///< number of found MC particles (to compute the global MCParticle efficiency and purity)
    int                     m_numberNotFoundMCs;                  ///< number of not found MC particles (to compute the global MCParticle efficiency and purity)
    int                     m_numberFakes;                        ///< number of found fakes (to compute the global MCParticle efficiency and purity)
    int                     m_numberEvents;                       ///< number of events
    float                   m_eventEfficiency;                    ///< average MCParticle efficiency per event
    float                   m_eventPurity;                        ///< average MCParticle purity per event
};


inline pandora::Algorithm *EfficiencyMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new EfficiencyMonitoringAlgorithm();
}

#endif // #ifndef EFFICIENCY_MONITORING_ALGORITHM_H
