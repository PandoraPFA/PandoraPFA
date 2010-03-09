/**
 *  @file   PandoraPFANew/src/Algorithms/Cheating/PerfectClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the cheating clustering algorithm class
 * 
 *  $Log: $
 */

#include "Algorithms/Cheating/PerfectClusteringAlgorithm.h"

#include "Api/PandoraContentApi.h"

#include "Helpers/CaloHitHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

using namespace pandora;



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectClusteringAlgorithm::Run()
{
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    // match calohitlists to their MCParticles
    std::map< const MCParticle*, CaloHitList* > hitsPerMcParticle;
    std::map< const MCParticle*, CaloHitList* >::iterator itHitsPerMcParticle;

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

        CaloHitList* currentHits = NULL;
        for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
        {
            CaloHit* pCaloHit = (*itCaloHit);

            if( !CaloHitHelper::IsCaloHitAvailable( pCaloHit ) )
                continue;

            // fetch the MCParticle
            const MCParticle* mc = NULL; 
            pCaloHit->GetMCParticle( mc );
            if( mc == NULL ) continue; // has to be continue, since sometimes some CalorimeterHits don't have a MCParticle (e.g. noise)


            // some selection criteria possible
            if( !SelectCaloHitsOfMcParticleForClustering( mc ) )
            {
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


            // add hit to calohitlist
            itHitsPerMcParticle = hitsPerMcParticle.find( mc );
            if( itHitsPerMcParticle == hitsPerMcParticle.end() )
            {
                // if there is no calohitlist for the MCParticle yet, create one
                currentHits = new CaloHitList();
                hitsPerMcParticle.insert( std::make_pair( mc, currentHits ) );
            }
            else
            {
                currentHits = itHitsPerMcParticle->second; // take the calohitlist corresponding to the MCParticle
            }

            currentHits->insert( pCaloHit ); // add the calohit
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

    for( std::map< const MCParticle*, CaloHitList* >::const_iterator itCHList = hitsPerMcParticle.begin(), itCHListEnd = hitsPerMcParticle.end(); 
         itCHList != itCHListEnd; ++itCHList )
    {
        const MCParticle* mc = itCHList->first;

        if( itCHList->first == NULL ) continue; // hits without MCParticle are not clustered
        CaloHitList* pCaloHitList = itCHList->second;
        if( !pCaloHitList->empty() )
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHitList, pCluster ));
            if( mc->GetParticleId() == 22 ) // set photon flag for photon clusters
                pCluster->SetIsPhotonFlag(true);
        
            if( !clusterList.insert( pCluster ).second )
                return STATUS_CODE_FAILURE;
        }

        // delete the created CaloHitLists
        delete pCaloHitList;
    }

    if( !m_clusterListName.empty() && !clusterList.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_clusterListName, clusterList ));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PerfectClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "ClusterListName", m_clusterListName) );

    m_debug = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle, "Debug", m_debug));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle, "ParticleIdList", m_particleIdList));

    return STATUS_CODE_SUCCESS;
}



//------------------------------------------------------------------------------------------------------------------------------------------

bool PerfectClusteringAlgorithm::SelectCaloHitsOfMcParticleForClustering( const MCParticle* pMcParticle ) const
{
    if( m_particleIdList.empty() )
        return true;
    for( IntVector::const_iterator itParticleId = m_particleIdList.begin(), itParticleIdEnd = m_particleIdList.end(); itParticleId != itParticleIdEnd; ++itParticleId )
    {
        const int& particleId = (*itParticleId);
        if( pMcParticle->GetParticleId() == particleId )
            return true;
    }
    return false;
}

