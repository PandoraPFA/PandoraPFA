/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/IsolatedHitMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the isolated hit merging algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/IsolatedHitMergingAlgorithm.h"

#include <limits>

using namespace pandora;

StatusCode IsolatedHitMergingAlgorithm::Run()
{
    std::string inputClusterListName;
    const ClusterList *pInputClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pInputClusterList, inputClusterListName));

    // Create a list containing both input and photon clusters
    ClusterList combinedClusterList(pInputClusterList->begin(), pInputClusterList->end());

    const ClusterList *pPhotonClusterList = NULL;

    if (m_shouldUsePhotonClusters)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, m_photonClusterListName, pPhotonClusterList));
        combinedClusterList.insert(pPhotonClusterList->begin(), pPhotonClusterList->end());
    }


    // FIRST PART - find "small" clusters, below threshold number of calo hits, delete them and associate hits with other clusters
    for (ClusterList::const_iterator iterI = pInputClusterList->begin(), iterIEnd = pInputClusterList->end(); iterI != iterIEnd;)
    {
        Cluster *pClusterI = *iterI;
        ++iterI;

        const unsigned int nCaloHitsI(pClusterI->GetNCaloHits());

        if (nCaloHitsI > m_minHitsInCluster)
            continue;

        CaloHitVector caloHitVectorI;
        pClusterI->GetOrderedCaloHitList().GetCaloHitVector(caloHitVectorI);

        combinedClusterList.erase(pClusterI);
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteCluster(*this, pClusterI));

        // Redistribute hits that used to be in cluster I amoungst other clusters
        for (CaloHitVector::const_iterator hitIterI = caloHitVectorI.begin(), hitIterIEnd = caloHitVectorI.end(); hitIterI != hitIterIEnd; ++hitIterI)
        {
            // TODO should these hits be flagged as isolated?
            CaloHit *pCaloHitI = *hitIterI;

            Cluster *pBestCluster = NULL;
            float minDistance(std::numeric_limits<float>::max());

            // Find the most appropriate cluster for this newly-available hit
            for (ClusterList::const_iterator iterJ = combinedClusterList.begin(), iterJEnd = combinedClusterList.end(); iterJ != iterJEnd; ++iterJ)
            {
                Cluster *pClusterJ = *iterJ;

                const float distance(this->GetDistanceToHit(pClusterJ, pCaloHitI));

                if (distance < minDistance)
                {
                    minDistance = distance;
                    pBestCluster = pClusterJ;
                }
            }

            if ((NULL != pBestCluster) && (minDistance < m_maxRecombinationDistance))
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pBestCluster, pCaloHitI));
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
            CaloHit *pCaloHitI = *hitIterI;

            if (!pCaloHitI->IsIsolated() || !CaloHitHelper::IsCaloHitAvailable(pCaloHitI))
                continue;

            Cluster *pBestCluster = NULL;
            float minDistance(std::numeric_limits<float>::max());

            // Find most appropriate cluster for this isolated hit
            for (ClusterList::const_iterator iterJ = combinedClusterList.begin(), iterJEnd = combinedClusterList.end(); iterJ != iterJEnd; ++iterJ)
            {
                Cluster *pClusterJ = *iterJ;

                const float distance(this->GetDistanceToHit(pClusterJ, pCaloHitI));

                if (distance < minDistance)
                {
                    minDistance = distance;
                    pBestCluster = pClusterJ;
                }
            }

            if ((NULL != pBestCluster) && (minDistance < m_maxRecombinationDistance))
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pBestCluster, pCaloHitI));
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
            // Don't consider isolated hits already present in cluster
            if ((*hitIter)->IsIsolated())
                continue;

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

    m_minHitsInCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsInCluster", m_minHitsInCluster));

    m_maxRecombinationDistance = 200.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxRecombinationDistance", m_maxRecombinationDistance));

    return STATUS_CODE_SUCCESS;
}
