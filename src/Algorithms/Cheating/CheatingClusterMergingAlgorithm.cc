/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/CheatingClusterMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/CheatingClusterMergingAlgorithm.h"

#include "Helpers/CaloHitHelper.h"

#include "Objects/MCParticle.h"
#include "Objects/Cluster.h"

using namespace pandora;

StatusCode CheatingClusterMergingAlgorithm::Run()
{
    const ClusterList *pCurrentClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pCurrentClusterList));

    std::string currentClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterListName(*this, currentClusterListName));

    // collect the mc particles of the clusters in the current list 
    // the mcparticle which provides the most energy to the cluster is taken 
    typedef std::map<const MCParticle*,Cluster*> MCParticleToClusterMap;

    MCParticleToClusterMap mcParticleToClusterMap;
    
    for( ClusterList::const_iterator itCluster = pCurrentClusterList->begin(), itClusterEnd = pCurrentClusterList->end(); itCluster != itClusterEnd; ++itCluster )
    {
        Cluster* pCluster = (*itCluster);
        const MCParticle* pSelectedMCParticle = GetMainMCParticle(pCluster);

        if( !pSelectedMCParticle )
            continue;

        MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pSelectedMCParticle);
        if( itMCParticle == mcParticleToClusterMap.end() )
        {
            mcParticleToClusterMap.insert(std::make_pair(pSelectedMCParticle,pCluster));
        }
        else
        {
            Cluster* pBaseCluster = itMCParticle->second;

            // merge clsuters within the current list if their main MCParticles are the same
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBaseCluster, pCluster));
            std::cout << "merged clusters in current list" << std::endl;
            // don't have to add the cluster, since the base cluster is already in the map
        }
    }


    for( StringVector::iterator itClusterListName = m_clusterListNames.begin(), itClusterListNameEnd = m_clusterListNames.end(); itClusterListName != itClusterListNameEnd; ++itClusterListName )
    {
        std::string clusterListName = (*itClusterListName);
        const ClusterList *pClusterList = NULL;
        std::cout << "clusterlistname " << clusterListName << std::endl;
        if( STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, clusterListName, pClusterList))
        {
            std::cout << "-->not found" << std::endl;
            continue;
        }
        
        for( ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
        {
            Cluster* pCluster = (*itCluster);
            const MCParticle* pSelectedMCParticle = GetMainMCParticle(pCluster);

            if( !pSelectedMCParticle )
                continue;

            MCParticleToClusterMap::iterator itMCParticle = mcParticleToClusterMap.find(pSelectedMCParticle);
            if( itMCParticle != mcParticleToClusterMap.end() ) // if the main mcparticle of the cluster is not present in the list, don't touch the cluster
            {
                std::cout << "pid of mcparticle " << pSelectedMCParticle->GetParticleId() << std::endl;
                Cluster* pBaseCluster = itMCParticle->second;

                // merge clusters of other list to the cluster in the current list if their main MCParticles are the same
                std::cout << "merge clusters from list " << clusterListName << " to current list with name " << currentClusterListName << std::endl;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::MergeAndDeleteClusters(*this, pBaseCluster, pCluster, 
                                                                                                            currentClusterListName, clusterListName ));

            }
        }
    }    


    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const MCParticle* CheatingClusterMergingAlgorithm::GetMainMCParticle(const Cluster* pCluster)
{
    typedef std::map<const MCParticle*,float> MCParticleToFloatMap;

    MCParticleToFloatMap mcParticleToFloatMap;

    const OrderedCaloHitList& orderedCaloHitList = pCluster->GetOrderedCaloHitList();

    for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
    {
        CaloHitList *pCurrentHits = itLyr->second;

        for (CaloHitList::const_iterator hitIter = pCurrentHits->begin(), hitIterEnd = pCurrentHits->end(); hitIter != hitIterEnd; ++hitIter)
        {
            CaloHit* pCaloHit = *hitIter;
            const MCParticle* pMCParticle = NULL;
            pCaloHit->GetMCParticle(pMCParticle);
                
            float energy = pCaloHit->GetElectromagneticEnergy();

            if( !pMCParticle )
                continue;

            MCParticleToFloatMap::iterator itMCParticleToFloat = mcParticleToFloatMap.find(pMCParticle);
            if( itMCParticleToFloat == mcParticleToFloatMap.end() )
            {
                mcParticleToFloatMap.insert(std::make_pair(pMCParticle,energy));
                continue;
            }
                
            float oldEnergy = itMCParticleToFloat->second;
            itMCParticleToFloat->second = oldEnergy+energy;
        }
    }

    const MCParticle* pSelectedMCParticle = NULL;
    float energyOfSelectedMCParticle = 0;
    for( MCParticleToFloatMap::const_iterator itMCParticle = mcParticleToFloatMap.begin(), itMCParticleEnd = mcParticleToFloatMap.end(); itMCParticle != itMCParticleEnd; ++itMCParticle )
    {
        const MCParticle* pCurrentMCParticle = itMCParticle->first;
        float currentEnergy = itMCParticle->second;

        if( currentEnergy > energyOfSelectedMCParticle )
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

    return STATUS_CODE_SUCCESS;
}
