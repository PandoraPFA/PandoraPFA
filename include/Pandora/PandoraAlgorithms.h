/**
 *  @file   PandoraPFANew/include/Pandora/PandoraAlgorithms.h
 * 
 *  @brief  Header file detailing algorithms included in pandora library
 * 
 *  $Log: $
 */
#ifndef PANDORA_ALGORITHMS_H
#define PANDORA_ALGORITHMS_H 1

#include "Algorithms/FragmentRemovalAlgorithm.h"
#include "Algorithms/PfoConstructionAlgorithm.h"
#include "Algorithms/ECalPhotonIdAlgorithm.h"
#include "Algorithms/ECalPhotonFindingAlgorithm.h"
#include "Algorithms/PrimaryClusteringAlgorithm.h"
#include "Algorithms/ReclusteringAlgorithm.h"
#include "Algorithms/TopologicalAssociationAlgorithm.h"
#include "Algorithms/TrackSelectionAlgorithm.h"

#include "Algorithms/Cheating/CheatingAlgorithm.h"

#include "Algorithms/Clustering/ClusteringAlgorithm.h"
#include "Algorithms/Clustering/PerfectClusteringAlgorithm.h"

#include "Algorithms/Monitoring/CaloHitMonitoringAlgorithm.h"

#include "Algorithms/PfoConstruction/ClusterPreparationAlgorithm.h"
#include "Algorithms/PfoConstruction/TrackPreparationAlgorithm.h"
#include "Algorithms/PfoConstruction/PfoCreationAlgorithm.h"

#include "Algorithms/Reclustering/SplitTrackAssociationsAlg.h"
#include "Algorithms/Reclustering/ResolveTrackAssociationsAlg.h"
#include "Algorithms/Reclustering/TrackDrivenAssociationAlg.h"
#include "Algorithms/Reclustering/TrackDrivenMergingAlg.h"

#include "Algorithms/TopologicalAssociation/BackscatteredTracksAlgorithm.h"
#include "Algorithms/TopologicalAssociation/BackscatteredTracks2Algorithm.h"
#include "Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.h"
#include "Algorithms/TopologicalAssociation/ConeBasedMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/IsolatedHitMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/LoopingTracksAlgorithm.h"
#include "Algorithms/TopologicalAssociation/ProximityBasedMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/ShowerMipMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/ShowerMipMerging2Algorithm.h"
#include "Algorithms/TopologicalAssociation/ShowerMipMerging3Algorithm.h"
#include "Algorithms/TopologicalAssociation/ShowerMipMerging4Algorithm.h"
#include "Algorithms/TopologicalAssociation/SoftClusterMergingAlgorithm.h"
#include "Algorithms/TopologicalAssociation/TrackClusterAssociationAlgorithm.h"

#define PANDORA_ALGORITHM_LIST(d)                                                                           \
    d("FragmentRemoval",                        FragmentRemovalAlgorithm::Factory)                          \
    d("PfoConstruction",                        PfoConstructionAlgorithm::Factory)                          \
    d("ECalPhotonId",                           ECalPhotonIdAlgorithm::Factory)                             \
    d("ECalPhotonFinding",                      ECalPhotonFindingAlgorithm::Factory)                        \
    d("PrimaryClustering",                      PrimaryClusteringAlgorithm::Factory)                        \
    d("Reclustering",                           ReclusteringAlgorithm::Factory)                             \
    d("TopologicalAssociation",                 TopologicalAssociationAlgorithm::Factory)                   \
    d("TrackSelection",                         TrackSelectionAlgorithm::Factory)                           \
    d("Cheating",                               CheatingAlgorithm::Factory)                                 \
    d("Clustering",                             ClusteringAlgorithm::Factory)                               \
    d("PerfectClustering",                      PerfectClusteringAlgorithm::Factory)                        \
    d("CaloHitMonitoring",                      CaloHitMonitoringAlgorithm::Factory)                        \
    d("ClusterPreparation",                     ClusterPreparationAlgorithm::Factory)                       \
    d("TrackPreparation",                       TrackPreparationAlgorithm::Factory)                         \
    d("PfoCreation",                            PfoCreationAlgorithm::Factory)                              \
    d("ResolveTrackAssociations",               ResolveTrackAssociationsAlg::Factory)                       \
    d("SplitTrackAssociations",                 SplitTrackAssociationsAlg::Factory)                         \
    d("TrackDrivenAssociation",                 TrackDrivenAssociationAlg::Factory)                         \
    d("TrackDrivenMerging",                     TrackDrivenMergingAlg::Factory)                             \
    d("BackscatteredTracks",                    BackscatteredTracksAlgorithm::Factory)                      \
    d("BackscatteredTracks2",                   BackscatteredTracks2Algorithm::Factory)                     \
    d("BrokenTracks",                           BrokenTracksAlgorithm::Factory)                             \
    d("ConeBasedMerging",                       ConeBasedMergingAlgorithm::Factory)                         \
    d("IsolatedHitMerging",                     IsolatedHitMergingAlgorithm::Factory)                       \
    d("LoopingTracks",                          LoopingTracksAlgorithm::Factory)                            \
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
