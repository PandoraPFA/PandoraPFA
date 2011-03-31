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
    float removedEnergy(0.f);

    for (StringVector::const_iterator itClusterListName = m_clusterListNames.begin(), itClusterListNameEnd = m_clusterListNames.end();
        itClusterListName != itClusterListNameEnd; ++itClusterListName)
    {
        const ClusterList* pClusterList = NULL;

        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, *itClusterListName, pClusterList))
            continue;

        for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd;
            ++itCluster )
        {
            float removedCaloHitEnergy;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, FragmentRemoval((*itCluster), removedCaloHitEnergy));
            removedEnergy += removedCaloHitEnergy;
        }
    }
    
    if (m_debug)
    {
        std::cout << "total removed Energy : " << removedEnergy << std::endl;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::FragmentRemoval(Cluster *pCluster, float &removedCaloHitEnergy) const
{
    OrderedCaloHitList orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(pCluster->GetIsolatedCaloHitList()));

    // Match calohitvectors and energy to their MCParticles
    typedef std::map< const MCParticle*, std::pair<float, CaloHitVector*> > CaloHitsPerMCParticle;
    CaloHitsPerMCParticle hitsPerMCParticle;

    for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
    {
        for (CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit* pCaloHit = *hitIter;

            const MCParticle *pMCParticle = NULL; 
            pCaloHit->GetMCParticle(pMCParticle);

            // Some CalorimeterHits don't have a MCParticle (e.g. noise)
            if (NULL == pMCParticle)
                continue;

            CaloHitsPerMCParticle::iterator itHitsPerMCParticle(hitsPerMCParticle.find(pMCParticle));

            if (hitsPerMCParticle.end() == itHitsPerMCParticle)
            {
                CaloHitVector *pCurrentHits = new CaloHitVector();
                pCurrentHits->push_back(pCaloHit);
                hitsPerMCParticle.insert(std::make_pair(pMCParticle, std::make_pair(pCaloHit->GetElectromagneticEnergy(), pCurrentHits)));
            }
            else
            {
                itHitsPerMCParticle->second.first += pCaloHit->GetElectromagneticEnergy();
                itHitsPerMCParticle->second.second->push_back(pCaloHit);
            }
        }
    }

    // Select the mc particle which contributed most of the calohit-energy to the cluster
    CaloHitsPerMCParticle::const_iterator itHitsPerMCParticleMostEnergy(hitsPerMCParticle.begin());

    CaloHitsPerMCParticle::const_iterator itHits = hitsPerMCParticle.begin();
    CaloHitsPerMCParticle::const_iterator itHitsEnd = hitsPerMCParticle.end();
    ++itHits;

    float removedEnergy = 0.f;

    for(; itHits != itHitsEnd; ++itHits)
    {
        float mostEnergy = itHitsPerMCParticleMostEnergy->second.first;
        float currentEnergy = itHits->second.first;

        CaloHitVector *pCaloHitVectorToRemove = NULL;

        if (currentEnergy > mostEnergy)
        {
            // Remove the hits of the mc particle which had largest energy contribution
            pCaloHitVectorToRemove = itHitsPerMCParticleMostEnergy->second.second;
            itHitsPerMCParticleMostEnergy = itHits;
        }
        else
        {
            // Remove the hits of the mcpfo which had lower energy contribution
            pCaloHitVectorToRemove = itHits->second.second;
        }

        if (NULL == pCaloHitVectorToRemove)
            continue;

        if (m_debug)
        {
            std::cout << "size to remove  : " << pCaloHitVectorToRemove->size() << std::endl;
            std::cout << "size most energy: " << itHitsPerMCParticleMostEnergy->second.second->size() << std::endl;
        }

        // Remove the calorimeter hits
        for (CaloHitVector::const_iterator itCaloHit = pCaloHitVectorToRemove->begin(), itCaloHitEnd = pCaloHitVectorToRemove->end();
            itCaloHit != itCaloHitEnd; ++itCaloHit)
        {
            CaloHit* pCaloHit = (*itCaloHit);
            removedEnergy += pCaloHit->GetElectromagneticEnergy();
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCaloHitFromCluster(*this, pCluster, pCaloHit));
        }
    }

    if (m_debug && (removedEnergy > 0.f))
    {
        const MCParticle *pMCPfo = itHitsPerMCParticleMostEnergy->first;
        CaloHitVector *pCaloHitVector = itHitsPerMCParticleMostEnergy->second.second;

        float retainedEnergy(0.f);

        for (CaloHitVector::const_iterator itCaloHit = pCaloHitVector->begin(), itCaloHitEnd = pCaloHitVector->end(); itCaloHit != itCaloHitEnd;
            ++itCaloHit)
        {
            retainedEnergy += (*itCaloHit)->GetElectromagneticEnergy();
        }

        std::cout << "Cluster " << pCluster << ", largest MCPFO contribution from : " << pMCPfo << " with PDG code " << pMCPfo->GetParticleId()
                  << " removed energy : " << removedEnergy << " retained energy : " << retainedEnergy << std::endl;
    }

    removedCaloHitEnergy = removedEnergy;

    // Tidy up CaloHitVectors
    for (CaloHitsPerMCParticle::const_iterator iter = hitsPerMCParticle.begin(), iterEnd = hitsPerMCParticle.end(); iter != iterEnd; ++iter)
    {
        delete iter->second.second;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames));

    m_debug = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Debug", m_debug));

    return STATUS_CODE_SUCCESS;
}
