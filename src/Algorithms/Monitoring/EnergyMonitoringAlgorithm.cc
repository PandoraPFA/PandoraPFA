/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/EnergyMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Monitoring/EnergyMonitoringAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

#include <iostream>
#include <iomanip>

using namespace pandora;

#define K0L      130
#define K0S      310
#define NEUTRON  2112
#define PHOTON   22
#define ELECTRON 11
#define POSITRON -11


//------------------------------------------------------------------------------------------------------------------------------------------

EnergyMonitoringAlgorithm::EnergyMixing::EnergyMixing()
    : m_chargedCalorimetric(0),
      m_chargedTracks(0),
      m_neutralCalorimetric(0),
      m_photonCalorimetric(0)
{
    m_mcParticleSet.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

float EnergyMonitoringAlgorithm::EnergyMixing::GetMcParticleSetEnergy()
{
    MCPARTICLESET& mcParticleSet = GetMcParticleSet();
    
    float energy = 0.f;
    for( MCPARTICLESET::iterator itMc = mcParticleSet.begin(), itMcEnd = mcParticleSet.end(); itMc != itMcEnd; ++itMc )
    {
        const pandora::MCParticle *const mc = (*itMc);
        energy += mc->GetEnergy();
    }
    
    return energy;
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
    typedef std::vector<const ClusterList*> CLUSTERVECTOR;
    CLUSTERVECTOR clusterListVector;

    for( STRINGVECTOR::iterator itClusterName = m_clusterListNames.begin(), itClusterNameEnd = m_clusterListNames.end(); itClusterName != itClusterNameEnd; ++itClusterName )
    {
        const ClusterList* pClusterList = NULL;
        if( STATUS_CODE_SUCCESS == PandoraContentApi::GetClusterList(*this, (*itClusterName), pClusterList))
            clusterListVector.push_back( pClusterList ); // add the cluster list
    }
    
    EnergyMixing trueCharged;
//     EnergyMixing trueChargedEm;
    EnergyMixing trueNeutral;
    EnergyMixing truePhotons;
    

    for( CLUSTERVECTOR::iterator itClusterList = clusterListVector.begin(), itClusterListEnd = clusterListVector.end(); itClusterList != itClusterListEnd; ++itClusterList )
    {
        const ClusterList* pClusterList = (*itClusterList);
        for( ClusterList::iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
        {
            const Cluster* pCluster = (*itCluster);

            const TrackList& pTrackList = pCluster->GetAssociatedTrackList();
            bool clusterHasTracks = !(pTrackList.empty());
            bool clusterIsPhoton  = pCluster->IsPhoton();
            
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

                    bool mcIsNeutral = (mc->GetParticleId() == K0L || 
//                                        mc->GetParticleId() == K0S ||
                                        mc->GetParticleId() == NEUTRON ); 
//                     bool mcIsEm      = (mc->GetParticleId() == ELECTRON || 
//                                         mc->GetParticleId() == POSITRON);
                    bool mcIsPhoton  = (mc->GetParticleId() == PHOTON );
 
//                     EnergyMixing& energyMixing = (mcIsPhoton? &truePhotons : (mcIsNeutral? &trueNeutral : (mcIsEm? &trueChargedEm : &trueCharged ) ) );
                    EnergyMixing& energyMixing = (mcIsPhoton? truePhotons : (mcIsNeutral? trueNeutral : trueCharged ) );

                    energyMixing.AddMcParticle( mc );

                    if( clusterHasTracks )
                    {
                        energyMixing.AddChargedClusterCalorimetricEnergy( pCaloHit->GetElectromagneticEnergy() );
                    }
                    else if( clusterIsPhoton )
                    {
                        energyMixing.AddPhotonClusterCalorimetricEnergy( pCaloHit->GetElectromagneticEnergy() );
                    }
                    else
                    {
                        energyMixing.AddNeutralClusterCalorimetricEnergy( pCaloHit->GetElectromagneticEnergy() );
                    }

                    
                }
            }
            
            // now for the tracks
            for( TrackList::const_iterator itTrack = pTrackList.begin(), itTrackEnd = pTrackList.end(); itTrack != itTrackEnd; ++itTrack )
            {
                Track* pTrack = (*itTrack);
                const MCParticle* mc = NULL;
                pTrack->GetMCParticle( mc );
                if( mc == NULL ) continue; // maybe an error should be thrown here?

                bool mcIsNeutral = (mc->GetParticleId() == K0L || 
//                                    mc->GetParticleId() == K0S ||
                                    mc->GetParticleId() == NEUTRON ); 
//                 bool mcIsEm      = (mc->GetParticleId() == ELECTRON || 
//                                     mc->GetParticleId() == POSITRON);
                bool mcIsPhoton  = (mc->GetParticleId() == PHOTON );
 
//                     EnergyMixing& energyMixing = (mcIsPhoton? &truePhotons : (mcIsNeutral? &trueNeutral : (mcIsEm? &trueChargedEm : &trueCharged ) ) );
                EnergyMixing& energyMixing = (mcIsPhoton? truePhotons : (mcIsNeutral? trueNeutral : trueCharged ) );

                energyMixing.AddMcParticle( mc );
                
                energyMixing.AddChargedClusterTracksEnergy( pTrack->GetEnergyAtDca() );
                
            }
        }
    }

    return MonitoringOutput( trueCharged, trueNeutral, truePhotons );
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "ClusterListNames", m_clusterListNames));

    if( m_clusterListNames.empty() )
        return STATUS_CODE_NOT_INITIALIZED;

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "MonitoringFileName", m_monitoringFileName));

    m_treeName = "emon";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "TreeName", m_treeName));
    m_print = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "Print", m_print));
    
    return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyMonitoringAlgorithm::MonitoringOutput( EnergyMixing& trueCharged, EnergyMixing& trueNeutral, EnergyMixing& truePhotons )
{

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

    if( m_print )
    {
        std::cout << "cluster list names : ";
        for( STRINGVECTOR::iterator itClusterName = m_clusterListNames.begin(), itClusterNameEnd = m_clusterListNames.end(); itClusterName != itClusterNameEnd; ++itClusterName )
        {
            std::cout << (*itClusterName) << " " << std::flush;
        }
        std::cout << std::endl;

        #define width 12
        #define precision 1

        std::ios_base::fmtflags original_flags = std::cout.flags();   // store the original flags

        std::cout << std::fixed;
        std::cout << std::setw(width) << " " << std::setw(width) << "true +-" << std::setw(width) << "true 0" << std::setw(width) << "true phot" << std::endl;
        std::cout << std::setprecision(precision);
        std::cout << std::setw(width) << "true     : " << std::setw(width) << trueCharged_mc << std::setw(width) 
                  << trueNeutral_mc << std::setw(width) << truePhotons_mc << std::endl;
        std::cout << std::setw(width) << "---" << std::setw(width) << "---" << std::setw(width) << "---" << std::setw(width) << "---" << std::endl;
        std::cout << std::setw(width) << "+- calo  : " << std::setw(width) << trueCharged_recoChargedCalo << std::setw(width) 
                  << trueNeutral_recoChargedCalo << std::setw(width) << truePhotons_recoChargedCalo << std::endl;
        std::cout << std::setw(width) << "+- tracks: " << std::setw(width) << trueCharged_recoChargedTracks << std::setw(width) 
                  << trueNeutral_recoChargedTracks << std::setw(width) << truePhotons_recoChargedTracks << std::endl;
        std::cout << std::setw(width) << "0  calo  : " << std::setw(width) << trueCharged_recoNeutralCalo << std::setw(width) 
                  << trueNeutral_recoNeutralCalo << std::setw(width) << truePhotons_recoNeutralCalo << std::endl;
        std::cout << std::setw(width) << "phot     : " << std::setw(width) << trueCharged_recoPhotonCalo << std::setw(width) 
                  << trueNeutral_recoPhotonCalo << std::setw(width) << truePhotons_recoPhotonCalo << std::endl;

        std::cout.flags(original_flags);                              // restore the flags

    }

    if( !(m_monitoringFileName.empty()) && !(m_treeName.empty()) )
    {
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ch2ch", trueCharged_recoChargedCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ch2ne", trueCharged_recoNeutralCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ch2ph", trueCharged_recoPhotonCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ch2tr", trueCharged_recoChargedTracks ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "chMc",  trueCharged_mc ));

        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ne2ch", trueNeutral_recoChargedCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ne2ne",  trueNeutral_recoNeutralCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ne2ph", trueNeutral_recoPhotonCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ne2tr", trueNeutral_recoChargedTracks ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "neMc",  trueNeutral_mc ));

        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ph2ch", truePhotons_recoChargedCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ph2ne",  truePhotons_recoNeutralCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ph2ph", truePhotons_recoPhotonCalo ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ph2tr", truePhotons_recoChargedTracks ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "phMc",  truePhotons_mc ));

//        PANDORA_MONITORING_API(PrintTree(m_treeName));
        PANDORA_MONITORING_API(FillTree(m_treeName));
    }

    return STATUS_CODE_SUCCESS;
}




