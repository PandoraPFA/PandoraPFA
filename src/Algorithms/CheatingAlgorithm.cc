/**
 *  @file   PandoraPFANew/src/Algorithms/CheatingAlgorithm.cc
 * 
 *  @brief  Implementation of the clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/CheatingAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

#include <sstream>
#include <math.h>

using namespace pandora;

StatusCode CheatingAlgorithm::Run()
{
    // Run initial clustering algorithm
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));

    // create PFOs
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    double energySum = 0.0;
    for( ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; itCluster++ )
    {
        PandoraContentApi::ParticleFlowObject::Parameters pfo;
        pfo.m_clusterList.insert( (*itCluster) );
        
        TrackList trackList = (*itCluster)->GetAssociatedTrackList();
        for( TrackList::iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack )
        {
            pfo.m_trackList.insert( (*itTrack) );
        }

        double energy = 0.0;
        if( m_energyFrom == "MC" )
        {
            energy = (*itCluster)->GetBestEnergyEstimate();
        }
        else if( m_energyFrom == "calorimeter" )
        {
            energy = (*itCluster)->GetHadronicEnergy();
        }
        else if( m_energyFrom == "tracks" )
        {
            int num = 0;
            for( TrackList::iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack )
            {
                // take the momentum from the track and assume the mass of a pion
                CartesianVector momentumVec = (*itTrack)->GetMomentumAtDca();
                double momentum = momentumVec.GetMagnitude();
                double mass = 0.139;
                energy += sqrt(momentum*momentum+mass*mass);
//                 MCParticle *mc = NULL;
//                 (*itTrack)->GetMCParticle( mc );
//                std::cout << "track number " << num << " energy " << sqrt(momentum*momentum+mass*mass) << "  mc->energy " << mc->GetEnergy() << " mc->momentum " << mc->GetMomentum() << " momentum " << momentum << std::endl;
                ++num;
            }
        }
        else
        {
            return STATUS_CODE_FAILURE; // unknown parameter
        }
//        std::cout << "energy " << energy << std::endl;
        pfo.m_energy = energy; 
        pfo.m_chargeSign = 0;
        pfo.m_mass = 10;
        pfo.m_momentum = CartesianVector( 10, 20, 30);
        pfo.m_particleId = 211;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfo));
        energySum += energy;
    }
//    std::cout << "energySum " << energySum << std::endl;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
//     settings example:
//
//     <algorithm type = "Cheating">
//         <algorithm type = "PerfectClustering" description = "Clustering">
//         </algorithm>
//         <energyFrom> tracks </energyFrom>
//         <clusterListName> CheatedClusterList </clusterListName>
//     </algorithm> 
//
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_clusteringAlgorithmName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "clusterListName", m_cheatingListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "energyFrom", m_energyFrom));

    return STATUS_CODE_SUCCESS;
}
