/**
 *  @file   PandoraPFANew/FineGranularityContent/include/FineGranularityContent.h
 * 
 *  @brief  Header file detailing content for use with a fine granularity particle flow detector
 * 
 *  $Log: $
 */
#ifndef FINE_GRANULARITY_CONTENT_H
#define FINE_GRANULARITY_CONTENT_H 1

#include "Cheating/CheatingClusterMergingAlgorithm.h"
#include "Cheating/CheatingPfoCreationAlgorithm.h"
#include "Cheating/CheatingTrackToClusterMatching.h"
#include "Cheating/PerfectClusteringAlgorithm.h"
#include "Cheating/PerfectFragmentRemovalAlgorithm.h"

#include "Clustering/ClusteringParentAlgorithm.h"
#include "Clustering/ConeClusteringAlgorithm.h"
#include "Clustering/ForcedClusteringAlgorithm.h"
#include "Clustering/InwardConeClusteringAlgorithm.h"
#include "Clustering/LineClusteringAlgorithm.h"

#include "FragmentRemoval/FragmentRemovalParentAlgorithm.h"
#include "FragmentRemoval/MainFragmentRemovalAlgorithm.h"
#include "FragmentRemoval/MergeSplitPhotonsAlgorithm.h"
#include "FragmentRemoval/NeutralFragmentRemovalAlgorithm.h"
#include "FragmentRemoval/PhotonFragmentRemovalAlgorithm.h"
#include "FragmentRemoval/BeamHaloMuonRemovalAlgorithm.h"

#include "Monitoring/CaloHitMonitoringAlgorithm.h"
#include "Monitoring/DumpPfosMonitoringAlgorithm.h"
#include "Monitoring/EfficiencyMonitoringAlgorithm.h"
#include "Monitoring/EnergyMonitoringAlgorithm.h"
#include "Monitoring/MCParticlesMonitoringAlgorithm.h"
#include "Monitoring/VisualMonitoringAlgorithm.h"

#include "ParticleId/FinalParticleIdAlgorithm.h"
#include "ParticleId/MuonReconstructionAlgorithm.h"
#include "ParticleId/PhotonReconstructionAlgorithm.h"
#include "ParticleId/PhotonRecoveryAlgorithm.h"

#include "Persistency/EventReadingAlgorithm.h"
#include "Persistency/EventWritingAlgorithm.h"

#include "PfoConstruction/CLICPfoSelectionAlgorithm.h"
#include "PfoConstruction/KinkPfoCreationAlgorithm.h"
#include "PfoConstruction/PfoCreationAlgorithm.h"
#include "PfoConstruction/PfoCreationParentAlgorithm.h"
#include "PfoConstruction/V0PfoCreationAlgorithm.h"

#include "Reclustering/ExitingTrackAlg.h"
#include "Reclustering/ForceSplitTrackAssociationsAlg.h"
#include "Reclustering/ReclusteringParentAlgorithm.h"
#include "Reclustering/ResolveTrackAssociationsAlg.h"
#include "Reclustering/SplitMergedClustersAlg.h"
#include "Reclustering/SplitTrackAssociationsAlg.h"
#include "Reclustering/TrackDrivenAssociationAlg.h"
#include "Reclustering/TrackDrivenMergingAlg.h"

#include "TopologicalAssociation/BackscatteredTracksAlgorithm.h"
#include "TopologicalAssociation/BackscatteredTracks2Algorithm.h"
#include "TopologicalAssociation/BrokenTracksAlgorithm.h"
#include "TopologicalAssociation/ConeBasedMergingAlgorithm.h"
#include "TopologicalAssociation/IsolatedHitMergingAlgorithm.h"
#include "TopologicalAssociation/LoopingTracksAlgorithm.h"
#include "TopologicalAssociation/MipPhotonSeparationAlgorithm.h"
#include "TopologicalAssociation/MuonPhotonSeparationAlgorithm.h"
#include "TopologicalAssociation/MuonClusterAssociationAlgorithm.h"
#include "TopologicalAssociation/ProximityBasedMergingAlgorithm.h"
#include "TopologicalAssociation/ShowerMipMergingAlgorithm.h"
#include "TopologicalAssociation/ShowerMipMerging2Algorithm.h"
#include "TopologicalAssociation/ShowerMipMerging3Algorithm.h"
#include "TopologicalAssociation/ShowerMipMerging4Algorithm.h"
#include "TopologicalAssociation/SoftClusterMergingAlgorithm.h"
#include "TopologicalAssociation/TopologicalAssociationParentAlgorithm.h"

#include "TrackClusterAssociation/LoopingTrackAssociationAlgorithm.h"
#include "TrackClusterAssociation/TrackClusterAssociationAlgorithm.h"
#include "TrackClusterAssociation/TrackRecoveryAlgorithm.h"
#include "TrackClusterAssociation/TrackRecoveryHelixAlgorithm.h"
#include "TrackClusterAssociation/TrackRecoveryInteractionsAlgorithm.h"

