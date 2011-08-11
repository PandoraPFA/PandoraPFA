/**
 *  @file   PandoraPFANew/FineGranularityContent/src/ParticleId/MuonReconstructionAlgorithm.cc
 * 
 *  @brief  Implementation of the muon reconstruction algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ParticleId/MuonReconstructionAlgorithm.h"

using namespace pandora;

StatusCode MuonReconstructionAlgorithm::Run()
{
    // Store names of input track and calo hit lists
    std::string inputTrackListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackListName(*this, inputTrackListName));

    std::string inputCaloHitListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentCaloHitListName(*this, inputCaloHitListName));

    // Cluster the muon hits
    std::string muonClusterListName;
    const ClusterList *pMuonClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentCaloHitList(*this, m_muonCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_muonClusteringAlgName, pMuonClusterList,
        muonClusterListName));

    // If muon clusters formed, complete the muon reconstruction
    if (!pMuonClusterList->empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssociateMuonTracks(pMuonClusterList));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHits(pMuonClusterList, inputCaloHitListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateMuonPfos(pMuonClusterList));
    }

    // Tidy up
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->TidyLists(inputTrackListName, inputCaloHitListName, muonClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::AssociateMuonTracks(const ClusterList *const pMuonClusterList) const
{
    static const float coilMidPointR(0.5f * (GeometryHelper::GetCoilOuterRadius() + GeometryHelper::GetCoilInnerRadius()));
    static const float muonBarrelBField(GeometryHelper::GetBField(CartesianVector(GeometryHelper::GetMuonBarrelParameters().GetInnerRCoordinate(), 0.f, 0.f)));
    static const float muonEndCapBField(GeometryHelper::GetBField(CartesianVector(0.f, 0.f, std::fabs(GeometryHelper::GetMuonEndCapParameters().GetInnerZCoordinate()))));

    const TrackList *pTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    for (ClusterList::const_iterator iter = pMuonClusterList->begin(), iterEnd = pMuonClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        // Simple cuts on cluster properties
        if (pCluster->GetNCaloHits() > m_maxClusterCaloHits)
            continue;

        if (pCluster->GetOrderedCaloHitList().size() < m_minClusterOccupiedLayers)
            continue;

        if ((pCluster->GetOuterPseudoLayer() - pCluster->GetInnerPseudoLayer() + 1) < m_minClusterLayerSpan)
            continue;

        // Get direction of the cluster
        ClusterHelper::ClusterFitResult clusterFitResult;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, ClusterHelper::FitStart(pCluster, m_nClusterLayersToFit, clusterFitResult));

        if (!clusterFitResult.IsFitSuccessful())
            continue;

        const CartesianVector clusterInnerCentroid(pCluster->GetCentroid(pCluster->GetInnerPseudoLayer()));
        const bool isPositiveZ(clusterInnerCentroid.GetZ() > 0.f);

        // Loop over all non-associated tracks in the current track list to find bestTrack
        Track *pBestTrack(NULL);
        float bestTrackEnergy(0.f);
        float bestDistanceToTrack(m_maxDistanceToTrack);

        for (TrackList::const_iterator iterT = pTrackList->begin(), iterTEnd = pTrackList->end(); iterT != iterTEnd; ++iterT)
        {
            Track *pTrack = *iterT;

            // Simple cuts on track properties
            if (pTrack->HasAssociatedCluster() || !pTrack->CanFormPfo())
                continue;

            if (!pTrack->GetDaughterTrackList().empty())
                continue;

            if (pTrack->GetEnergyAtDca() < m_minTrackCandidateEnergy)
                continue;

            if (pTrack->IsProjectedToEndCap() && (pTrack->GetTrackStateAtCalorimeter().GetPosition().GetZ() * clusterInnerCentroid.GetZ() < 0.f))
                continue;

            // Extract track helix fit
            const Helix *const pHelix(pTrack->GetHelixFitAtCalorimeter());

            // Compare cluster and helix directions
            CartesianVector muonEntryPoint(0.f, 0.f, 0.f);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetMuonEntryPoint(pHelix, isPositiveZ, muonEntryPoint));

            bool isInBarrel(false);
            const float muonEntryR(std::sqrt(muonEntryPoint.GetX() * muonEntryPoint.GetX() + muonEntryPoint.GetY() * muonEntryPoint.GetY()));

            if (muonEntryR > coilMidPointR)
            {
                isInBarrel = true;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetPointOnCircle(coilMidPointR, pHelix->GetReferencePoint(), muonEntryPoint));
            }

            // Create helix that can be propagated in muon system, outside central detector
            const float externalBField(isInBarrel ? muonBarrelBField : muonEndCapBField);

            const Helix externalHelix(muonEntryPoint, pHelix->GetExtrapolatedMomentum(muonEntryPoint),
                (externalBField < 0.f) ? -pHelix->GetCharge() : pHelix->GetCharge(), std::fabs(externalBField));

            CartesianVector correctedMuonEntryPoint(0.f, 0.f, 0.f);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetMuonEntryPoint(&externalHelix, isPositiveZ, correctedMuonEntryPoint));

            const CartesianVector helixDirection(externalHelix.GetExtrapolatedMomentum(correctedMuonEntryPoint).GetUnitVector());
            const float helixClusterCosAngle(helixDirection.GetCosOpeningAngle(clusterFitResult.GetDirection()));

            if (helixClusterCosAngle < m_minHelixClusterCosAngle)
                continue;

            // Calculate separation of helix and cluster inner centroid
            CartesianVector helixSeparation(0.f, 0.f, 0.f);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, externalHelix.GetDistanceToPoint(clusterInnerCentroid, helixSeparation));

            const float distanceToTrack(helixSeparation.GetZ());
 
            if ((distanceToTrack < bestDistanceToTrack) || ((distanceToTrack == bestDistanceToTrack) && (pTrack->GetEnergyAtDca() > bestTrackEnergy)))
            {
                pBestTrack = pTrack;
                bestDistanceToTrack = distanceToTrack;
                bestTrackEnergy = pTrack->GetEnergyAtDca();
            }
        }

        if (NULL != pBestTrack)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddTrackClusterAssociation(*this, pBestTrack, pCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::GetMuonEntryPoint(const Helix *const pHelix, const bool isPositiveZ, CartesianVector &muonEntryPoint) const
{
    static const float muonEndCapInnerZ(std::fabs(GeometryHelper::GetMuonEndCapParameters().GetInnerZCoordinate()));

    float minGenericTime(std::numeric_limits<float>::max());
    const CartesianVector &referencePoint(pHelix->GetReferencePoint());

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetPointInZ(isPositiveZ ? muonEndCapInnerZ : -muonEndCapInnerZ,
        referencePoint, muonEntryPoint, minGenericTime));

    static const unsigned int muonBarrelInnerSymmetry = GeometryHelper::GetMuonBarrelParameters().GetInnerSymmetryOrder();
    static const float muonBarrelInnerPhi = GeometryHelper::GetMuonBarrelParameters().GetInnerPhiCoordinate();
    static const float muonBarrelInnerR = GeometryHelper::GetMuonBarrelParameters().GetInnerRCoordinate();

    if (muonBarrelInnerSymmetry > 0)
    {
        static const float pi(std::acos(-1.f));
        static const float twopi_n = 2.f * pi / (static_cast<float>(muonBarrelInnerSymmetry));

        for (unsigned int i = 0; i < muonBarrelInnerSymmetry; ++i)
        {
            const float phi(twopi_n * static_cast<float>(i) + muonBarrelInnerPhi);

            CartesianVector barrelEntryPoint(0.f, 0.f, 0.f);
            float genericTime(std::numeric_limits<float>::max());

            const StatusCode statusCode(pHelix->GetPointInXY(muonBarrelInnerR * std::cos(phi), muonBarrelInnerR * std::sin(phi),
                 std::cos(phi + 0.5f * pi), std::sin(phi + 0.5f * pi), referencePoint, barrelEntryPoint, genericTime));

            if ((STATUS_CODE_SUCCESS == statusCode) && (genericTime < minGenericTime))
            {
                minGenericTime = genericTime;
                muonEntryPoint = barrelEntryPoint;
            }
        }
    }
    else
    {
        CartesianVector barrelEntryPoint(0.f, 0.f, 0.f);
        float genericTime(std::numeric_limits<float>::max());

        const StatusCode statusCode(pHelix->GetPointOnCircle(muonBarrelInnerR, referencePoint, barrelEntryPoint, genericTime));

        if ((STATUS_CODE_SUCCESS == statusCode) && (genericTime < minGenericTime))
        {
            minGenericTime = genericTime;
            muonEntryPoint = barrelEntryPoint;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::AddCaloHits(const ClusterList *const pMuonClusterList, const std::string &inputCaloHitListName) const
{
    static const float hCalEndCapInnerR(GeometryHelper::GetHCalEndCapParameters().GetInnerRCoordinate());
    static const float eCalEndCapInnerR(GeometryHelper::GetECalEndCapParameters().GetInnerRCoordinate());

    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCaloHitList(*this, inputCaloHitListName, pCaloHitList));

    OrderedCaloHitList orderedCaloHitList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(*pCaloHitList));

    for (ClusterList::const_iterator clusterIter = pMuonClusterList->begin(), clusterIterEnd = pMuonClusterList->end(); clusterIter != clusterIterEnd; ++clusterIter)
    {
        Cluster *pCluster = *clusterIter;

        // Check track associations
        const TrackList &trackList(pCluster->GetAssociatedTrackList());

        if (trackList.size() != m_nExpectedTracksPerCluster)
            continue;

        Track *pTrack = *(trackList.begin());
        const Helix *const pHelix(pTrack->GetHelixFitAtCalorimeter());

        for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end(); layerIter != layerIterEnd; ++layerIter)
        {
            TrackDistanceInfoVector trackDistanceInfoVector;
            unsigned int nHitsInRegion1(0), nHitsInRegion2(0);

            for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                CaloHit *pCaloHit = *hitIter;

                if ((!m_shouldClusterIsolatedHits && pCaloHit->IsIsolated()) || !PandoraContentApi::IsCaloHitAvailable(*this, pCaloHit))
                    continue;

                const CartesianVector &caloHitPosition(pCaloHit->GetPositionVector());
                const CartesianVector helixDirection(pHelix->GetExtrapolatedMomentum(caloHitPosition).GetUnitVector());

                if (pCaloHit->GetExpectedDirection().GetCosOpeningAngle(helixDirection) < m_minHelixCaloHitCosAngle)
                    continue;

                if (ENDCAP == pCaloHit->GetDetectorRegion())
                {
                    CartesianVector intersectionPoint(0.f, 0.f, 0.f);
                    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetPointInZ(caloHitPosition.GetZ(), pHelix->GetReferencePoint(), intersectionPoint));

                    const float helixR(std::sqrt(intersectionPoint.GetX() * intersectionPoint.GetX() + intersectionPoint.GetY() * intersectionPoint.GetY()));

                    if ((HCAL == pCaloHit->GetHitType()) && (helixR < hCalEndCapInnerR))
                        continue;

                    if ((ECAL == pCaloHit->GetHitType()) && (helixR < eCalEndCapInnerR))
                        continue;
                }

                CartesianVector helixSeparation(0.f, 0.f, 0.f);
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pHelix->GetDistanceToPoint(caloHitPosition, helixSeparation));

                const float cellLengthScale(pCaloHit->GetCellLengthScale());

                if (0.f == cellLengthScale)
                    continue;

                const float genericDistance(helixSeparation.GetMagnitude() / cellLengthScale);
                trackDistanceInfoVector.push_back(TrackDistanceInfo(pCaloHit, genericDistance));

                if (genericDistance < m_region1GenericDistance)
                {
                    ++nHitsInRegion1;
                }
                else if (genericDistance < m_region2GenericDistance)
                {
                    ++nHitsInRegion2;
                }
            }

            const bool isIsolated((nHitsInRegion1 >= m_isolatedMinRegion1Hits) && (nHitsInRegion2 <= m_isolatedMaxRegion2Hits));
            std::sort(trackDistanceInfoVector.begin(), trackDistanceInfoVector.end(), MuonReconstructionAlgorithm::SortByDistanceToTrack);

            for (TrackDistanceInfoVector::const_iterator iter = trackDistanceInfoVector.begin(), iterEnd = trackDistanceInfoVector.end(); iter != iterEnd; ++iter)
            {
                if ((iter->second > m_maxGenericDistance) || (isIsolated && (iter->second > m_isolatedMaxGenericDistance)))
                    break;

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pCluster, iter->first));

                if (!isIsolated)
                    break;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::CreateMuonPfos(const ClusterList *const pMuonClusterList) const
{
    const PfoList *pPfoList = NULL; std::string pfoListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::CreateTemporaryPfoListAndSetCurrent(*this, pPfoList, pfoListName));

    for (ClusterList::const_iterator iter = pMuonClusterList->begin(), iterEnd = pMuonClusterList->end(); iter != iterEnd; ++iter)
    {
        PandoraContentApi::ParticleFlowObject::Parameters pfoParameters;

        Cluster *pCluster = *iter;
        pfoParameters.m_clusterList.insert(pCluster);

        // Consider associated tracks
        const TrackList &trackList(pCluster->GetAssociatedTrackList());

        if (trackList.size() != m_nExpectedTracksPerCluster)
            continue;

        Track *pTrack = *(trackList.begin());
        pfoParameters.m_trackList.insert(pTrack);

        // Examine track relationships
        const TrackList &parentTrackList(pTrack->GetParentTrackList());

        if ((parentTrackList.size() > m_nExpectedParentTracks) || !pTrack->GetDaughterTrackList().empty() || !pTrack->GetSiblingTrackList().empty())
        {
            std::cout << "MuonReconstructionAlgorithm: invalid/unexpected track relationships for muon." << std::endl;
            continue;
        }

        if (!parentTrackList.empty())
        {
            pfoParameters.m_trackList.insert(parentTrackList.begin(), parentTrackList.end());
        }

        pfoParameters.m_energy = pTrack->GetEnergyAtDca();
        pfoParameters.m_momentum = pTrack->GetMomentumAtDca();
        pfoParameters.m_mass = pTrack->GetMass();
        pfoParameters.m_charge = pTrack->GetCharge();
        pfoParameters.m_particleId = (pTrack->GetCharge() > 0) ? MU_PLUS : MU_MINUS;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ParticleFlowObject::Create(*this, pfoParameters));
    }

    if (!pPfoList->empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SavePfoList(*this, m_outputMuonPfoListName));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentPfoList(*this, m_outputMuonPfoListName));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::TidyLists(const std::string &inputTrackListName, const std::string &inputCaloHitListName,
    const std::string &muonClusterListName) const
{
    // Make list of all tracks, clusters and calo hits in muon pfos
    TrackList pfoTrackList; CaloHitList pfoCaloHitList; ClusterList pfoClusterList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetPfoComponents(pfoTrackList, pfoCaloHitList, pfoClusterList));

    // Save the muon-removed track list
    const TrackList *pInputTrackList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetTrackList(*this, inputTrackListName, pInputTrackList));

    TrackList outputTrackList(*pInputTrackList);

    if (!pfoTrackList.empty())
    {
        for (TrackList::const_iterator iter = pfoTrackList.begin(), iterEnd = pfoTrackList.end(); iter != iterEnd; ++iter)
            outputTrackList.erase(*iter);
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveTrackList(*this, outputTrackList, m_outputTrackListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentTrackList(*this, m_outputTrackListName));

    // Save the muon-removed calo hit list
    const CaloHitList *pInputCaloHitList = NULL;
    const CaloHitList *pMuonCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCaloHitList(*this, inputCaloHitListName, pInputCaloHitList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCaloHitList(*this, m_muonCaloHitListName, pMuonCaloHitList));

    CaloHitList outputCaloHitList(*pInputCaloHitList);
    CaloHitList outputMuonCaloHitList(*pMuonCaloHitList);

    if (!pfoCaloHitList.empty())
    {
        for (CaloHitList::const_iterator iter = pfoCaloHitList.begin(), iterEnd = pfoCaloHitList.end(); iter != iterEnd; ++iter)
        {
            outputCaloHitList.erase(*iter);
            outputMuonCaloHitList.erase(*iter);
        }
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveCaloHitList(*this, outputMuonCaloHitList, m_outputMuonCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveCaloHitList(*this, outputCaloHitList, m_outputCaloHitListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentCaloHitList(*this, m_outputCaloHitListName));

    // Save the muon cluster list
    if (!pfoClusterList.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, muonClusterListName, m_outputMuonClusterListName, pfoClusterList));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::GetPfoComponents(TrackList &pfoTrackList, CaloHitList &pfoCaloHitList, ClusterList &pfoClusterList) const
{
    const PfoList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pPfoList));

    for (PfoList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end(); iter != iterEnd; ++iter)
    {
        ParticleFlowObject *pPfo = *iter;
        const int particleId(pPfo->GetParticleId());

        if ((particleId != MU_MINUS) && (particleId != MU_PLUS))
            continue;

        pfoTrackList.insert(pPfo->GetTrackList().begin(), pPfo->GetTrackList().end());
        pfoClusterList.insert(pPfo->GetClusterList().begin(), pPfo->GetClusterList().end());
    }

    for (ClusterList::const_iterator iter = pfoClusterList.begin(), iterEnd = pfoClusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        pCluster->GetOrderedCaloHitList().GetCaloHitList(pfoCaloHitList);
        pfoCaloHitList.insert(pCluster->GetIsolatedCaloHitList().begin(), pCluster->GetIsolatedCaloHitList().end());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MuonReconstructionAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Input lists
    m_muonCaloHitListName = "MuonYokeHits";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "m_muonCaloHitListName", m_muonCaloHitListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle,
        "MuonClusterFormation", m_muonClusteringAlgName));

    // Cluster-track association
    m_maxClusterCaloHits = 30;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterCaloHits", m_maxClusterCaloHits));

    m_minClusterOccupiedLayers = 8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterOccupiedLayers", m_minClusterOccupiedLayers));

    m_minClusterLayerSpan = 8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinClusterLayerSpan", m_minClusterLayerSpan));

    m_nClusterLayersToFit = 100;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NClusterLayersToFit", m_nClusterLayersToFit));

    m_maxClusterFitChi2 = 4.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxClusterFitChi2", m_maxClusterFitChi2));

    m_maxDistanceToTrack = 200.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxDistanceToTrack", m_maxDistanceToTrack));

    m_minTrackCandidateEnergy = 7.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinTrackCandidateEnergy", m_minTrackCandidateEnergy));

    m_minHelixClusterCosAngle = 0.98f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHelixClusterCosAngle", m_minHelixClusterCosAngle));

    // Addition of ecal/hcal hits
    m_nExpectedTracksPerCluster = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NExpectedTracksPerCluster", m_nExpectedTracksPerCluster));

    if (0 == m_nExpectedTracksPerCluster)
        return STATUS_CODE_INVALID_PARAMETER;

    m_nExpectedParentTracks = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NExpectedParentTracks", m_nExpectedParentTracks));

    m_minHelixCaloHitCosAngle = 0.95f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHelixCaloHitCosAngle", m_minHelixCaloHitCosAngle));

    m_region1GenericDistance = 3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Region1GenericDistance", m_region1GenericDistance));

    m_region2GenericDistance = 6.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Region2GenericDistance", m_region2GenericDistance));

    m_isolatedMinRegion1Hits = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolatedMinRegion1Hits", m_isolatedMinRegion1Hits));

    m_isolatedMaxRegion2Hits = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolatedMaxRegion2Hits", m_isolatedMaxRegion2Hits));

    m_maxGenericDistance = 6.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxGenericDistance", m_maxGenericDistance));

    m_isolatedMaxGenericDistance = 3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "IsolatedMaxGenericDistance", m_isolatedMaxGenericDistance));

    // Output lists
    m_outputMuonClusterListName = "MuonClusters";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputMuonClusterListName", m_outputMuonClusterListName));

    m_outputMuonPfoListName = "PrimaryPfoList";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputMuonPfoListName", m_outputMuonPfoListName));

    m_outputTrackListName = "MuonRemovedTracks";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputTrackListName", m_outputTrackListName));

    m_outputCaloHitListName = "MuonRemovedCaloHits";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputCaloHitListName", m_outputCaloHitListName));

    m_outputMuonCaloHitListName = "MuonRemovedYokeHits";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "OutputMuonCaloHitListName", m_outputMuonCaloHitListName));

    m_shouldClusterIsolatedHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldClusterIsolatedHits", m_shouldClusterIsolatedHits));

    return STATUS_CODE_SUCCESS;
}
