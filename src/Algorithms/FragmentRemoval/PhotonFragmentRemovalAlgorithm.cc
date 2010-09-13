/**
 *  @file   PandoraPFANew/src/Algorithms/FragmentRemoval/PhotonFragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the photon fragment removal algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/FragmentRemoval/PhotonFragmentRemovalAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode PhotonFragmentRemovalAlgorithm::Run()
{
    unsigned int nPasses(0);
    bool isFirstPass(true), shouldRecalculate(true);

    ClusterList affectedClusters;
    PhotonClusterContactMap clusterContactMap;

    while ((nPasses++ < m_nMaxPasses) && shouldRecalculate)
    {
        shouldRecalculate = false;
        Cluster *pBestParentCluster(NULL), *pBestDaughterCluster(NULL);

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterContactMap(isFirstPass, affectedClusters, clusterContactMap));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetClusterMergingCandidates(clusterContactMap, pBestParentCluster,
            pBestDaughterCluster));

        if ((NULL != pBestParentCluster) && (NULL != pBestDaughterCluster))
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetAffectedClusters(clusterContactMap, pBestParentCluster,
                pBestDaughterCluster, affectedClusters));

            clusterContactMap.erase(clusterContactMap.find(pBestDaughterCluster));
            shouldRecalculate = true;

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster,
                pBestDaughterCluster));

            pBestParentCluster->SetIsFixedPhotonFlag(true);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentRemovalAlgorithm::GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters,
    PhotonClusterContactMap &clusterContactMap) const
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iterI = pClusterList->begin(), iterIEnd = pClusterList->end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = *iterI;

        // Identify whether cluster contacts need to be recalculated
        if (!isFirstPass)
        {
            if (affectedClusters.end() == affectedClusters.find(pDaughterCluster))
                continue;

            PhotonClusterContactMap::iterator pastEntryIter = clusterContactMap.find(pDaughterCluster);

            if (clusterContactMap.end() != pastEntryIter)
                clusterContactMap.erase(clusterContactMap.find(pDaughterCluster));
        }

        // Apply simple daughter selection cuts
        if (!pDaughterCluster->GetAssociatedTrackList().empty())
            continue;

        if ((pDaughterCluster->GetNCaloHits() < m_minDaughterCaloHits) || (pDaughterCluster->GetHadronicEnergy() < m_minDaughterHadronicEnergy))
            continue;

        if (!this->IsPhotonLike(pDaughterCluster))
            continue;

        const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

        // Calculate the cluster contact information
        for (ClusterList::const_iterator iterJ = pClusterList->begin(), iterJEnd = pClusterList->end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pParentCluster = *iterJ;

            if (pDaughterCluster == pParentCluster)
                continue;

            // Parent selection cuts
            if (!pParentCluster->GetAssociatedTrackList().empty())
                continue;

            if (pParentCluster->GetInnerPseudoLayer() > daughterInnerLayer + m_innerLayerTolerance)
                continue;

            if (pDaughterCluster->GetInitialDirection().GetCosOpeningAngle(pParentCluster->GetInitialDirection()) < m_minCosOpeningAngle)
                continue;

            if (!pParentCluster->IsPhotonFast())
                continue;

            // Evaluate cluster contact properties
            const PhotonClusterContact clusterContact(pDaughterCluster, pParentCluster);

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

bool PhotonFragmentRemovalAlgorithm::IsPhotonLike(Cluster *const pDaughterCluster) const
{
    if (pDaughterCluster->IsPhotonFast())
        return true;

    static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
    const PseudoLayer innerPseudoLayer(pDaughterCluster->GetInnerPseudoLayer());
    const ClusterHelper::ClusterFitResult &clusterFitResult(pDaughterCluster->GetFitToAllHitsResult());

    if ((innerPseudoLayer < nECalLayers) && (innerPseudoLayer < m_photonLikeMaxInnerLayer) &&
        (clusterFitResult.IsFitSuccessful()) && (clusterFitResult.GetRadialDirectionCosine() > m_photonLikeMinDCosR) &&
        (pDaughterCluster->GetShowerProfileStart() < m_photonLikeMaxShowerStart) &&
        (pDaughterCluster->GetShowerProfileDiscrepancy() < m_photonLikeMaxProfileDiscrepancy))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PhotonFragmentRemovalAlgorithm::PassesClusterContactCuts(const PhotonClusterContact &clusterContact) const
{
    if (clusterContact.GetDistanceToClosestHit() > m_contactCutMaxDistance)
        return false;

    if ((clusterContact.GetNContactLayers() > m_contactCutNLayers) ||
        (clusterContact.GetConeFraction() > m_contactCutConeFraction) ||
        (clusterContact.GetCloseHitFraction1() > m_contactCutCloseHitFraction1) ||
        (clusterContact.GetCloseHitFraction2() > m_contactCutCloseHitFraction2))
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentRemovalAlgorithm::GetClusterMergingCandidates(const PhotonClusterContactMap &clusterContactMap, Cluster *&pBestParentCluster,
    Cluster *&pBestDaughterCluster) const
{
    float highestEvidence(m_minEvidence);
    float highestEvidenceParentEnergy(0.);

    for (PhotonClusterContactMap::const_iterator iterI = clusterContactMap.begin(), iterIEnd = clusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = iterI->first;

        for (PhotonClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
        {
            PhotonClusterContact clusterContact = *iterJ;

            if (pDaughterCluster != clusterContact.GetDaughterCluster())
                throw StatusCodeException(STATUS_CODE_FAILURE);

            const float evidence(this->GetEvidenceForMerge(clusterContact));
            const float parentEnergy(clusterContact.GetParentCluster()->GetHadronicEnergy());

            if ((evidence > highestEvidence) || ((evidence == highestEvidence) && (parentEnergy > highestEvidenceParentEnergy)))
            {
                highestEvidence = evidence;
                pBestDaughterCluster = pDaughterCluster;
                pBestParentCluster = clusterContact.GetParentCluster();
                highestEvidenceParentEnergy = parentEnergy;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonFragmentRemovalAlgorithm::GetEvidenceForMerge(const PhotonClusterContact &clusterContact) const
{
    // Calculate a measure of the evidence that the daughter candidate cluster is a fragment of the parent candidate cluster:

    // 1. Layers in contact
    float contactEvidence(0.f);
    if ((clusterContact.GetNContactLayers() > m_contactEvidenceNLayers) && (clusterContact.GetContactFraction() > m_contactEvidenceFraction))
    {
        contactEvidence = clusterContact.GetContactFraction();
    }

    // 2. Cone extrapolation
    float coneEvidence(0.f);
    if (clusterContact.GetConeFraction() > m_coneEvidenceFraction)
    {
        coneEvidence = clusterContact.GetConeFraction();
    }

    // 3. Distance of closest approach
    float distanceEvidence(0.f);
    if (clusterContact.GetDistanceToClosestHit() < m_distanceEvidence1)
    {
        distanceEvidence = (m_distanceEvidence1 - clusterContact.GetDistanceToClosestHit()) / m_distanceEvidence1d;
        distanceEvidence += m_distanceEvidenceCloseFraction1Multiplier * clusterContact.GetCloseHitFraction1();
        distanceEvidence += m_distanceEvidenceCloseFraction2Multiplier * clusterContact.GetCloseHitFraction2();
    }

    return ((m_contactWeight * contactEvidence) + (m_coneWeight * coneEvidence) + (m_distanceWeight * distanceEvidence));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentRemovalAlgorithm::GetAffectedClusters(const PhotonClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
    Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const
{
    if (clusterContactMap.end() == clusterContactMap.find(pBestDaughterCluster))
        return STATUS_CODE_FAILURE;

    affectedClusters.clear();
    for (PhotonClusterContactMap::const_iterator iterI = clusterContactMap.begin(), iterIEnd = clusterContactMap.end(); iterI != iterIEnd; ++iterI)
    {
        // Store addresses of all clusters that were in contact with the newly deleted daughter cluster
        if (iterI->first == pBestDaughterCluster)
        {
            for (PhotonClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
            {
                affectedClusters.insert(iterJ->GetParentCluster());
            }
            continue;
        }

        // Also store addresses of all clusters that contained either the parent or daughter clusters in their own ClusterContactVectors
        for (PhotonClusterContactVector::const_iterator iterJ = iterI->second.begin(), iterJEnd = iterI->second.end(); iterJ != iterJEnd; ++iterJ)
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
//------------------------------------------------------------------------------------------------------------------------------------------

PhotonClusterContact::PhotonClusterContact(Cluster *const pDaughterCluster, Cluster *const pParentCluster) :
    m_pDaughterCluster(pDaughterCluster),
    m_pParentCluster(pParentCluster),
    m_nContactLayers(0),
    m_contactFraction(0.f),
    m_closeHitFraction1(0.f),
    m_closeHitFraction2(0.f),
    m_distanceToClosestHit(std::numeric_limits<float>::max())
{
    m_coneFraction = FragmentRemovalHelper::GetFractionOfHitsInCone(pDaughterCluster, pParentCluster, m_coneCosineHalfAngle);

    (void) FragmentRemovalHelper::GetClusterContactDetails(pDaughterCluster, pParentCluster, m_distanceThreshold, m_nContactLayers, m_contactFraction);

    this->HitDistanceComparison(pDaughterCluster, pParentCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonClusterContact::HitDistanceComparison(Cluster *const pDaughterCluster, Cluster *const pParentCluster)
{
    static const float closeHitDistance1Squared(m_closeHitDistance1 * m_closeHitDistance1);
    static const float closeHitDistance2Squared(m_closeHitDistance2 * m_closeHitDistance2);

    // Calculate all hit distance properties in a single loop, for efficiency
    unsigned int nCloseHits1(0), nCloseHits2(0);
    float minDistanceSquared(std::numeric_limits<float>::max());

    const OrderedCaloHitList &orderedCaloHitListI(pDaughterCluster->GetOrderedCaloHitList());
    const OrderedCaloHitList &orderedCaloHitListJ(pParentCluster->GetOrderedCaloHitList());

    // Loop over hits in daughter cluster
    for (OrderedCaloHitList::const_iterator iterI = orderedCaloHitListI.begin(), iterIEnd = orderedCaloHitListI.end(); iterI != iterIEnd; ++iterI)
    {
        for (CaloHitList::const_iterator hitIterI = iterI->second->begin(), hitIterIEnd = iterI->second->end(); hitIterI != hitIterIEnd; ++hitIterI)
        {
            bool isCloseHit1(false), isCloseHit2(false);
            const CartesianVector &positionVectorI((*hitIterI)->GetPositionVector());

            // Compare each hit in daughter cluster with those in parent cluster
            for (OrderedCaloHitList::const_iterator iterJ = orderedCaloHitListJ.begin(), iterJEnd = orderedCaloHitListJ.end(); iterJ != iterJEnd; ++iterJ)
            {
                for (CaloHitList::const_iterator hitIterJ = iterJ->second->begin(), hitIterJEnd = iterJ->second->end(); hitIterJ != hitIterJEnd; ++hitIterJ)
                {
                    const float distanceSquared((positionVectorI - (*hitIterJ)->GetPositionVector()).GetMagnitudeSquared());

                    if (!isCloseHit1 && (distanceSquared < closeHitDistance1Squared))
                        isCloseHit1 = true;

                    if (!isCloseHit2 && (distanceSquared < closeHitDistance2Squared))
                        isCloseHit2 = true;

                    if (distanceSquared < minDistanceSquared)
                        minDistanceSquared = distanceSquared;
                }
            }

            if (isCloseHit1)
                nCloseHits1++;

            if (isCloseHit2)
                nCloseHits2++;
        }
    }

    const unsigned int nDaughterCaloHits(pDaughterCluster->GetNCaloHits());

    if (0 == nDaughterCaloHits)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_distanceToClosestHit = std::sqrt(minDistanceSquared);
    m_closeHitFraction1 = static_cast<float>(nCloseHits1) / static_cast<float>(nDaughterCaloHits);
    m_closeHitFraction2 = static_cast<float>(nCloseHits2) / static_cast<float>(nDaughterCaloHits);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonClusterContact::m_distanceThreshold = 2.f;
float PhotonClusterContact::m_coneCosineHalfAngle = 0.95f;
float PhotonClusterContact::m_closeHitDistance1 = 40.f;
float PhotonClusterContact::m_closeHitDistance2 = 20.f;

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_nMaxPasses = 200;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NMaxPasses", m_nMaxPasses));

    // Initial cluster candidate selection
    m_minDaughterCaloHits = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDaughterCaloHits", m_minDaughterCaloHits));

    m_minDaughterHadronicEnergy = 0.025f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinDaughterHadronicEnergy", m_minDaughterHadronicEnergy));

    m_innerLayerTolerance = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InnerLayerTolerance", m_innerLayerTolerance));

    m_minCosOpeningAngle = 0.95f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCosOpeningAngle", m_minCosOpeningAngle));

    // Photon-like cuts
    m_photonLikeMaxInnerLayer = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLikeMaxInnerLayer", m_photonLikeMaxInnerLayer));

    m_photonLikeMinDCosR = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLikeMinDCosR", m_photonLikeMinDCosR));

    m_photonLikeMaxShowerStart = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLikeMaxShowerStart", m_photonLikeMaxShowerStart));

    m_photonLikeMaxProfileDiscrepancy = 0.75f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonLikeMaxProfileDiscrepancy", m_photonLikeMaxProfileDiscrepancy));

    // Cluster contact cuts
    m_contactCutMaxDistance = 20.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutMaxDistance", m_contactCutMaxDistance));

    m_contactCutNLayers = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutNLayers", m_contactCutNLayers));

    m_contactCutConeFraction = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutConeFraction", m_contactCutConeFraction));

    m_contactCutCloseHitFraction1 = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutCloseHitFraction1", m_contactCutCloseHitFraction1));

    m_contactCutCloseHitFraction2 = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactCutCloseHitFraction2", m_contactCutCloseHitFraction2));

    // Total evidence: Contact evidence
    m_contactEvidenceNLayers = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidenceNLayers", m_contactEvidenceNLayers));

    m_contactEvidenceFraction = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ContactEvidenceFraction", m_contactEvidenceFraction));

    // Cone evidence
    m_coneEvidenceFraction = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeEvidenceFraction", m_coneEvidenceFraction));

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

    m_minEvidence = 2.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinEvidence", m_minEvidence));

    // Photon cluster contact parameters
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "DistanceThreshold", PhotonClusterContact::m_distanceThreshold));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeCosineHalfAngle", PhotonClusterContact::m_coneCosineHalfAngle));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CloseHitDistance1", PhotonClusterContact::m_closeHitDistance1));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CloseHitDistance2", PhotonClusterContact::m_closeHitDistance2));

    return STATUS_CODE_SUCCESS;
}
