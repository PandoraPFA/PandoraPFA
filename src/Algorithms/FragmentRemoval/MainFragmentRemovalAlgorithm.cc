/**
 *  @file   PandoraPFANew/src/Algorithms/FragmentRemoval/MainFragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the main fragment removal algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/FragmentRemoval/MainFragmentRemovalAlgorithm.h"

using namespace pandora;

StatusCode MainFragmentRemovalAlgorithm::Run()
{
    bool isFirstPass(true), shouldRecalculate(true);

    ClusterList affectedClusters;
    ClusterContactMap clusterContactMap;

    while (shouldRecalculate)
    {
        shouldRecalculate = false;
        Cluster *pBestParentCluster(NULL), *pBestDaughterCluster(NULL);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterContactMap(isFirstPass, affectedClusters, clusterContactMap));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterMergingCandidates(clusterContactMap, pBestParentCluster,
            pBestDaughterCluster));

        if ((NULL != pBestParentCluster) && (NULL != pBestDaughterCluster))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster,
                pBestDaughterCluster));

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetAffectedClusters(clusterContactMap, pBestParentCluster,
                pBestDaughterCluster, affectedClusters));

            clusterContactMap.erase(clusterContactMap.find(pBestDaughterCluster));
            shouldRecalculate = true;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters,
    ClusterContactMap &clusterContactMap) const
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iterI = pClusterList->begin(), iterIEnd = pClusterList->end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = *iterI;

        if (!pDaughterCluster->GetAssociatedTrackList().empty())
            continue;

        if ((pDaughterCluster->GetNCaloHits() < m_minDaughterCaloHits) || (pDaughterCluster->GetHadronicEnergy() < m_minDaughterHadronicEnergy))
            continue;

        // Identify whether cluster contacts need to be recalculated
        if (!isFirstPass && (affectedClusters.end() == affectedClusters.find(pDaughterCluster)))
            continue;

        // Calculate the cluster contact information
        for (ClusterList::const_iterator iterJ = pClusterList->begin(), iterJEnd = pClusterList->end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            if (pDaughterCluster == pParentCluster)
                continue;

            if (pParentCluster->GetAssociatedTrackList().empty())
                continue;

            const ClusterContact clusterContact(pDaughterCluster, pParentCluster);

            if (this->PassesClusterContactCuts(clusterContact))
            {
                clusterContactMap[pDaughterCluster].push_back(clusterContact);
            }
        }
    }
    isFirstPass = false;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MainFragmentRemovalAlgorithm::PassesClusterContactCuts(const ClusterContact &clusterContact) const
{
    if (clusterContact.GetDistanceToClosestHit() > m_contactCutMaxDistance)
        return false;

    if ((clusterContact.GetNContactLayers() > m_contactCutNLayers) ||
        (clusterContact.GetConeFraction1() > m_contactCutConeFraction1) ||
        (clusterContact.GetCloseHitFraction1() > m_contactCutCloseHitFraction1) ||
        (clusterContact.GetCloseHitFraction2() > m_contactCutCloseHitFraction2) ||
        (clusterContact.GetMeanDistanceToHelix() < m_contactCutMeanDistanceToHelix) ||
        (clusterContact.GetClosestDistanceToHelix() < m_contactCutClosestDistanceToHelix))
    {
        return true;
    }

    static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
    const PseudoLayer daughterInnerLayer(clusterContact.GetDaughterCluster()->GetInnerPseudoLayer());

    return ((clusterContact.GetDistanceToClosestHit() < m_contactCutNearECalDistance) &&
        (daughterInnerLayer + m_contactCutLayersFromECal > nECalLayers));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::GetClusterMergingCandidates(const ClusterContactMap &clusterContactMap, Cluster *&pBestParentCluster,
    Cluster *&pBestDaughterCluster) const
{
    for (ClusterContactMap::const_iterator iterI = clusterContactMap.begin(), iterIEnd = clusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = iterI->first;
        float globalDeltaChi2(0.f);

        // Check to see if merging parent and daughter clusters would improve track-cluster compatibility
        if (!this->PassesPreselection(pDaughterCluster, iterI->second, globalDeltaChi2))
            continue;

        float highestExcessEvidence(0.f);
        const PseudoLayer daughterCorrectionLayer(this->GetClusterCorrectionLayer(pDaughterCluster));

        for (ClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
        {
            ClusterContact clusterContact = *iterJ;

            if (pDaughterCluster != clusterContact.GetDaughterCluster())
                throw StatusCodeException(STATUS_CODE_FAILURE);

            const float totalEvidence(this->GetTotalEvidenceForMerge(clusterContact));
            const float requiredEvidence(this->GetRequiredEvidenceForMerge(pDaughterCluster, clusterContact, daughterCorrectionLayer,
                globalDeltaChi2));
            const float excessEvidence(totalEvidence - requiredEvidence);

            if(excessEvidence > highestExcessEvidence)
            {
                highestExcessEvidence = excessEvidence;
                pBestDaughterCluster = pDaughterCluster;
                pBestParentCluster = clusterContact.GetParentCluster();
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}
//------------------------------------------------------------------------------------------------------------------------------------------

bool MainFragmentRemovalAlgorithm::PassesPreselection(Cluster *const pDaughterCluster, const ClusterContactVector &clusterContactVector,
    float &globalDeltaChi2) const
{
    float totalTrackEnergy(0.f), totalClusterEnergy(0.f);
    const float daughterClusterEnergy(pDaughterCluster->GetHadronicEnergy());

    // Check to see if merging parent and daughter clusters would improve track-cluster compatibility
    for (ClusterContactVector::const_iterator iter = clusterContactVector.begin(), iterEnd = clusterContactVector.end(); iter != iterEnd;
        ++iter)
    {
        ClusterContact clusterContact = *iter;

        const float oldChi(this->GetTrackClusterCompatibility(clusterContact.GetParentClusterEnergy(), clusterContact.GetParentTrackEnergy()));
        const float newChi(this->GetTrackClusterCompatibility(daughterClusterEnergy + clusterContact.GetParentClusterEnergy(),
            clusterContact.GetParentTrackEnergy()));

        const float oldChi2(oldChi * oldChi);
        const float newChi2(newChi * newChi);

        if ((newChi2 < m_maxChi2) || (newChi2 < oldChi2))
            return true;

        totalTrackEnergy += clusterContact.GetParentTrackEnergy();
        totalClusterEnergy += clusterContact.GetParentClusterEnergy();
    }

    // Check again using total energies of all contact clusters and their associated tracks
    const float oldChiTotal(this->GetTrackClusterCompatibility(totalClusterEnergy, totalTrackEnergy));
    const float newChiTotal(this->GetTrackClusterCompatibility(daughterClusterEnergy + totalClusterEnergy, totalTrackEnergy));

    const float oldChi2Total(oldChiTotal * oldChiTotal);
    const float newChi2Total(newChiTotal * newChiTotal);

    globalDeltaChi2 = oldChi2Total - newChi2Total;

    if ((newChi2Total < m_maxGlobalChi2) || (newChi2Total < oldChi2Total))
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float MainFragmentRemovalAlgorithm::GetTotalEvidenceForMerge(const ClusterContact &clusterContact) const
{
    // Calculate a measure of the evidence that the daughter candidate cluster is a fragment of the parent candidate cluster:

    // 1. Layers in contact
    float contactEvidence(0.f);
    if (clusterContact.GetNContactLayers() > m_contactEvidenceNLayers1)
    {
        contactEvidence = m_contactEvidence1;
    }
    else if (clusterContact.GetNContactLayers() > m_contactEvidenceNLayers2)
    {
        contactEvidence = m_contactEvidence2;
    }
    else if (clusterContact.GetNContactLayers() > m_contactEvidenceNLayers3)
    {
        contactEvidence = m_contactEvidence3;
    }
    contactEvidence *= (1.f + clusterContact.GetContactFraction());

    // 2. Cone extrapolation
    float coneEvidence(0.f);
    if (clusterContact.GetConeFraction1() > m_coneEvidenceFraction1)
    {
        coneEvidence = clusterContact.GetConeFraction1() + clusterContact.GetConeFraction2() + clusterContact.GetConeFraction3();

        static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
        const PseudoLayer daughterInnerLayer(clusterContact.GetDaughterCluster()->GetInnerPseudoLayer());

        if (daughterInnerLayer < nECalLayers)
            coneEvidence *= m_coneEvidenceECalMultiplier;
    }

    // 3. Track extrapolation
    float trackExtrapolationEvidence(0.f);
    if (clusterContact.GetClosestDistanceToHelix() < m_closestTrackEvidence1)
    {
        trackExtrapolationEvidence = (m_closestTrackEvidence1 - clusterContact.GetClosestDistanceToHelix()) / m_closestTrackEvidence1d;

        if(clusterContact.GetClosestDistanceToHelix() < m_closestTrackEvidence2)
            trackExtrapolationEvidence += (m_closestTrackEvidence2 - clusterContact.GetClosestDistanceToHelix()) / m_closestTrackEvidence2d;

        if(clusterContact.GetMeanDistanceToHelix() < m_meanTrackEvidence1)
            trackExtrapolationEvidence += (m_meanTrackEvidence1 - clusterContact.GetMeanDistanceToHelix()) / m_meanTrackEvidence1d;

        if(clusterContact.GetMeanDistanceToHelix() < m_meanTrackEvidence2)
            trackExtrapolationEvidence += (m_meanTrackEvidence2 - clusterContact.GetClosestDistanceToHelix()) / m_meanTrackEvidence2d;
    }

    // 4. Distance of closest approach
    float distanceEvidence(0.f);
    if (clusterContact.GetDistanceToClosestHit() < m_distanceEvidence1)
    {
        distanceEvidence = (m_distanceEvidence1 - clusterContact.GetDistanceToClosestHit()) / m_distanceEvidence1d;
        distanceEvidence += m_distanceEvidenceCloseFraction1Multiplier * clusterContact.GetCloseHitFraction1();
        distanceEvidence += m_distanceEvidenceCloseFraction2Multiplier * clusterContact.GetCloseHitFraction2();
    }

    return ((m_contactWeight * contactEvidence) + (m_coneWeight * coneEvidence) + (m_distanceWeight * distanceEvidence) +
        (m_trackExtrapolationWeight * trackExtrapolationEvidence));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float MainFragmentRemovalAlgorithm::GetRequiredEvidenceForMerge(Cluster *const pDaughterCluster, const ClusterContact &clusterContact,
    const PseudoLayer correctionLayer, const float globalDeltaChi2) const
{
    // Primary evidence requirement is obtained from change in chi2.
    const float daughterClusterEnergy(pDaughterCluster->GetHadronicEnergy());

    const float oldChi(this->GetTrackClusterCompatibility(clusterContact.GetParentClusterEnergy(), clusterContact.GetParentTrackEnergy()));
    const float newChi(this->GetTrackClusterCompatibility(daughterClusterEnergy + clusterContact.GetParentClusterEnergy(),
        clusterContact.GetParentTrackEnergy()));

    const float oldChi2(oldChi * oldChi);
    const float newChi2(newChi * newChi);

    const float chi2Evidence(m_chi2Base - (oldChi2 - newChi2));
    const float globalChi2Evidence(m_chi2Base + m_globalChi2Penalty - globalDeltaChi2);
    const bool usingGlobalChi2(((newChi2 > oldChi2) && (newChi2 > m_maxGlobalChi2)) || (globalChi2Evidence < chi2Evidence));

    // Final evidence requirement is corrected to account for following factors:
    // 1. Layer corrections
    float layerCorrection(0.f);

    static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
    static const unsigned int halfHCal(GeometryHelper::GetInstance()->GetHCalBarrelParameters().GetNLayers() / 2);
    static const unsigned int halfECal(nECalLayers / 2);

    const PseudoLayer innerLayer(pDaughterCluster->GetInnerPseudoLayer());
    const PseudoLayer outerLayer(pDaughterCluster->GetOuterPseudoLayer());

    if(correctionLayer <= halfECal)
    {
        layerCorrection = m_layerCorrection1;
    }
    else if((correctionLayer > halfECal) && (correctionLayer <= nECalLayers))
    {
        layerCorrection = m_layerCorrection2;
    }
    else if(correctionLayer > nECalLayers)
    {
        layerCorrection = m_layerCorrection3;
    }
    else if(correctionLayer > nECalLayers + halfHCal)
    {
        layerCorrection = m_layerCorrection4;
    }

    if((outerLayer - innerLayer < m_layerCorrectionLayerSpan) && (innerLayer > m_layerCorrectionMinInnerLayer))
        layerCorrection = m_layerCorrection5;

    if(std::abs(static_cast<int>(correctionLayer) - static_cast<int>(nECalLayers)) < m_layerCorrectionLayersFromECal)
        layerCorrection = m_layerCorrection6;

    // 2. Leaving cluster corrections - TODO
    float leavingCorrection(0.f);

    // 3. Energy correction
    float energyCorrection(0.f);

    if(daughterClusterEnergy < m_energyCorrectionThreshold)
        energyCorrection = daughterClusterEnergy - m_energyCorrectionThreshold;

    // 4. Low energy fragment corrections
    float lowEnergyCorrection(0.f);

    if(daughterClusterEnergy < m_lowEnergyCorrectionThreshold)
    {
        const unsigned int nHitLayers(pDaughterCluster->GetOrderedCaloHitList().size());

        if(nHitLayers < m_lowEnergyCorrectionNHitLayers1)
        {
            lowEnergyCorrection += m_lowEnergyCorrection1;

            if(nHitLayers < m_lowEnergyCorrectionNHitLayers2)
                lowEnergyCorrection += m_lowEnergyCorrection2;
        }

        if(correctionLayer > nECalLayers)
            lowEnergyCorrection += m_lowEnergyCorrection3;
    }

    // 5. Angular corrections
    float angularCorrection(0.f);
    const float radialDirectionCosine(pDaughterCluster->GetFitToAllHitsResult().IsFitSuccessful() ?
        pDaughterCluster->GetFitToAllHitsResult().GetRadialDirectionCosine() : 0.f);

    if(radialDirectionCosine < m_angularCorrectionOffset)
        angularCorrection = m_angularCorrectionConstant + (radialDirectionCosine - m_angularCorrectionOffset) * m_angularCorrectionGradient;

    // 6. Photon cluster corrections
    float photonCorrection(0.f);

    if(pDaughterCluster->IsPhoton())
    {
        const float showerStart(pDaughterCluster->GetProfileShowerStart());
        const float photonFraction(pDaughterCluster->GetProfilePhotonFraction());

        if(daughterClusterEnergy > m_photonCorrectionEnergy1 && showerStart < m_photonCorrectionShowerStart1)
            photonCorrection = m_photonCorrection1;

        if(daughterClusterEnergy > m_photonCorrectionEnergy1 && showerStart < m_photonCorrectionShowerStart2)
            photonCorrection = m_photonCorrection2;

        if(daughterClusterEnergy < m_photonCorrectionEnergy1 && showerStart < m_photonCorrectionShowerStart2)
            photonCorrection = m_photonCorrection3;

        if(daughterClusterEnergy < m_photonCorrectionEnergy1 && showerStart < m_photonCorrectionShowerStart2 && photonFraction < m_photonCorrectionPhotonFraction1)
            photonCorrection = m_photonCorrection4;

        if(daughterClusterEnergy < m_photonCorrectionEnergy1 && showerStart > m_photonCorrectionShowerStart2)
            photonCorrection = m_photonCorrection5;

        if(daughterClusterEnergy < m_photonCorrectionEnergy2 && (showerStart > m_photonCorrectionShowerStart2 || photonFraction > m_photonCorrectionPhotonFraction2))
            photonCorrection = m_photonCorrection6;

        if(daughterClusterEnergy < m_photonCorrectionEnergy3 && showerStart > m_photonCorrectionShowerStart2)
            photonCorrection = m_photonCorrection7;
    }

    const float requiredEvidence(usingGlobalChi2 ?
        layerCorrection + angularCorrection + energyCorrection + leavingCorrection + photonCorrection :
        layerCorrection + angularCorrection + energyCorrection + leavingCorrection + photonCorrection + lowEnergyCorrection);

    return std::max(m_minRequiredEvidence, requiredEvidence);
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer MainFragmentRemovalAlgorithm::GetClusterCorrectionLayer(const Cluster *const pDaughterCluster) const
{
    float energySum(0.f);
    unsigned int layerCounter(0);

    const OrderedCaloHitList &orderedCaloHitList(pDaughterCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            energySum += (*hitIter)->GetHadronicEnergy();
        }

        if ((++layerCounter > m_correctionLayerNHitLayers) || (energySum > m_correctionLayerHadronicEnergy))
        {
            return iter->first;
        }
    }

    return pDaughterCluster->GetInnerPseudoLayer();
}

//------------------------------------------------------------------------------------------------------------------------------------------

float MainFragmentRemovalAlgorithm::GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy) const
{
    static const float hadronicEnergyResolution(PandoraSettings::GetInstance()->GetHadronicEnergyResolution());

    if ((0. == trackEnergy) || (0. == hadronicEnergyResolution))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergy / std::sqrt(trackEnergy));
    const float chi((clusterEnergy - trackEnergy) / sigmaE);

    return chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::GetAffectedClusters(const ClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
    Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const
{
    if (clusterContactMap.end() == clusterContactMap.find(pBestDaughterCluster))
        return STATUS_CODE_FAILURE;

    affectedClusters.clear();
    for (ClusterContactMap::const_iterator iterI = clusterContactMap.begin(), iterIEnd = clusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        // Store addresses of all clusters that were in contact with the newly deleted daughter cluster
        if (iterI->first == pBestDaughterCluster)
        {
            for (ClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
            {
                affectedClusters.insert(iterJ->GetParentCluster());
            }
            continue;
        }

        // Also store addresses of all clusters that contained either the parent or daughter clusters in their own ClusterContactVectors
        for (ClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
        {
            if ((iterJ->GetParentCluster() == pBestParentCluster) || (iterJ->GetParentCluster() == pBestDaughterCluster))
            {
                affectedClusters.insert(iterI->first);
                break;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MainFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Initial daughter cluster selection
    m_minDaughterCaloHits = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDaughterCaloHits", m_minDaughterCaloHits));

    m_minDaughterHadronicEnergy = 0.1f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDaughterHadronicEnergy", m_minDaughterHadronicEnergy));

    // Cluster contact cuts
    m_contactCutMaxDistance = 750.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutMaxDistance", m_contactCutMaxDistance));

    m_contactCutNLayers = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutNLayers", m_contactCutNLayers));

    m_contactCutConeFraction1 = 0.25f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutConeFraction1", m_contactCutConeFraction1));

    m_contactCutCloseHitFraction1 = 0.25f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutCloseHitFraction1", m_contactCutCloseHitFraction1));

    m_contactCutCloseHitFraction2 = 0.15f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutCloseHitFraction2", m_contactCutCloseHitFraction2));

    m_contactCutMeanDistanceToHelix = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutMeanDistanceToHelix", m_contactCutMeanDistanceToHelix));

    m_contactCutClosestDistanceToHelix = 150.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutClosestDistanceToHelix", m_contactCutClosestDistanceToHelix));

    m_contactCutNearECalDistance = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutNearECalDistance", m_contactCutNearECalDistance));

    m_contactCutLayersFromECal = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutLayersFromECal", m_contactCutLayersFromECal));

    // Track-cluster consistency Chi2 values
    m_maxChi2 = 16.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxChi2", m_maxChi2));

    m_maxGlobalChi2 = 9.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxGlobalChi2", m_maxGlobalChi2));

    m_chi2Base = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Chi2Base", m_chi2Base));

    m_globalChi2Penalty = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "GlobalChi2Penalty", m_globalChi2Penalty));

    // Correction layer parameters
    m_correctionLayerNHitLayers = 3;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CorrectionLayerNHitLayers", m_correctionLayerNHitLayers));

    m_correctionLayerHadronicEnergy = 0.25f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CorrectionLayerHadronicEnergy", m_correctionLayerHadronicEnergy));

    // Total evidence: Contact evidence
    m_contactEvidenceNLayers1 = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidenceNLayers1", m_contactEvidenceNLayers1));

    m_contactEvidence1 = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidence1", m_contactEvidence1));

    m_contactEvidenceNLayers2 = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidenceNLayers2", m_contactEvidenceNLayers2));

    m_contactEvidence2 = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidence2", m_contactEvidence2));

    m_contactEvidenceNLayers3 = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidenceNLayers3", m_contactEvidenceNLayers3));

    m_contactEvidence3 = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidence3", m_contactEvidence3));

    // Cone evidence
    m_coneEvidenceFraction1 = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeEvidenceFraction1", m_coneEvidenceFraction1));

    m_coneEvidenceECalMultiplier = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeEvidenceECalMultiplier", m_coneEvidenceECalMultiplier));

    // Track extrapolation evidence
    m_closestTrackEvidence1 = 200.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestTrackEvidence1", m_closestTrackEvidence1));

    m_closestTrackEvidence1d = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestTrackEvidence1d", m_closestTrackEvidence1d));

    if (0.f == m_closestTrackEvidence1d)
        return STATUS_CODE_INVALID_PARAMETER;

    m_closestTrackEvidence2 = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestTrackEvidence2", m_closestTrackEvidence2));

    m_closestTrackEvidence2d = 20.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ClosestTrackEvidence2d", m_closestTrackEvidence2d));

    if (0.f == m_closestTrackEvidence2d)
        return STATUS_CODE_INVALID_PARAMETER;

    m_meanTrackEvidence1 = 200.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MeanTrackEvidence1", m_meanTrackEvidence1));

    m_meanTrackEvidence1d = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MeanTrackEvidence1d", m_meanTrackEvidence1d));

    if (0.f == m_meanTrackEvidence1d)
        return STATUS_CODE_INVALID_PARAMETER;

    m_meanTrackEvidence2 = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MeanTrackEvidence2", m_meanTrackEvidence2));

    m_meanTrackEvidence2d = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MeanTrackEvidence2d", m_meanTrackEvidence2d));

    if (0.f == m_meanTrackEvidence2d)
        return STATUS_CODE_INVALID_PARAMETER;

    // Distance of closest approach evidence
    m_distanceEvidence1 = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceEvidence1", m_distanceEvidence1));

    m_distanceEvidence1d = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceEvidence1d", m_distanceEvidence1d));

    if (0.f == m_distanceEvidence1d)
        return STATUS_CODE_INVALID_PARAMETER;

    m_distanceEvidenceCloseFraction1Multiplier = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceEvidenceCloseFraction1Multiplier", m_distanceEvidenceCloseFraction1Multiplier));

    m_distanceEvidenceCloseFraction2Multiplier = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceEvidenceCloseFraction2Multiplier", m_distanceEvidenceCloseFraction2Multiplier));

    // Evidence weightings
    m_contactWeight = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactWeight", m_contactWeight));

    m_coneWeight = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeWeight", m_coneWeight));

    m_distanceWeight = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceWeight", m_distanceWeight));

    m_trackExtrapolationWeight = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackExtrapolationWeight", m_trackExtrapolationWeight));

    // Required evidence: Layer correction
    m_layerCorrection1 = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection1", m_layerCorrection1));

    m_layerCorrection2 = 0.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection2", m_layerCorrection2));

    m_layerCorrection3 = -1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection3", m_layerCorrection3));

    m_layerCorrection4 = -2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection4", m_layerCorrection4));

    m_layerCorrection5 = -2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection5", m_layerCorrection5));

    m_layerCorrection6 = -3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrection6", m_layerCorrection6));

    m_layerCorrectionLayerSpan = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrectionLayerSpan", m_layerCorrectionLayerSpan));

    m_layerCorrectionMinInnerLayer = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrectionMinInnerLayer", m_layerCorrectionMinInnerLayer));

    m_layerCorrectionLayersFromECal = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LayerCorrectionLayersFromECal", m_layerCorrectionLayersFromECal));

    // Energy correction
    m_energyCorrectionThreshold = 3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "EnergyCorrectionThreshold", m_energyCorrectionThreshold));

    // Low energy correction
    m_lowEnergyCorrectionThreshold = 1.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrectionThreshold", m_lowEnergyCorrectionThreshold));

    m_lowEnergyCorrectionNHitLayers1 = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrectionNHitLayers1", m_lowEnergyCorrectionNHitLayers1));

    m_lowEnergyCorrectionNHitLayers2 = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrectionNHitLayers2", m_lowEnergyCorrectionNHitLayers2));

    m_lowEnergyCorrection1 = -1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrection1", m_lowEnergyCorrection1));

    m_lowEnergyCorrection2 = -1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrection2", m_lowEnergyCorrection2));

    m_lowEnergyCorrection3 = -1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowEnergyCorrection3", m_lowEnergyCorrection3));

    // Angular correction
    m_angularCorrectionOffset = 0.75f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AngularCorrectionOffset", m_angularCorrectionOffset));

    m_angularCorrectionConstant = -0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AngularCorrectionConstant", m_angularCorrectionConstant));

    m_angularCorrectionGradient = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "AngularCorrectionGradient", m_angularCorrectionGradient));

    // Photon correction
    m_photonCorrectionEnergy1 = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionEnergy1", m_photonCorrectionEnergy1));

    m_photonCorrectionEnergy2 = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionEnergy2", m_photonCorrectionEnergy2));

    m_photonCorrectionEnergy3 = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionEnergy3", m_photonCorrectionEnergy3));

    m_photonCorrectionShowerStart1 = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionShowerStart1", m_photonCorrectionShowerStart1));

    m_photonCorrectionShowerStart2 = 2.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionShowerStart2", m_photonCorrectionShowerStart2));

    m_photonCorrectionPhotonFraction1 = 0.8f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionPhotonFraction1", m_photonCorrectionPhotonFraction1));

    m_photonCorrectionPhotonFraction2 = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrectionPhotonFraction2", m_photonCorrectionPhotonFraction2));

    m_photonCorrection1 = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection1", m_photonCorrection1));

    m_photonCorrection2 = 100.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection2", m_photonCorrection2));

    m_photonCorrection3 = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection3", m_photonCorrection3));

    m_photonCorrection4 = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection4", m_photonCorrection4));

    m_photonCorrection5 = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection5", m_photonCorrection5));

    m_photonCorrection6 = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection6", m_photonCorrection6));

    m_photonCorrection7 = 0.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonCorrection7", m_photonCorrection7));

    m_minRequiredEvidence = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinRequiredEvidence", m_minRequiredEvidence));

    return STATUS_CODE_SUCCESS;
}
