/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Monitoring/MCParticlesMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of an algorithm to monitor the mc particles
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Monitoring/MCParticlesMonitoringAlgorithm.h"

#include <iostream>
#include <iomanip>
#include <assert.h>

using namespace pandora;

StatusCode MCParticlesMonitoringAlgorithm::Initialize()
{
    m_energy = new FloatVector();
    m_momentumX = new FloatVector();
    m_momentumY = new FloatVector();
    m_momentumZ = new FloatVector();
    m_particleId = new IntVector();
    m_outerRadius = new FloatVector();
    m_innerRadius = new FloatVector();
    m_caloHitEnergy = new FloatVector();
    m_trackEnergy = new FloatVector();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCParticlesMonitoringAlgorithm::~MCParticlesMonitoringAlgorithm()
{
    if (!m_monitoringFileName.empty() && !m_treeName.empty())
    {
        PANDORA_MONITORING_API(SaveTree(m_treeName, m_monitoringFileName, "UPDATE" ));
    }

    delete m_energy;
    delete m_momentumX;
    delete m_momentumY;
    delete m_momentumZ;
    delete m_particleId;
    delete m_outerRadius;
    delete m_innerRadius;
    delete m_caloHitEnergy;
    delete m_trackEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticlesMonitoringAlgorithm::Run()
{
    MCParticleList mcParticleList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetMCParticleList(*this, mcParticleList));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, FillListOfUsedMCParticles());

    MonitorMCParticleList(mcParticleList);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MCParticlesMonitoringAlgorithm::MonitorMCParticleList(const MCParticleList& mcParticleList)
{
    m_energy->clear();
    m_momentumX->clear();
    m_momentumY->clear();
    m_momentumZ->clear();
    m_particleId->clear();
    m_outerRadius->clear();
    m_innerRadius->clear();
    m_caloHitEnergy->clear();
    m_trackEnergy->clear();

    typedef std::multimap<float, int, std::greater<float> > SortIndex;
    SortIndex sortIndex;
    int mcParticleNumber = 0;

    typedef std::vector<const MCParticle*> MCParticleVector;
    MCParticleVector mcParticleVector;

    for (MCParticleList::const_iterator itMc = mcParticleList.begin(), itMcEnd = mcParticleList.end(); itMc != itMcEnd; ++itMc)
    {
        const MCParticle *pMCParticle = (*itMc);

        float caloHitEnergy = 0.f, trackEnergy = 0.f;

        if (TakeMCParticle(pMCParticle, caloHitEnergy, trackEnergy))
        {
            mcParticleVector.push_back(pMCParticle);

            float energy = pMCParticle->GetEnergy();
            sortIndex.insert(std::pair<float,int>(energy, mcParticleNumber));
            m_energy->push_back(energy);

            const CartesianVector &momentum = pMCParticle->GetMomentum();
            m_momentumX->push_back(momentum.GetX());
            m_momentumY->push_back(momentum.GetY());
            m_momentumZ->push_back(momentum.GetZ());

            m_particleId->push_back(pMCParticle->GetParticleId());
            m_outerRadius->push_back(pMCParticle->GetOuterRadius());
            m_innerRadius->push_back(pMCParticle->GetInnerRadius());

            m_caloHitEnergy->push_back(caloHitEnergy);
            m_trackEnergy->push_back(trackEnergy);

            ++mcParticleNumber;
        }
    }

    if (m_sort)
    {
        for (SortIndex::iterator itIdx = sortIndex.begin(), itIdxEnd = sortIndex.end(); itIdx != itIdxEnd; ++itIdx)
        {
            const int idx(itIdx->second);

            assert(std::fabs(itIdx->first - m_energy->at(idx)) < 0.1);

            m_energy->push_back     ( m_energy->at(idx)      );
            m_momentumX->push_back  ( m_momentumX->at(idx)   );
            m_momentumY->push_back  ( m_momentumY->at(idx)   );
            m_momentumZ->push_back  ( m_momentumZ->at(idx)   );
            m_particleId->push_back ( m_particleId->at(idx)  );
            m_outerRadius->push_back( m_outerRadius->at(idx) );
            m_innerRadius->push_back( m_innerRadius->at(idx) );

            m_caloHitEnergy->push_back( m_caloHitEnergy->at(idx) );
            m_trackEnergy->push_back  ( m_trackEnergy->at(idx)   );

            mcParticleVector.push_back( mcParticleVector.at(idx) );
        }

        size_t sortIndexSize = sortIndex.size();
        m_energy->erase( m_energy->begin(), m_energy->begin() + sortIndexSize );
        m_momentumX->erase( m_momentumX->begin(), m_momentumX->begin() + sortIndexSize );
        m_momentumY->erase( m_momentumY->begin(), m_momentumY->begin() + sortIndexSize );
        m_momentumZ->erase( m_momentumZ->begin(), m_momentumZ->begin() + sortIndexSize );
        m_particleId->erase ( m_particleId->begin(),   m_particleId->begin() + sortIndexSize );
        m_outerRadius->erase( m_outerRadius->begin(), m_outerRadius->begin() + sortIndexSize );
        m_innerRadius->erase( m_innerRadius->begin(), m_innerRadius->begin() + sortIndexSize );

        m_caloHitEnergy->erase( m_caloHitEnergy->begin(), m_caloHitEnergy->begin() + sortIndexSize );
        m_trackEnergy->erase  ( m_trackEnergy->begin(),   m_trackEnergy->begin()   + sortIndexSize );

        mcParticleVector.erase( mcParticleVector.begin(), mcParticleVector.begin() + sortIndexSize );
    }

    if (m_print)
    {
        int idx = 0;

        std::sort(mcParticleVector.begin(), mcParticleVector.end(), MCParticle::SortByEnergy);

        for (MCParticleVector::iterator itMc = mcParticleVector.begin(), itMcEnd = mcParticleVector.end(); itMc != itMcEnd; ++itMc)
        {
            const MCParticle *pMcParticle = (*itMc);

            float caloHitEnergy(m_caloHitEnergy->at(idx));
            float trackEnergy(m_trackEnergy->at(idx));

            PrintMCParticle(pMcParticle, caloHitEnergy, trackEnergy, std::cout);
            std::cout << std::endl;
            ++idx;
        }

        std::cout << "Total number of MCPFOs : " << mcParticleNumber << std::endl;
    }

    if (!m_monitoringFileName.empty() && !m_treeName.empty())
    {
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "energy", m_energy));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "pX", m_momentumX));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "pY", m_momentumY));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "pZ", m_momentumZ));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "pdg", m_particleId));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ro", m_outerRadius));
        PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ri", m_innerRadius));

        if (m_haveCaloHits)
        {
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ECalo", m_caloHitEnergy ));
        }

        if (m_haveTracks)
        {
            PANDORA_MONITORING_API(SetTreeVariable(m_treeName, "ETrack", m_trackEnergy ));
        }

        PANDORA_MONITORING_API(FillTree(m_treeName));
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void MCParticlesMonitoringAlgorithm::PrintMCParticle(const MCParticle *pMCParticle, float &caloHitEnergy, float &trackEnergy, std::ostream &o)
{
    if (m_indent)
    {
        int printDepth = (int)(pMCParticle->GetOuterRadius() / 100); // this can be changed if the printout doesn't look good
        o << std::setw (printDepth) << " ";
    }

    const CartesianVector &momentum = pMCParticle->GetMomentum();
    const CartesianVector &endPoint = pMCParticle->GetEndpoint();

    float radius(0.f), phi(0.f), theta(0.f);
    endPoint.GetSphericalCoordinates(radius, phi, theta);
    const float eta(-1. * std::log(std::tan(theta / 2.)));

    o << std::fixed << std::setprecision(2)
      << "PID=" << pMCParticle->GetParticleId()
      << " E=" << pMCParticle->GetEnergy()
      << std::fixed << std::setprecision(4)
      << " ETA=" << eta
      << " PHI=" << phi
      << std::fixed << std::setprecision(1)
      << " r_i=" << pMCParticle->GetInnerRadius()
      << " r_o=" << pMCParticle->GetOuterRadius()
      << std::scientific
      << " px=" << momentum.GetX()
      << " py=" << momentum.GetY()
      << " pz=" << momentum.GetZ()
      << std::fixed << std::setprecision(4);

    if(m_haveCaloHits)
        o << " ECalo=" << caloHitEnergy;

    if(m_haveTracks)
        o << " ETrack=" << trackEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticlesMonitoringAlgorithm::FillListOfUsedMCParticles()
{
    if (m_clusterListNames.empty())
    {
        if (m_haveCaloHits)
        {
            const CaloHitList *pCaloHitList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentCaloHitList(*this, pCaloHitList));

            for(CaloHitList::const_iterator hitIter = pCaloHitList->begin(), hitIterEnd = pCaloHitList->end(); hitIter != hitIterEnd; ++hitIter)
            {
                CaloHit *pCaloHit = (*hitIter);

                const MCParticle *pMCParticle = NULL;
                pCaloHit->GetMCParticle(pMCParticle);

                if (pMCParticle == NULL)
                    continue;

                float energy = pCaloHit->GetElectromagneticEnergy();
                ConstMCParticleToEnergyMap::iterator mcIter = m_mcParticleToEnergyMap.find(pMCParticle);

                if (mcIter == m_mcParticleToEnergyMap.end())
                {
                    m_mcParticleToEnergyMap.insert(std::make_pair(pMCParticle, std::make_pair(energy, 0.f)));
                }
                else
                {
                    mcIter->second.first += energy;
                }
            }
        }

        if (m_haveTracks)
        {
            const TrackList *pTrackList = NULL;
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

            // now for the tracks
            for (TrackList::const_iterator itTrack = pTrackList->begin(), itTrackEnd = pTrackList->end(); itTrack != itTrackEnd; ++itTrack)
            {
                Track *pTrack = (*itTrack);

                const MCParticle* mc = NULL;
                pTrack->GetMCParticle( mc );

                if (mc == NULL)
                    continue; // maybe an error should be thrown here?

                float energy = pTrack->GetEnergyAtDca();
                ConstMCParticleToEnergyMap::iterator itMc = m_mcParticleToEnergyMap.find(mc);

                if (itMc == m_mcParticleToEnergyMap.end())
                {
                    m_mcParticleToEnergyMap.insert(std::make_pair(mc, std::make_pair(0.f, energy)));
                }
                else
                {
                    if (itMc->second.second < energy)
                        itMc->second.second = energy;
                }
            }
        }
    }
    else
    {
        typedef std::vector<const ClusterList*> ClusterVector;
        ClusterVector clusterListVector;

        for (StringVector::iterator itClusterName = m_clusterListNames.begin(), itClusterNameEnd = m_clusterListNames.end();
            itClusterName != itClusterNameEnd; ++itClusterName)
        {
            const ClusterList* pClusterList = NULL;

            if (STATUS_CODE_SUCCESS == PandoraContentApi::GetClusterList(*this, (*itClusterName), pClusterList))
                clusterListVector.push_back(pClusterList); // add the cluster list
        }
    
        for (ClusterVector::const_iterator itClusterList = clusterListVector.begin(), itClusterListEnd = clusterListVector.end();
            itClusterList != itClusterListEnd; ++itClusterList )
        {
            const ClusterList *pClusterList = (*itClusterList);

            for (ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; ++itCluster )
            {
                const Cluster *pCluster = (*itCluster);

                if (m_haveCaloHits)
                {
                    const OrderedCaloHitList &pOrderedCaloHitList = pCluster->GetOrderedCaloHitList();

                    for (OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList.begin(), itLyrEnd = pOrderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
                    {
                        // int pseudoLayer = itLyr->first;
                        CaloHitList::iterator itCaloHit = itLyr->second->begin();
                        CaloHitList::iterator itCaloHitEnd = itLyr->second->end();

                        for( ; itCaloHit != itCaloHitEnd; ++itCaloHit)
                        {
                            CaloHit *pCaloHit = (*itCaloHit);

                            // fetch the MCParticle
                            const MCParticle *mc = NULL;
                            pCaloHit->GetMCParticle(mc);

                            if (mc == NULL)
                                continue;

                            float energy = pCaloHit->GetElectromagneticEnergy();
                            ConstMCParticleToEnergyMap::iterator itMc = m_mcParticleToEnergyMap.find(mc);

                            if (itMc == m_mcParticleToEnergyMap.end())
                            {
                                m_mcParticleToEnergyMap.insert(std::make_pair( mc, std::make_pair(energy, 0.f)));
                            }
                            else
                            {
                                itMc->second.first += energy;
                            }
                        }
                    }
                }

                if (m_haveTracks)
                {
                    const TrackList &trackList = pCluster->GetAssociatedTrackList();

                    // now for the tracks
                    for( TrackList::const_iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack )
                    {
                        Track *pTrack = (*itTrack);

                        const MCParticle *mc = NULL;
                        pTrack->GetMCParticle(mc);

                        if (mc == NULL)
                            continue; // maybe an error should be thrown here?

                        float energy = pTrack->GetEnergyAtDca();
                        ConstMCParticleToEnergyMap::iterator itMc = m_mcParticleToEnergyMap.find(mc);

                        if (itMc == m_mcParticleToEnergyMap.end())
                        {
                            m_mcParticleToEnergyMap.insert(std::make_pair(mc, std::make_pair(0.f, energy)));
                        }
                        else
                        {
                            if (itMc->second.second < energy)
                                itMc->second.second = energy;
                        }
                    }
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MCParticlesMonitoringAlgorithm::TakeMCParticle(const MCParticle* pMCParticle, float& caloHitEnergy, float& trackEnergy)
{
    if (m_onlyFinal && !pMCParticle->GetDaughterList().empty())
        return false;

    if (m_excludeRootParticles && pMCParticle->GetParentList().empty())
        return false;

    if (m_onlyRootParticles && !pMCParticle->GetParentList().empty())
        return false;

    ConstMCParticleToEnergyMap::iterator itMc = m_mcParticleToEnergyMap.find(pMCParticle);

    if (itMc == m_mcParticleToEnergyMap.end())
    {
        caloHitEnergy = 0.f;
        trackEnergy = 0.f;

        if (m_haveCaloHits || m_haveTracks)
        {
            return false;
        }
    }
    else
    {
        caloHitEnergy = itMc->second.first;
        trackEnergy = itMc->second.second;
        return true;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticlesMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MonitoringFileName", m_monitoringFileName));

    m_treeName = "emon";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeName", m_treeName));

    m_print = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Print", m_print));

    m_indent = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Indent", m_indent));

    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "ClusterListNames", m_clusterListNames));

    // "Final" takes only particles without daughters;
    // "ExcludeRoot" excludes particles which don't have parents;
    // "OnlyRoot" takes only particles which don't have parents;
    // "CalorimeterHits"/"Tracks" takes only particles which have caused Tracks or Calorimeterhits.
    //
    // With current rules for MCPFO selection only particles which cross spherical boundary around the IP are retained.
    // All particles are therefore ROOT-particles, the ExcludeRoot, the OnlyRoot and the Final options are therefore nonsensical.
    StringVector mcParticleSelection;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "Selection", mcParticleSelection));

    m_onlyFinal = false;
    m_haveCaloHits = false;
    m_haveTracks = false;

    m_excludeRootParticles = false;
    m_onlyRootParticles = false;   

    for (StringVector::iterator itStr = mcParticleSelection.begin(), itStrEnd = mcParticleSelection.end(); itStr != itStrEnd; ++itStr)
    {
        std::string currentString = (*itStr);

        if (currentString == "Final")
        {
            m_onlyFinal = true;
        }
        else if (currentString == "CalorimeterHits")
        {
            m_haveCaloHits = true;
        }
        else if (currentString == "Tracks")
        {
            m_haveTracks = true;
        }
        else if (currentString == "ExcludeRoot")
        {
            m_excludeRootParticles = true;
        }
        else if (currentString == "OnlyRoot")
        {
            m_onlyRootParticles = true;
        }
        else
        {
            std::cout << "<Selection> '" << currentString << "' unknown in algorithm 'MCParticlesMonitoring'." << std::endl;
            return STATUS_CODE_NOT_FOUND;
        }
    }

    m_sort = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Sort", m_sort));

    return STATUS_CODE_SUCCESS;
}
