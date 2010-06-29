/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the broken tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode BrokenTracksAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), Cluster::SortByInnerLayer);

    static const GeometryHelper *const pGeometryHelper(GeometryHelper::GetInstance());

    // Fit a straight line to start and end of all clusters in the current list
    ClusterFitRelationList clusterFitRelationList;

    for (ClusterVector::const_iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!ClusterHelper::CanMergeCluster(pCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        if (pCluster->GetNCaloHits() < m_minHitsInCluster)
            continue;

        ClusterFitResult startFitResult, endFitResult;
        const unsigned int nOccupiedLayers(pCluster->GetOrderedCaloHitList().size());
 
        if (nOccupiedLayers >= m_minOccupiedLayersForStartFit)
            (void) ClusterHelper::FitStart(pCluster, m_nStartLayersToFit, startFitResult);

        if (nOccupiedLayers >= m_minOccupiedLayersForEndFit)
            (void) ClusterHelper::FitEnd(pCluster, m_nEndLayersToFit, endFitResult);

        if ((startFitResult.IsFitSuccessful() && (startFitResult.GetRms() < m_maxFitRms)) ||
            (endFitResult.IsFitSuccessful() && (endFitResult.GetRms() < m_maxFitRms)))
        {
            clusterFitRelationList.push_back(new ClusterFitRelation(pCluster, startFitResult, endFitResult));
        }
    }

    // Loop over cluster combinations, comparing fit results to determine whether clusters should be merged
    for (ClusterFitRelationList::const_iterator iterI = clusterFitRelationList.begin(), iterIEnd = clusterFitRelationList.end(); iterI != iterIEnd; ++iterI)
    {
        if ((*iterI)->IsDefunct())
            continue;

        Cluster *pParentCluster((*iterI)->GetCluster());
        const ClusterHelper::ClusterFitResult &parentClusterFitResult((*iterI)->GetEndFitResult());

        if (!parentClusterFitResult.IsFitSuccessful() || (parentClusterFitResult.GetRms() > m_maxFitRms))
            continue;

        const PseudoLayer parentOuterLayer(pParentCluster->GetOuterPseudoLayer());
        const CartesianVector parentOuterCentroid(pParentCluster->GetCentroid(parentOuterLayer));

        ClusterFitRelation *pBestClusterFitRelation(NULL);
        float minDistanceProduct(std::numeric_limits<float>::max());

        // For each end fit, examine start fits for all other clusters
        for (ClusterFitRelationList::const_iterator iterJ = clusterFitRelationList.begin(), iterJEnd = clusterFitRelationList.end(); iterJ != iterJEnd; ++iterJ)
        {
            // Check to see if cluster has already been changed
            if ((*iterJ)->IsDefunct())
                continue;

            Cluster *pDaughterCluster((*iterJ)->GetCluster());
            const ClusterHelper::ClusterFitResult &daughterClusterFitResult((*iterJ)->GetStartFitResult());

            if (pParentCluster == pDaughterCluster)
                continue;

            if (!daughterClusterFitResult.IsFitSuccessful() || (daughterClusterFitResult.GetRms() > m_maxFitRms))
                continue;

            const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());
            const CartesianVector daughterInnerCentroid(pDaughterCluster->GetCentroid(daughterInnerLayer));

            // Cut on layer separation between the two clusters
            if ((daughterInnerLayer <= parentOuterLayer) || ((daughterInnerLayer - parentOuterLayer) > m_maxLayerDifference))
                continue;

            // Also cut on physical separation between the two clusters
            const CartesianVector centroidDifference(daughterInnerCentroid - parentOuterCentroid);

            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Check that cluster fit directions are compatible
            const float fitDirectionDotProduct(parentClusterFitResult.GetDirection().GetDotProduct(daughterClusterFitResult.GetDirection()));

            if (fitDirectionDotProduct < m_fitDirectionDotProductCut)
                continue;

            // Cut on distance of closest approach between start and end fits
            const bool isDaughterOutsideECal(pGeometryHelper->IsOutsideECal(daughterInnerCentroid));
            const float trackMergeCut(isDaughterOutsideECal ? m_trackMergeCutHcal : m_trackMergeCutEcal);
            float fitResultsClosestApproach(std::numeric_limits<float>::max());

            if (STATUS_CODE_SUCCESS != ClusterHelper::GetFitResultsClosestApproach(parentClusterFitResult, daughterClusterFitResult, fitResultsClosestApproach))
                continue;

            if (fitResultsClosestApproach > trackMergeCut)
                continue;

            // Cut on perpendicular distance between fit directions and centroid difference vector.
            const float trackMergePerpCut(isDaughterOutsideECal ? m_trackMergePerpCutHcal : m_trackMergePerpCutEcal);

            const CartesianVector parentCrossProduct(parentClusterFitResult.GetDirection().GetCrossProduct(centroidDifference));
            const float parentPerpendicularDistance(parentCrossProduct.GetMagnitude());

            const CartesianVector daughterCrossProduct(daughterClusterFitResult.GetDirection().GetCrossProduct(centroidDifference));
            const float daughterPerpendicularDistance(daughterCrossProduct.GetMagnitude());

            if ((parentPerpendicularDistance > trackMergePerpCut) && (daughterPerpendicularDistance > trackMergePerpCut))
                continue;

            const float distanceProduct(parentPerpendicularDistance * daughterPerpendicularDistance);

            if (distanceProduct < minDistanceProduct)
            {
                pBestClusterFitRelation = *iterJ;
                minDistanceProduct = distanceProduct;
            }
        }

        if (NULL != pBestClusterFitRelation)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, pBestClusterFitRelation->GetCluster()));
            pBestClusterFitRelation->SetAsDefunct();
        }
    }

    // Tidy up
    for (ClusterFitRelationList::const_iterator iter = clusterFitRelationList.begin(), iterEnd = clusterFitRelationList.end(); iter != iterEnd; ++iter)
        delete (*iter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BrokenTracksAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_minHitsInCluster = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    m_minOccupiedLayersForStartFit = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersForStartFit", m_minOccupiedLayersForStartFit));

    m_minOccupiedLayersForEndFit = 2;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersForEndFit", m_minOccupiedLayersForEndFit));

    m_nStartLayersToFit = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NStartLayersToFit", m_nStartLayersToFit));

    m_nEndLayersToFit = 8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NEndLayersToFit", m_nEndLayersToFit));

    m_maxFitRms = 15.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitRms", m_maxFitRms));

    m_maxLayerDifference = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxLayerDifference", m_maxLayerDifference));

    m_maxCentroidDifference = 2000.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxCentroidDifference", m_maxCentroidDifference));

    m_fitDirectionDotProductCut = 0.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FitDirectionDotProductCut", m_fitDirectionDotProductCut));

    m_trackMergeCutEcal = 25.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergeCutEcal", m_trackMergeCutEcal));

    m_trackMergeCutHcal = 25.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergeCutHcal", m_trackMergeCutHcal));

    m_trackMergePerpCutEcal = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergePerpCutEcal", m_trackMergePerpCutEcal));

    m_trackMergePerpCutHcal = 75.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TrackMergePerpCutHcal", m_trackMergePerpCutHcal));

    return STATUS_CODE_SUCCESS;
}
