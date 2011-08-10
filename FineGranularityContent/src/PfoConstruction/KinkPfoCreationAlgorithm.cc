/**
 *  @file   PandoraPfaNew/Algoirthms/src/KinkPfoCreationAlgorithm.cc
 * 
 *  @brief  Analyse kink pfos to identify decays to muons and neutral hadrons
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "PfoConstruction/KinkPfoCreationAlgorithm.h"

using namespace pandora;

StatusCode KinkPfoCreationAlgorithm::Run()
{
    const PfoList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pPfoList));

// Under Development:
//    PfoList lambdaPfos, kShortPfos;
//
//    for (PfoList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end(); iter != iterEnd; ++iter)
//    {
//        ParticleFlowObject *pPfo = *iter;
//
//        if (pPfo->GetParticleId() == K_SHORT)
//            kShortPfos.insert(pPfo);
//
//        if (std::abs(pPfo->GetParticleId()) == LAMBDA)
//            lambdaPfos.insert(pPfo);
//    }

    for (PfoList::const_iterator iter = pPfoList->begin(); iter != pPfoList->end(); ++iter)
    {
        ParticleFlowObject *pPfo = *iter;
        const TrackList pfoTrackList(pPfo->GetTrackList());

        // Kink should have a single daughter track
        if (pfoTrackList.size() < 2)
            continue;

        for (TrackList::const_iterator iterTrack = pfoTrackList.begin(), iterTrackEnd = pfoTrackList.end(); iterTrack != iterTrackEnd; ++iterTrack)
        {
            Track *pTrack = *iterTrack;
            const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());

            if (daughterTrackList.size() != 1)
                continue;

            const TrackList &siblingTrackList(pTrack->GetSiblingTrackList());
            const TrackList &parentTrackList(pTrack->GetParentTrackList());

            if (!parentTrackList.empty() || !siblingTrackList.empty())
                continue;

            Track *pParentTrack(pTrack);
            Track *pDaughterTrack(*(daughterTrackList.begin()));

            const float trackEnergyDifference(pParentTrack->GetEnergyAtDca() - pDaughterTrack->GetEnergyAtDca());

            if (trackEnergyDifference < m_minTrackEnergyDifference)
                continue;

            // Check to see if this is a decay kink 
            if ((std::abs(pParentTrack->GetParticleId()) == std::abs(PI_PLUS)) && (std::abs(pDaughterTrack->GetParticleId()) == std::abs(PI_PLUS)))
                continue; 

            CartesianVector parentMomentumAtEnd(pParentTrack->GetTrackStateAtEnd().GetMomentum());
            const CartesianVector &parentPositionAtEnd(pParentTrack->GetTrackStateAtEnd().GetPosition());
            const CartesianVector &daughterMomentumAtStart(pDaughterTrack->GetTrackStateAtStart().GetMomentum());

            const float scale((pTrack->GetMomentumAtDca()).GetMagnitude() / parentMomentumAtEnd.GetMagnitude());

            if ((scale > m_maxDcaToEndMomentumRatio) || (scale < m_minDcaToEndMomentumRatio))
            {
                parentMomentumAtEnd *= scale;

                if(m_monitoring)
                    std::cout << "Scaling parent momentum at end" << std::endl;
            }

            const CartesianVector neutralMomentum(parentMomentumAtEnd - daughterMomentumAtStart);
            const float neutralEnergy(neutralMomentum.GetMagnitude());

            bool isMuon = false;
            Cluster *pCluster = NULL;

            if (STATUS_CODE_SUCCESS == pDaughterTrack->GetAssociatedCluster(pCluster))
            {
                isMuon = ParticleIdHelper::IsMuonFast(pCluster);
            }

            if (m_mcMonitoring)
            {
                this->DisplayMcInformation(pParentTrack);
            }

            if (m_monitoring)
            {
                std::cout << " Reco Parent Track : " << pParentTrack->GetParticleId() << " E = " << pParentTrack->GetEnergyAtDca() << " " << parentMomentumAtEnd.GetMagnitude() << std::endl;
                std::cout << "   daughter   : " << pDaughterTrack->GetParticleId() << " E = " << pDaughterTrack->GetEnergyAtDca() << " " << daughterMomentumAtStart.GetMagnitude() << std::endl;
                std::cout << "   neutral    : " << " E = " << neutralEnergy << std::endl;

                if (pCluster != NULL)
                    std::cout << "   cluster    : " << " E = " << pCluster->GetCorrectedHadronicEnergy() << std::endl;

                if (isMuon)
                    std::cout << " Track is a muon " << std::endl;
            }

            // Find potential neutral clusters
            ClusterList neutralPfoCandidateClusterList;
            PfosOrderedByDistanceMap neutralPfosByDistance, photonPfosByDistance, chargedPfosByDistance;

            for (PfoList::const_iterator neutralIter = pPfoList->begin(), neutralIterEnd = pPfoList->end(); neutralIter != neutralIterEnd;)
            {
                ParticleFlowObject *pNeutralPfo = *neutralIter;
                ++neutralIter;

                const TrackList &candidateTrackList(pNeutralPfo->GetTrackList());
                const ClusterList &candidateClusterList(pNeutralPfo->GetClusterList());

                if (candidateClusterList.size() != 1)
                    continue;

                Cluster *pNeutralCluster(*(candidateClusterList.begin()));

                const CartesianVector centroid(pNeutralCluster->GetCentroid(pNeutralCluster->GetInnerPseudoLayer()));
                const CartesianVector &clusterMomentum(pNeutralPfo->GetMomentum());

                const float scale(centroid.GetMagnitude() / clusterMomentum.GetMagnitude());
                const CartesianVector radialVector(clusterMomentum * scale + parentPositionAtEnd);

                const float angle(radialVector.GetOpeningAngle(neutralMomentum));
                float deltaR(angle * centroid.GetMagnitude());

                if (pNeutralPfo->GetParticleId() == PHOTON)
                    deltaR *= m_photonDistancePenalty;

                if (candidateTrackList.size() == 1)
                    deltaR = deltaR / 2.f;

                if (deltaR < m_maxProjectedDistanceToNeutral)
                {
                    if (candidateTrackList.empty())
                    {
                        switch (pNeutralPfo->GetParticleId())
                        {
                        case PHOTON:
                            photonPfosByDistance.insert(PfosOrderedByDistanceMap::value_type(deltaR, pNeutralPfo));
                            break;
                        case NEUTRON:
                            neutralPfosByDistance.insert(PfosOrderedByDistanceMap::value_type(deltaR, pNeutralPfo));
                            break;
                        default:
                            break;
                        }

                        neutralPfoCandidateClusterList.insert(pNeutralCluster);
                    }
                    else
                    {
                        chargedPfosByDistance.insert(PfosOrderedByDistanceMap::value_type(deltaR, pNeutralPfo));
                    }
                }
            }

            // Now apply corrections
            bool correctedThisPfo = false;

            if (m_findDecaysWithNeutrinos && ((std::abs(pParentTrack->GetParticleId()) == std::abs(PI_PLUS)) ||
                (std::abs(pParentTrack->GetParticleId()) == std::abs(K_PLUS))))
            {
                float munuMass, pipiMass;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetKinkMass(parentMomentumAtEnd,daughterMomentumAtStart,
                    PdgTable::GetParticleMass(MU_MINUS), 0.f, munuMass));
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetKinkMass(parentMomentumAtEnd,daughterMomentumAtStart,
                    PdgTable::GetParticleMass(PI_MINUS), 0.f, pipiMass));

                if (m_monitoring)
                {
                    std::cout << " Kink mass (reco) mu nu : " << munuMass << std::endl;
                    std::cout << " Kink mass (reco) pi pi : " << pipiMass << std::endl;
                }

                const bool isPiMass((munuMass > m_lowerCutOnPiToMuNuMass) && (munuMass < m_upperCutOnPiToMuNuMass));
                bool isKaonMass((munuMass > m_lowerCutOnKToMuNuMass) && (munuMass < m_upperCutOnKToMuNuMass));

                if (std::fabs(pipiMass - PdgTable::GetParticleMass(K_MINUS)) < std::fabs(munuMass - PdgTable::GetParticleMass(K_MINUS)))
                    isKaonMass = false;

                if (isMuon || isPiMass || isKaonMass)
                {
                    int parentId((pPfo->GetCharge() > 0) ? PI_PLUS : PI_MINUS);

                    if (std::fabs(munuMass - PdgTable::GetParticleMass(PI_MINUS)) > std::fabs(munuMass - PdgTable::GetParticleMass(K_MINUS)))
                        (pPfo->GetCharge() > 0) ? parentId = K_PLUS : parentId = K_MINUS;

                    pPfo->SetParticleId(parentId);
                    this->SetPfoParametersFromParentTrack(pPfo);
                    correctedThisPfo = true;

                    if(m_monitoring)
                        std::cout << " New PFO  " << pPfo->GetParticleId() << " energy " << pPfo->GetEnergy() << std::endl; 
                }
                else
                {
                    // would like to define daughter track as pion not a muon - don't have a way to do this or propagate it to PFOs
                    // no impact on performance
                }
            }

// Under Development:
//            if (m_findDecaysWithLambdas && !correctedThisPfo)
//            {
//                const float targetEnergy(pParentTrack->GetEnergyAtDca() - pDaughterTrack->GetEnergyAtDca());
//
//                for (PfoList::const_iterator lIter = lambdaPfos.begin(), lIterEnd = lambdaPfos.end(); lIter != lIterEnd; ++lIter)
//                {
//                    ParticleFlowObject *lambdaPfo = (*lIter);
//                    const CartesianVector lambdaMomentum(lambdaPfo->GetMomentum());
//
//                    float recoMass;
//                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetV0Mass(daughterMomentumAtStart, lambdaMomentum,
//                        PdgTable::GetParticleMass(PI_MINUS), PdgTable::GetParticleMass(LAMBDA), recoMass));
//
//                    if (m_monitoring)
//                    {
//                        std::cout << " Lambda E = " << lambdaPfo->GetEnergy() << std::endl;
//                        std::cout << " Lamda kink mass = " << recoMass << std::endl;
//                    }
//                }
//            }

            if (m_findDecaysWithNeutrons && !correctedThisPfo)
            {
                const float targetEnergy(pParentTrack->GetEnergyAtDca() - pDaughterTrack->GetEnergyAtDca());

                float clusterEnergy(0.);
                PfoList pfosToRemove;
                float chi2Current(std::numeric_limits<float>::max());

                for (PfosOrderedByDistanceMap::const_iterator cIter = neutralPfosByDistance.begin(), cIterEnd = neutralPfosByDistance.end();
                    cIter != cIterEnd; ++cIter)
                {
                    ParticleFlowObject *clusterPfo = (*cIter).second;
                    clusterEnergy += clusterPfo->GetEnergy();
                    const float chi(ReclusterHelper::GetTrackClusterCompatibility(clusterEnergy, targetEnergy));

                    float recoMass;
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetKinkMass(parentMomentumAtEnd,daughterMomentumAtStart,
                        PdgTable::GetParticleMass(PI_MINUS), PdgTable::GetParticleMass(NEUTRON), recoMass));

                    if (chi * chi < chi2Current)
                    {
                        pfosToRemove.insert(clusterPfo);
                        chi2Current = chi*chi;
                    }

                    if(m_monitoring)
                    {
                        std::cout << " Neutral cluster " << (*cIter).first << " pid " << clusterPfo->GetParticleId() << " E = " << clusterPfo->GetEnergy() << std::endl;
                        std::cout << " Kink mass = " << recoMass << std::endl;
                    }
                }

                if (m_monitoring)
                    std::cout << " adding " << pfosToRemove.size() << " clusters, chi2 = " << chi2Current << std::endl; 

                if (chi2Current < m_chi2CutForMergingPfos)
                {
                    // Use the track parent for the energy 
                    (pPfo->GetCharge() > 0) ?  pPfo->SetParticleId(SIGMA_PLUS) : pPfo->SetParticleId(SIGMA_MINUS);
                    this->SetPfoParametersFromParentTrack(pPfo);
                    this->MergeClustersFromPfoListToPfo(pfosToRemove, pPfo);
                    correctedThisPfo = true;

                    if (m_monitoring)
                        std::cout << " New PFO  " << pPfo->GetParticleId() << " energy " << pPfo->GetEnergy() << std::endl; 
                }
                else
                {
                    if (clusterEnergy > targetEnergy)
                    {
                        // Cluster lost in larger neutral cluster
                        (pPfo->GetCharge() > 0) ?  pPfo->SetParticleId(SIGMA_PLUS) : pPfo->SetParticleId(SIGMA_MINUS);
                        correctedThisPfo = true;

                        if (m_monitoring)
                            std::cout << " Setting PFO ID " << pPfo->GetParticleId() << " energy " << pPfo->GetEnergy() << std::endl; 
                    }
                }
            }

// Under Development:
//            if (m_findDecaysWithPiZeros && !correctedThisPfo)
//            {
//                for (PfosOrderedByDistanceMap::const_iterator cIter = photonPfosByDistance.begin(), cIterEnd = photonPfosByDistance.end(); 
//                    cIter != cIterEnd; ++cIter)
//                {
//                    ParticleFlowObject *pPfo = (*cIter).second;
//
//                    if (m_monitoring)
//                        std::cout << " Photon cluster " << (*cIter).first << " pid " << pPfo->GetParticleId() << " E = " << pPfo->GetEnergy() << std::endl;
//                }
//            }
//
//            if (m_findDecaysMergedWithTrackClusters && !correctedThisPfo)
//            {
//                for (PfosOrderedByDistanceMap::const_iterator cIter = chargedPfosByDistance.begin(), cIterEnd = chargedPfosByDistance.end();
//                    cIter != cIterEnd; ++cIter)
//                {
//                    ParticleFlowObject *pPfo = (*cIter).second;
//                    const ClusterList &clusterList = pPfo->GetClusterList();
//
//                    if (clusterList.size() != 1)
//                        continue;
//
//                    Cluster *pNeutralCluster = *(clusterList.begin());
//                    const float neutralClusterEnergy(pNeutralCluster->GetCorrectedHadronicEnergy());
//                    const float chiEC(ReclusterHelper::GetTrackClusterCompatibility(neutralClusterEnergy, pPfo->GetEnergy()));
//
//                    if (m_monitoring)
//                        std::cout << " Charged cluster " << (*cIter).first << " pid " << " Et = " << pPfo->GetEnergy() << " Ec = " << neutralClusterEnergy << " chi = " << chiEC <<std::endl;
//                }
//            }
//
//            TrackList v0s;
//
//            for (PfoList::const_iterator iterV0 = pPfoList->begin(), iterV0End = pPfoList->end(); iterV0 != iterV0End; ++iterV0)
//            {
//                ParticleFlowObject *pPfoV0 = *iterV0;
//                const TrackList &trackListV0(pPfoV0->GetTrackList());
//
//                if (trackListV0.size() != 2)
//                    continue;
//
//                Track *pTrack = (*trackListV0.begin());
//                const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());
//
//                if (daughterTrackList.size() != 0)
//                    continue;
//
//                const TrackList &siblingTrackList(pTrack->GetSiblingTrackList());
//                const TrackList &parentTrackList(pTrack->GetParentTrackList());
//
//                if (!parentTrackList.empty() || (siblingTrackList.size() != 1))
//                    continue;
//
//                for (TrackList::const_iterator iterTrackV0 = trackListV0.begin(), iterTrackEnd = trackListV0.end(); iterTrackV0 != iterTrackEnd;
//                    ++iterTrackV0)
//                {
//                    v0s.insert(*iterTrackV0);
//                }
//            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode KinkPfoCreationAlgorithm::GetKinkMass(const CartesianVector &parentMomentum, const CartesianVector &daughterMomentum,
    float daughterMass, float neutralMass, float &mass) const
{
    // Calculate the invariant mass for a decaying charged particle
    mass = 0.f;

    const float daughterEnergy(std::sqrt(daughterMomentum.GetMagnitudeSquared() + daughterMass * daughterMass));

    const CartesianVector neutralMomentum(parentMomentum - daughterMomentum);

    const float neutralEnergy(std::sqrt(neutralMomentum.GetMagnitudeSquared() + neutralMass * neutralMass));
    const float massSquared((daughterEnergy + neutralEnergy) * (daughterEnergy + neutralEnergy) - parentMomentum.GetMagnitudeSquared());

    if (massSquared <= 0.f)
        return STATUS_CODE_FAILURE;

    mass = std::sqrt(massSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode KinkPfoCreationAlgorithm::GetV0Mass(const CartesianVector &momentum1, const CartesianVector &momentum2, float mass1, float mass2,
    float &v0mass) const
{
    // Calculate the invariant mass for a decaying charged particle
    v0mass = 0.f;

    const float energy1(std::sqrt(momentum1.GetMagnitudeSquared() + mass1 * mass1));
    const float energy2(std::sqrt(momentum2.GetMagnitudeSquared() + mass2 * mass2));

    const CartesianVector totalMomentum(momentum1 + momentum2);

    const float totalEnergy(energy1 + energy2);
    const float massSquared(totalEnergy * totalEnergy - totalMomentum.GetMagnitudeSquared());

    if (massSquared <= 0.f)
        return STATUS_CODE_FAILURE;

    v0mass = std::sqrt(massSquared);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode KinkPfoCreationAlgorithm::SetPfoParametersFromParentTrack(ParticleFlowObject *pPfo) const
{
    const float mass(PdgTable::GetParticleMass(PdgTable::GetParticleType(pPfo->GetParticleId())));

    const TrackList &trackList(pPfo->GetTrackList());

    for (TrackList::const_iterator iter = trackList.begin(), iterEnd = trackList.end(); iter != iterEnd; ++iter)
    {
        Track *pTrack = *iter;

        const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());

        if (daughterTrackList.size() != 1)
            continue;

        const TrackList &siblingTrackList(pTrack->GetSiblingTrackList());
        const TrackList &parentTrackList(pTrack->GetParentTrackList());

        if (!parentTrackList.empty() || !siblingTrackList.empty())
            continue;

        const CartesianVector &momentumAtDca(pTrack->GetMomentumAtDca());
        pPfo->SetMomentum(momentumAtDca);

        const float energy(std::sqrt(momentumAtDca.GetMagnitudeSquared() + mass * mass));
        pPfo->SetEnergy(energy);
        pPfo->SetMass(mass);

        return STATUS_CODE_SUCCESS;
    }

    return STATUS_CODE_FAILURE;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode KinkPfoCreationAlgorithm::MergeClustersFromPfoListToPfo(PfoList &pfosToMerge, ParticleFlowObject *pPfo) const
{
    ClusterList clustersToAdd;

    for (PfoList::const_iterator iter = pfosToMerge.begin(), iterEnd = pfosToMerge.end(); iter != iterEnd;)
    {
        ParticleFlowObject *pPfoToMerge = *iter;
        iter++;

        const ClusterList &clusterList(pPfoToMerge->GetClusterList());

        for (ClusterList::const_iterator clusterIter = clusterList.begin(), clusterIterEnd = clusterList.end(); clusterIter != clusterIterEnd; ++clusterIter)
        {
            clustersToAdd.insert(*clusterIter);
        }

        try
        {
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeletePfo(*this, pPfoToMerge));
        }
        catch (StatusCodeException &statusCodeException)
        {
            std::cout << "Failed to delete PFO !!! " << statusCodeException.ToString() << std::endl;
            return STATUS_CODE_FAILURE;
        }
    }

    for (ClusterList::const_iterator clusterIter = clustersToAdd.begin(), clusterIterEnd = clustersToAdd.end(); clusterIter != clusterIterEnd; ++clusterIter)
    {
        try
        {
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddClusterToPfo(*this, pPfo, *clusterIter));
        }
        catch (StatusCodeException &statusCodeException)
        {
            std::cout << "Failed to add cluster to PFO !!! " << statusCodeException.ToString() << std::endl;
            return STATUS_CODE_FAILURE;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void KinkPfoCreationAlgorithm::DisplayMcInformation(Track *pParentTrack) const
{
    try
    {
        const MCParticle* pMCParticle = NULL;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, pParentTrack->GetMCParticle(pMCParticle));

        const int ipdg(pMCParticle->GetParticleId());
        std::cout << " MC Parent Track : " << " ipdg = " << ipdg << " E = " << pMCParticle->GetEnergy() << std::endl;

        const MCParticleList &daughterList(pMCParticle->GetDaughterList());

        for (MCParticleList::const_iterator iter = daughterList.begin(), iterEnd = daughterList.end(); iter != iterEnd; ++iter)
        {
            const int ipdgD((*iter)->GetParticleId());
            const float energyD((*iter)->GetEnergy());

            if(energyD > 0.1f)
            {
                std::cout << "    decay daughter " << ipdgD <<  " E = " << energyD << std::endl;
            }
        }

        const MCParticleList &parentList(pMCParticle->GetParentList());

        for (MCParticleList::const_iterator iter = parentList.begin(), iterEnd = parentList.end(); iter != iterEnd; ++iter)
        {
            const int ipdgP((*iter)->GetParticleId());
            const float energyP((*iter)->GetEnergy());

            if(energyP > 0.1f)
            {
                std::cout << "    decay parent " << ipdgP <<  " E = " << energyP << std::endl;
            }
        }
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to find MC particle for track " << statusCodeException.ToString() << std::endl;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode KinkPfoCreationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_mcMonitoring = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MCMonitoring", m_mcMonitoring));

    m_monitoring = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Monitoring", m_monitoring));

    m_minTrackEnergyDifference = 0.4f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackEnergyDifference", m_minTrackEnergyDifference));

    m_maxDcaToEndMomentumRatio = 1.1f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxDcaToEndMomentumRatio", m_maxDcaToEndMomentumRatio));

    m_minDcaToEndMomentumRatio = 0.9f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDcaToEndMomentumRatio", m_minDcaToEndMomentumRatio));

    m_maxProjectedDistanceToNeutral = 150.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxProjectedDistanceToNeutral", m_maxProjectedDistanceToNeutral));

    m_photonDistancePenalty = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonDistancePenalty", m_photonDistancePenalty));

    m_chi2CutForMergingPfos = 6.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Chi2CutForMergingPfos", m_chi2CutForMergingPfos));

    m_findDecaysMergedWithTrackClusters = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysMergedWithTrackClusters", m_findDecaysMergedWithTrackClusters));   

    m_findDecaysWithNeutrinos = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysWithNeutrinos", m_findDecaysWithNeutrinos));

    m_findDecaysWithPiZeros = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysWithPiZeros", m_findDecaysWithPiZeros));

    m_findDecaysWithNeutrons = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysWithNeutrons", m_findDecaysWithNeutrons));

    m_findDecaysWithLambdas = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysWithLambdas", m_findDecaysWithLambdas));

    m_lowerCutOnPiZeroMass = 0.12f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowerCutOnPiZeroMass", m_lowerCutOnPiZeroMass));

    m_upperCutOnPiZeroMass = 0.16f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UpperCutOnPiZeroMass", m_upperCutOnPiZeroMass));

    m_lowerCutOnPiToMuNuMass = 0.1f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowerCutOnPiToMuNuMass", m_lowerCutOnPiToMuNuMass));

    m_upperCutOnPiToMuNuMass = 0.16f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UpperCutOnPiToMuNuMass", m_upperCutOnPiToMuNuMass));

    m_lowerCutOnKToMuNuMass = 0.45f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowerCutOnKToMuNuMass", m_lowerCutOnKToMuNuMass));

    m_upperCutOnKToMuNuMass = 0.55f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UpperCutOnKToMuNuMass", m_upperCutOnKToMuNuMass));

    return STATUS_CODE_SUCCESS;
}
