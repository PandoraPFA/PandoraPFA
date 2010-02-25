/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.cc
 * 
 *  @brief  Implementation of the broken tracks algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/BrokenTracksAlgorithm.h"

#include <cmath>

using namespace pandora;

StatusCode BrokenTracksAlgorithm::Run()
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), Cluster::SortByInnerLayer);

    static const GeometryHelper *const pGeometryHelper(GeometryHelper::GetInstance());

    // Fit a straight line to start and end of all clusters in the current list
    ClusterFitRelationList startFitRelationList;
    ClusterFitRelationList endFitRelationList;

    for (ClusterVector::const_iterator iter = clusterVector.begin(), iterEnd = clusterVector.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (!ClusterHelper::CanMergeCluster(pCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        if ((pCluster->GetNCaloHits() < m_minHitsInCluster) || (pCluster->GetOrderedCaloHitList().size() < m_minOccupiedLayersInCluster))
            continue;

        ClusterFitResult startClusterFitResult;
        (void) ClusterHelper::FitStart(pCluster, m_nStartLayersToFit, startClusterFitResult);

        if (startClusterFitResult.IsFitSuccessful() && (startClusterFitResult.GetRms() < m_maxFitRms))
            startFitRelationList.push_back(new ClusterFitRelation(pCluster, startClusterFitResult));

        ClusterFitResult endClusterFitResult;
        (void) ClusterHelper::FitEnd(pCluster, m_nEndLayersToFit, endClusterFitResult);

        if (endClusterFitResult.IsFitSuccessful() && (endClusterFitResult.GetRms() < m_maxFitRms))
            endFitRelationList.push_back(new ClusterFitRelation(pCluster, endClusterFitResult));
    }

    // Loop over cluster combinations, comparing fit results to determine whether clusters should be merged
    for (ClusterFitRelationList::const_iterator iterI = endFitRelationList.begin(), iterIEnd = endFitRelationList.end(); iterI != iterIEnd; ++iterI)
    {
        if ((*iterI)->IsDefunct())
            continue;

        Cluster *pParentCluster((*iterI)->GetCluster());
        const ClusterHelper::ClusterFitResult &parentClusterFitResult((*iterI)->GetClusterFitResult());

        const PseudoLayer parentOuterLayer(pParentCluster->GetOuterPseudoLayer());
        const CartesianVector parentOuterCentroid(pParentCluster->GetCentroid(parentOuterLayer));

        ClusterFitRelation *pBestClusterFitRelation(NULL);
        float minDistanceProduct(std::numeric_limits<float>::max());

        // For each end fit, examine start fits for all other clusters
        for (ClusterFitRelationList::const_iterator iterJ = startFitRelationList.begin(), iterJEnd = startFitRelationList.end(); iterJ != iterJEnd; ++iterJ)
        {
            // Check to see if cluster has already been changed
            if ((*iterJ)->IsDefunct())
                continue;

            Cluster *pDaughterCluster((*iterJ)->GetCluster());
            const ClusterHelper::ClusterFitResult &daughterClusterFitResult((*iterJ)->GetClusterFitResult());

            if (pParentCluster == pDaughterCluster)
                continue;

            const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());
            const CartesianVector daughterInnerCentroid(pDaughterCluster->GetCentroid(daughterInnerLayer));

            // Cut on layer separation between the two clusters
            if ((daughterInnerLayer < parentOuterLayer) || ((daughterInnerLayer - parentOuterLayer) > m_maxLayerDifference))
                continue;

            // Also cut on physical separation between the two clusters
            const CartesianVector centroidDifference(daughterInnerCentroid - parentOuterCentroid);
            if (centroidDifference.GetMagnitude() > m_maxCentroidDifference)
                continue;

            // Check that cluster fit directions are compatible
            const float fitDirectionDotProduct(parentClusterFitResult.GetDirection().GetDotProduct(daughterClusterFitResult.GetDirection()));
            if (fitDirectionDotProduct > m_fitDirectionDotProductCut)
                continue;

            // Cut on distance of closest approach between start and end fits
            const bool isDaughterOutsideECal(pGeometryHelper->IsOutsideECal(daughterInnerCentroid));
            const float trackMergeCut(isDaughterOutsideECal ? m_trackMergeCutHcal : m_trackMergeCutEcal);

            if (ClusterHelper::GetFitResultsClosestApproach(parentClusterFitResult, daughterClusterFitResult) > trackMergeCut)
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
            Cluster *pBestDaughterCluster = pBestClusterFitRelation->GetCluster();
            pBestClusterFitRelation->SetAsDefunct();

            for (ClusterFitRelationList::const_iterator defunctIter = endFitRelationList.begin(), defunctIterEnd = endFitRelationList.end();
                defunctIter != defunctIterEnd; ++defunctIter)
            {
                if (!(*defunctIter)->IsDefunct() && ((*defunctIter)->GetCluster() == pBestDaughterCluster))
                    (*defunctIter)->SetAsDefunct();
            }

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pParentCluster, pBestDaughterCluster));
        }
    }

    // Tidy up
    for (ClusterFitRelationList::iterator iter = startFitRelationList.begin(); iter != startFitRelationList.end(); ++iter)
        delete (*iter);

    for (ClusterFitRelationList::iterator iter = endFitRelationList.begin(); iter != endFitRelationList.end(); ++iter)
        delete (*iter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode BrokenTracksAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_nStartLayersToFit = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NStartLayersToFit", m_nStartLayersToFit));

    m_nEndLayersToFit = 8;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NEndLayersToFit", m_nEndLayersToFit));

    m_maxFitRms = 15.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxFitRms", m_maxFitRms));

    m_canMergeMinMipFraction = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_minHitsInCluster = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    m_minOccupiedLayersInCluster = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinOccupiedLayersInCluster", m_minOccupiedLayersInCluster));

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
