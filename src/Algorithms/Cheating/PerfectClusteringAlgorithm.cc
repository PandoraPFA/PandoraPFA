/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/PerfectClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/PerfectClusteringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode PerfectClusteringAlgorithm::Run()
{
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    if (m_debug)
    {
        std::cout << std::endl;
        std::string currentCaloHitListName;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitListName(*this, currentCaloHitListName));
        std::cout << "PerfectClusteringAlgorithm/ current OrderedCaloHitList - name '" << currentCaloHitListName << "'" << std::endl;
        std::cout << "PerfectClusteringAlgorithm/ current OrderedCaloHitList - size '" << pOrderedCaloHitList->size() << "' (number of pseudo layers)" << std::endl;
        for (OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList->begin(), itLyrEnd = pOrderedCaloHitList->end(); itLyr != itLyrEnd; ++itLyr)
        {
            PseudoLayer pseudoLayer = itLyr->first;
            CaloHitList *pCaloHitList= itLyr->second;
            CaloHitList *pCurrentHits = NULL;

            std::cout << "PerfectClusteringAlgorithm/ pseudo layer " << pseudoLayer << " - size '" << pCaloHitList->size() << "' (number of hits in pseudo layer)" << std::endl;
            
        }
    }

    // Match CaloHitLists to their MCParticles
    std::map< const MCParticle*, CaloHitList* > hitsPerMCParticle;
    std::map< const MCParticle*, CaloHitList* >::iterator itHitsPerMCParticle;

    int selected = 0, notSelected = 0;

    for (OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList->begin(), itLyrEnd = pOrderedCaloHitList->end(); itLyr != itLyrEnd; ++itLyr)
    {
        CaloHitList *pCaloHitList= itLyr->second;
        CaloHitList *pCurrentHits = NULL;

        for (CaloHitList::const_iterator hitIter = pCaloHitList->begin(), hitIterEnd = pCaloHitList->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit* pCaloHit = *hitIter;

            if( m_debug )
            {
                if(!CaloHitHelper::IsCaloHitAvailable(pCaloHit))
                    std::cout << "N" << std::flush;

                if(pCaloHit->IsIsolated() && !m_shouldUseIsolatedHits)
                    std::cout << "I" << std::flush;

            }

            if(!CaloHitHelper::IsCaloHitAvailable(pCaloHit))
                continue;

            if(!m_shouldUseIsolatedHits && pCaloHit->IsIsolated())
                continue;

            const MCParticle *pMCParticle = NULL;
            pCaloHit->GetMCParticle(pMCParticle);

            // Some CalorimeterHits don't have a MCParticle (e.g. noise)
            if (NULL == pMCParticle)
            {
                if( m_debug )
                    std::cout << "[no MC]" << std::flush;
                
                continue;
            }

            // Apply calo hit selection
            if (!SelectMCParticlesForClustering(pMCParticle))
            {
                if (m_debug)
                {
                    std::cout << "." << std::flush;
                    ++notSelected;
                }
                continue;
            }
            else
            {
                if (m_debug)
                {
                    std::cout << "|" << std::flush;
                    ++selected;
                }
            }

            // Add hit to calohitlist
            itHitsPerMCParticle = hitsPerMCParticle.find(pMCParticle);

            if (itHitsPerMCParticle == hitsPerMCParticle.end())
            {
                // If there is no calohitlist for the MCParticle yet, create one
                pCurrentHits = new CaloHitList();
                hitsPerMCParticle.insert(std::make_pair(pMCParticle, pCurrentHits));
            }
            else
            {
                // Take the calohitlist corresponding to the MCParticle
                pCurrentHits = itHitsPerMCParticle->second;
            }

            pCurrentHits->insert(pCaloHit);
        }
    }

    if (m_debug)
    {
        std::cout << std::endl;
        std::cout << "selected hits= " << selected << "  not selected hits= " << notSelected << std::endl;
    }


    // Create the clusters
    ClusterList clusterList;
    Cluster *pCluster = NULL;

    typedef std::map< const MCParticle*, CaloHitList* > MCParticleToCaloHitListMap;

    for (MCParticleToCaloHitListMap::const_iterator itCHList = hitsPerMCParticle.begin(), itCHListEnd = hitsPerMCParticle.end(); 
         itCHList != itCHListEnd; ++itCHList )
    {
        const MCParticle *pMCParticle = itCHList->first;

        // Hits without MCParticle are not clustered
        if(NULL == pMCParticle)
            continue;

        CaloHitList *pCaloHitList = itCHList->second;

        if (!pCaloHitList->empty())
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHitList, pCluster));

            // Set photon flag for photon clusters
            if (pMCParticle->GetParticleId() == PHOTON)
                pCluster->SetIsFixedPhotonFlag(true);

            if (!clusterList.insert(pCluster).second)
                return STATUS_CODE_FAILURE;
        }

        // Delete the created CaloHitLists
        delete pCaloHitList;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PerfectClusteringAlgorithm::SelectMCParticlesForClustering(const MCParticle* pMCParticle) const
{
    if (m_particleIdList.empty())
        return true;

    for (IntVector::const_iterator itParticleId = m_particleIdList.begin(), itParticleIdEnd = m_particleIdList.end();
        itParticleId != itParticleIdEnd; ++itParticleId)
    {
        if (pMCParticle->GetParticleId() == *itParticleId)
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_debug = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Debug", m_debug));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ParticleIdList", m_particleIdList));

    m_shouldUseIsolatedHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseIsolatedHits", m_shouldUseIsolatedHits));

    return STATUS_CODE_SUCCESS;
}
