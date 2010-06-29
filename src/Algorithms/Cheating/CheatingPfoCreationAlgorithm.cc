/**
 *  @file   PandoraPFANew/src/Algorithms/CheatingCheatingPfoCreationAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating pfo creation algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/CheatingPfoCreationAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode CheatingPfoCreationAlgorithm::Run()
{
    // Run initial clustering algorithm
    const ClusterList *pClusterList = NULL;

    if(!m_clusteringAlgorithmName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));
    }

    if(!m_inputClusterListName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, m_inputClusterListName, pClusterList));
    }

    if(m_clusteringAlgorithmName.empty() && m_inputClusterListName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));
    }

    double energySum(0.);
    CartesianVector momentumSum(0., 0., 0.);

    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster)
    {
        CartesianVector momentum(0., 0., 0.);
        float  mass = 0.;
        int    particleId = PI_PLUS;
        int    charge = 0;
        float  energy = 0.;

        if (m_energyFrom == "MC")
        {
            ComputeFromMC((*itCluster), energy, momentum, mass, particleId, charge);
        }
        else if (m_energyFrom == "calorimeter")
        {
            ComputeFromCalorimeter((*itCluster), energy, momentum, mass, particleId, charge);
        }
        else if (m_energyFrom == "tracks")
        {
            ComputeFromTracks((*itCluster), energy, momentum, mass, particleId, charge);
        }
        else if (m_energyFrom == "calorimeterAndTracks")
        {
            ComputeFromCalorimeterAndTracks((*itCluster), energy, momentum, mass, particleId, charge);
        }
        else
        {
            return STATUS_CODE_INVALID_PARAMETER;
        }

        PandoraContentApi::ParticleFlowObjectParameters particleFlowObjectParameters;

        // Insert cluster into pfo
        particleFlowObjectParameters.m_clusterList.insert((*itCluster));

        // Insert tracks into pfo
        const TrackList &trackList = (*itCluster)->GetAssociatedTrackList();

        for (TrackList::const_iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack)
        {
            particleFlowObjectParameters.m_trackList.insert((*itTrack));
        }

        // Set energy, charge, mass, momentum, particleId
        particleFlowObjectParameters.m_energy = energy;
        particleFlowObjectParameters.m_charge = charge;
        particleFlowObjectParameters.m_mass = mass;
        particleFlowObjectParameters.m_momentum = momentum;
        particleFlowObjectParameters.m_particleId = particleId;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, particleFlowObjectParameters));

        energySum += energy;
        momentumSum = momentumSum + momentum;
    }

    double pT(std::sqrt(momentumSum.GetX() * momentumSum.GetX() + momentumSum.GetY() * momentumSum.GetY()));

    if (m_debug)
    {
        std::cout << "energySum " << energySum << ", pT " << pT << std::endl;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeEnergyWeightedClusterPosition(Cluster *pCluster, CartesianVector &energyWeightedClusterPosition) const
{
    energyWeightedClusterPosition.SetValues(0., 0., 0.);
    float energySum(0.);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            float hitEnergy = (*hitIter)->GetElectromagneticEnergy(); 
            energySum += hitEnergy;

            energyWeightedClusterPosition += (*hitIter)->GetPositionVector() * hitEnergy;
        }
    }

    if (0. == energySum)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    energyWeightedClusterPosition *= (1. / energySum);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromCalorimeter(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge) const
{
    const TrackList &trackList(pCluster->GetAssociatedTrackList());

    if (trackList.empty())
    {
        if (pCluster->IsPhoton())
        {
            energy = pCluster->GetElectromagneticEnergy();
            momentum = pCluster->GetFitToAllHitsResult().GetDirection();
            momentum = momentum * energy;
            particleId = PHOTON;
            mass = GetParticleMass(PHOTON);
            charge = GetParticleCharge(PHOTON);
        }
        else
        {
            energy = pCluster->GetHadronicEnergy();
            particleId = NEUTRON;
            mass = GetParticleMass(NEUTRON);
            charge = GetParticleCharge(NEUTRON);

            CartesianVector energyWeightedClusterPosition(0., 0., 0.);
            ComputeEnergyWeightedClusterPosition(pCluster, energyWeightedClusterPosition);

            CartesianVector clusterMomentum(energyWeightedClusterPosition * pCluster->GetHadronicEnergy());
            const float totalGravity(energyWeightedClusterPosition.GetMagnitude());

            if (0. == totalGravity)
                throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

            momentum = clusterMomentum * (1. / totalGravity);
        }
    }
    else
    {
        energy = pCluster->GetHadronicEnergy();
        particleId = PI_PLUS;
        mass = 0.1396f;
        charge = 1;

        CartesianVector energyWeightedClusterPosition(0., 0., 0.);
        ComputeEnergyWeightedClusterPosition(pCluster, energyWeightedClusterPosition);

        CartesianVector clusterMomentum(energyWeightedClusterPosition * pCluster->GetHadronicEnergy());
        const float totalGravity(energyWeightedClusterPosition.GetMagnitude());

        if (0. == totalGravity)
            throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

        momentum = clusterMomentum * (1. / totalGravity);
    }

    if(m_debug)
    {
        std::cout << "energy from calo: " << energy << std::endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromMC(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge) const
{
    // Match CaloHitVectors to their MCParticles
    std::map< const MCParticle*, float > energyPerMCParticle;
    std::map< const MCParticle*, float >::iterator itEnergyPerMCParticle;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const MCParticle *pMCParticle = NULL; 
            (*hitIter)->GetMCParticle(pMCParticle);

             // Some CalorimeterHits don't have a MCParticle (e.g. noise)
            if (NULL == pMCParticle)
                continue;

            const float energy((*hitIter)->GetInputEnergy());

            itEnergyPerMCParticle = energyPerMCParticle.find(pMCParticle);

            if (energyPerMCParticle.end() == itEnergyPerMCParticle)
            {
                energyPerMCParticle.insert(std::make_pair(pMCParticle, energy));
            }
            else
            {
                itEnergyPerMCParticle->second += energy;
            }
        }
    }

    std::map<const MCParticle*, float>::const_iterator it = max_element( energyPerMCParticle.begin(), energyPerMCParticle.end(), 
         pandora::Select2nd<std::map<const MCParticle*, float>::value_type, std::greater<float> >() );

    const MCParticle *pMCParticle = it->first;
    energy = pMCParticle->GetEnergy();
    particleId = pMCParticle->GetParticleId();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromTracks(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge) const
{
    const TrackList &trackList(pCluster->GetAssociatedTrackList());

    for (TrackList::const_iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack)
    {
        mass += (*itTrack)->GetMass();
        energy += (*itTrack)->GetEnergyAtDca();
        particleId = PI_PLUS;
        charge += (*itTrack)->GetChargeSign();
        momentum += (*itTrack)->GetMomentumAtDca();
    }

    if (m_debug)
    {
        std::cout << "energy from tracks " << energy << std::endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromCalorimeterAndTracks(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge) const
{
    if (pCluster->GetAssociatedTrackList().empty())
    {
        ComputeFromCalorimeter(pCluster, energy, momentum, mass, particleId, charge);
    }
    else
    {
        ComputeFromTracks(pCluster, energy, momentum, mass, particleId, charge);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingPfoCreationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
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

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle,
        m_clusteringAlgorithmName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InputClusterListName", m_inputClusterListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyFrom", m_energyFrom));

    m_debug   = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Debug", m_debug));


    return STATUS_CODE_SUCCESS;
}
