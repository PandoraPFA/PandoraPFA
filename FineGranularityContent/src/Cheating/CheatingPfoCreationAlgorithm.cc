/**
 *  @file   PandoraPFANew/FineGranularityContent/src/CheatingCheatingPfoCreationAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating pfo creation algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Cheating/CheatingPfoCreationAlgorithm.h"

using namespace pandora;

StatusCode CheatingPfoCreationAlgorithm::Run()
{
    // Run initial clustering algorithm
    const ClusterList *pClusterList = NULL;

    if (!m_clusteringAlgorithmName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));
    }
    else if (!m_inputClusterListName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, m_inputClusterListName, pClusterList));
    }
    else if (m_clusteringAlgorithmName.empty() && m_inputClusterListName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));
    }

    float energySum(0.f);
    CartesianVector momentumSum(0.f, 0.f, 0.f);

    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster)
    {
        CartesianVector momentum(0.f, 0.f, 0.f);
        float mass = 0.f;
        int charge = 0;
        float energy = 0.f;
        int particleId = PI_PLUS;

        if (m_energyFrom == "MC")
        {
            this->ComputeFromMC((*itCluster), energy, momentum, mass, particleId, charge);
        }
        else if (m_energyFrom == "Calorimeter")
        {
            this->ComputeFromCalorimeter((*itCluster), energy, momentum, mass, particleId, charge);
        }
        else if (m_energyFrom == "Tracks")
        {
            this->ComputeFromTracks((*itCluster), energy, momentum, mass, particleId, charge);
        }
        else if (m_energyFrom == "CalorimeterAndTracks")
        {
            this->ComputeFromCalorimeterAndTracks((*itCluster), energy, momentum, mass, particleId, charge);
        }
        else
        {
            return STATUS_CODE_INVALID_PARAMETER;
        }

        PandoraContentApi::ParticleFlowObjectParameters particleFlowObjectParameters;

        // Insert cluster into pfo
        particleFlowObjectParameters.m_clusterList.insert((*itCluster));

        // Insert tracks into pfo
        const TrackList &associatedTrackList((*itCluster)->GetAssociatedTrackList());
        particleFlowObjectParameters.m_trackList.insert(associatedTrackList.begin(), associatedTrackList.end());

        // Set energy, charge, mass, momentum, particleId
        particleFlowObjectParameters.m_energy = energy;
        particleFlowObjectParameters.m_charge = charge;
        particleFlowObjectParameters.m_mass = mass;
        particleFlowObjectParameters.m_momentum = momentum;
        particleFlowObjectParameters.m_particleId = particleId;

        energySum += energy;
        momentumSum = momentumSum + momentum;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, particleFlowObjectParameters));
    }

    const float pT(std::sqrt(momentumSum.GetX() * momentumSum.GetX() + momentumSum.GetY() * momentumSum.GetY()));

    if (m_debug)
    {
        std::cout << "energySum " << energySum << ", pT " << pT << std::endl;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromCalorimeter(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge) const
{
    const TrackList &trackList(pCluster->GetAssociatedTrackList());

    if (trackList.empty())
    {
        if (pCluster->IsFixedPhoton())
        {
            energy = pCluster->GetElectromagneticEnergy();
            momentum = pCluster->GetFitToAllHitsResult().GetDirection();
            momentum = momentum * energy;
            particleId = PHOTON;
            mass = PdgTable::GetParticleMass(PHOTON);
            charge = PdgTable::GetParticleCharge(PHOTON);
        }
        else
        {
            energy = pCluster->GetHadronicEnergy();
            particleId = NEUTRON;
            mass = PdgTable::GetParticleMass(NEUTRON);
            charge = PdgTable::GetParticleCharge(NEUTRON);

            CartesianVector energyWeightedClusterPosition(0.f, 0.f, 0.f);
            this->ComputeEnergyWeightedClusterPosition(pCluster, energyWeightedClusterPosition);

            const CartesianVector clusterMomentum(energyWeightedClusterPosition * pCluster->GetHadronicEnergy());
            const float totalGravity(energyWeightedClusterPosition.GetMagnitude());

            if (0.f == totalGravity)
                throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

            momentum = clusterMomentum * (1. / totalGravity);
        }
    }
    else
    {
        energy = pCluster->GetHadronicEnergy();
        particleId = PI_PLUS;
        mass = PdgTable::GetParticleMass(PI_PLUS);
        charge = PdgTable::GetParticleCharge(PI_PLUS);

        CartesianVector energyWeightedClusterPosition(0.f, 0.f, 0.f);
        this->ComputeEnergyWeightedClusterPosition(pCluster, energyWeightedClusterPosition);

        const CartesianVector clusterMomentum(energyWeightedClusterPosition * pCluster->GetHadronicEnergy());
        const float totalGravity(energyWeightedClusterPosition.GetMagnitude());

        if (0.f == totalGravity)
            throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

        momentum = clusterMomentum * (1. / totalGravity);
    }

    if (m_debug)
    {
        std::cout << "energy from calo: " << energy << std::endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromMC(Cluster *pCluster, float &energy, CartesianVector &/*momentum*/, float &/*mass*/,
    int &particleId, int &/*charge*/) const
{
    // Match CaloHitVectors to their MCParticles
    typedef std::map< const MCParticle*, float > MCParticleToEnergyMap;
    MCParticleToEnergyMap energyPerMCParticle;
    MCParticleToEnergyMap::iterator itEnergyPerMCParticle;

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

            const float hitEnergy((*hitIter)->GetInputEnergy());

            itEnergyPerMCParticle = energyPerMCParticle.find(pMCParticle);

            if (energyPerMCParticle.end() == itEnergyPerMCParticle)
            {
                energyPerMCParticle.insert(std::make_pair(pMCParticle, hitEnergy));
            }
            else
            {
                itEnergyPerMCParticle->second += hitEnergy;
            }
        }
    }

    MCParticleToEnergyMap::const_iterator it = std::max_element( energyPerMCParticle.begin(), energyPerMCParticle.end(),
         pandora::Select2nd<MCParticleToEnergyMap::value_type, std::greater<float> >() );

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
        charge += (*itTrack)->GetCharge();
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
        this->ComputeFromCalorimeter(pCluster, energy, momentum, mass, particleId, charge);
    }
    else
    {
        this->ComputeFromTracks(pCluster, energy, momentum, mass, particleId, charge);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeEnergyWeightedClusterPosition(Cluster *pCluster, CartesianVector &energyWeightedClusterPosition) const
{
    energyWeightedClusterPosition.SetValues(0.f, 0.f, 0.f);
    float energySum(0.f);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const float hitEnergy = (*hitIter)->GetElectromagneticEnergy();
            energySum += hitEnergy;

            energyWeightedClusterPosition += (*hitIter)->GetPositionVector() * hitEnergy;
        }
    }

    if (0.f == energySum)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    energyWeightedClusterPosition *= (1.f / energySum);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingPfoCreationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
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
