/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/PerfectFragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/PerfectFragmentRemovalAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

using namespace pandora;



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::Run()
{
    ClusterList cleanedClusters;

    float removedEnergy = 0.f;

    for( STRINGVECTOR::iterator itClusterName = m_clusterListNames.begin(), itClusterNameEnd = m_clusterListNames.end(); itClusterName != itClusterNameEnd; ++itClusterName )
    {
        std::string& clusterName = (*itClusterName);
        const ClusterList* pClusterList = NULL;
        if( STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, clusterName, pClusterList))
            continue;

        for( ClusterList::iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
        {
            Cluster* pCluster = (*itCluster);

            float removedCaloHitEnergy;
            PANDORA_THROW_RESULT_IF( STATUS_CODE_SUCCESS, !=, FragmentRemoval( pCluster, removedCaloHitEnergy ) );

            removedEnergy += removedCaloHitEnergy;
        }
    }
    
    if( m_debug )
    {
        std::cout << "total removed Energy : " << removedEnergy << std::endl;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "ClusterListNames", m_clusterListNames));

    m_debug = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "Debug", m_debug));

    return STATUS_CODE_SUCCESS;
}



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectFragmentRemovalAlgorithm::FragmentRemoval( Cluster* pCluster, float& removedCaloHitEnergy ) 
{
    
    const OrderedCaloHitList& pOrderedCaloHitList = pCluster->GetOrderedCaloHitList();

    // match calohitvectors and energy to their MCParticles
    typedef std::map< const MCParticle*, std::pair<float, CaloHitVector*> > CALOHITSPERMCPARTICLE;
    CALOHITSPERMCPARTICLE hitsPerMcParticle;
    CALOHITSPERMCPARTICLE::iterator itHitsPerMcParticle;

    for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList.begin(), itLyrEnd = pOrderedCaloHitList.end(); itLyr != itLyrEnd; itLyr++ )
    {
        // int pseudoLayer = itLyr->first;
        CaloHitList::iterator itCaloHit    = itLyr->second->begin();
        CaloHitList::iterator itCaloHitEnd = itLyr->second->end();

        CaloHitVector* currentHits = NULL;
        float energy = 0.f;
        for( ; itCaloHit != itCaloHitEnd; ++itCaloHit )
        {
            CaloHit* pCaloHit = (*itCaloHit);

            // fetch the MCParticle
            const MCParticle* mc = NULL; 
            pCaloHit->GetMCParticle( mc );
            if( mc == NULL ) continue; // has to be continue, since sometimes some CalorimeterHits don't have a MCParticle (e.g. noise)


            // add hit to calohitvector
            itHitsPerMcParticle = hitsPerMcParticle.find( mc );
            if( itHitsPerMcParticle == hitsPerMcParticle.end() )
            {
                // if there is no calohitvector for the MCParticle yet, create one
                currentHits = new CaloHitVector();
                hitsPerMcParticle.insert( std::make_pair( mc, std::make_pair(0.f, currentHits) ) );
            }
            else
            {
                currentHits = itHitsPerMcParticle->second.second; // take the calohitvector corresponding to the MCParticle
                energy = itHitsPerMcParticle->second.first;               // get the sum of energy of the CaloHits collected so far
            }

            currentHits->push_back( pCaloHit ); // add the calohit
            energy += pCaloHit->GetElectromagneticEnergy();
            itHitsPerMcParticle->second.first = energy;
        }
    }

    // select the mcparticle which contributed most of the calohit-energy to the cluster
    CALOHITSPERMCPARTICLE::iterator itHitsPerMcParticleMostEnergy = hitsPerMcParticle.begin();
    CALOHITSPERMCPARTICLE::iterator itHits = hitsPerMcParticle.begin();
    ++itHits;
    CALOHITSPERMCPARTICLE::iterator itHitsEnd = hitsPerMcParticle.end();

    float removedEnergy  = 0.f;

    for( ; itHits != itHitsEnd; ++itHits )
    {
        float& mostEnergy    = itHitsPerMcParticleMostEnergy->second.first;
        float& currentEnergy = itHits->second.first;

        CaloHitVector* pCaloHitVectorToRemove = NULL;

        if( currentEnergy > mostEnergy )
        { // remove the hits of the mcparticle which had be the one with the largest energy contribution
            std::cout << "curr >  most " << std::endl;
            pCaloHitVectorToRemove = itHitsPerMcParticleMostEnergy->second.second;
            itHitsPerMcParticleMostEnergy = itHits;
        }
        else
        { // remove the hits of the mcpfo which has less energy contribution
            std::cout << "curr <= most " << std::endl;
            pCaloHitVectorToRemove = itHits->second.second;
        }

        if( !pCaloHitVectorToRemove )
            continue;

        std::cout << "size to remove  : " << pCaloHitVectorToRemove->size() << std::endl;
        std::cout << "size most energy: " << itHitsPerMcParticleMostEnergy->second.second->size() << std::endl;

        // remove the calorimeter hits
        for( CaloHitVector::iterator itCaloHit = pCaloHitVectorToRemove->begin(), itCaloHitEnd = pCaloHitVectorToRemove->end(); itCaloHit != itCaloHitEnd; ++itCaloHit )
        {
            CaloHit* pCaloHit = (*itCaloHit);
            removedEnergy += pCaloHit->GetElectromagneticEnergy();
            PANDORA_THROW_RESULT_IF( STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCaloHitFromCluster(*this, pCluster, pCaloHit ));
        }
    }

    if( m_debug && removedEnergy>0.f )
//     if( m_debug )
    {
        const MCParticle *pMcPfo = itHitsPerMcParticleMostEnergy->first;
        CaloHitVector* pCaloHitVectorTo = itHitsPerMcParticleMostEnergy->second.second;
        
        float retainedEnergy = 0.f;
        
        for( CaloHitVector::iterator itCaloHit = pCaloHitVectorTo->begin(), itCaloHitEnd = pCaloHitVectorTo->end(); itCaloHit != itCaloHitEnd; ++itCaloHit )
        {
            CaloHit* pCaloHit = (*itCaloHit);
            retainedEnergy += pCaloHit->GetElectromagneticEnergy();
        }

        std::cout << "cluster " << pCluster << ", largest MCPFO contribution from : " << pMcPfo << " with PDG code " << pMcPfo->GetParticleId() 
                  << " removed energy : " << removedEnergy << " retained energy : " << retainedEnergy << std::endl;
    }


    removedCaloHitEnergy = removedEnergy;

    return STATUS_CODE_SUCCESS;
}

