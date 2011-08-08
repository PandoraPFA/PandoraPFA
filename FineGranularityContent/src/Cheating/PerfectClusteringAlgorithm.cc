/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Cheating/PerfectClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Cheating/PerfectClusteringAlgorithm.h"

using namespace pandora;

StatusCode PerfectClusteringAlgorithm::Run()
{
    const CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentCaloHitList(*this, pCaloHitList));

    // Match CaloHitLists to their MCParticles
    typedef std::map< const MCParticle*, CaloHitList* > MCParticleToCaloHitListMap;
    MCParticleToCaloHitListMap hitsPerMCParticle;

    for (CaloHitList::const_iterator hitIter = pCaloHitList->begin(), hitIterEnd = pCaloHitList->end(); hitIter != hitIterEnd; ++hitIter)
    {
        CaloHit* pCaloHit = *hitIter;

        if (!PandoraContentApi::IsCaloHitAvailable(*this, pCaloHit))
            continue;

        if (m_shouldUseOnlyECalHits && (ECAL != pCaloHit->GetHitType()))
            continue;

        if (!m_shouldUseIsolatedHits && pCaloHit->IsIsolated())
            continue;

        const MCParticle *pMCParticle = NULL;
        pCaloHit->GetMCParticle(pMCParticle);

        // Some CalorimeterHits don't have a MCParticle (e.g. noise)
        if (NULL == pMCParticle)
            continue;

        // Apply calo hit selection
        if (!this->SelectMCParticlesForClustering(pMCParticle))
            continue;

        // Add hit to calohitlist
        MCParticleToCaloHitListMap::iterator itHitsPerMCParticle(hitsPerMCParticle.find(pMCParticle));

        if (hitsPerMCParticle.end() == itHitsPerMCParticle)
        {
            CaloHitList *pCurrentHits = new CaloHitList();
            pCurrentHits->insert(pCaloHit);
            hitsPerMCParticle.insert(std::make_pair(pMCParticle, pCurrentHits));
        }
        else
        {
            itHitsPerMCParticle->second->insert(pCaloHit);
        }
    }

    // Create the clusters
    for (MCParticleToCaloHitListMap::const_iterator itCHList = hitsPerMCParticle.begin(), itCHListEnd = hitsPerMCParticle.end(); 
         itCHList != itCHListEnd; ++itCHList )
    {
        const MCParticle *pMCParticle = itCHList->first;
        CaloHitList *pCaloHitList = itCHList->second;

        if (!pCaloHitList->empty())
        {
            Cluster *pCluster = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHitList, pCluster));

            switch (pMCParticle->GetParticleId())
            {
            case PHOTON:
                pCluster->SetIsFixedPhotonFlag(true);
                break;
            case E_PLUS:
            case E_MINUS:
                pCluster->SetIsFixedElectronFlag(true);
                break;
            case MU_PLUS:
            case MU_MINUS:
                pCluster->SetIsFixedMuonFlag(true);
                break;
            default:
                break;
            }
        }

        delete pCaloHitList;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PerfectClusteringAlgorithm::SelectMCParticlesForClustering(const MCParticle *const pMCParticle) const
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
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ParticleIdList", m_particleIdList));

    m_shouldUseOnlyECalHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseOnlyECalHits", m_shouldUseOnlyECalHits));

    m_shouldUseIsolatedHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseIsolatedHits", m_shouldUseIsolatedHits));

    return STATUS_CODE_SUCCESS;
}
