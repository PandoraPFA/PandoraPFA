/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Cheating/CheatingClusterMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Cheating/CheatingClusterMergingAlgorithm.h"

using namespace pandora;

StatusCode CheatingClusterMergingAlgorithm::Run()
{
    std::string currentClusterListName;
    const ClusterList *pCurrentClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pCurrentClusterList, currentClusterListName));

    // Merge cluster fragments within the current list that originate from same main mc particle
    typedef std::map<const pandora::MCParticle*, pandora::Cluster*> MCParticleToClusterMap;
    MCParticleToClusterMap mcParticleToClusterMap;

    for (ClusterList::const_iterator itCluster = pCurrentClusterList->begin(); itCluster != pCurrentClusterList->end();)
    {
        Cluster *pCluster = *itCluster;
        ++itCluster;

        const MCParticle *pSelectedMCParticle = this->GetMainMCParticle(pCluster);

        if (NULL == pSelectedMCParticle)
            continue;

        MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pSelectedMCParticle);

        if (itMCParticle == mcParticleToClusterMap.end())
        {
            mcParticleToClusterMap.insert(std::make_pair(pSelectedMCParticle,pCluster));
        }
        else
        {
            Cluster *pBaseCluster = itMCParticle->second;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBaseCluster, pCluster));

            if (m_debug)
                std::cout << "CheatingClusterMerging: Clusters within current cluster list '" << currentClusterListName << "' have been merged" << std::endl;
        }
    }

    // Merge cluster fragments from named lists that originate from same main mc particle as a cluster in the current list
    for (StringVector::iterator itClusterListName = m_clusterListNames.begin(), itClusterListNameEnd = m_clusterListNames.end();
        itClusterListName != itClusterListNameEnd; ++itClusterListName)
    {
        std::string clusterListName(*itClusterListName);
        const ClusterList *pClusterList = NULL;

        if (m_debug)
            std::cout << "clusterlistname " << clusterListName << std::endl;

        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, clusterListName, pClusterList))
        {
            if (m_debug)
                std::cout << "CheatingClusterMerging: Cluster list '" << clusterListName << "' not found" << std::endl;

            continue;
        }

        for (ClusterList::const_iterator itCluster = pClusterList->begin(); itCluster != pClusterList->end();)
        {
            Cluster *pCluster = *itCluster;
            ++itCluster;

            const MCParticle *pSelectedMCParticle = this->GetMainMCParticle(pCluster);

            if (NULL == pSelectedMCParticle)
                continue;

            MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pSelectedMCParticle);

            // If no cluster in the current list has the same main mc particle, skip this cluster.
            if (itMCParticle != mcParticleToClusterMap.end())
            {
                if (m_debug)
                    std::cout << "pid of mcparticle " << pSelectedMCParticle->GetParticleId() << std::endl;

                Cluster *pBaseCluster = itMCParticle->second;

                if (clusterListName == currentClusterListName)
                {
                    if (m_debug)
                        std::cout << "CheatingClusterMerging: Merging of clusters from cluster list '" << clusterListName << "' into current cluster list '" << currentClusterListName << "' is not possible." << std::endl;

                    return STATUS_CODE_NOT_ALLOWED;
                }

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBaseCluster, pCluster, 
                    currentClusterListName, clusterListName));

                if (m_debug)
                    std::cout << "CheatingClusterMerging: Merged clusters from cluster list '" << clusterListName << "' into current cluster list '" << currentClusterListName << "'" << std::endl;
            }
            else if (m_debug)
            {
                std::cout << "CheatingClusterMerging: No cluster with given main mc particle found in the current list" << std::endl;
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const MCParticle *CheatingClusterMergingAlgorithm::GetMainMCParticle(const Cluster *const pCluster) const
{
    typedef std::map<const pandora::MCParticle*, float> MCParticleToEnergyMap;
    MCParticleToEnergyMap mcParticleToEnergyMap;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
    {
        for (CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit *pCaloHit = *hitIter;

            const MCParticle* pMCParticle = NULL;
            pCaloHit->GetMCParticle(pMCParticle);

            if (NULL == pMCParticle)
                continue;

            const float hitEnergy(pCaloHit->GetElectromagneticEnergy());

            MCParticleToEnergyMap::iterator itMCParticleToEnergy = mcParticleToEnergyMap.find(pMCParticle);

            if (itMCParticleToEnergy == mcParticleToEnergyMap.end())
            {
                mcParticleToEnergyMap.insert(std::make_pair(pMCParticle, hitEnergy));
                continue;
            }

            itMCParticleToEnergy->second += hitEnergy;
        }
    }

    const MCParticle *pSelectedMCParticle = NULL;
    float energyOfSelectedMCParticle = 0;

    for (MCParticleToEnergyMap::const_iterator itMCParticle = mcParticleToEnergyMap.begin(), itMCParticleEnd = mcParticleToEnergyMap.end();
        itMCParticle != itMCParticleEnd; ++itMCParticle)
    {
        const MCParticle *const pCurrentMCParticle = itMCParticle->first;
        const float currentEnergy = itMCParticle->second;

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
