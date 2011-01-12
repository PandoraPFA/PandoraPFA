/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Monitoring/EnergyMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the energy monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Monitoring/EnergyMonitoringAlgorithm.h"

#include <iostream>
#include <iomanip>

using namespace pandora;

EnergyMonitoringAlgorithm::~EnergyMonitoringAlgorithm()
{
    if (!m_monitoringFileName.empty() && !m_treeName.empty())
    {
        PANDORA_MONITORING_API(SaveTree(m_treeName, m_monitoringFileName, "UPDATE" ));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyMonitoringAlgorithm::Run()
{
    typedef std::vector<const ClusterList*> ClusterListVector;
    ClusterListVector clusterListVector;

    for (StringVector::iterator iter = m_clusterListNames.begin(), iterEnd = m_clusterListNames.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *pClusterList = NULL;

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetClusterList(*this, (*iter), pClusterList))
        {
            clusterListVector.push_back(pClusterList);
        }
    }

    // Show current cluster list as well
    if (m_clusters)
    {
        const ClusterList *pClusterList = NULL;

        if (STATUS_CODE_SUCCESS == PandoraContentApi::GetCurrentClusterList(*this, pClusterList))
        {
            clusterListVector.push_back(pClusterList);
        }
    }

    EnergyMixing trueCharged;
    EnergyMixing trueNeutral;
    EnergyMixing truePhotons;

    unsigned int clusterNumber = 0;
    unsigned int trackNumber = 0;
    m_clusterEnergies.clear();

    for (ClusterListVector::const_iterator iter = clusterListVector.begin(), iterEnd = clusterListVector.end(); iter != iterEnd; ++iter)
    {
        const ClusterList *pClusterList = (*iter);

        for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster)
        {
            const Cluster* pCluster = (*itCluster);
            ++clusterNumber;

            const TrackList &trackList = pCluster->GetAssociatedTrackList();
            bool clusterHasTracks = !(trackList.empty());
            bool clusterIsPhoton = pCluster->IsPhotonFast();

            float clusterEnergy = 0;
            if (clusterIsPhoton)
            {
                clusterEnergy = pCluster->GetCorrectedElectromagneticEnergy();
            }
            else
            {
                clusterEnergy = pCluster->GetCorrectedHadronicEnergy();
            }

            m_clusterEnergies.push_back(clusterEnergy);

            float energyMcPhoton  = 0.f;
            float energyMcNeutral = 0.f;
            float energyMcCharged = 0.f;

            const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();

            for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
            {

                for (CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
                {
                    CaloHit* pCaloHit = (*hitIter);

                    const MCParticle *pMCParticle = NULL; 
                    pCaloHit->GetMCParticle(pMCParticle);

                    if (NULL == pMCParticle)
                        continue;

                    const int particleId = pMCParticle->GetParticleId();

                    bool mcIsNeutral = ((particleId == K_LONG) || (particleId == NEUTRON));
                    bool mcIsPhoton  = (particleId == PHOTON);
 
                    EnergyMixing &energyMixing = (mcIsPhoton? truePhotons : (mcIsNeutral? trueNeutral : trueCharged));
                    energyMixing.AddMCParticle(pMCParticle);

                    const float electromagneticEnergy = pCaloHit->GetElectromagneticEnergy();
                    const float hadronicEnergy = pCaloHit->GetHadronicEnergy();
                    
                    if (clusterHasTracks)
                    {
                        energyMixing.AddChargedClusterCalorimetricEnergy(electromagneticEnergy);
                    }
                    else if (clusterIsPhoton)
                    {
                        energyMixing.AddPhotonClusterCalorimetricEnergy(electromagneticEnergy);
                    }
                    else
                    {
                        energyMixing.AddNeutralClusterCalorimetricEnergy( hadronicEnergy );
                    }

                    if (mcIsPhoton)
                    {
                        energyMcPhoton += electromagneticEnergy;
                    }
                    else if (mcIsNeutral)
                    {
                        energyMcNeutral += electromagneticEnergy;
                    }
                    else
                    {
                        energyMcCharged += hadronicEnergy;
                    }
                }
            }

            // add number of clusters
            EnergyMixing &energyMixingQuantity = (energyMcPhoton>energyMcNeutral?
                (energyMcPhoton > energyMcCharged ? truePhotons : trueCharged) :
                (energyMcNeutral > energyMcCharged? trueNeutral : trueCharged) );

            if (clusterHasTracks)
            {
                energyMixingQuantity.AddChargedCluster();
            }
            else if (clusterIsPhoton)
            {
                energyMixingQuantity.AddPhotonCluster();
            }
            else
            {
                energyMixingQuantity.AddNeutralCluster();
            }

            // Now for the tracks
            for (TrackList::const_iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack)
            {
                Track *pTrack = (*itTrack);
                ++trackNumber;

                const MCParticle *pMCParticle = NULL; 
                pTrack->GetMCParticle(pMCParticle);

                // Maybe an error should be thrown here?
                if (NULL == pMCParticle)
                    continue;

                const int particleId = pMCParticle->GetParticleId();
                bool mcIsNeutral = ((particleId == K_LONG) || (particleId == NEUTRON));
                bool mcIsPhoton = (particleId == PHOTON);

                EnergyMixing &energyMixing = (mcIsPhoton ? truePhotons : (mcIsNeutral? trueNeutral : trueCharged ) );

                energyMixing.AddMCParticle(pMCParticle);
                energyMixing.AddChargedClusterTracksEnergy(pTrack->GetEnergyAtDca());
                energyMixing.AddTrack();
            }
        }
    }

    return MonitoringOutput(trueCharged, trueNeutral, truePhotons, clusterNumber, trackNumber);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyMonitoringAlgorithm::MonitoringOutput(const EnergyMixing &trueCharged, const EnergyMixing &trueNeutral, const EnergyMixing &truePhotons,
    const int numberClusters, const int numberTracks)
{
    // Get energies
    const float trueCharged_recoChargedCalo   = trueCharged.GetChargedClusterCalorimetricEnergy();
    const float trueCharged_recoNeutralCalo   = trueCharged.GetNeutralClusterCalorimetricEnergy();
    const float trueCharged_recoPhotonCalo    = trueCharged.GetPhotonClusterCalorimetricEnergy();
    const float trueCharged_recoChargedTracks = trueCharged.GetChargedClusterTracksEnergy();
    const float trueCharged_mc                = trueCharged.GetMCParticleSetEnergy();

    const float trueNeutral_recoChargedCalo   = trueNeutral.GetChargedClusterCalorimetricEnergy();
    const float trueNeutral_recoNeutralCalo   = trueNeutral.GetNeutralClusterCalorimetricEnergy();
    const float trueNeutral_recoPhotonCalo    = trueNeutral.GetPhotonClusterCalorimetricEnergy();
    const float trueNeutral_recoChargedTracks = trueNeutral.GetChargedClusterTracksEnergy();
    const float trueNeutral_mc                = trueNeutral.GetMCParticleSetEnergy();

    const float truePhotons_recoChargedCalo   = truePhotons.GetChargedClusterCalorimetricEnergy();
    const float truePhotons_recoNeutralCalo   = truePhotons.GetNeutralClusterCalorimetricEnergy();
    const float truePhotons_recoPhotonCalo    = truePhotons.GetPhotonClusterCalorimetricEnergy();
    const float truePhotons_recoChargedTracks = truePhotons.GetChargedClusterTracksEnergy();
    const float truePhotons_mc                = truePhotons.GetMCParticleSetEnergy();

    // Get quantities
    const int trueCharged_recoChargedCaloClusters = trueCharged.GetChargedClusterCalorimetricClusters();
    const int trueCharged_recoNeutralCaloClusters = trueCharged.GetNeutralClusterCalorimetricClusters();
    const int trueCharged_recoPhotonCaloClusters  = trueCharged.GetPhotonClusterCalorimetricClusters();
    const int trueCharged_recoChargedTracksNumber = trueCharged.GetChargedClusterTracks();
    const int trueCharged_mcNumber                = trueCharged.GetMCParticleNumber();

    const int trueNeutral_recoChargedCaloClusters = trueNeutral.GetChargedClusterCalorimetricClusters();
    const int trueNeutral_recoNeutralCaloClusters = trueNeutral.GetNeutralClusterCalorimetricClusters();
    const int trueNeutral_recoPhotonCaloClusters  = trueNeutral.GetPhotonClusterCalorimetricClusters();
    const int trueNeutral_recoChargedTracksNumber = trueNeutral.GetChargedClusterTracks();
    const int trueNeutral_mcNumber                = trueNeutral.GetMCParticleNumber();

    const int truePhotons_recoChargedCaloClusters = truePhotons.GetChargedClusterCalorimetricClusters();
    const int truePhotons_recoNeutralCaloClusters = truePhotons.GetNeutralClusterCalorimetricClusters();
    const int truePhotons_recoPhotonCaloClusters  = truePhotons.GetPhotonClusterCalorimetricClusters();
    const int truePhotons_recoChargedTracksNumber = truePhotons.GetChargedClusterTracks();
    const int truePhotons_mcNumber                = truePhotons.GetMCParticleNumber();

    if (m_print)
    {
        std::cout << "cluster list names : ";
        for (StringVector::iterator iter = m_clusterListNames.begin(), iterEnd = m_clusterListNames.end(); iter != iterEnd; ++iter)
        {
            std::cout << (*iter) << " " << std::flush;
        }
        std::cout << std::endl;
        std::cout << "number of clusters  : " << numberClusters << std::endl;
        std::cout << "number of tracks    : " << numberTracks << std::endl;
        std::cout << "energies of clusters: " << std::endl;

        std::ios_base::fmtflags original_flags = std::cout.flags();   // store the original flags

        static const int precision = 1;
        static const int width = 12;
        static const int widthNum = 3;

        std::cout << std::fixed;
        std::cout << std::setprecision(precision);

        for (FloatVector::const_iterator iter = m_clusterEnergies.begin(), iterEnd = m_clusterEnergies.end(); iter != iterEnd; ++iter)
        {
            std::cout << "  " << (*iter);
        }
        std::cout << std::endl;

        if (m_quantity)
        {
            FORMATTED_OUTPUT_SHORT( "           ", "true +-", "true 0", "true phot");
            FORMATTED_OUTPUT_LONG("true     : ",
                trueCharged_mc, trueCharged_mcNumber,
                trueNeutral_mc, trueNeutral_mcNumber,
                truePhotons_mc, truePhotons_mcNumber);

            FORMATTED_OUTPUT_SHORT( "--- ", "--- ", "--- ", "--- " );
            FORMATTED_OUTPUT_LONG( "+- calo  : ",
                                 trueCharged_recoChargedCalo, trueCharged_recoChargedCaloClusters,
                                 trueNeutral_recoChargedCalo, trueNeutral_recoChargedCaloClusters,
                                 truePhotons_recoChargedCalo, truePhotons_recoChargedCaloClusters);
            FORMATTED_OUTPUT_LONG( "+- tracks: ",
                                 trueCharged_recoChargedTracks, trueCharged_recoChargedTracksNumber,
                                 trueNeutral_recoChargedTracks, trueNeutral_recoChargedTracksNumber,
                                 truePhotons_recoChargedTracks, truePhotons_recoChargedTracksNumber);
            FORMATTED_OUTPUT_LONG( "0  calo  : ",
                                 trueCharged_recoNeutralCalo, trueCharged_recoNeutralCaloClusters,
                                 trueNeutral_recoNeutralCalo, trueNeutral_recoNeutralCaloClusters,
                                 truePhotons_recoNeutralCalo, truePhotons_recoNeutralCaloClusters);
            FORMATTED_OUTPUT_LONG( "phot     : ",
                                 trueCharged_recoPhotonCalo, trueCharged_recoPhotonCaloClusters,
                                 trueNeutral_recoPhotonCalo, trueNeutral_recoPhotonCaloClusters,
                                 truePhotons_recoPhotonCalo, truePhotons_recoPhotonCaloClusters);
        }
        else
        {

            FORMATTED_OUTPUT( "           ", "true +-", "true 0", "true phot" );
            FORMATTED_OUTPUT( "true     : ", trueCharged_mc, trueNeutral_mc, truePhotons_mc);
            FORMATTED_OUTPUT( "---", "---", "---", "---" );
            FORMATTED_OUTPUT( "+- calo  : ",
                             trueCharged_recoChargedCalo,
                             trueNeutral_recoChargedCalo,
                             truePhotons_recoChargedCalo);
            FORMATTED_OUTPUT( "+- tracks: ",
                             trueCharged_recoChargedTracks,
                             trueNeutral_recoChargedTracks,
                             truePhotons_recoChargedTracks);
            FORMATTED_OUTPUT( "0  calo  : ",
                             trueCharged_recoNeutralCalo,
                             trueNeutral_recoNeutralCalo,
                             truePhotons_recoNeutralCalo);
            FORMATTED_OUTPUT( "phot     : ",
                             trueCharged_recoPhotonCalo,
                             trueNeutral_recoPhotonCalo,
                             truePhotons_recoPhotonCalo);
        }

        // Restore original flags
        std::cout.flags(original_flags);
    }

    if (!(m_monitoringFileName.empty()) && !(m_treeName.empty()))
    {
        if (m_print)
            std::cout << "energy monitoring written into tree : " << m_treeName << std::endl;

        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "clusters", numberClusters ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "tracks",   numberTracks ));

        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "c2c", trueCharged_recoChargedCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "c2n", trueCharged_recoNeutralCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "c2p", trueCharged_recoPhotonCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "c2t", trueCharged_recoChargedTracks ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "cMc", trueCharged_mc ));

        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "n2c", trueNeutral_recoChargedCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "n2n", trueNeutral_recoNeutralCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "n2p", trueNeutral_recoPhotonCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "n2t", trueNeutral_recoChargedTracks ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "nMc", trueNeutral_mc ));

        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "p2c", truePhotons_recoChargedCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "p2n", truePhotons_recoNeutralCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "p2p", truePhotons_recoPhotonCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "p2t", truePhotons_recoChargedTracks ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "pMc", truePhotons_mc ));

        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ECluster", &m_clusterEnergies ));

        if (m_quantity)
        {
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Np2c", truePhotons_recoChargedCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Np2n", truePhotons_recoNeutralCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Np2p", truePhotons_recoPhotonCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Np2t", truePhotons_recoChargedTracksNumber));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "NpMc", truePhotons_mcNumber));

            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nc2c", trueCharged_recoChargedCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nc2n", trueCharged_recoNeutralCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nc2p", trueCharged_recoPhotonCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nc2t", trueCharged_recoChargedTracksNumber));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "NcMc", trueCharged_mcNumber));

            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nn2c", trueNeutral_recoChargedCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nn2n", trueNeutral_recoNeutralCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nn2p", trueNeutral_recoPhotonCaloClusters));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nn2t", trueNeutral_recoChargedTracksNumber));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "NnMc", trueNeutral_mcNumber));
        }

        // PANDORA_MONITORING_API(PrintTree(m_treeName));
        PANDORA_MONITORING_API(FillTree(m_treeName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

EnergyMonitoringAlgorithm::EnergyMixing::EnergyMixing() :
    m_chargedCalorimetric(0),
    m_chargedTracks(0),
    m_neutralCalorimetric(0),
    m_photonCalorimetric(0),
    m_chargedCalorimetricClusters(0),
    m_chargedTracksNumber(0),
    m_neutralCalorimetricClusters(0),
    m_photonCalorimetricClusters(0)
{
    m_mcParticleSet.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

float EnergyMonitoringAlgorithm::EnergyMixing::GetMCParticleSetEnergy() const
{
    float energy(0.f);

    for (MCParticleSet::const_iterator iter = m_mcParticleSet.begin(), iterEnd = m_mcParticleSet.end(); iter != iterEnd; ++iter)
    {
        energy += (*iter)->GetEnergy();
    }

    return energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "ClusterListNames", m_clusterListNames));

    if (m_clusterListNames.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    m_clusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShowCurrentClusters", m_clusters));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MonitoringFileName", m_monitoringFileName));

    m_treeName = "emon";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeName", m_treeName));

    m_print = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Print", m_print));

    m_quantity = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Quantity", m_quantity));

    return STATUS_CODE_SUCCESS;
}
