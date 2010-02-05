/**
 *  @file   PandoraPFANew/src/Algorithms/FragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the fragment removal algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/FragmentRemovalAlgorithm.h"

using namespace pandora;

StatusCode FragmentRemovalAlgorithm::Run()
{
    bool isFirstPass(true), shouldRecalculate(true);
    Cluster *pBestParentCluster(NULL), *pBestDaughterCluster(NULL);

    ClusterList affectedClusters;
    ClusterContactMap clusterContactMap;

    while (shouldRecalculate)
    {
        shouldRecalculate = false;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterContactMap(isFirstPass, affectedClusters, clusterContactMap,
            pBestParentCluster, pBestDaughterCluster));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterMergingCandidates(clusterContactMap, pBestParentCluster,
            pBestDaughterCluster));

        if ((NULL != pBestParentCluster) && (NULL != pBestDaughterCluster))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster,
                pBestDaughterCluster));

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetAffectedClusters(clusterContactMap, pBestParentCluster,
                pBestDaughterCluster, affectedClusters));

            shouldRecalculate = true;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FragmentRemovalAlgorithm::GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters,
    ClusterContactMap &clusterContactMap, const Cluster *const pBestParentCluster, const Cluster *const pBestDaughterCluster) const
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

bool FragmentRemovalAlgorithm::PassesClusterContactCuts(const ClusterContact &clusterContact) const
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

StatusCode FragmentRemovalAlgorithm::GetClusterMergingCandidates(const ClusterContactMap &clusterContactMap, Cluster *&pBestParentCluster,
    Cluster *&pBestDaughterCluster) const
{
    for (ClusterContactMap::const_iterator iterI = clusterContactMap.begin(), iterIEnd = clusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = iterI->first;

        // Check to see if merging parent and daughter clusters would improve track-cluster compatibility
        if (!this->PassesPreselection(pDaughterCluster, iterI->second))
            continue;

        float highestExcessEvidence(0.f);
        const float daughterClusterEnergy(pDaughterCluster->GetHadronicEnergy());
        const PseudoLayer daughterCorrectionLayer(this->GetClusterCorrectionLayer(pDaughterCluster));

        for (ClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
        {
            ClusterContact clusterContact = *iterJ;

            if (pDaughterCluster != clusterContact.GetDaughterCluster())
                throw StatusCodeException(STATUS_CODE_FAILURE);

            const float totalEvidence(this->GetTotalEvidenceForMerge(clusterContact));
            const float requiredEvidence(this->GetRequiredEvidenceForMerge(daughterClusterEnergy, daughterCorrectionLayer, clusterContact));
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

bool FragmentRemovalAlgorithm::PassesPreselection(Cluster *const pDaughterCluster, const ClusterContactVector &clusterContactVector) const
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

    if ((newChi2Total < /* m_ */9.f) || (newChi2Total < oldChi2Total))
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float FragmentRemovalAlgorithm::GetTotalEvidenceForMerge(const ClusterContact &clusterContact) const
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

float FragmentRemovalAlgorithm::GetRequiredEvidenceForMerge(const float daughterClusterEnergy, const PseudoLayer correctionLayer,
    const ClusterContact &clusterContact) const
{
    return 0.f;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer FragmentRemovalAlgorithm::GetClusterCorrectionLayer(const Cluster *const pDaughterCluster) const
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

float FragmentRemovalAlgorithm::GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy) const
{
    static const float hadronicEnergyResolution(PandoraSettings::GetInstance()->GetHadronicEnergyResolution());

    if ((0. == trackEnergy) || (0. == hadronicEnergyResolution))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    const float sigmaE(hadronicEnergyResolution * trackEnergy / std::sqrt(trackEnergy));
    const float chi((clusterEnergy - trackEnergy) / sigmaE);

    return chi;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FragmentRemovalAlgorithm::GetAffectedClusters(const ClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
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

StatusCode FragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

FragmentRemovalAlgorithm::ClusterContact::ClusterContact(Cluster *const pDaughterCluster, Cluster *const pParentCluster) :
    m_pDaughterCluster(pDaughterCluster),
    m_pParentCluster(pParentCluster),
    m_parentClusterEnergy(pParentCluster->GetHadronicEnergy()),
    m_meanDistanceToHelix(std::numeric_limits<float>::max()),
    m_closestDistanceToHelix(std::numeric_limits<float>::max())
{
    m_distanceToClosestHit = ClusterHelper::GetDistanceToClosestHit(pDaughterCluster, pParentCluster);
    m_coneFraction1 = FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, /* m_ */0.90f);
    m_coneFraction2 = FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, /* m_ */0.95f);
    m_coneFraction3 = FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, /* m_ */0.985f);
    m_closeHitFraction1 = FragmentRemovalHelper::GetFractionOfCloseHits(pDaughterCluster, pParentCluster, /* m_ */100.f);
    m_closeHitFraction2 = FragmentRemovalHelper::GetFractionOfCloseHits(pDaughterCluster, pParentCluster, /* m_ */50.f);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, FragmentRemovalHelper::GetClusterContactDetails(pDaughterCluster, pParentCluster,
        /* m_ */2.f, m_nContactLayers, m_contactFraction));

    // Configure range of layers in which daughter cluster will be compared to helix fits
    const PseudoLayer startLayer(pDaughterCluster->GetInnerPseudoLayer());

    const PseudoLayer endLayer((pParentCluster->GetMipFraction() > /* m_ */0.8f) ?
        startLayer + /* m_ */20 : std::max(startLayer + /* m_ */20, pParentCluster->GetOuterPseudoLayer() + /* m_ */10));

    const unsigned int maxOccupiedLayers((pParentCluster->GetMipFraction() > /* m_ */0.8f) ?
        std::numeric_limits<unsigned int>::max() : /* m_ */9);

    // Calculate closest distance between daughter cluster and helix fits to parent associated tracks
    float trackEnergySum(0.);
    const TrackList &parentTrackList(pParentCluster->GetAssociatedTrackList());

    for (TrackList::const_iterator iter = parentTrackList.begin(), iterEnd = parentTrackList.end(); iter != iterEnd; ++iter)
    {
        trackEnergySum += (*iter)->GetEnergyAtDca();
        float meanDistanceToHelix(std::numeric_limits<float>::max()), closestDistanceToHelix(std::numeric_limits<float>::max());

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, FragmentRemovalHelper::GetClusterHelixDistance(pDaughterCluster,
            (*iter)->GetHelixFitAtECal(), startLayer, endLayer, maxOccupiedLayers, closestDistanceToHelix, meanDistanceToHelix));

        if (closestDistanceToHelix < m_closestDistanceToHelix)
        {
            m_meanDistanceToHelix = meanDistanceToHelix;
            m_closestDistanceToHelix = closestDistanceToHelix;
        }
    }

    m_parentTrackEnergy = trackEnergySum;
}
