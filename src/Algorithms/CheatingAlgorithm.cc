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
#include <cmath>

using namespace pandora;

StatusCode CheatingAlgorithm::Run()
{
    // Run initial clustering algorithm
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));

//     // create PFOs
//     PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    double energySum = 0.;
    CartesianVector momentumSum(0, 0, 0);
    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; itCluster++)
    {
//        std::cout << " cluster " << std::endl;
        CartesianVector momentum(0, 0, 0);
        double mass = 0.0;
        int particleId = 211;
        int charge = 0;

        PandoraContentApi::ParticleFlowObject::Parameters pfo;
        pfo.m_clusterList.insert((*itCluster));
        
        TrackList trackList = (*itCluster)->GetAssociatedTrackList();
        for (TrackList::iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack)
        {
            pfo.m_trackList.insert((*itTrack));
            momentum = momentum + (*itTrack)->GetMomentumAtDca();
        }

        double energy = 0.0;
        if (m_energyFrom == "MC")
        {
            energy = (*itCluster)->GetBestEnergyEstimate();
        }
        else if (m_energyFrom == "calorimeter")
        {
            if (trackList.empty())
            {
                if ((*itCluster)->IsPhoton())
                {
                    energy = (*itCluster)->GetElectromagneticEnergy();
                    momentum = (*itCluster)->GetFitToAllHitsResult().GetDirection();
                    momentum = momentum * energy;
                    particleId = 22;
                    mass += 0.0;
                }
                else
                {
                    energy = (*itCluster)->GetHadronicEnergy();
                    particleId = 2112;
                    mass += 0.9396;
                    momentum = (*itCluster)->GetFitToAllHitsResult().GetDirection() * std::sqrt( energy*energy - mass*mass );
                }
            }
        }
        else if (m_energyFrom == "tracks")
        {
            int num = 0;
            for (TrackList::iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack)
            {
                // take the momentum from the track and assume the mass of a pion
                CartesianVector momentumVec = (*itTrack)->GetMomentumAtDca();
                double momentum = momentumVec.GetMagnitude();
                double trackMass = 0.139;
                mass += trackMass;
                energy += std::sqrt(momentum*momentum+trackMass*trackMass);
                particleId = 211;
//                charge <== get charge from track

//                 MCParticle *mc = NULL;
//                 (*itTrack)->GetMCParticle( mc );
//                std::cout << "track number " << num << " energy " << std::sqrt(momentum*momentum+mass*mass) << "  mc->energy " << mc->GetEnergy() << " mc->momentum " << mc->GetMomentum() << " momentum " << momentum << std::endl;
                ++num;
            }
        }
        else
        {
            return STATUS_CODE_INVALID_PARAMETER;
        }

//          std::cout << "energy " << energy << std::endl;
        pfo.m_energy = static_cast<float>(energy);
        pfo.m_chargeSign = charge;
        pfo.m_mass = static_cast<float>(mass);
        pfo.m_momentum = momentum;
        pfo.m_particleId = particleId;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfo));
        energySum += energy;
        momentumSum = momentumSum + momentum;
    }

//    double pt = std::sqrt(momentumSum.GetX() * momentumSum.GetX() + momentumSum.GetY() * momentumSum.GetY());
//    std::cout << "energySum " << energySum << "  pt " << pt << std::endl;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
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
