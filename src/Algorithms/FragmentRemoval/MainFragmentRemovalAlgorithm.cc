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

        if ((pDaughterCluster->GetNCaloHits() < /* m_ */5) || (pDaughterCluster->GetHadronicEnergy() < /* m_ */0.1))
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
    if (clusterContact.GetDistanceToClosestHit() > /* m_ */750.f)
        return false;

    if ((clusterContact.GetNContactLayers() > /* m_ */0) ||
        (clusterContact.GetConeFraction1() > /* m_ */0.25f) ||
        (clusterContact.GetCloseHitFraction1() > /* m_ */0.25) ||
        (clusterContact.GetCloseHitFraction2() > /* m_ */0.15) ||
        (clusterContact.GetMeanDistanceToHelix() < /* m_ */250.f) ||
        (clusterContact.GetClosestDistanceToHelix() < /* m_ */150.f))
    {
        return true;
    }

    static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
    const PseudoLayer daughterInnerLayer(clusterContact.GetDaughterCluster()->GetInnerPseudoLayer());

    return ((clusterContact.GetDistanceToClosestHit() < /* m_ */250.f) && (daughterInnerLayer + /* m_ */10 > nECalLayers));
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

        if ((newChi2 < /* m_ */16.f) || (newChi2 < oldChi2))
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

    if ((newChi2Total < /* m_ */9.f) || (newChi2Total < oldChi2Total))
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float MainFragmentRemovalAlgorithm::GetTotalEvidenceForMerge(const ClusterContact &clusterContact) const
{
    // Calculate a measure of the evidence that the daughter candidate cluster is a fragment of the parent candidate cluster:

    // 1. Layers in contact
    float contactEvidence(0.f);
    if (clusterContact.GetNContactLayers() > /* m_ */10)
    {
        contactEvidence = /* m_ */2.f;
    }
    else if (clusterContact.GetNContactLayers() > /* m_ */4)
    {
        contactEvidence = /* m_ */1.f;
    }
    else if (clusterContact.GetNContactLayers() > /* m_ */1)
    {
        contactEvidence = /* m_ */0.5f;
    }
    contactEvidence *= (/* m_ */1.f + clusterContact.GetContactFraction());

    // 2. Cone extrapolation
    float coneEvidence(0.f);
    if (clusterContact.GetConeFraction1() > /* m_ */0.5f)
    {
        coneEvidence = clusterContact.GetConeFraction1() + clusterContact.GetConeFraction2() + clusterContact.GetConeFraction3();

        static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
        const PseudoLayer daughterInnerLayer(clusterContact.GetDaughterCluster()->GetInnerPseudoLayer());

        if (daughterInnerLayer < nECalLayers)
            coneEvidence *= /* m_ */0.5f;
    }

    // 3. Track extrapolation
    float trackExtrapolationEvidence(0.f);
    if (clusterContact.GetClosestDistanceToHelix() < /* m_ */200.f)
    {
        trackExtrapolationEvidence = (/* m_ */200.f - clusterContact.GetClosestDistanceToHelix()) / /* m_ */100.f;

        if(clusterContact.GetClosestDistanceToHelix() < /* m_ */50.f)
            trackExtrapolationEvidence += (/* m_ */50.f - clusterContact.GetClosestDistanceToHelix()) / /* m_ */20.f;

        trackExtrapolationEvidence += (/* m_ */200.f - clusterContact.GetMeanDistanceToHelix()) / /* m_ */100.f;

        if(clusterContact.GetMeanDistanceToHelix() < /* m_ */50.f)
            trackExtrapolationEvidence += (/* m_ */50.f - clusterContact.GetClosestDistanceToHelix()) / /* m_ */50.f;
    }

    // 4. Distance of closest approach
    float distanceEvidence(0.f);
    if (clusterContact.GetDistanceToClosestHit() < /* m_ */100.)
    {
        distanceEvidence = (/* m_ */100.f - clusterContact.GetDistanceToClosestHit()) / /* m_ */100.f;
        distanceEvidence += clusterContact.GetCloseHitFraction1();
        distanceEvidence += /* m_ */2.f * clusterContact.GetCloseHitFraction2();
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
    const bool usingGlobalChi2(((newChi2 > oldChi2) && (newChi2 > 9.f)) || (globalChi2Evidence < chi2Evidence));

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
        layerCorrection = 2.f;
    }
    else if((correctionLayer > halfECal) && (correctionLayer <= nECalLayers))
    {
        layerCorrection = 0.f;
    }
    else if(correctionLayer > nECalLayers)
    {
        layerCorrection = -1.f;
    }
    else if(correctionLayer > nECalLayers + halfHCal)
    {
        layerCorrection = -2.f;
    }

    if((outerLayer - innerLayer < 4) && (innerLayer > 5))
        layerCorrection = -2.f;

    if(std::abs(static_cast<int>(correctionLayer) - static_cast<int>(nECalLayers)) < 4)
        layerCorrection = -3.f;

    // 2. Leaving cluster corrections - TODO
    float leavingCorrection(0.f);

    // 3. Energy correction
    float energyCorrection(0.f);

    if(daughterClusterEnergy < 3.)
        energyCorrection = daughterClusterEnergy - 3;

    // 4. Low energy fragment corrections
    float lowEnergyCorrection(0.f);

    if(daughterClusterEnergy < 1.5f)
    {
        const unsigned int nHitLayers(pDaughterCluster->GetOrderedCaloHitList().size());

        if(nHitLayers < 6)
        {
            lowEnergyCorrection += -1.f;

            if(nHitLayers < 4)
                lowEnergyCorrection += -1.f;
        }

        if(correctionLayer > nECalLayers)
            lowEnergyCorrection += -1.f;
    }

    // 5. Angular corrections
    float angularCorrection(0.f);
    const float radialDirectionCosine(pDaughterCluster->GetFitToAllHitsResult().GetRadialDirectionCosine());

    if(radialDirectionCosine < 0.75f)
        angularCorrection = -0.5f + (radialDirectionCosine - 0.75f) * 2.f;

    // 6. Photon cluster corrections
    float photonCorrection(0.f);

    if(pDaughterCluster->IsPhoton())
    {
        const float showerStart(pDaughterCluster->GetProfileShowerStart());
        const float photonFraction(pDaughterCluster->GetProfilePhotonFraction());

        if(daughterClusterEnergy > 2.f && showerStart < 5.f)
            photonCorrection = 10.f;

        if(daughterClusterEnergy > 2.f && showerStart < 2.5f)
            photonCorrection = 100.f;

        if(daughterClusterEnergy < 2.f && showerStart < 2.5f)
            photonCorrection = 5.f;

        if(daughterClusterEnergy < 2.f && showerStart < 2.5f && photonFraction < 0.8f)
            photonCorrection = 10.f;

        if(daughterClusterEnergy < 2.f && showerStart > 2.5f)
            photonCorrection = 2.f;

        if(daughterClusterEnergy < 0.5f && (showerStart > 2.5f || photonFraction > 1.f))
            photonCorrection = 2.f;

        if(daughterClusterEnergy < 1.f && showerStart > 2.5f)
            photonCorrection = 0.f;
    }

    const float requiredEvidence(usingGlobalChi2 ?
        layerCorrection + angularCorrection + energyCorrection + leavingCorrection + photonCorrection :
        layerCorrection + angularCorrection + energyCorrection + leavingCorrection + photonCorrection + lowEnergyCorrection);

    return std::max(0.5f, requiredEvidence);
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

        if ((++layerCounter > /* m_ */3) || (energySum > /* m_ */0.25f))
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
    m_contactWeight = 1.f;
    m_coneWeight = 1.f;
    m_distanceWeight = 1.f;
    m_trackExtrapolationWeight = 1.f;

    m_chi2Base = 5.f;
    m_globalChi2Penalty = 5.f;

    return STATUS_CODE_SUCCESS;
}
