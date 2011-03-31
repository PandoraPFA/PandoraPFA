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

        const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

        if (NULL == pMainMCParticle)
            continue;

        MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pMainMCParticle);

        if (itMCParticle == mcParticleToClusterMap.end())
        {
            mcParticleToClusterMap.insert(std::make_pair(pMainMCParticle, pCluster));
        }
        else
        {
            Cluster *pBaseCluster = itMCParticle->second;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBaseCluster, pCluster));
        }
    }

    // Merge cluster fragments from named lists that originate from same main mc particle as a cluster in the current list
    for (StringVector::iterator iter = m_clusterListNames.begin(), iterEnd = m_clusterListNames.end(); iter != iterEnd; ++iter)
    {
        std::string clusterListName(*iter);

        if (clusterListName == currentClusterListName)
            return STATUS_CODE_NOT_ALLOWED;

        const ClusterList *pClusterList = NULL;

        if (STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, clusterListName, pClusterList))
            continue;

        for (ClusterList::const_iterator itCluster = pClusterList->begin(); itCluster != pClusterList->end();)
        {
            Cluster *pCluster = *itCluster;
            ++itCluster;

            const MCParticle *const pMainMCParticle(MCParticleHelper::GetMainMCParticle(pCluster));

            if (NULL == pMainMCParticle)
                continue;

            MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pMainMCParticle);

            // If no cluster in the current list has the same main mc particle, skip this cluster.
            if (itMCParticle != mcParticleToClusterMap.end())
            {
                Cluster *pBaseCluster = itMCParticle->second;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBaseCluster, pCluster, 
                    currentClusterListName, clusterListName));
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CheatingClusterMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames) );

    return STATUS_CODE_SUCCESS;
}
