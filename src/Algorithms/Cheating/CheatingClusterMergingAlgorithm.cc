/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/CheatingClusterMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/CheatingClusterMergingAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingClusterMergingAlgorithm::Run()
{
    std::string currentClusterListName;
    const ClusterList *pCurrentClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pCurrentClusterList, currentClusterListName));

    // Collect mc particles of the clusters in the current list. The mcparticle which provides the most energy to the cluster is taken
    MCParticleToClusterMap mcParticleToClusterMap;

    for (ClusterList::const_iterator itCluster = pCurrentClusterList->begin(), itClusterEnd = pCurrentClusterList->end(); itCluster != itClusterEnd;)
    {
        Cluster *pCluster = *itCluster;
        ++itCluster;

        const MCParticle *pSelectedMCParticle = GetMainMCParticle(pCluster);

        if (!pSelectedMCParticle)
            continue;

        MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pSelectedMCParticle);

        if(itMCParticle == mcParticleToClusterMap.end())
        {
            mcParticleToClusterMap.insert(std::make_pair(pSelectedMCParticle,pCluster));
        }
        else
        {
            Cluster *pBaseCluster = itMCParticle->second;

            // merge clusters within the current list if their main MCParticles are the same
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBaseCluster, pCluster));
            if( m_debug )
                std::cout << "CheatingClusterMerging: Clusters within current cluster list '" << currentClusterListName << "' have been merged" << std::endl;
            // don't have to add the cluster, since the base cluster is already in the map
        }
    }

    for (StringVector::iterator itClusterListName = m_clusterListNames.begin(), itClusterListNameEnd = m_clusterListNames.end(); itClusterListName != itClusterListNameEnd; ++itClusterListName )
    {
        std::string clusterListName = (*itClusterListName);
        const ClusterList *pClusterList = NULL;
        if( m_debug )
            std::cout << "clusterlistname " << clusterListName << std::endl;

        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, clusterListName, pClusterList))
        {
            if( m_debug )
                std::cout << "CheatingClusterMerging: Cluster list '" << clusterListName << "' not found" << std::endl;
            continue;
        }
        
        for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd;)
        {
            Cluster *pCluster = *itCluster;
            ++itCluster;

            const MCParticle* pSelectedMCParticle = GetMainMCParticle(pCluster);

            if( !pSelectedMCParticle )
                continue;

            MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pSelectedMCParticle);

            if (itMCParticle != mcParticleToClusterMap.end()) // if the main mcparticle of the cluster is not present in the list, don't touch the cluster
            {
                if( m_debug )
                    std::cout << "pid of mcparticle " << pSelectedMCParticle->GetParticleId() << std::endl;
                Cluster* pBaseCluster = itMCParticle->second;

                if( clusterListName == currentClusterListName )
                {
                    if( m_debug )
                        std::cout << "CheatingClusterMerging: Merging of clusters from cluster list '" << clusterListName << "' into current cluster list '" << currentClusterListName << "' is not possible." << std::endl;
                    return STATUS_CODE_NOT_ALLOWED;
                }

                // merge clusters of other list to the cluster in the current list if their main MCParticles are the same
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBaseCluster, pCluster, 
                    currentClusterListName, clusterListName ));
                if( m_debug )
                    std::cout << "CheatingClusterMerging: Merged clusters from cluster list '" << clusterListName << "' into current cluster list '" << currentClusterListName << "'" << std::endl;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const MCParticle* CheatingClusterMergingAlgorithm::GetMainMCParticle(const Cluster *const pCluster) const
{
    MCParticleToFloatMap mcParticleToFloatMap;

    const OrderedCaloHitList& orderedCaloHitList = pCluster->GetOrderedCaloHitList();

    for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
    {
        CaloHitList *pCurrentHits = itLyr->second;

        for (CaloHitList::const_iterator hitIter = pCurrentHits->begin(), hitIterEnd = pCurrentHits->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const MCParticle* pMCParticle = NULL;
            pCaloHit->GetMCParticle(pMCParticle);

            const float energy(pCaloHit->GetElectromagneticEnergy());

            if (!pMCParticle)
                continue;

            MCParticleToFloatMap::iterator itMCParticleToFloat = mcParticleToFloatMap.find(pMCParticle);

            if (itMCParticleToFloat == mcParticleToFloatMap.end())
            {
                mcParticleToFloatMap.insert(std::make_pair(pMCParticle,energy));
                continue;
            }

            itMCParticleToFloat->second += energy;
        }
    }

    const MCParticle *pSelectedMCParticle = NULL;
    float energyOfSelectedMCParticle = 0;

    for (MCParticleToFloatMap::const_iterator itMCParticle = mcParticleToFloatMap.begin(), itMCParticleEnd = mcParticleToFloatMap.end(); itMCParticle != itMCParticleEnd; ++itMCParticle)
    {
        const MCParticle* pCurrentMCParticle = itMCParticle->first;
        float currentEnergy = itMCParticle->second;

        if (currentEnergy > energyOfSelectedMCParticle)
        {
            pSelectedMCParticle = pCurrentMCParticle;
            energyOfSelectedMCParticle = currentEnergy;
        }
    }

    return pSelectedMCParticle;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingClusterMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_clusterListNames.clear();
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames) );

    m_debug = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Debug", m_debug));

    return STATUS_CODE_SUCCESS;
}
