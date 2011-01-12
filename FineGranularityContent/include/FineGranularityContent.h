/**
 *  @file   PandoraPFANew/FineGranularityContent/include/FineGranularityContent.h
 * 
 *  @brief  Header file detailing content for use with a fine granularity particle flow detector
 * 
 *  $Log: $
 */
#ifndef FINE_GRANULARITY_CONTENT_H
#define FINE_GRANULARITY_CONTENT_H 1

#include "EventPreparationAlgorithm.h"
#include "FinalParticleIdAlgorithm.h"
#include "FragmentRemovalAlgorithm.h"
#include "MuonReconstructionAlgorithm.h"
#include "PhotonRecoveryAlgorithm.h"
#include "PrimaryClusteringAlgorithm.h"
#include "ReclusteringAlgorithm.h"
#include "TopologicalAssociationAlgorithm.h"

#include "Cheating/PerfectClusteringAlgorithm.h"
#include "Cheating/PerfectPhotonClusteringAlgorithm.h"
#include "Cheating/PerfectNeutralHadronClusteringAlgorithm.h"
#include "Cheating/CheatingTrackToClusterMatching.h"
#include "Cheating/CheatingClusterMergingAlgorithm.h"
#include "Cheating/CheatingPfoCreationAlgorithm.h"
#include "Cheating/PerfectFragmentRemovalAlgorithm.h"

#include "Clustering/ClusteringAlgorithm.h"
#include "Clustering/ECalPhotonClusteringAlgorithm.h"
#include "Clustering/ForcedClusteringAlgorithm.h"
#include "Clustering/InwardClusteringAlgorithm.h"

#include "FragmentRemoval/MainFragmentRemovalAlgorithm.h"
#include "FragmentRemoval/NeutralFragmentRemovalAlgorithm.h"
#include "FragmentRemoval/PhotonFragmentRemovalAlgorithm.h"

#include "Monitoring/CaloHitMonitoringAlgorithm.h"
#include "Monitoring/EfficiencyMonitoringAlgorithm.h"
#include "Monitoring/EnergyMonitoringAlgorithm.h"
#include "Monitoring/MCParticlesMonitoringAlgorithm.h"
#include "Monitoring/VisualMonitoringAlgorithm.h"
#include "Monitoring/DumpPfosMonitoringAlgorithm.h"

#include "PfoConstruction/ClusterPreparationAlgorithm.h"
#include "PfoConstruction/LoopingTrackAssociationAlgorithm.h"
#include "PfoConstruction/PfoCreationAlgorithm.h"
#include "PfoConstruction/V0PfoCreationAlgorithm.h"
#include "PfoConstruction/KinkPfoCreationAlgorithm.h"
#include "PfoConstruction/TrackPreparationAlgorithm.h"
#include "PfoConstruction/TrackRecoveryAlgorithm.h"
#include "PfoConstruction/TrackRecoveryHelixAlgorithm.h"
#include "PfoConstruction/TrackRecoveryInteractionsAlgorithm.h"

#include "Reclustering/ExitingTrackAlg.h"
#include "Reclustering/ForceSplitTrackAssociationsAlg.h"
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
#include "TopologicalAssociation/TrackClusterAssociationAlgorithm.h"

#include "FineGranularityEnergyCorrections.h"

/**
 *  @brief  FineGranularityContent class
 */
