/**
 *  @file   PandoraPFANew/include/Pandora/PandoraAlgorithms.h
 * 
 *  @brief  Header file detailing algorithms included in pandora library
 * 
 *  $Log: $
 */
#ifndef PANDORA_ALGORITHMS_H
#define PANDORA_ALGORITHMS_H 1

#include "Algorithms/FinalParticleIdAlgorithm.h"
#include "Algorithms/FragmentRemovalAlgorithm.h"
#include "Algorithms/PhotonRecoveryAlgorithm.h"
#include "Algorithms/PrimaryClusteringAlgorithm.h"
#include "Algorithms/ReclusteringAlgorithm.h"
#include "Algorithms/TopologicalAssociationAlgorithm.h"
#include "Algorithms/TrackSelectionAlgorithm.h"

#include "Algorithms/Cheating/PerfectClusteringAlgorithm.h"
#include "Algorithms/Cheating/PerfectPhotonClusteringAlgorithm.h"
#include "Algorithms/Cheating/PerfectNeutralHadronClusteringAlgorithm.h"
#include "Algorithms/Cheating/CheatingTrackToClusterMatching.h"
#include "Algorithms/Cheating/CheatingClusterMergingAlgorithm.h"
#include "Algorithms/Cheating/CheatingPfoCreationAlgorithm.h"
#include "Algorithms/Cheating/PerfectFragmentRemovalAlgorithm.h"

#include "Algorithms/Clustering/ClusteringAlgorithm.h"
#include "Algorithms/Clustering/ECalPhotonClusteringAlgorithm.h"
#include "Algorithms/Clustering/ForcedClusteringAlgorithm.h"

#include "Algorithms/FragmentRemoval/MainFragmentRemovalAlgorithm.h"
#include "Algorithms/FragmentRemoval/NeutralFragmentRemovalAlgorithm.h"

#include "Algorithms/Monitoring/CaloHitMonitoringAlgorithm.h"
#include "Algorithms/Monitoring/EnergyMonitoringAlgorithm.h"
#include "Algorithms/Monitoring/MCParticlesMonitoringAlgorithm.h"
#include "Algorithms/Monitoring/VisualMonitoringAlgorithm.h"

#include "Algorithms/PfoConstruction/ClusterPreparationAlgorithm.h"
#include "Algorithms/PfoConstruction/LoopingTrackAssociationAlgorithm.h"
#include "Algorithms/PfoConstruction/PfoCreationAlgorithm.h"
#include "Algorithms/PfoConstruction/V0PfoCreationAlgorithm.h"
#include "Algorithms/PfoConstruction/TrackPreparationAlgorithm.h"
#include "Algorithms/PfoConstruction/TrackRecoveryAlgorithm.h"
#include "Algorithms/PfoConstruction/TrackRecoveryHelixAlgorithm.h"
#include "Algorithms/PfoConstruction/TrackRecoveryInteractionsAlgorithm.h"

#include "Algorithms/Reclustering/ExitingTrackAlg.h"
#include "Algorithms/Reclustering/ForceSplitTrackAssociationsAlg.h"
#include "Algorithms/Reclustering/ResolveTrackAssociationsAlg.h"
#include "Algorithms/Reclustering/SplitMergedClustersAlg.h"
#include "Algorithms/Reclustering/SplitTrackAssociationsAlg.h"
#include "Algorithms/Reclustering/TrackDrivenAssociationAlg.h"
#include "Algorithms/Reclustering/TrackDrivenMergingAlg.h"

#include "Algorithms/TopologicalAssociation/BackscatteredTracksAlgorithm.h"
#include "Algorithms/TopologicalAssociation/BackscatteredTracks2Algorithm.h"
#include "Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.h"
#include "Algorithms/TopologicalAssociation/ConeBasedMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/IsolatedHitMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/LoopingTracksAlgorithm.h"
#include "Algorithms/TopologicalAssociation/MipPhotonSeparationAlgorithm.h"
#include "Algorithms/TopologicalAssociation/MuonPhotonSeparationAlgorithm.h"
#include "Algorithms/TopologicalAssociation/MuonClusterAssociationAlgorithm.h"
#include "Algorithms/TopologicalAssociation/ProximityBasedMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/ShowerMipMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/ShowerMipMerging2Algorithm.h"
#include "Algorithms/TopologicalAssociation/ShowerMipMerging3Algorithm.h"
#include "Algorithms/TopologicalAssociation/ShowerMipMerging4Algorithm.h"
#include "Algorithms/TopologicalAssociation/SoftClusterMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/TrackClusterAssociationAlgorithm.h"

