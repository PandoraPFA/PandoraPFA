/**
 *  @file   PandoraPFANew/include/Algorithms/Monitoring/VisualMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the visual monitoring algorithm class
 * 
 *  $Log: $
 */
#ifndef VISUAL_MONITORING_ALGORITHM_H
#define VISUAL_MONITORING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief VisualMonitoringAlgorithm class
 */
class VisualMonitoringAlgorithm : public pandora::Algorithm
{
private:
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    pandora::StringVector   m_clusterListNames;         ///< List of strings denoting clusternames 
    pandora::StringVector   m_suppressMCParticles;      ///< List of PDG numbers and energies for MC particles to be suppressed (e.g. " 22:0.1 2112:1.0 ")
    bool                    m_mcParticles;              ///< Whether to show MC particles
    bool                    m_particleFlowObjects;      ///< Whether to show current particle flow object list
    bool                    m_clusters;                 ///< Whether to show current cluster list
    bool                    m_hits;                     ///< Whether to show current ordered calohitlist
    bool                    m_tracks;                   ///< Whether to show current tracklist
    bool                    m_onlyAvailable;            ///< Whether to show only available  (i.e. non-clustered) calohits and tracks
    bool                    m_displayEvent;             ///< Whether to display the event

    typedef std::map<int, float> PdgCodeToEnergyMap;

    PdgCodeToEnergyMap      m_particleSuppressionMap;   ///< Map from pdg-codes to energy for suppression of particles types below specific energies
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *VisualMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new VisualMonitoringAlgorithm();
}

#endif // #ifndef VISUAL_MONITORING_ALGORITHM_H
