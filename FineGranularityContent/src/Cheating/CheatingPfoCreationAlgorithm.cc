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
    const ClusterList *pClusterList = NULL;

    if (!m_inputClusterListName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, m_inputClusterListName, pClusterList));
    }
    else
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));
    }

    if (NULL == pClusterList)
        return STATUS_CODE_NOT_INITIALIZED;

    // Configure computation of pfo parameters
    void (*pPfoParametersFunction)(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass, int &particleId,
        int &charge) = &CheatingPfoCreationAlgorithm::ComputeFromMC;

    if (m_pfoParameterDetermination == "Calorimeter")
    {
        pPfoParametersFunction = &CheatingPfoCreationAlgorithm::ComputeFromCalorimeter;
    }
    else if (m_pfoParameterDetermination == "Tracks")
    {
        pPfoParametersFunction = &CheatingPfoCreationAlgorithm::ComputeFromTracks;
    }
    else if (m_pfoParameterDetermination == "CalorimeterAndTracks")
    {
        pPfoParametersFunction = &CheatingPfoCreationAlgorithm::ComputeFromCalorimeterAndTracks;
    }

    const PfoList *pPfoList = NULL; std::string pfoListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryPfoListAndSetCurrent(*this, pPfoList, pfoListName));

    // For each cluster, use specified function to cheat determination of pfo parameters
    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster)
    {
        try
        {
            CartesianVector momentum(0.f, 0.f, 0.f);
            float mass = 0.f;
            int charge = 0;
            float energy = 0.f;
            int particleId = PI_PLUS;
            pPfoParametersFunction((*itCluster), energy, momentum, mass, particleId, charge);

            PandoraContentApi::ParticleFlowObjectParameters particleFlowObjectParameters;
            const TrackList &associatedTrackList((*itCluster)->GetAssociatedTrackList());
            particleFlowObjectParameters.m_trackList.insert(associatedTrackList.begin(), associatedTrackList.end());
            particleFlowObjectParameters.m_clusterList.insert(*itCluster);
            particleFlowObjectParameters.m_energy = energy;
            particleFlowObjectParameters.m_charge = charge;
            particleFlowObjectParameters.m_mass = mass;
            particleFlowObjectParameters.m_momentum = momentum;
            particleFlowObjectParameters.m_particleId = particleId;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, particleFlowObjectParameters));
        }
        catch (StatusCodeException &statusCodeException)
        {
            std::cout << "CheatingPfoCreationAlgorithm: Failed to cheat construction of a particle flow object" << std::endl;
        }
    }

    if (!pPfoList->empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SavePfoList(*this, m_outputPfoListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentPfoList(*this, m_outputPfoListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromCalorimeter(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge)
{
    if (pCluster->IsFixedPhoton() && pCluster->GetAssociatedTrackList().empty())
    {
        energy = pCluster->GetElectromagneticEnergy();
        particleId = PHOTON;
        mass = PdgTable::GetParticleMass(PHOTON);
        charge = PdgTable::GetParticleCharge(PHOTON);
        momentum = pCluster->GetFitToAllHitsResult().GetDirection() * energy;
        return;
    }

    if (pCluster->GetAssociatedTrackList().empty())
    {
        energy = pCluster->GetHadronicEnergy();
        particleId = NEUTRON;
        mass = PdgTable::GetParticleMass(NEUTRON);
        charge = PdgTable::GetParticleCharge(NEUTRON);
    }
    else
    {
        energy = pCluster->GetHadronicEnergy();
        particleId = PI_PLUS;
        mass = PdgTable::GetParticleMass(PI_PLUS);
        charge = PdgTable::GetParticleCharge(PI_PLUS);
    }

    CartesianVector energyWeightedClusterPosition(0.f, 0.f, 0.f);
    CheatingPfoCreationAlgorithm::ComputeEnergyWeightedClusterPosition(pCluster, energyWeightedClusterPosition);

    const CartesianVector clusterMomentum(energyWeightedClusterPosition * pCluster->GetHadronicEnergy());
    const float totalGravity(energyWeightedClusterPosition.GetMagnitude());

    if (0.f == totalGravity)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    momentum = clusterMomentum * (1. / totalGravity);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromMC(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge)
{
    const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

    if (NULL == pMainMCParticle)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    energy = pMainMCParticle->GetEnergy();
    particleId = pMainMCParticle->GetParticleId();
    mass = PdgTable::GetParticleMass(particleId);
    charge = PdgTable::GetParticleCharge(particleId);
    momentum = pMainMCParticle->GetMomentum();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromTracks(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge)
{
    energy = 0.f;
    momentum = CartesianVector(0.f, 0.f, 0.f);
    mass = 0.f;
    particleId = PI_PLUS;
    charge = 0;

    const TrackList &trackList(pCluster->GetAssociatedTrackList());

    for (TrackList::const_iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack)
    {
        energy += (*itTrack)->GetEnergyAtDca();
        momentum += (*itTrack)->GetMomentumAtDca();
        mass += (*itTrack)->GetMass();
        charge += (*itTrack)->GetCharge();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeFromCalorimeterAndTracks(Cluster *pCluster, float &energy, CartesianVector &momentum, float &mass,
    int &particleId, int &charge)
{
    if (pCluster->GetAssociatedTrackList().empty())
    {
        CheatingPfoCreationAlgorithm::ComputeFromCalorimeter(pCluster, energy, momentum, mass, particleId, charge);
    }
    else
    {
        CheatingPfoCreationAlgorithm::ComputeFromTracks(pCluster, energy, momentum, mass, particleId, charge);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CheatingPfoCreationAlgorithm::ComputeEnergyWeightedClusterPosition(Cluster *pCluster, CartesianVector &energyWeightedClusterPosition)
{
    energyWeightedClusterPosition.SetValues(0.f, 0.f, 0.f);
    float energySum(0.f);

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const float hitEnergy = (*hitIter)->GetHadronicEnergy();
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
    m_outputPfoListName = "CheatingPfoList";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputPfoListName", m_outputPfoListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InputClusterListName", m_inputClusterListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PfoParameterDetermination", m_pfoParameterDetermination));

    return STATUS_CODE_SUCCESS;
}
