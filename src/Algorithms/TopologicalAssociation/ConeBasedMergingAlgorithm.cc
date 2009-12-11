/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/ConeBasedMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the cone based merging algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/ConeBasedMergingAlgorithm.h"

using namespace pandora;

StatusCode ConeBasedMergingAlgorithm::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    // Then prepare clusters for this merging algorithm
    ClusterVector daughterVector;
    ClusterFitResultMap parentFitResultMap;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareClusters(daughterVector, parentFitResultMap));

    // Loop over daughter candidates and, for each, examine all possible parents
    for (ClusterVector::reverse_iterator iterI = daughterVector.rbegin(), iterIEnd = daughterVector.rend(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pDaughterCluster = *iterI;

        if (NULL == pDaughterCluster)
            continue;

        Cluster *pBestParentCluster(NULL);
        float highestConeFraction(m_minConeFraction);
        const PseudoLayer daughterInnerLayer(pDaughterCluster->GetInnerPseudoLayer());

        for (ClusterFitResultMap::const_iterator iterJ = parentFitResultMap.begin(); iterJ != parentFitResultMap.end(); ++iterJ)
        {
            Cluster *pParentCluster = iterJ->first;

            if (pDaughterCluster == pParentCluster)
                continue;

            // Cut on inner layer separation
            const CartesianVector parentInnerLayerCentroid(pParentCluster->GetCentroid(pParentCluster->GetInnerPseudoLayer()));
            const CartesianVector daughterInnerLayerCentroid(pDaughterCluster->GetCentroid(daughterInnerLayer));

            const float innerLayerSeparation((parentInnerLayerCentroid - daughterInnerLayerCentroid).GetMagnitude());

            if (innerLayerSeparation > m_maxInnerLayerSeparation)
                continue;

            if (pParentCluster->GetAssociatedTrackList().empty() && (innerLayerSeparation > m_maxInnerLayerSeparationNoTrack))
                continue;

            // 
            const ClusterFitResult &mipFitResult = iterJ->second;
            const float fractionInCone(this->GetFractionInCone(pDaughterCluster, mipFitResult, m_coneCosineHalfAngle));

            if (fractionInCone > highestConeFraction)
            {
                highestConeFraction = fractionInCone;
                pBestParentCluster = pParentCluster;
            }
        }

        if (NULL != pBestParentCluster)
        {
            (*iterI) = NULL;
            parentFitResultMap.erase(pDaughterCluster);
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBestParentCluster, pDaughterCluster));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeBasedMergingAlgorithm::PrepareClusters(ClusterVector &daughterVector, ClusterFitResultMap &parentFitResultMap) const
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    // Store cluster list in a vector and sort by descending inner layer, and by number of hits within a layer
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        if ((*iter)->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        if (ClusterHelper::CanMergeCluster(*iter, m_canMergeMinMipFraction, m_canMergeMaxRms))
            daughterVector.push_back(*iter);
    }

    std::sort(daughterVector.begin(), daughterVector.end(), ConeBasedMergingAlgorithm::SortClustersByInnerLayer);

    // Perform a mip fit to all parent candidate clusters
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (pCluster->GetNCaloHits() < m_minCaloHitsPerCluster)
            continue;

        if (!ClusterHelper::CanMergeCluster(pCluster, m_canMergeMinMipFraction, m_canMergeMaxRms))
            continue;

        const PseudoLayer innerLayer(pCluster->GetInnerPseudoLayer());
        const PseudoLayer showerMaxLayer(pCluster->GetShowerMaxLayer());

        if ((innerLayer > showerMaxLayer) || ((showerMaxLayer - innerLayer) < m_minLayersToShowerMax))
            continue;

        const PseudoLayer fitEndLayer((showerMaxLayer > 1) ? showerMaxLayer - 1 : 0);

        ClusterFitResult mipFitResult;
        if (STATUS_CODE_SUCCESS != ClusterHelper::FitLayers(pCluster, innerLayer, fitEndLayer, mipFitResult))
            continue;

        if (!parentFitResultMap.insert(ClusterFitResultMap::value_type(pCluster, mipFitResult)).second)
            return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ConeBasedMergingAlgorithm::GetFractionInCone(const Cluster *const pDaughterCluster, const ClusterFitResult &parentMipFitResult,
    const float coneCosineHalfAngle) const
{
    return 0.; // TODO under construction
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ConeBasedMergingAlgorithm::SortClustersByInnerLayer(const Cluster *const pLhs, const Cluster *const pRhs)
{
    const PseudoLayer innerLayerLhs(pLhs->GetInnerPseudoLayer()), innerLayerRhs(pRhs->GetInnerPseudoLayer());

    if (innerLayerLhs != innerLayerRhs)
        return (innerLayerLhs < innerLayerRhs);

    return (pLhs->GetNCaloHits() > pRhs->GetNCaloHits());
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ConeBasedMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_trackClusterAssociationAlgName));

    m_canMergeMinMipFraction = 0.7;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMinMipFraction", m_canMergeMinMipFraction));

    m_canMergeMaxRms = 5.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CanMergeMaxRms", m_canMergeMaxRms));

    m_minCaloHitsPerCluster = 6;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitsPerCluster", m_minCaloHitsPerCluster));

    m_minLayersToShowerMax = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinLayersToShowerMax", m_minLayersToShowerMax));

    m_minConeFraction = 0.5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinConeFraction", m_minConeFraction));

    m_maxInnerLayerSeparation = 1000.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxInnerLayerSeparation", m_maxInnerLayerSeparation));

    m_maxInnerLayerSeparationNoTrack = 250.;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxInnerLayerSeparationNoTrack", m_maxInnerLayerSeparationNoTrack));

    m_coneCosineHalfAngle = 0.9;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ConeCosineHalfAngle", m_coneCosineHalfAngle));

    return STATUS_CODE_SUCCESS;
}
