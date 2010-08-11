/**
 *  @file   PandoraPFANew/include/Algorithms/Monitoring/DumpPfosMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the energy monitoring algorithm
 * 
 *  $Log: $
 */
#ifndef DUMP_PFOS_MONITORING_ALGORITHM_H
#define DUMP_PFOS_MONITORING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"
#include "Objects/Track.h"
#include "Objects/Cluster.h"
#include "Objects/MCParticle.h"
#include <set>

enum TrackErrorTypes_t {OK, UNKNOWN, SPLIT_TRACK, MISSED_CONVERSION, MISSED_KSHORT};

/**
 *  @brief DumpPfosMonitoringAlgorithm class
 */
class DumpPfosMonitoringAlgorithm : public pandora::Algorithm
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

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);
    StatusCode DumpChargedPfo(const pandora::ParticleFlowObject *pPfo);
    StatusCode DumpNeutralPfo(const pandora::ParticleFlowObject *pPfo);
    StatusCode DumpPhotonPfo(const pandora::ParticleFlowObject *pPfo);
    StatusCode ClusterEnergyFractions(const pandora::Cluster* pCluster, float &fCharged, float &fPhoton, float &fneutral, const pandora::MCParticle* &bestMatchedMcPfo);

    std::set<const pandora::MCParticle*>m_trackMcPfoTargets;
    std::map<const pandora::MCParticle*, const pandora::Track*>m_mcParticleToTrackMap;
    std::map<const pandora::Track*,TrackErrorTypes_t>m_trackToErrorTypeMap;

    float m_trackRecoAsTrackEnergy;
    float m_trackRecoAsPhotonEnergy;
    float m_trackRecoAsNeutralEnergy;
    float m_photonRecoAsTrackEnergy;
    float m_photonRecoAsPhotonEnergy;
    float m_photonRecoAsNeutralEnergy;
    float m_neutralRecoAsTrackEnergy;
    float m_neutralRecoAsPhotonEnergy;
    float m_neutralRecoAsNeutralEnergy;

    float m_minPfoEnergyToDisplay;
    float m_minAbsChiToDisplay;
    float m_minConfusionEnergyToDisplay;
    float m_minFragmentEnergyToDisplay;
    float m_totalPfoEnergyDisplayLessThan;
    float m_totalPfoEnergyDisplayGreaterThan;
    float m_fragmentEnergyToDisplay;
    float m_photonIdEnergyToDisplay;
    bool m_firstChargedPfoToPrint;
    bool m_firstNeutralPfoToPrint;
    bool m_firstPhotonPfoToPrint;

};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *DumpPfosMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new DumpPfosMonitoringAlgorithm();
}

#endif // #ifndef DUMP_PFOS_MONITORING_ALGORITHM_H
