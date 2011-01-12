/**
 *  @file   PandoraPFANew/Algorithms/include/Monitoring/DumpPfosMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the dump pfos monitoring algorithm class
 * 
 *  $Log: $
 */
#ifndef DUMP_PFOS_MONITORING_ALGORITHM_H
#define DUMP_PFOS_MONITORING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  the track error types enum
 */
enum TrackErrorTypes
{
    OK,
    UNKNOWN,
    SPLIT_TRACK,
    MISSED_CONVERSION,
    MISSED_KSHORT
};

//------------------------------------------------------------------------------------------------------------------------------------------

#define FORMATTED_OUTPUT_PFO_HEADER(N1, E2)                                                     \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    N1        <<                                   \
    std::right << std::setw(widthFloat)    <<    E2

#define FORMATTED_OUTPUT_PFO_HEADER_TITLE()                                                     \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    "Pfo"     <<                                   \
    std::right << std::setw(widthFloat)    <<    "EPfo"

#define FORMATTED_PFO_PADDING(TITLE1, TITLE2)                                                   \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    TITLE1    <<                                   \
    std::right << std::setw(widthFloat)    <<    TITLE2

#define FORMATTED_OUTPUT_CONFUSION(E1, E2, E3, E4, E5, E6, E7, E8, E9)                          \
    std::cout  <<                                                                               \
      std::right << std::setw(16) << " Generated as   "    <<                                   \
      std::right << std::setw(widthFloat)  <<    "track"   <<                                   \
      std::right << std::setw(widthFloat)  <<    "gamma"   <<                                   \
      std::right << std::setw(widthFloat)  <<    "hadron"  <<  std::endl <<                     \
      std::right << std::setw(16) << "Reco as track : "    <<                                   \
      std::right << std::setw(widthFloat)  <<    E1        <<                                   \
      std::right << std::setw(widthFloat)  <<    E2        <<                                   \
      std::right << std::setw(widthFloat)  <<    E3        <<  std::endl <<                     \
      std::right << std::setw(16) <<  "Reco as gamma : "   <<                                   \
      std::right << std::setw(widthFloat)  <<    E4        <<                                   \
      std::right << std::setw(widthFloat)  <<    E5        <<                                   \
      std::right << std::setw(widthFloat)  <<    E6        << std::endl <<                      \
      std::right << std::setw(16) <<  "Reco as hadron: "   <<                                   \
      std::right << std::setw(widthFloat)  <<    E7        <<                                   \
      std::right << std::setw(widthFloat)  <<    E8        <<                                   \
      std::right << std::setw(widthFloat)  <<    E9        << std::endl

#define FORMATTED_OUTPUT_TRACK_TITLE()                                                          \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    "Track"   <<                                   \
    std::right << std::setw(widthFlag)     <<    "("       <<                                   \
    std::right << std::setw(widthInt)      <<    "mc"      <<                                   \
    std::left  << std::setw(widthFlag)     <<    ")"       <<                                   \
    std::right << std::setw(widthFlag)     <<    "C"       <<                                   \
    std::right << std::setw(widthFlag)     <<    "R"       <<                                   \
    std::right << std::setw(widthFloat)    <<    "Mom"     <<                                   \
    std::right << std::setw(widthFlag)     <<    "("       <<                                   \
    std::right << std::setw(widthFloat)    <<    "mc"      <<                                   \
    std::left  << std::setw(widthFlag)     <<    ")"       <<                                   \
    std::right << std::setw(widthFloat)    <<    "Eclust"  <<                                   \
    std::right << std::setw(widthFloat)    <<    "chi"     <<                                   \
    std::right << std::setw(widthFlag)     <<    "L"       <<                                   \
    std::right << std::setw(widthSmallFloat)    <<    "fC" <<                                   \
    std::right << std::setw(widthSmallFloat)    <<    "fP" <<                                   \
    std::right << std::setw(widthSmallFloat)    <<    "fN" <<                                   \
    std::endl

#define FORMATTED_OUTPUT_TRACK(N1, N2, FLAG1, FLAG2, E1, E2, E3, E4, FLAG3, E5, E6, E7)         \
    std::cout  <<                                                                               \
    std::right << std::setw(widthInt)      <<    N1        <<                                   \
    std::right << std::setw(widthFlag)     <<    "("       <<                                   \
    std::right << std::setw(widthInt)      <<    N2        <<                                   \
    std::left  << std::setw(widthFlag)     <<    ")"       <<                                   \
    std::right << std::setw(widthFlag)     <<    FLAG1     <<                                   \
    std::right << std::setw(widthFlag)     <<    FLAG2     <<                                   \
    std::right << std::setw(widthFloat)    <<    E1        <<                                   \
    std::right << std::setw(widthFlag)     <<    "("       <<                                   \
    std::right << std::setw(widthFloat)    <<    E2        <<                                   \
    std::left  << std::setw(widthFlag)     <<    ")"       <<                                   \
    std::right << std::setw(widthFloat)    <<    E3        <<                                   \
    std::right << std::setw(widthFloat)    <<    E4        <<                                   \
    std::right << std::setw(widthFlag)     <<    FLAG3     <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E5        <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E6        <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E7

