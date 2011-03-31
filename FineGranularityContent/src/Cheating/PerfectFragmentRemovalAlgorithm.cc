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
    const ClusterList *pClusterList = NULL;

    if (!m_inputClusterListName.empty())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetClusterList(*this, m_inputClusterListName, pClusterList));
    }
    else
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));
    }

    if (NULL == pClusterList)
        return STATUS_CODE_NOT_INITIALIZED;

    // Examine clusters, reducing each to just the hits corresponding to the main mc particle
    for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
    {
        Cluster *pCluster = *itCluster;
        const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

        if (NULL == pMainMCParticle)
            continue;

        // Remove all calo hits that do not correspond to the cluster main mc particle
        OrderedCaloHitList orderedCaloHitList(pCluster->GetOrderedCaloHitList());

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

        // Repeat for isolated hits
        CaloHitList isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());

        for (CaloHitList::const_iterator hitIter = isolatedCaloHitList.begin(), hitIterEnd = isolatedCaloHitList.end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const MCParticle *pMCParticle = NULL;
            (void) pCaloHit->GetMCParticle(pMCParticle);

            if (pMainMCParticle != pMCParticle)
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveIsolatedCaloHitFromCluster(*this, pCluster, pCaloHit));
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "InputClusterListName", m_inputClusterListName));

    return STATUS_CODE_SUCCESS;
}
