/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/IsolatedHitMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the isolated hit merging algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/IsolatedHitMergingAlgorithm.h"

#include "Helpers/CaloHitHelper.h"

#include <limits>

using namespace pandora;

StatusCode IsolatedHitMergingAlgorithm::Run()
{
    std::string inputClusterListName;
    const ClusterList *pInputClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pInputClusterList, inputClusterListName));

    // Create a vector of input clusters, ordered by inner layer
    ClusterVector inputClusterVector(pInputClusterList->begin(), pInputClusterList->end());
    std::sort(inputClusterVector.begin(), inputClusterVector.end(), Cluster::SortByInnerLayerIncEnergy);

    // Create a list containing both input and photon clusters
    ClusterList combinedClusterList(pInputClusterList->begin(), pInputClusterList->end());

    const ClusterList *pPhotonClusterList = NULL;

    if (m_shouldUsePhotonClusters)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, m_photonClusterListName, pPhotonClusterList));
        combinedClusterList.insert(pPhotonClusterList->begin(), pPhotonClusterList->end());
    }


    // FIRST PART - find "small" clusters, below threshold number of calo hits, delete them and associate hits with other clusters
    for (ClusterVector::iterator iterI = inputClusterVector.begin(), iterIEnd = inputClusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pClusterToDelete = *iterI;

        if (NULL == pClusterToDelete)
            continue;

        const unsigned int nCaloHits(pClusterToDelete->GetNCaloHits());

        if (nCaloHits > m_minHitsInCluster)
            continue;

        // TODO should these hits be flagged as isolated?
        CaloHitList caloHitList;
        pClusterToDelete->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

        combinedClusterList.erase(pClusterToDelete);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteCluster(*this, pClusterToDelete));
        *iterI = NULL;

        // Redistribute hits that used to be in cluster I amongst other clusters
        for (CaloHitList::const_iterator hitIter = caloHitList.begin(), hitIterEnd = caloHitList.end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            Cluster *pBestHostCluster(NULL);
            PseudoLayer bestHostInnerLayer(0);
            float minDistance(m_maxRecombinationDistance);

            // Find the most appropriate cluster for this newly-available hit
            for (ClusterList::const_iterator iterJ = combinedClusterList.begin(), iterJEnd = combinedClusterList.end(); iterJ != iterJEnd; ++iterJ)
            {
                Cluster *pNewHostCluster = *iterJ;

                if (pNewHostCluster->GetNCaloHits() < nCaloHits)
                    continue;

                const float distance(this->GetDistanceToHit(pNewHostCluster, pCaloHit));

                // In event of equidistant host candidates, choose outermost cluster
                if ((distance < minDistance) || ((distance == minDistance) && (pNewHostCluster->GetInnerPseudoLayer() > bestHostInnerLayer)))
                {
                    minDistance = distance;
                    pBestHostCluster = pNewHostCluster;
                    bestHostInnerLayer = pNewHostCluster->GetInnerPseudoLayer();
                }
            }

            if (NULL != pBestHostCluster)
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddIsolatedCaloHitToCluster(*this, pBestHostCluster, pCaloHit));
            }
        }
    }


    // SECOND PART - loop over the remaining available isolated hits, and associate them with other clusters
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    for (OrderedCaloHitList::const_iterator iterI = pOrderedCaloHitList->begin(); iterI != pOrderedCaloHitList->end(); ++iterI)
    {
        for (CaloHitList::const_iterator hitIterI = iterI->second->begin(); hitIterI != iterI->second->end(); ++hitIterI)
        {
            CaloHit *pCaloHit = *hitIterI;

            if (!pCaloHit->IsIsolated() || !CaloHitHelper::IsCaloHitAvailable(pCaloHit))
                continue;

            Cluster *pBestHostCluster(NULL);
            PseudoLayer bestHostInnerLayer(0);
            float minDistance(m_maxRecombinationDistance);

            // Find most appropriate cluster for this isolated hit
            for (ClusterList::const_iterator iterJ = combinedClusterList.begin(), iterJEnd = combinedClusterList.end(); iterJ != iterJEnd; ++iterJ)
            {
                Cluster *pCluster = *iterJ;

                const float distance(this->GetDistanceToHit(pCluster, pCaloHit));

                if ((distance < minDistance) || ((distance == minDistance) && (pCluster->GetInnerPseudoLayer() > bestHostInnerLayer)))
                {
                    minDistance = distance;
                    pBestHostCluster = pCluster;
                    bestHostInnerLayer = pCluster->GetInnerPseudoLayer();
                }
            }

            if (NULL != pBestHostCluster)
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddIsolatedCaloHitToCluster(*this, pBestHostCluster, pCaloHit));
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float IsolatedHitMergingAlgorithm::GetDistanceToHit(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    const CartesianVector &hitPosition(pCaloHit->GetPositionVector());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    float minDistance(std::numeric_limits<float>::max());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const CartesianVector positionDifference(hitPosition - (*hitIter)->GetPositionVector());
            const float distance(positionDifference.GetMagnitude());

            if (distance < minDistance)
                minDistance = distance;
        }
    }

    return minDistance;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode IsolatedHitMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_shouldUsePhotonClusters = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUsePhotonClusters", m_shouldUsePhotonClusters));

    if (m_shouldUsePhotonClusters)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
            "PhotonClusterListName", m_photonClusterListName));
    }

    m_minHitsInCluster = 4;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    m_maxRecombinationDistance = 250.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxRecombinationDistance", m_maxRecombinationDistance));

    return STATUS_CODE_SUCCESS;
}
