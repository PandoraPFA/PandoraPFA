/**
 *  @file   PandoraPFANew/include/Algorithms/Monitoring/MCParticlesMonitoringAlgorithm.h
 * 
 *  @brief  monitoring of the MC Particles
 * 
 *  $Log: $
 */
#ifndef MC_PARTICLES_MONITORING_ALGORITHM_H
#define MC_PARTICLES_MONITORING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"


/**
 *  @brief MCParticlesMonitoringAlgorithm class
 */
class MCParticlesMonitoringAlgorithm : public pandora::Algorithm
{
private:
public:

    class pandora::MCParticle;

    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

    virtual StatusCode Initialize();
    virtual ~MCParticlesMonitoringAlgorithm();


private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    void       MonitorMCParticleList( const pandora::MCParticleList& mcParticleList );
    StatusCode FillListOfUsedMCParticles();
    bool       TakeMCParticle(const pandora::MCParticle* pMCParticle);

    /**
     *  @brief  Print the MCParticle's parameters
     * 
     *  @param mcParticle mcparticle of which the information is printed
     *  @param o output-stream where the information is written to
     */
    void PrintMCParticle(const pandora::MCParticle* mcParticle, std::ostream & o );

    pandora::StringVector m_clusterListNames;            ///< list of strings denoting clusternames 
    std::string  m_monitoringFileName;                   ///< filename for storing the monitoring information (ROOT)
    std::string  m_treeName;                             ///< tree name for the monitoring output
    bool         m_print;                                ///< print the monitoring info
    bool         m_oldRoot;                              ///< monitoring format for writing ROOT ttree if ROOT is older than version 5.20
    bool         m_sort;                                 ///< sort mc particles according to their energy
    bool         m_indent;                               ///< indent mc particles at printout according to their outer radius

    bool         m_onlyFinal;                            ///< monitor only the final PFOs (those without daughters)
    bool         m_haveCaloHits;                         ///< monitor PFOs which have calohits
    bool         m_haveTracks;                           ///< monitor PFOs which have tracks

    typedef std::vector<float> FloatVector;
    typedef std::vector<int>   IntVector;
    FloatVector* m_energy;                               ///< energy of mc particle
    FloatVector* m_momentumX;                            ///< x component of momentum of mc particle
    FloatVector* m_momentumY;                            ///< y component of momentum of mc particle
    FloatVector* m_momentumZ;                            ///< z component of momentum of mc particle
    IntVector*   m_particleId;                           ///< particle id of mc particle
    FloatVector* m_outerRadius;                          ///< outer radius of mcparticle
    FloatVector* m_innerRadius;                          ///< inner radius of mcparticle

    typedef std::set<const pandora::MCParticle*> ConstMCParticleList;
    ConstMCParticleList     m_mcParticleList;            ///< list of mc particles from calohits and tracks

    int m_eventCounter;                                  ///< event counter; only necessary for older ROOT versions where the tree is written "flat"
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MCParticlesMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new MCParticlesMonitoringAlgorithm();
}

#endif // #ifndef MC_PARTICLES_MONITORING_ALGORITHM_H
