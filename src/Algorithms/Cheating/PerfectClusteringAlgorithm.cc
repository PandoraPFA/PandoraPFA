/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/PerfectClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/PerfectClusteringAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

using namespace pandora;



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectClusteringAlgorithm::Run()
{
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    // match calohitvectors to their MCParticles
    std::map< const MCParticle*, CaloHitVector* > hitsPerMcParticle;
    std::map< const MCParticle*, CaloHitVector* >::iterator itHitsPerMcParticle;

    OrderedCaloHitList pNewOrderedCaloHitList; 

    int selected = 0, notSelected = 0;

    if( m_debug )
    {
        std::cout << std::endl;
    }


    for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList->begin(), itLyrEnd = pOrderedCaloHitList->end(); itLyr != itLyrEnd; itLyr++ )
    {
        // int pseudoLayer = itLyr->first;
        CaloHitList::iterator itCaloHit    = itLyr->second->begin();
        CaloHitList::iterator itCaloHitEnd = itLyr->second->end();

        CaloHitVector* currentHits = NULL;
        for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
        {
            CaloHit* pCaloHit = (*itCaloHit);

            // fetch the MCParticle
            const MCParticle* mc = NULL; 
            pCaloHit->GetMCParticle( mc );
            if( mc == NULL ) continue; // has to be continue, since sometimes some CalorimeterHits don't have a MCParticle (e.g. noise)


            // some selection criteria possible
            if( !SelectCaloHitsOfMcParticleForClustering( mc ) )
            {
                pNewOrderedCaloHitList.AddCaloHit( pCaloHit );
                if( m_debug )
                {
                    std::cout << "." << std::flush;
                    ++notSelected;
                }
                continue;
            }
            else
            {
                if( m_debug )
                {
                    std::cout << "|" << std::flush;
                    ++selected;
                }
            }


            // add hit to calohitvector
            itHitsPerMcParticle = hitsPerMcParticle.find( mc );
            if( itHitsPerMcParticle == hitsPerMcParticle.end() )
            {
                // if there is no calohitvector for the MCParticle yet, create one
                currentHits = new CaloHitVector();
                hitsPerMcParticle.insert( std::make_pair( mc, currentHits ) );
            }
            else
            {
                currentHits = itHitsPerMcParticle->second; // take the calohitvector corresponding to the MCParticle
            }

            currentHits->push_back( pCaloHit ); // add the calohit
        }
    }

    if( m_debug )
    {
        std::cout << std::endl;
        std::cout << "selected hits= " << selected << "  not selected hits= " << notSelected << std::endl;
    }


    // create the clusters
    ClusterList clusterList;
    pandora::Cluster *pCluster;

    for( std::map< const MCParticle*, CaloHitVector* >::const_iterator itCHList = hitsPerMcParticle.begin(), itCHListEnd = hitsPerMcParticle.end(); 
         itCHList != itCHListEnd; ++itCHList )
    {
        const MCParticle* mc = itCHList->first;

        if( itCHList->first == NULL ) continue; // hits without MCParticle are not clustered
        CaloHitVector* pCaloHitVector = itCHList->second;
        if( !pCaloHitVector->empty() )
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHitVector, pCluster ));
            if( mc->GetParticleId() == 22 ) // set photon flag for photon clusters
                pCluster->SetIsPhotonFlag(true);
        
            if( !clusterList.insert( pCluster ).second )
                return STATUS_CODE_FAILURE;
        }

        // delete the created CaloHitVectors
        delete pCaloHitVector;
    }

    if( !m_clusterListName.empty() && !clusterList.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_clusterListName, clusterList ));

    if( !m_orderedCaloHitListName.empty() )
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveOrderedCaloHitListAndReplaceCurrent(*this, pNewOrderedCaloHitList, m_orderedCaloHitListName ));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ClusterListName", m_clusterListName) );
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "OrderedCaloHitListName", m_orderedCaloHitListName));

    m_debug = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "Debug", m_debug));

    return STATUS_CODE_SUCCESS;
}



//------------------------------------------------------------------------------------------------------------------------------------------

bool PerfectClusteringAlgorithm::SelectCaloHitsOfMcParticleForClustering( const MCParticle* pMcParticle ) const
{
    return true;
}