#define FORMATTED_OUTPUT_NEUTRAL(E1, E2, E3, E4, N1, N2, E5, E6)                                \
    std::cout  <<                                                                               \
    std::right << std::setw(widthFloat)      <<  E1        <<                                   \
    std::right << std::setw(widthInt4)       <<  "     "   <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E2        <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E3        <<                                   \
    std::right << std::setw(widthSmallFloat) <<  E4        <<                                   \
    std::right << std::setw(widthInt4)     <<    N1        <<                                   \
    std::left  << std::setw(widthFlag)     <<    "-"       <<                                   \
    std::left  << std::setw(widthInt4)     <<    N2        <<                                   \
    std::right << std::setw(widthFloat)    <<    E5        <<                                   \
    std::right << std::setw(widthFloat)    <<    E6

#define FORMATTED_OUTPUT_NEUTRAL_TITLE()                                                        \
    std::cout  <<                                                                               \
    std::right << std::setw(widthFloat)    <<    "Eclust"  <<                                   \
    std::right << std::setw(widthInt4)     <<    "     "   <<                                   \
    std::right << std::setw(widthSmallFloat) <<  "fC"      <<                                   \
    std::right << std::setw(widthSmallFloat) <<  "fP"      <<                                   \
    std::right << std::setw(widthSmallFloat) <<  "fN"      <<                                   \
    std::left  << std::setw(widthInt4+widthFlag+widthInt4) <<  " Layers " <<                    \
    std::right << std::setw(widthFloat)      <<   "sStart" <<                                   \
    std::right << std::setw(widthFloat)      <<   "sDisc"  <<                                   \
    std::endl

//------------------------------------------------------------------------------------------------------------------------------------------

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
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  DumpChargedPfo
     * 
     *  @param  pPfo
     */
    pandora::StatusCode DumpChargedPfo(const pandora::ParticleFlowObject *pPfo);

    /**
     *  @brief  DumpNeutralPfo
     * 
     *  @param  pPfo
     */
    pandora::StatusCode DumpNeutralPfo(const pandora::ParticleFlowObject *pPfo);

    /**
     *  @brief  DumpNeutralOrPhotonPfo
     * 
     *  @param  pPfo
     *  @param  isPhotonPfo
     */
    pandora::StatusCode DumpNeutralOrPhotonPfo(const pandora::ParticleFlowObject *pPfo, bool isPhotonPfo);

    /**
     *  @brief  DumpPhotonPfo
     * 
     *  @param  pPfo
     */
    pandora::StatusCode DumpPhotonPfo(const pandora::ParticleFlowObject *pPfo);

    /**
     *  @brief  ClusterEnergyFractions
     * 
     *  @param  pCluster
     *  @param  fCharged
     *  @param  fPhoton
     *  @param  fneutral
     *  @param  pBestMatchedMcPfo
     */
    void ClusterEnergyFractions(const pandora::Cluster *pCluster, float &fCharged, float &fPhoton, float &fneutral,
        const pandora::MCParticle *&pBestMatchedMcPfo) const;

    typedef std::set<const pandora::MCParticle*> MCParticleList;
    typedef std::vector<pandora::ParticleFlowObject*> ParticleFlowObjectVector;

    typedef std::map<const pandora::MCParticle*, float> MCParticleToFloatMap;
    typedef std::map<const pandora::MCParticle*, const pandora::Track*> MCParticleToTrackMap;
    typedef std::map<const pandora::Track*, TrackErrorTypes> TrackToErrorTypeMap;

    MCParticleList          m_trackMcPfoTargets;                        ///< 
    MCParticleToTrackMap    m_mcParticleToTrackMap;                     ///< 
    TrackToErrorTypeMap     m_trackToErrorTypeMap;                      ///< 

    float                   m_trackRecoAsTrackEnergy;                   ///< 
    float                   m_trackRecoAsPhotonEnergy;                  ///< 
    float                   m_trackRecoAsNeutralEnergy;                 ///< 
    float                   m_photonRecoAsTrackEnergy;                  ///< 
    float                   m_photonRecoAsPhotonEnergy;                 ///< 
    float                   m_photonRecoAsNeutralEnergy;                ///< 
    float                   m_neutralRecoAsTrackEnergy;                 ///< 
    float                   m_neutralRecoAsPhotonEnergy;                ///< 
    float                   m_neutralRecoAsNeutralEnergy;               ///< 
                    
    float                   m_minPfoEnergyToDisplay;                    ///< 
    float                   m_minAbsChiToDisplay;                       ///< 
    float                   m_minConfusionEnergyToDisplay;              ///< 
    float                   m_minFragmentEnergyToDisplay;               ///< 
    float                   m_totalPfoEnergyDisplayLessThan;            ///< 
    float                   m_totalPfoEnergyDisplayGreaterThan;         ///< 
    float                   m_fragmentEnergyToDisplay;                  ///< 
    float                   m_photonIdEnergyToDisplay;                  ///< 
                    
    bool                    m_firstChargedPfoToPrint;                   ///< 
    bool                    m_firstNeutralPfoToPrint;                   ///< 
    bool                    m_firstPhotonPfoToPrint;                    ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode DumpPfosMonitoringAlgorithm::DumpPhotonPfo(const pandora::ParticleFlowObject *pPfo)
{
    return this->DumpNeutralOrPhotonPfo(pPfo, true);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode DumpPfosMonitoringAlgorithm::DumpNeutralPfo(const pandora::ParticleFlowObject *pPfo)
{
    return this->DumpNeutralOrPhotonPfo(pPfo, false);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *DumpPfosMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new DumpPfosMonitoringAlgorithm();
}

#endif // #ifndef DUMP_PFOS_MONITORING_ALGORITHM_H