#include "Utility/ClusterPreparationAlgorithm.h"
#include "Utility/EventPreparationAlgorithm.h"
#include "Utility/PfoPreparationAlgorithm.h"
#include "Utility/TrackPreparationAlgorithm.h"

#include "FineGranularityEnergyCorrections.h"
#include "FineGranularityParticleId.h"

#include "FineGranularityPseudoLayerCalculator.h"
#include "FineGranularityShowerProfileCalculator.h"

/**
 *  @brief  FineGranularityContent class
 */
class FineGranularityContent
{
public:
    #define FINE_GRANULARITY_ALGORITHM_LIST(d)                                                                                  \
        d("CheatingClusterMerging",                 CheatingClusterMergingAlgorithm::Factory)                                   \
        d("CheatingPfoCreation",                    CheatingPfoCreationAlgorithm::Factory)                                      \
        d("CheatingTrackToClusterMatching",         CheatingTrackToClusterMatching::Factory)                                    \
        d("PerfectClustering",                      PerfectClusteringAlgorithm::Factory)                                        \
        d("PerfectFragmentRemoval",                 PerfectFragmentRemovalAlgorithm::Factory)                                   \
        d("ClusteringParent",                       ClusteringParentAlgorithm::Factory)                                         \
        d("ConeClustering",                         ConeClusteringAlgorithm::Factory)                                           \
        d("ForcedClustering",                       ForcedClusteringAlgorithm::Factory)                                         \
        d("InwardConeClustering",                   InwardConeClusteringAlgorithm::Factory)                                     \
        d("LineClustering",                         LineClusteringAlgorithm::Factory)                                           \
        d("FragmentRemovalParent",                  FragmentRemovalParentAlgorithm::Factory)                                    \
        d("MainFragmentRemoval",                    MainFragmentRemovalAlgorithm::Factory)                                      \
        d("MergeSplitPhotons",                       MergeSplitPhotonsAlgorithm::Factory)                                        \
        d("NeutralFragmentRemoval",                 NeutralFragmentRemovalAlgorithm::Factory)                                   \
        d("PhotonFragmentRemoval",                  PhotonFragmentRemovalAlgorithm::Factory)                                    \
        d("BeamHaloMuonRemoval",                    BeamHaloMuonRemovalAlgorithm::Factory)                                      \
        d("CaloHitMonitoring",                      CaloHitMonitoringAlgorithm::Factory)                                        \
        d("DumpPfosMonitoring",                     DumpPfosMonitoringAlgorithm::Factory)                                       \
        d("EfficiencyMonitoring",                   EfficiencyMonitoringAlgorithm::Factory)                                     \
        d("EnergyMonitoring",                       EnergyMonitoringAlgorithm::Factory)                                         \
        d("MCParticlesMonitoring",                  MCParticlesMonitoringAlgorithm::Factory)                                    \
        d("VisualMonitoring",                       VisualMonitoringAlgorithm::Factory)                                         \
        d("FinalParticleId",                        FinalParticleIdAlgorithm::Factory)                                          \
        d("MuonReconstruction",                     MuonReconstructionAlgorithm::Factory)                                       \
        d("PhotonReconstruction",                   PhotonReconstructionAlgorithm::Factory)                                     \
        d("PhotonRecovery",                         PhotonRecoveryAlgorithm::Factory)                                           \
        d("EventReading",                           EventReadingAlgorithm::Factory)                                             \
        d("EventWriting",                           EventWritingAlgorithm::Factory)                                             \
        d("CLICPfoSelection",                       CLICPfoSelectionAlgorithm::Factory)                                         \
        d("KinkPfoCreation",                        KinkPfoCreationAlgorithm::Factory)                                          \
        d("PfoCreation",                            PfoCreationAlgorithm::Factory)                                              \
        d("PfoCreationParent",                      PfoCreationParentAlgorithm::Factory)                                        \
        d("V0PfoCreation",                          V0PfoCreationAlgorithm::Factory)                                            \
        d("ExitingTrack",                           ExitingTrackAlg::Factory)                                                   \
        d("ForceSplitTrackAssociations",            ForceSplitTrackAssociationsAlg::Factory)                                    \
        d("ReclusteringParent",                     ReclusteringParentAlgorithm::Factory)                                       \
        d("ResolveTrackAssociations",               ResolveTrackAssociationsAlg::Factory)                                       \
        d("SplitMergedClusters",                    SplitMergedClustersAlg::Factory)                                            \
        d("SplitTrackAssociations",                 SplitTrackAssociationsAlg::Factory)                                         \
        d("TrackDrivenAssociation",                 TrackDrivenAssociationAlg::Factory)                                         \
        d("TrackDrivenMerging",                     TrackDrivenMergingAlg::Factory)                                             \
        d("BackscatteredTracks",                    BackscatteredTracksAlgorithm::Factory)                                      \
        d("BackscatteredTracks2",                   BackscatteredTracks2Algorithm::Factory)                                     \
        d("BrokenTracks",                           BrokenTracksAlgorithm::Factory)                                             \
        d("ConeBasedMerging",                       ConeBasedMergingAlgorithm::Factory)                                         \
        d("IsolatedHitMerging",                     IsolatedHitMergingAlgorithm::Factory)                                       \
        d("LoopingTracks",                          LoopingTracksAlgorithm::Factory)                                            \
        d("MipPhotonSeparation",                    MipPhotonSeparationAlgorithm::Factory)                                      \
        d("MuonPhotonSeparation",                   MuonPhotonSeparationAlgorithm::Factory)                                     \
        d("MuonClusterAssociation",                 MuonClusterAssociationAlgorithm::Factory)                                   \
        d("ProximityBasedMerging",                  ProximityBasedMergingAlgorithm::Factory)                                    \
        d("ShowerMipMerging",                       ShowerMipMergingAlgorithm::Factory)                                         \
        d("ShowerMipMerging2",                      ShowerMipMerging2Algorithm::Factory)                                        \
        d("ShowerMipMerging3",                      ShowerMipMerging3Algorithm::Factory)                                        \
        d("ShowerMipMerging4",                      ShowerMipMerging4Algorithm::Factory)                                        \
        d("SoftClusterMerging",                     SoftClusterMergingAlgorithm::Factory)                                       \
        d("TopologicalAssociationParent",           TopologicalAssociationParentAlgorithm::Factory)                             \
        d("LoopingTrackAssociation",                LoopingTrackAssociationAlgorithm::Factory)                                  \
        d("TrackRecovery",                          TrackRecoveryAlgorithm::Factory)                                            \
        d("TrackRecoveryHelix",                     TrackRecoveryHelixAlgorithm::Factory)                                       \
        d("TrackRecoveryInteractions",              TrackRecoveryInteractionsAlgorithm::Factory)                                \
        d("TrackClusterAssociation",                TrackClusterAssociationAlgorithm::Factory)                                  \
        d("ClusterPreparation",                     ClusterPreparationAlgorithm::Factory)                                       \
        d("EventPreparation",                       EventPreparationAlgorithm::Factory)                                         \
        d("PfoPreparation",                         PfoPreparationAlgorithm::Factory)                                           \
        d("TrackPreparation",                       TrackPreparationAlgorithm::Factory)

