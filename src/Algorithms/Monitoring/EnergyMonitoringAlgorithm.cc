/**
 *  @file   PandoraPFANew/src/Algorithms/Monitoring/EnergyMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the energy monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Algorithms/Monitoring/EnergyMonitoringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

#include <iostream>
#include <iomanip>

using namespace pandora;



//------------------------------------------------------------------------------------------------------------------------------------------

EnergyMonitoringAlgorithm::EnergyMixing::EnergyMixing()
    : m_chargedCalorimetric(0),
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

float EnergyMonitoringAlgorithm::EnergyMixing::GetMcParticleSetEnergy()
{
    McParticleSet& mcParticleSet = GetMcParticleSet();
    
    float energy = 0.f;
    for( McParticleSet::iterator itMc = mcParticleSet.begin(), itMcEnd = mcParticleSet.end(); itMc != itMcEnd; ++itMc )
    {
        const pandora::MCParticle *const mc = (*itMc);
        energy += mc->GetEnergy();
    }
    
    return energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

int EnergyMonitoringAlgorithm::EnergyMixing::GetMcParticleNumber()
{
    McParticleSet& mcParticleSet = GetMcParticleSet();
    return mcParticleSet.size();
}


//------------------------------------------------------------------------------------------------------------------------------------------

EnergyMonitoringAlgorithm::~EnergyMonitoringAlgorithm()
{
    if( !m_monitoringFileName.empty() && !m_treeName.empty() )
    {
//         PANDORA_MONITORING_API(PrintTree(m_treeName));
        PANDORA_MONITORING_API(SaveTree(m_treeName, m_monitoringFileName, "UPDATE" ));
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyMonitoringAlgorithm::Run()
{
    typedef std::vector<const ClusterList*> ClusterVector;
    ClusterVector clusterListVector;

    for( pandora::StringVector::iterator itClusterName = m_clusterListNames.begin(), itClusterNameEnd = m_clusterListNames.end(); itClusterName != itClusterNameEnd; ++itClusterName )
    {
        const ClusterList* pClusterList = NULL;
        if( STATUS_CODE_SUCCESS == PandoraContentApi::GetClusterList(*this, (*itClusterName), pClusterList))
            clusterListVector.push_back( pClusterList ); // add the cluster list
    }
    

    if(m_clusters) // show current cluster list as well
    {
        const ClusterList* pClusterList = NULL;
        if( STATUS_CODE_SUCCESS == PandoraContentApi::GetCurrentClusterList(*this, pClusterList) )
            clusterListVector.push_back( pClusterList ); // add the cluster list
    }

    EnergyMixing trueCharged;
//     EnergyMixing trueChargedEm;
    EnergyMixing trueNeutral;
    EnergyMixing truePhotons;
    
    int clusterNumber = 0;
    int trackNumber   = 0;

    for( ClusterVector::const_iterator itClusterList = clusterListVector.begin(), itClusterListEnd = clusterListVector.end(); itClusterList != itClusterListEnd; ++itClusterList )
    {
        const ClusterList* pClusterList = (*itClusterList);
        for( ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
        {
            const Cluster* pCluster = (*itCluster);
            ++clusterNumber;

            const TrackList& pTrackList = pCluster->GetAssociatedTrackList();
            bool clusterHasTracks = !(pTrackList.empty());
            bool clusterIsPhoton  = pCluster->IsPhoton();
            
            float energyMcPhoton  = 0.f;
            float energyMcNeutral = 0.f;
            float energyMcCharged = 0.f;

            const OrderedCaloHitList& pOrderedCaloHitList = pCluster->GetOrderedCaloHitList();
            for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList.begin(), itLyrEnd = pOrderedCaloHitList.end(); itLyr != itLyrEnd; itLyr++ )
            {
                // int pseudoLayer = itLyr->first;
                CaloHitList::iterator itCaloHit    = itLyr->second->begin();
                CaloHitList::iterator itCaloHitEnd = itLyr->second->end();

                for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
                {
                    CaloHit* pCaloHit = (*itCaloHit);

                    // fetch the MCParticle
                    const MCParticle* mc = NULL; 
                    pCaloHit->GetMCParticle( mc );

                    if( mc == NULL ) continue; // has to be continue, since sometimes some CalorimeterHits don't have a MCParticle (e.g. noise)

                    const int particleId = mc->GetParticleId();
                    bool mcIsNeutral = ((particleId == K_LONG) || (particleId == NEUTRON));
                    bool mcIsPhoton  = (particleId == PHOTON);
 
//                     EnergyMixing& energyMixing = (mcIsPhoton? &truePhotons : (mcIsNeutral? &trueNeutral : (mcIsEm? &trueChargedEm : &trueCharged ) ) );
                    EnergyMixing& energyMixing = (mcIsPhoton? truePhotons : (mcIsNeutral? trueNeutral : trueCharged ) );

                    energyMixing.AddMcParticle( mc );

                    float electromagneticEnergy = pCaloHit->GetElectromagneticEnergy();
                    if( clusterHasTracks )
                    {
                        energyMixing.AddChargedClusterCalorimetricEnergy( electromagneticEnergy );
                    }
                    else if( clusterIsPhoton )
                    {
                        energyMixing.AddPhotonClusterCalorimetricEnergy( electromagneticEnergy );
                    }
                    else
                    {
                        energyMixing.AddNeutralClusterCalorimetricEnergy( electromagneticEnergy );
                    }

                    if( mcIsPhoton )
                        energyMcPhoton  += electromagneticEnergy;
                    else if( mcIsNeutral )
                        energyMcNeutral += electromagneticEnergy;
                    else 
                        energyMcCharged += electromagneticEnergy;
                    
                }
            }

            // add number of clusters
            EnergyMixing& energyMixingQuantity = (energyMcPhoton>energyMcNeutral? 
                                                  (energyMcPhoton>energyMcCharged? truePhotons : trueCharged) : 
                                                  (energyMcNeutral>energyMcCharged? trueNeutral : trueCharged ) );

            if( clusterHasTracks )
                energyMixingQuantity.AddChargedCluster();
            else if( clusterIsPhoton )
                energyMixingQuantity.AddPhotonCluster();
            else
                energyMixingQuantity.AddNeutralCluster();
                    

            // now for the tracks
            for( TrackList::const_iterator itTrack = pTrackList.begin(), itTrackEnd = pTrackList.end(); itTrack != itTrackEnd; ++itTrack )
            {
                Track* pTrack = (*itTrack);
                
                ++trackNumber;

                const MCParticle* mc = NULL;
                pTrack->GetMCParticle( mc );
                if( mc == NULL ) continue; // maybe an error should be thrown here?

                const int particleId = mc->GetParticleId();
                bool mcIsNeutral = ((particleId == K_LONG) || (particleId == NEUTRON));
                bool mcIsPhoton  = (particleId == PHOTON);

//                     EnergyMixing& energyMixing = (mcIsPhoton? &truePhotons : (mcIsNeutral? &trueNeutral : (mcIsEm? &trueChargedEm : &trueCharged ) ) );
                EnergyMixing& energyMixing = (mcIsPhoton? truePhotons : (mcIsNeutral? trueNeutral : trueCharged ) );

                energyMixing.AddMcParticle( mc );

                energyMixing.AddChargedClusterTracksEnergy( pTrack->GetEnergyAtDca() );
                energyMixing.AddTrack();
                
            }
        }
    }


    return MonitoringOutput( trueCharged, trueNeutral, truePhotons, clusterNumber, trackNumber );
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "ClusterListNames", m_clusterListNames));

    if( m_clusterListNames.empty() )
        return STATUS_CODE_NOT_INITIALIZED;

    m_clusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ShowCurrentClusters", m_clusters));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "MonitoringFileName", m_monitoringFileName));

    m_treeName = "emon";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "TreeName", m_treeName));
    m_print = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "Print", m_print));

    m_quantity = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "Quantity", m_quantity));
    
    return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyMonitoringAlgorithm::MonitoringOutput( EnergyMixing& trueCharged, EnergyMixing& trueNeutral, EnergyMixing& truePhotons, int numberClusters, int numberTracks )
{

    // get energies
    float trueCharged_recoChargedCalo   = trueCharged.GetChargedClusterCalorimetricEnergy();
    float trueCharged_recoNeutralCalo   = trueCharged.GetNeutralClusterCalorimetricEnergy();
    float trueCharged_recoPhotonCalo    = trueCharged.GetPhotonClusterCalorimetricEnergy();
    float trueCharged_recoChargedTracks = trueCharged.GetChargedClusterTracksEnergy();
    float trueCharged_mc                = trueCharged.GetMcParticleSetEnergy();

    float trueNeutral_recoChargedCalo   = trueNeutral.GetChargedClusterCalorimetricEnergy();
    float trueNeutral_recoNeutralCalo   = trueNeutral.GetNeutralClusterCalorimetricEnergy();
    float trueNeutral_recoPhotonCalo    = trueNeutral.GetPhotonClusterCalorimetricEnergy();
    float trueNeutral_recoChargedTracks = trueNeutral.GetChargedClusterTracksEnergy();
    float trueNeutral_mc                = trueNeutral.GetMcParticleSetEnergy();
    
    float truePhotons_recoChargedCalo   = truePhotons.GetChargedClusterCalorimetricEnergy();
    float truePhotons_recoNeutralCalo   = truePhotons.GetNeutralClusterCalorimetricEnergy();
    float truePhotons_recoPhotonCalo    = truePhotons.GetPhotonClusterCalorimetricEnergy();
    float truePhotons_recoChargedTracks = truePhotons.GetChargedClusterTracksEnergy();
    float truePhotons_mc                = truePhotons.GetMcParticleSetEnergy();


    // get quantity
    int trueCharged_recoChargedCaloClusters   = trueCharged.GetChargedClusterCalorimetricClusters();
    int trueCharged_recoNeutralCaloClusters   = trueCharged.GetNeutralClusterCalorimetricClusters();
    int trueCharged_recoPhotonCaloClusters    = trueCharged.GetPhotonClusterCalorimetricClusters();
    int trueCharged_recoChargedTracksNumber   = trueCharged.GetChargedClusterTracks();
    int trueCharged_mcNumber                  = trueCharged.GetMcParticleNumber();

    int trueNeutral_recoChargedCaloClusters   = trueNeutral.GetChargedClusterCalorimetricClusters();
    int trueNeutral_recoNeutralCaloClusters   = trueNeutral.GetNeutralClusterCalorimetricClusters();
    int trueNeutral_recoPhotonCaloClusters    = trueNeutral.GetPhotonClusterCalorimetricClusters();
    int trueNeutral_recoChargedTracksNumber   = trueNeutral.GetChargedClusterTracks();
    int trueNeutral_mcNumber                  = trueNeutral.GetMcParticleNumber();
    
    int truePhotons_recoChargedCaloClusters   = truePhotons.GetChargedClusterCalorimetricClusters();
    int truePhotons_recoNeutralCaloClusters   = truePhotons.GetNeutralClusterCalorimetricClusters();
    int truePhotons_recoPhotonCaloClusters    = truePhotons.GetPhotonClusterCalorimetricClusters();
    int truePhotons_recoChargedTracksNumber   = truePhotons.GetChargedClusterTracks();
    int truePhotons_mcNumber                  = truePhotons.GetMcParticleNumber();



    if( m_print )
    {
        std::cout << "cluster list names : ";
        for( pandora::StringVector::iterator itClusterName = m_clusterListNames.begin(), itClusterNameEnd = m_clusterListNames.end(); itClusterName != itClusterNameEnd; ++itClusterName )
        {
            std::cout << (*itClusterName) << " " << std::flush;
        }
        std::cout << std::endl;
        std::cout << "number of clusters : " << numberClusters << std::endl;
        std::cout << "number of tracks   : " << numberTracks << std::endl;

        std::ios_base::fmtflags original_flags = std::cout.flags();   // store the original flags

        static const int precision = 1;
        static const int width = 12;
        static const int widthNum = 3;
        
        std::cout << std::fixed;
        std::cout << std::setprecision(precision);

#define FORMATTEDOUTPUTSHORT(TITLE,E1,E2,E3) std::cout << std::left << std::setw(width) << TITLE \
                                                       << std::right << std::setw(widthNum) << " " << std::setw(width) << E1 \
                                                       << std::right << std::setw(widthNum) << " " << std::setw(width) << E2 \
                                                       << std::right << std::setw(widthNum) << " " << std::setw(width) << E3 << std::endl
#define FORMATTEDOUTPUTLONG(TITLE,E1,N1,E2,N2,E3,N3) std::cout << std::setw(width) << std::left << TITLE \
                                                               << std::right << std::setw(width) << E1 << "/" << std::left << std::setw(widthNum) << N1 \
                                                               << std::right << std::setw(width) << E2 << "/" << std::left << std::setw(widthNum) << N2 \
                                                               << std::right << std::setw(width) << E3 << "/" << std::left << std::setw(widthNum) << N3 << std::endl

#define FORMATTEDOUTPUT(TITLE,E1,E2,E3) std::cout << std::left << std::setw(width) << TITLE \
                                                  << std::right << std::setw(width) << E1 \
                                                  << std::right << std::setw(width) << E2 \
                                                  << std::right << std::setw(width) << E3 << std::endl

        if( m_quantity )
        {

            FORMATTEDOUTPUTSHORT( "           ", "true +-", "true 0", "true phot" );
            FORMATTEDOUTPUTLONG( "true     : ", trueCharged_mc, trueCharged_mcNumber, trueNeutral_mc,trueNeutral_mcNumber, truePhotons_mc, truePhotons_mcNumber );
            FORMATTEDOUTPUTSHORT( "--- ", "--- ", "--- ", "--- " );
            FORMATTEDOUTPUTLONG( "+- calo  : ", \
                                 trueCharged_recoChargedCalo, trueCharged_recoChargedCaloClusters, \
                                 trueNeutral_recoChargedCalo, trueNeutral_recoChargedCaloClusters, \
                                 truePhotons_recoChargedCalo, truePhotons_recoChargedCaloClusters );
            FORMATTEDOUTPUTLONG( "+- tracks: ", \
                                 trueCharged_recoChargedTracks, trueCharged_recoChargedTracksNumber, \
                                 trueNeutral_recoChargedTracks, trueNeutral_recoChargedTracksNumber, \
                                 truePhotons_recoChargedTracks, truePhotons_recoChargedTracksNumber );
            FORMATTEDOUTPUTLONG( "0  calo  : ", \
                                 trueCharged_recoNeutralCalo, trueCharged_recoNeutralCaloClusters, \
                                 trueNeutral_recoNeutralCalo, trueNeutral_recoNeutralCaloClusters, \
                                 truePhotons_recoNeutralCalo, truePhotons_recoNeutralCaloClusters );
            FORMATTEDOUTPUTLONG( "phot     : ", \
                                 trueCharged_recoPhotonCalo, trueCharged_recoPhotonCaloClusters, \
                                 trueNeutral_recoPhotonCalo, trueNeutral_recoPhotonCaloClusters, \
                                 truePhotons_recoPhotonCalo, truePhotons_recoPhotonCaloClusters );
        }
        else
        {

            FORMATTEDOUTPUT( "           ", "true +-", "true 0", "true phot" );
            FORMATTEDOUTPUT( "true     : ", trueCharged_mc, trueNeutral_mc, truePhotons_mc );
            FORMATTEDOUTPUT( "---", "---", "---", "---" );
            FORMATTEDOUTPUT( "+- calo  : ", \
                             trueCharged_recoChargedCalo, \
                             trueNeutral_recoChargedCalo, \
                             truePhotons_recoChargedCalo );
            FORMATTEDOUTPUT( "+- tracks: ", \
                             trueCharged_recoChargedTracks, \
                             trueNeutral_recoChargedTracks, \
                             truePhotons_recoChargedTracks );
            FORMATTEDOUTPUT( "0  calo  : ", \
                             trueCharged_recoNeutralCalo, \
                             trueNeutral_recoNeutralCalo, \
                             truePhotons_recoNeutralCalo );
            FORMATTEDOUTPUT( "phot     : ", \
                             trueCharged_recoPhotonCalo, \
                             trueNeutral_recoPhotonCalo, \
                             truePhotons_recoPhotonCalo );
        }

        std::cout.flags(original_flags);                              // restore the flags

    }

    if( !(m_monitoringFileName.empty()) && !(m_treeName.empty()) )
    {
        if( m_print )
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

        if( m_quantity )
        {
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Np2c", truePhotons_recoChargedCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Np2n", truePhotons_recoNeutralCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Np2p", truePhotons_recoPhotonCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Np2t", truePhotons_recoChargedTracksNumber ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "NpMc", truePhotons_mcNumber ));

            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nc2c", trueCharged_recoChargedCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nc2n", trueCharged_recoNeutralCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nc2p", trueCharged_recoPhotonCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nc2t", trueCharged_recoChargedTracksNumber ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "NcMc", trueCharged_mcNumber ));

            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nn2c", trueNeutral_recoChargedCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nn2n", trueNeutral_recoNeutralCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nn2p", trueNeutral_recoPhotonCaloClusters ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "Nn2t", trueNeutral_recoChargedTracksNumber ));
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "NnMc", trueNeutral_mcNumber ));
        }

//        PANDORA_MONITORING_API(PrintTree(m_treeName));
        PANDORA_MONITORING_API(FillTree(m_treeName));
    }

    return STATUS_CODE_SUCCESS;
}