#define PANDORA_ALGORITHM_LIST(d)                                                                           \
    d("FinalParticleId",                        FinalParticleIdAlgorithm::Factory)                          \
    d("FragmentRemoval",                        FragmentRemovalAlgorithm::Factory)                          \
    d("PhotonRecovery",                         PhotonRecoveryAlgorithm::Factory)                           \
    d("PrimaryClustering",                      PrimaryClusteringAlgorithm::Factory)                        \
    d("Reclustering",                           ReclusteringAlgorithm::Factory)                             \
    d("TopologicalAssociation",                 TopologicalAssociationAlgorithm::Factory)                   \
    d("TrackSelection",                         TrackSelectionAlgorithm::Factory)                           \
    d("PerfectClustering",                      PerfectClusteringAlgorithm::Factory)                        \
    d("PerfectPhotonClustering",                PerfectPhotonClusteringAlgorithm::Factory)                  \
    d("PerfectNeutralHadronClustering",         PerfectNeutralHadronClusteringAlgorithm::Factory)           \
    d("CheatingTrackToClusterMatching",         CheatingTrackToClusterMatching::Factory)                    \
    d("CheatingClusterMerging",                 CheatingClusterMergingAlgorithm::Factory)                   \
    d("CheatingPfoCreation",                    CheatingPfoCreationAlgorithm::Factory)                      \
    d("PerfectFragmentRemoval",                 PerfectFragmentRemovalAlgorithm::Factory)                   \
    d("Clustering",                             ClusteringAlgorithm::Factory)                               \
    d("ECalPhotonClustering",                   ECalPhotonClusteringAlgorithm::Factory)                     \
    d("ForcedClustering",                       ForcedClusteringAlgorithm::Factory)                         \
    d("MainFragmentRemoval",                    MainFragmentRemovalAlgorithm::Factory)                      \
    d("NeutralFragmentRemoval",                 NeutralFragmentRemovalAlgorithm::Factory)                   \
    d("CaloHitMonitoring",                      CaloHitMonitoringAlgorithm::Factory)                        \
    d("EnergyMonitoring",                       EnergyMonitoringAlgorithm::Factory)                         \
    d("VisualMonitoring",                       VisualMonitoringAlgorithm::Factory)                         \
    d("MCParticlesMonitoring",                  MCParticlesMonitoringAlgorithm::Factory)                    \
    d("ClusterPreparation",                     ClusterPreparationAlgorithm::Factory)                       \
    d("LoopingTrackAssociation",                LoopingTrackAssociationAlgorithm::Factory)                  \
    d("PfoCreation",                            PfoCreationAlgorithm::Factory)                              \
    d("V0PfoCreation",                          V0PfoCreationAlgorithm::Factory)                            \
    d("TrackPreparation",                       TrackPreparationAlgorithm::Factory)                         \
    d("TrackRecovery",                          TrackRecoveryAlgorithm::Factory)                            \
    d("TrackRecoveryHelix",                     TrackRecoveryHelixAlgorithm::Factory)                       \
    d("TrackRecoveryInteractions",              TrackRecoveryInteractionsAlgorithm::Factory)                \
    d("ExitingTrack",                           ExitingTrackAlg::Factory)                                   \
    d("ForceSplitTrackAssociations",            ForceSplitTrackAssociationsAlg::Factory)                    \
    d("ResolveTrackAssociations",               ResolveTrackAssociationsAlg::Factory)                       \
    d("SplitMergedClusters",                    SplitMergedClustersAlg::Factory)                            \
    d("SplitTrackAssociations",                 SplitTrackAssociationsAlg::Factory)                         \
    d("TrackDrivenAssociation",                 TrackDrivenAssociationAlg::Factory)                         \
    d("TrackDrivenMerging",                     TrackDrivenMergingAlg::Factory)                             \
    d("BackscatteredTracks",                    BackscatteredTracksAlgorithm::Factory)                      \
    d("BackscatteredTracks2",                   BackscatteredTracks2Algorithm::Factory)                     \
    d("BrokenTracks",                           BrokenTracksAlgorithm::Factory)                             \
    d("ConeBasedMerging",                       ConeBasedMergingAlgorithm::Factory)                         \
    d("IsolatedHitMerging",                     IsolatedHitMergingAlgorithm::Factory)                       \
    d("LoopingTracks",                          LoopingTracksAlgorithm::Factory)                            \
    d("MipPhotonSeparation",                    MipPhotonSeparationAlgorithm::Factory)                      \
    d("MuonPhotonSeparation",                   MuonPhotonSeparationAlgorithm::Factory)                     \
    d("MuonClusterAssociation",                 MuonClusterAssociationAlgorithm::Factory)                   \
    d("ProximityBasedMerging",                  ProximityBasedMergingAlgorithm::Factory)                    \
    d("ShowerMipMerging",                       ShowerMipMergingAlgorithm::Factory)                         \
    d("ShowerMipMerging2",                      ShowerMipMerging2Algorithm::Factory)                        \
    d("ShowerMipMerging3",                      ShowerMipMerging3Algorithm::Factory)                        \
    d("ShowerMipMerging4",                      ShowerMipMerging4Algorithm::Factory)                        \
    d("SoftClusterMerging",                     SoftClusterMergingAlgorithm::Factory)                       \
    d("TrackClusterAssociation",                TrackClusterAssociationAlgorithm::Factory)

#define PANDORA_CREATE_ALGORITHM(a, b)                                                                      \
    {                                                                                                       \
        StatusCode statusCode = RegisterAlgorithmFactory(a, new b);                                         \
                                                                                                            \
        if (STATUS_CODE_SUCCESS != statusCode)                                                              \
            throw StatusCodeException(statusCode);                                                          \
    }

#define PANDORA_CREATE_ALL_ALGORITHMS()                                                                     \
    PANDORA_ALGORITHM_LIST(PANDORA_CREATE_ALGORITHM)

#endif // #ifndef PANDORA_ALGORITHMS_H