    #define FINE_GRANULARITY_ENERGY_CORRECTION_LIST(d)                                                                          \
        d("CleanClusters",          pandora::HADRONIC,          &FineGranularityEnergyCorrections::CleanCluster)                \
        d("ScaleHotHadrons",        pandora::HADRONIC,          &FineGranularityEnergyCorrections::ScaleHotHadronEnergy)        \
        d("MuonCoilCorrection",     pandora::HADRONIC,          &FineGranularityEnergyCorrections::ApplyMuonEnergyCorrection)

    #define FINE_GRANULARITY_PARTICLE_ID_LIST(d)                                                                                \
        d("FineGranularityEmShowerId",                          &FineGranularityParticleId::FineGranularityEmShowerId)          \
        d("FineGranularityPhotonId",                            &FineGranularityParticleId::FineGranularityPhotonId)            \
        d("FineGranularityElectronId",                          &FineGranularityParticleId::FineGranularityElectronId)          \
        d("FineGranularityMuonId",                              &FineGranularityParticleId::FineGranularityMuonId)

    #define FINE_GRANULARITY_SETTINGS_LIST(d)                                                                                   \
        d("EnergyCorrections",                                  &FineGranularityEnergyCorrections::ReadSettings)                \
        d("ParticleId",                                         &FineGranularityParticleId::ReadSettings)

    /**
     *  @brief  Register all the fine granularity algorithms with pandora
     * 
     *  @param  pandora the pandora instance with which to register content
     */
    static pandora::StatusCode RegisterAlgorithms(pandora::Pandora &pandora);

    /**
     *  @brief  Register all the fine granularity helper functions with pandora
     * 
     *  @param  pandora the pandora instance with which to register content
     */
    static pandora::StatusCode RegisterHelperFunctions(pandora::Pandora &pandora);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode FineGranularityContent::RegisterAlgorithms(pandora::Pandora &pandora)
{
    FINE_GRANULARITY_ALGORITHM_LIST(PANDORA_REGISTER_ALGORITHM);

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode FineGranularityContent::RegisterHelperFunctions(pandora::Pandora &pandora)
{
    FINE_GRANULARITY_ENERGY_CORRECTION_LIST(PANDORA_REGISTER_ENERGY_CORRECTION);
    FINE_GRANULARITY_PARTICLE_ID_LIST(PANDORA_REGISTER_PARTICLE_ID);
    FINE_GRANULARITY_SETTINGS_LIST(PANDORA_REGISTER_SETTINGS);

    return pandora::STATUS_CODE_SUCCESS;
}

#endif // #ifndef FINE_GRANULARITY_CONTENT_H
