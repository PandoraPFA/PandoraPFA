/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Cheating/PerfectFragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Cheating/PerfectFragmentRemovalAlgorithm.h"

using namespace pandora;

StatusCode PerfectFragmentRemovalAlgorithm::Run()
{
    for (StringVector::const_iterator iter = m_clusterListNames.begin(), iterEnd = m_clusterListNames.end(); iter != iterEnd; ++iter)
    {
        const ClusterList* pClusterList = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, *iter, pClusterList));

        // Examine clusters, reducing each to just the hits corresponding to the main mc particle
        for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
        {
            Cluster *pCluster = *itCluster;
            const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

            if (NULL == pMainMCParticle)
                continue;

            OrderedCaloHitList orderedCaloHitList(pCluster->GetOrderedCaloHitList());
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(pCluster->GetIsolatedCaloHitList()));

            // Remove all calo hits that do not correspond to the cluster main mc particle
            for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
            {
                for (CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
                {
                    CaloHit *pCaloHit = *hitIter;

                    const MCParticle *pMCParticle = NULL;
                    (void) pCaloHit->GetMCParticle(pMCParticle);

                    if (pMainMCParticle != pMCParticle)
                    {
                        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCaloHitFromCluster(*this, pCluster, pCaloHit));
                    }
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames));

    return STATUS_CODE_SUCCESS;
}