class FineGranularityContent
{
public:
    #define FINE_GRANULARITY_ALGORITHM_LIST(d)                                                                                  \
        d("EventPreparation",                       EventPreparationAlgorithm::Factory)                                         \
        d("FinalParticleId",                        FinalParticleIdAlgorithm::Factory)                                          \
        d("FragmentRemoval",                        FragmentRemovalAlgorithm::Factory)                                          \
        d("MuonReconstruction",                     MuonReconstructionAlgorithm::Factory)                                       \
        d("PhotonRecovery",                         PhotonRecoveryAlgorithm::Factory)                                           \
        d("PrimaryClustering",                      PrimaryClusteringAlgorithm::Factory)                                        \
        d("Reclustering",                           ReclusteringAlgorithm::Factory)                                             \
        d("TopologicalAssociation",                 TopologicalAssociationAlgorithm::Factory)                                   \
        d("PerfectClustering",                      PerfectClusteringAlgorithm::Factory)                                        \
        d("PerfectPhotonClustering",                PerfectPhotonClusteringAlgorithm::Factory)                                  \
        d("PerfectNeutralHadronClustering",         PerfectNeutralHadronClusteringAlgorithm::Factory)                           \
        d("CheatingTrackToClusterMatching",         CheatingTrackToClusterMatching::Factory)                                    \
        d("CheatingClusterMerging",                 CheatingClusterMergingAlgorithm::Factory)                                   \
        d("CheatingPfoCreation",                    CheatingPfoCreationAlgorithm::Factory)                                      \
        d("PerfectFragmentRemoval",                 PerfectFragmentRemovalAlgorithm::Factory)                                   \
        d("Clustering",                             ClusteringAlgorithm::Factory)                                               \
        d("ECalPhotonClustering",                   ECalPhotonClusteringAlgorithm::Factory)                                     \
        d("ForcedClustering",                       ForcedClusteringAlgorithm::Factory)                                         \
        d("InwardClustering",                       InwardClusteringAlgorithm::Factory)                                         \
        d("MainFragmentRemoval",                    MainFragmentRemovalAlgorithm::Factory)                                      \
        d("NeutralFragmentRemoval",                 NeutralFragmentRemovalAlgorithm::Factory)                                   \
        d("PhotonFragmentRemoval",                  PhotonFragmentRemovalAlgorithm::Factory)                                    \
        d("CaloHitMonitoring",                      CaloHitMonitoringAlgorithm::Factory)                                        \
        d("EfficiencyMonitoring",                   EfficiencyMonitoringAlgorithm::Factory)                                     \
        d("EnergyMonitoring",                       EnergyMonitoringAlgorithm::Factory)                                         \
        d("VisualMonitoring",                       VisualMonitoringAlgorithm::Factory)                                         \
        d("MCParticlesMonitoring",                  MCParticlesMonitoringAlgorithm::Factory)                                    \
        d("DumpPfosMonitoring",                     DumpPfosMonitoringAlgorithm::Factory)                                       \
        d("ClusterPreparation",                     ClusterPreparationAlgorithm::Factory)                                       \
        d("LoopingTrackAssociation",                LoopingTrackAssociationAlgorithm::Factory)                                  \
        d("PfoCreation",                            PfoCreationAlgorithm::Factory)                                              \
        d("V0PfoCreation",                          V0PfoCreationAlgorithm::Factory)                                            \
        d("KinkPfoCreation",                        KinkPfoCreationAlgorithm::Factory)                                          \
        d("TrackPreparation",                       TrackPreparationAlgorithm::Factory)                                         \
        d("TrackRecovery",                          TrackRecoveryAlgorithm::Factory)                                            \
        d("TrackRecoveryHelix",                     TrackRecoveryHelixAlgorithm::Factory)                                       \
        d("TrackRecoveryInteractions",              TrackRecoveryInteractionsAlgorithm::Factory)                                \
        d("ExitingTrack",                           ExitingTrackAlg::Factory)                                                   \
        d("ForceSplitTrackAssociations",            ForceSplitTrackAssociationsAlg::Factory)                                    \
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
        d("TrackClusterAssociation",                TrackClusterAssociationAlgorithm::Factory)

    #define CREATE_FINE_GRANULARITY_ALGORITHM(a, b)                                                                             \
        {                                                                                                                       \
            const pandora::StatusCode statusCode(PandoraApi::RegisterAlgorithmFactory(pandora, a, new b));                      \
                                                                                                                                \
            if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                                     \
                return statusCode;                                                                                              \
        }

    #define FINE_GRANULARITY_ENERGY_CORRECTION_LIST(d)                                                                          \
        d("CleanClusters",          pandora::HADRONIC,          &FineGranularityEnergyCorrections::CleanCluster)                \
        d("ScaleHotHadrons",        pandora::HADRONIC,          &FineGranularityEnergyCorrections::ScaleHotHadronEnergy)        \
        d("MuonCoilCorrection",     pandora::HADRONIC,          &FineGranularityEnergyCorrections::ApplyMuonEnergyCorrection)

    #define REGISTER_FINE_GRANULARITY_ENERGY_CORRECTION(a, b, c)                                                                \
        {                                                                                                                       \
            const pandora::StatusCode statusCode(PandoraApi::RegisterEnergyCorrectionFunction(pandora, a, b, c));               \
                                                                                                                                \
            if (pandora::STATUS_CODE_SUCCESS != statusCode)                                                                     \
                return statusCode;                                                                                              \
        }

    /**
     *  @brief  Register the fine granularity content with pandora
     * 
     *  @param  pandora the pandora instance with which to register content
     */
    static pandora::StatusCode Register(pandora::Pandora &pandora);
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::StatusCode FineGranularityContent::Register(pandora::Pandora &pandora)
{
    FINE_GRANULARITY_ALGORITHM_LIST(CREATE_FINE_GRANULARITY_ALGORITHM);
    FINE_GRANULARITY_ENERGY_CORRECTION_LIST(REGISTER_FINE_GRANULARITY_ENERGY_CORRECTION);

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraApi::RegisterSettingsFunction(pandora, "EnergyCorrections",
        &FineGranularityEnergyCorrections::ReadSettings));

    return pandora::STATUS_CODE_SUCCESS;
}

#endif // #ifndef FINE_GRANULARITY_CONTENT_H
