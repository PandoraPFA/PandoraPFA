/**
 *  @file   PandoraPFANew/src/Algorithms/Monitoring/EfficiencyMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the energy monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Algorithms/Monitoring/EfficiencyMonitoringAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EfficiencyMonitoringAlgorithm::Initialize() 
{
    m_numberFindableMCs = 0; // for MCParticle efficiency (global)
    m_numberFoundMCs = 0; // for MCParticle efficiency (global)
    m_numberNotFoundMCs = 0; // for MCParticle efficiency (global)
    m_numberFakes = 0; // for MCParticle efficiency (global)
    m_numberEvents = 0;
    m_eventEfficiency = 0.f; // for MCParticle efficiency averaged over the events
    m_eventPurity = 0.f; // for MCParticle efficiency averaged over the events


    std::stringstream sstr;
    sstr << "_";
    sstr << m_particleId;
    std::string particleIdString("");
    sstr >> particleIdString;

    while (size_t pos = particleIdString.find("-") != std::string::npos)
    {
        particleIdString.replace(pos,1,"m");
    } 

    m_histEntriesForEffPerThetaBin = "EntriesForEffPerThetaBin";
    m_histEntriesForEffPerEnergyBin = "EntriesForEffPerEnergyBin";
    m_histEntriesForPurityPerThetaBin = "EntriesForPurityPerThetaBin";
    m_histEntriesForPurityPerEnergyBin = "EntriesForPurityPerEnergyBin";

    m_histMCParticleEffVsTheta = "MCParticleEffVsTheta";
    m_histMCParticleEffVsEnergy = "MCParticleEffVsEnergy";
    m_histMCParticlePurityVsTheta = "MCParticlePurityVsTheta";
    m_histMCParticlePurityVsEnergy = "MCParticlePurityVsEnergy";

    m_histCaloHitMCEffVsTheta = "CaloHitMCEffVsTheta";
    m_histCaloHitMCEffVsEnergy = "CaloHitMCEffVsEnergy";
    m_histCaloHitMCPurityVsTheta = "CaloHitMCPurityVsTheta";
    m_histCaloHitMCPurityVsEnergy = "CaloHitMCPurityVsEnergy";

    m_histPFO_MC_EDiffMulByEMC = "PFO_MC_EDiffMulByEMC";
    m_histMCFractionLargestContribution = "MCFractionLargestContribution";


    m_histEntriesForEffPerThetaBin.append(particleIdString);
    m_histEntriesForEffPerEnergyBin.append(particleIdString);
    m_histEntriesForPurityPerThetaBin.append(particleIdString);
    m_histEntriesForPurityPerEnergyBin.append(particleIdString);

    m_histMCParticleEffVsTheta.append(particleIdString);
    m_histMCParticleEffVsEnergy.append(particleIdString);
    m_histMCParticlePurityVsTheta.append(particleIdString);
    m_histMCParticlePurityVsEnergy.append(particleIdString);

    m_histCaloHitMCEffVsTheta.append(particleIdString);
    m_histCaloHitMCEffVsEnergy.append(particleIdString);
    m_histCaloHitMCPurityVsTheta.append(particleIdString);
    m_histCaloHitMCPurityVsEnergy.append(particleIdString);

    m_histPFO_MC_EDiffMulByEMC.append(particleIdString);
    m_histMCFractionLargestContribution.append(particleIdString);




    PANDORA_MONITORING_API(Create1DHistogram(m_histEntriesForEffPerThetaBin,"Entries per #Theta bin", m_thetaBins, m_thetaMin, m_thetaMax, "#Theta", "N"));
    PANDORA_MONITORING_API(Create1DHistogram(m_histEntriesForEffPerEnergyBin,"Entries per energy bin", m_energyBins, m_energyMin, m_energyMax, "E", "N"));

    PANDORA_MONITORING_API(Create1DHistogram(m_histEntriesForPurityPerThetaBin,"Entries per #Theta bin", m_thetaBins, m_thetaMin, m_thetaMax, "#Theta", "N"));
    PANDORA_MONITORING_API(Create1DHistogram(m_histEntriesForPurityPerEnergyBin,"Entries per energy bin", m_energyBins, m_energyMin, m_energyMax, "E", "N"));


    PANDORA_MONITORING_API(Create1DHistogram(m_histMCParticleEffVsTheta,"MC particle efficiency vs. #Theta", m_thetaBins, m_thetaMin, m_thetaMax, "#Theta", "#epsilon"));
    PANDORA_MONITORING_API(Create1DHistogram(m_histMCParticleEffVsEnergy,"MC particle efficiency vs. Energy", m_energyBins, m_energyMin, m_energyMax, "E_{MC}", "#epsilon"));

    PANDORA_MONITORING_API(Create1DHistogram(m_histMCParticlePurityVsTheta,"MC particle purity vs. #Theta", m_thetaBins, m_thetaMin, m_thetaMax, "#Theta", "purity"));
    PANDORA_MONITORING_API(Create1DHistogram(m_histMCParticlePurityVsEnergy,"MC particle purity vs. Energy", m_energyBins, m_energyMin, m_energyMax, "E_{PFO}", "purity"));

    PANDORA_MONITORING_API(Create2DHistogram(m_histCaloHitMCEffVsTheta,"Calorimeter-hit-MC efficiency vs. #Theta", m_thetaBins, m_thetaMin, m_thetaMax, 24, -0.1, 1.1, "#Theta", "#epsilon_{Calohit}"));
    PANDORA_MONITORING_API(Create2DHistogram(m_histCaloHitMCEffVsEnergy,"Calorimeter-hit-MC efficiency vs. Energy", m_energyBins, m_energyMin, m_energyMax, 24, -0.1, 1.1, "E_{PFO}", "#epsilon_{Calohit}"));

    PANDORA_MONITORING_API(Create2DHistogram(m_histCaloHitMCPurityVsTheta,"Calorimeter-hit-MC purity vs. #Theta", m_thetaBins, m_thetaMin, m_thetaMax, 24, -0.1, 1.1, "#Theta", "purity_{Calohit}"));
    PANDORA_MONITORING_API(Create2DHistogram(m_histCaloHitMCPurityVsEnergy,"Calorimeter-hit-MC purity vs. Energy", m_energyBins, m_energyMin, m_energyMax, 24, -0.1, 1.1, "E_{PFO}", "purity_{Calohit}"));

    PANDORA_MONITORING_API(Create1DHistogram(m_histPFO_MC_EDiffMulByEMC,"#Delta(E_{PFO}-E_{MC})/#sqrt{E_{MC}}", 2*m_energyBins, -m_calorimeterResolutionStochasticCut*3, 
                                             m_calorimeterResolutionStochasticCut*3, "#Delta(E_{PFO}-E_{MC})/#sqrt{E_{MC}}", "N [%]"));

    PANDORA_MONITORING_API(Create1DHistogram(m_histMCFractionLargestContribution,"Calohit energy fraction of the MC which gives the largest contribution", m_energyBins, 0.f, 110.f, 
                                             "Calohit energy fraction in PFO", "N [%]" ));


    m_foundTreeName.append(particleIdString);
    m_notFoundTreeName.append(particleIdString);
    m_fakesTreeName.append(particleIdString);
    m_otherTreeName.append(particleIdString);

    m_pfoCaloHitETreeName.append(particleIdString);
    m_mcCaloHitETreeName.append(particleIdString);
    m_controlTreeName.append(particleIdString);
    m_eventTreeName.append(particleIdString);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

EfficiencyMonitoringAlgorithm::~EfficiencyMonitoringAlgorithm()
{
    float mcParticleEfficiency = (m_numberFindableMCs                    ==0)?0:float(m_numberFoundMCs)/float(m_numberFindableMCs);
    float mcParticlePurity     = (m_numberFoundMCs+m_numberFakes         ==0)?1:float(m_numberFoundMCs)/float(m_numberFoundMCs+m_numberFakes);

    if (m_numberEvents != 0)
    {
        m_eventEfficiency /= float(m_numberEvents);
        m_eventPurity     /= float(m_numberEvents);
    }

    std::cout << "------------------------ Results: Efficiency/Purity ---------------------------" << std::endl;
    std::cout << "MCParticle efficiency : " << mcParticleEfficiency << "  (number of MC particles found divided by total number of MC particles)" << std::endl;
    std::cout << "MCParticle purity     : " << mcParticlePurity <<     "  (number of found MCs divided by the found and fake MC particles)" << std::endl;
    std::cout << std::endl;
    std::cout << "#Events               : " << m_numberEvents << std::endl; 
    std::cout << "avg. MCParticle efficiency per Event : " << m_eventEfficiency << std::endl;
    std::cout << "avg. MCParticle purity per Event     : " << m_eventPurity << std::endl;


    PANDORA_MONITORING_API(DivideHistograms(m_histMCParticleEffVsTheta,m_histEntriesForEffPerThetaBin, 1.f, 1.f)); // divide the histograms
    PANDORA_MONITORING_API(DivideHistograms(m_histMCParticleEffVsEnergy,m_histEntriesForEffPerEnergyBin, 1.f, 1.f)); // divide the histograms

    PANDORA_MONITORING_API(DivideHistograms(m_histMCParticlePurityVsTheta,m_histEntriesForPurityPerThetaBin, 1.f, 1.f)); // divide the histograms
    PANDORA_MONITORING_API(DivideHistograms(m_histMCParticlePurityVsEnergy,m_histEntriesForPurityPerEnergyBin, 1.f, 1.f)); // divide the histograms

    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histEntriesForEffPerThetaBin,m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histEntriesForEffPerEnergyBin,m_monitoringFileName, "UPDATE"));

    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histEntriesForPurityPerThetaBin,m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histEntriesForPurityPerEnergyBin,m_monitoringFileName, "UPDATE"));

    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histMCParticleEffVsTheta,m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histMCParticleEffVsEnergy,m_monitoringFileName, "UPDATE"));

    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histMCParticlePurityVsTheta,m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histMCParticlePurityVsEnergy,m_monitoringFileName, "UPDATE"));

    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histCaloHitMCEffVsTheta,m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histCaloHitMCEffVsEnergy,m_monitoringFileName, "UPDATE"));

    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histCaloHitMCPurityVsTheta,m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histCaloHitMCPurityVsEnergy,m_monitoringFileName, "UPDATE"));

    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histPFO_MC_EDiffMulByEMC,m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveAndCloseHistogram(m_histMCFractionLargestContribution,m_monitoringFileName, "UPDATE"));


    PANDORA_MONITORING_API(SaveTree(m_foundTreeName, m_monitoringFileName, "UPDATE" ));
    PANDORA_MONITORING_API(SaveTree(m_notFoundTreeName, m_monitoringFileName, "UPDATE" ));
    PANDORA_MONITORING_API(SaveTree(m_otherTreeName, m_monitoringFileName, "UPDATE" ));
    PANDORA_MONITORING_API(SaveTree(m_fakesTreeName, m_monitoringFileName, "UPDATE" ));

    PANDORA_MONITORING_API(SaveTree(m_pfoCaloHitETreeName, m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveTree(m_mcCaloHitETreeName, m_monitoringFileName, "UPDATE"));
    PANDORA_MONITORING_API(SaveTree(m_controlTreeName, m_monitoringFileName, "UPDATE"));

    PANDORA_MONITORING_API(SaveTree(m_eventTreeName, m_monitoringFileName, "UPDATE"));

}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EfficiencyMonitoringAlgorithm::Run()
{

    MCParticleList mcParticleList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetMCParticleList(*this, mcParticleList));

    typedef std::set<const MCParticle*> ConstMCParticleList;
    ConstMCParticleList mcSelectedParticleList;



    for (MCParticleList::iterator itMc = mcParticleList.begin(), itMcEnd = mcParticleList.end(); itMc != itMcEnd; ++itMc )
    {
        const MCParticle* mcParticle = (*itMc);
        if( mcParticle == NULL )
            continue;

        if (mcParticle->GetParticleId() == m_particleId )
            mcSelectedParticleList.insert(mcParticle);
    }

    
    if (mcSelectedParticleList.empty())
        return STATUS_CODE_SUCCESS;


    const int findableMCs = mcSelectedParticleList.size(); 
    m_numberFindableMCs += findableMCs;


    typedef std::map<const MCParticle*,float> MCParticleEnergyMap;
    MCParticleEnergyMap mcParticleEnergyMap;

    typedef std::map<const ParticleFlowObject*,const MCParticle* > ParticleFlowObjectToMCParticle;
    typedef std::multimap<const MCParticle*,const ParticleFlowObject* > MCParticleToParticleFlowObject;

    // get MCPfos


    // write mc particles of type m_particleId into a list
    MCParticleToParticleFlowObject       foundMCToPfo;
    ParticleFlowObjectToMCParticle       falseFoundPfo;
    ParticleFlowObjectToMCParticle       otherPfo;

    typedef std::vector<float> FloatVec;
    typedef std::vector<int>   IntVec;

    FloatVec found_pfoTheta;
    FloatVec found_pfoPhi;
    FloatVec found_pfoEnergy;
    FloatVec found_pfoP;
    IntVec   found_pfoCharge;
    FloatVec found_mcTheta;
    FloatVec found_mcPhi;
    FloatVec found_mcEnergy;
    FloatVec found_mcP;

    FloatVec falseFound_pfoTheta;
    FloatVec falseFound_pfoPhi;
    FloatVec falseFound_pfoEnergy;
    FloatVec falseFound_pfoP;
    IntVec   falseFound_pfoCharge;

    FloatVec notFound_mcTheta;
    FloatVec notFound_mcPhi;
    FloatVec notFound_mcEnergy;
    FloatVec notFound_mcP;

    FloatVec other_pfoTheta;
    FloatVec other_pfoPhi;
    FloatVec other_pfoEnergy;
    FloatVec other_pfoP;
    IntVec   other_pfoCharge;

    FloatVec pfoCaloHitE_truePositive;
    FloatVec pfoCaloHitE_falsePositive;
    FloatVec pfoCaloHitE_trueNegative;
    FloatVec pfoCaloHitE_purity;

    FloatVec pfoCaloHitE_pfoTheta;
    FloatVec pfoCaloHitE_pfoPhi;
    FloatVec pfoCaloHitE_pfoEnergy;
    FloatVec pfoCaloHitE_pfoP;
    IntVec   pfoCaloHitE_pfoCharge;

    FloatVec mcCaloHitE_truePositive;
    FloatVec mcCaloHitE_falseNegative;
    FloatVec mcCaloHitE_efficiency;

    FloatVec mcCaloHitE_mcTheta;
    FloatVec mcCaloHitE_mcPhi;
    FloatVec mcCaloHitE_mcEnergy;
    FloatVec mcCaloHitE_mcP;


    FloatVec control_energyOfLargestAndMatchingMC;
    FloatVec control_pfoMCCaloHitEFraction;
    FloatVec control_largestContribution;
    FloatVec control_totalContribution;
    FloatVec control_pfoEnergy;
    FloatVec control_pfoMCEDiffMulByEMC;
    FloatVec control_passedCriteria;


    // clear all the vectors
    found_pfoTheta.clear();
    found_pfoPhi.clear();
    found_pfoEnergy.clear();
    found_pfoP.clear();
    found_pfoCharge.clear();
    found_mcTheta.clear();
    found_mcPhi.clear();
    found_mcEnergy.clear();
    found_mcP.clear();

    falseFound_pfoTheta.clear();
    falseFound_pfoPhi.clear();
    falseFound_pfoEnergy.clear();
    falseFound_pfoP.clear();
    falseFound_pfoCharge.clear();

    notFound_mcTheta.clear();
    notFound_mcPhi.clear();
    notFound_mcEnergy.clear();
    notFound_mcP.clear();

    other_pfoTheta.clear();
    other_pfoPhi.clear();
    other_pfoEnergy.clear();
    other_pfoP.clear();
    other_pfoCharge.clear();

    pfoCaloHitE_truePositive.clear();
    pfoCaloHitE_falsePositive.clear();
    pfoCaloHitE_trueNegative.clear();
    pfoCaloHitE_purity.clear();

    pfoCaloHitE_pfoTheta.clear();
    pfoCaloHitE_pfoPhi.clear();
    pfoCaloHitE_pfoEnergy.clear();
    pfoCaloHitE_pfoP.clear();
    pfoCaloHitE_pfoCharge.clear();

    mcCaloHitE_truePositive.clear();
    mcCaloHitE_falseNegative.clear();
    mcCaloHitE_efficiency.clear();

    mcCaloHitE_mcTheta.clear();
    mcCaloHitE_mcPhi.clear();
    mcCaloHitE_mcEnergy.clear();
    mcCaloHitE_mcP.clear();


    control_energyOfLargestAndMatchingMC.clear();
    control_pfoMCCaloHitEFraction.clear();
    control_largestContribution.clear();
    control_totalContribution.clear();
    control_pfoEnergy.clear();
    control_pfoMCEDiffMulByEMC.clear();
    control_passedCriteria.clear();



    // get Pfos
    const ParticleFlowObjectList* pParticleFlowObjectList = NULL;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_INITIALIZED, !=, PandoraContentApi::GetCurrentPfoList(*this,pParticleFlowObjectList));

    typedef std::map<const ParticleFlowObject*, CaloHitMCType> PFOToCaloHitMCTypeMap;
    static PFOToCaloHitMCTypeMap pfoToCaloHitMCTypeMap;
    typedef std::map<const MCParticle*, CaloHitMCType> MCParticleToCaloHitMCTypeMap;
    static MCParticleToCaloHitMCTypeMap mcParticleToCaloHitMCTypeMap;

    pfoToCaloHitMCTypeMap.clear();
    mcParticleToCaloHitMCTypeMap.clear();


    if (pParticleFlowObjectList)
    {
        for (ParticleFlowObjectList::const_iterator itPfo = pParticleFlowObjectList->begin(), itPfoEnd = pParticleFlowObjectList->end(); itPfo != itPfoEnd; ++itPfo )
        {
            const ParticleFlowObject* pPfo = (*itPfo);
        
            // get the important data from the pfo
            CartesianVector         momentum      = pPfo->GetMomentum();  
            float                   pfoEnergy     = pPfo->GetEnergy();
            int                     pfoParticleId = pPfo->GetParticleId();
            ClusterList             clusterList   = pPfo->GetClusterList();
            int                     pfoCharge     = pPfo->GetCharge();

            float pfoPMagnitude, pfoPhi, pfoTheta;
            momentum.GetSphericalCoordinates(pfoPMagnitude,pfoPhi,pfoTheta);


            mcParticleEnergyMap.clear();
            for (ClusterList::const_iterator itCluster = clusterList.begin(), itClusterEnd = clusterList.end(); itCluster != itClusterEnd; ++itCluster)
            {

                const Cluster* pCluster = (*itCluster);
        
                const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
                for (OrderedCaloHitList::const_iterator itLyr = orderedCaloHitList.begin(), itLyrEnd = orderedCaloHitList.end(); itLyr != itLyrEnd; ++itLyr)
                {
                    for (CaloHitList::const_iterator hitIter = itLyr->second->begin(), hitIterEnd = itLyr->second->end(); hitIter != hitIterEnd; ++hitIter)
                    {
                        CaloHit* pCaloHit = (*hitIter);

                        const MCParticle *pMCParticle = NULL; 
                        pCaloHit->GetMCParticle(pMCParticle);

                        if (NULL == pMCParticle)
                            continue;

                        int caloHitMCParticleId = pMCParticle->GetParticleId();
                        float energy = pCaloHit->GetElectromagneticEnergy();

                        MCParticleEnergyMap::iterator itMCPtclE = mcParticleEnergyMap.find(pMCParticle);
                        if (itMCPtclE == mcParticleEnergyMap.end() ) // if its not in this map, its in no map
                            itMCPtclE = mcParticleEnergyMap.insert(std::make_pair(pMCParticle,0.f)).first;

                        float oldEnergy = itMCPtclE->second;
                        itMCPtclE->second = oldEnergy + energy;

                        PFOToCaloHitMCTypeMap::iterator itCHPfo = pfoToCaloHitMCTypeMap.find(pPfo);
                        if (itCHPfo==pfoToCaloHitMCTypeMap.end())
                            itCHPfo    = pfoToCaloHitMCTypeMap.insert(std::make_pair(pPfo,CaloHitMCType())).first;
                        MCParticleToCaloHitMCTypeMap::iterator itCHMC = mcParticleToCaloHitMCTypeMap.find(pMCParticle);
                        if (itCHMC==mcParticleToCaloHitMCTypeMap.end())
                            itCHMC    = mcParticleToCaloHitMCTypeMap.insert(std::make_pair(pMCParticle,CaloHitMCType())).first;

                        // for calohitenergy-MC efficiency
                        if (caloHitMCParticleId == m_particleId)
                            if (pfoParticleId == m_particleId)
                            {
                                itCHPfo->second.m_truePositiveEnergy += energy;
                                itCHMC->second.m_truePositiveEnergy += energy;
                            }
                            else
                            {
                                itCHPfo->second.m_falseNegativeEnergy += energy;
                                itCHMC->second.m_falseNegativeEnergy += energy;
                            }
                        else
                            if (pfoParticleId == m_particleId)
                            {
                                itCHPfo->second.m_falsePositiveEnergy += energy;
                                itCHMC->second.m_falsePositiveEnergy += energy;
                            }
                            else
                            {
                                itCHPfo->second.m_trueNegativeEnergy += energy;
                                itCHMC->second.m_trueNegativeEnergy += energy;
                            }

                    }
                }
            }

            // search for particle with largest contribution
            const MCParticle* mcLargestContribution = NULL;
            float largestContribution = -1.f;
            float totalContributions = 0.f;
            float energyOfLargestAndMatchingMC = 0.f;
            for (MCParticleEnergyMap::iterator itMcE = mcParticleEnergyMap.begin(), itMcEEnd = mcParticleEnergyMap.end(); itMcE != itMcEEnd; ++itMcE )
            {
                const MCParticle* mc = itMcE->first;
                float eContrib = itMcE->second;
            
                totalContributions += eContrib;

                if (eContrib > largestContribution)
                {
                    largestContribution = eContrib;
                    mcLargestContribution = mc;
                    if( (mc != NULL) && (mc->GetParticleId()==m_particleId ) )
                        energyOfLargestAndMatchingMC = mc->GetEnergy();
                }
            }

            bool passedCriteria = largestContribution/totalContributions > 0.5 && largestContribution/totalContributions > (m_minCaloHitEnergyFraction/100.f);
            float caloCuts = 0.f;
            if (m_calorimeterResolutionStochasticCut > 0.f )
                caloCuts += (m_calorimeterResolutionStochasticCut/100.f)*sqrt(energyOfLargestAndMatchingMC);

            if (m_calorimeterResolutionConstantCut > 0.f )
                caloCuts += m_calorimeterResolutionConstantCut/100.f;

            if (caloCuts > 0.f )
                passedCriteria = passedCriteria && (fabs(energyOfLargestAndMatchingMC-pfoEnergy) < caloCuts);



            // find mc particle in selected ones
            ConstMCParticleList::iterator itFoundMC = mcSelectedParticleList.find(mcLargestContribution);

            if (itFoundMC != mcSelectedParticleList.end()  ) // not found 
            {
                float pfoMCEDiffMulByEMC = (energyOfLargestAndMatchingMC-pfoEnergy) / sqrt(energyOfLargestAndMatchingMC)*100.f;
                float pfoMCCaloHitEFraction = 100.f*largestContribution/totalContributions;
                PANDORA_MONITORING_API(Fill1DHistogram(m_histPFO_MC_EDiffMulByEMC, pfoMCEDiffMulByEMC )); 
                PANDORA_MONITORING_API(Fill1DHistogram(m_histMCFractionLargestContribution, pfoMCCaloHitEFraction )); 

                control_energyOfLargestAndMatchingMC.push_back(energyOfLargestAndMatchingMC);
                control_pfoMCCaloHitEFraction.push_back(pfoMCCaloHitEFraction);
                control_largestContribution.push_back(largestContribution);
                control_totalContribution.push_back(totalContributions);
                control_pfoEnergy.push_back(pfoEnergy);
                control_pfoMCEDiffMulByEMC.push_back(pfoMCEDiffMulByEMC);
                control_passedCriteria.push_back(int(passedCriteria));
            }

            if (itFoundMC == mcSelectedParticleList.end() || !passedCriteria ) // not found or doesn't pass selection criteria
            {
                if( pPfo->GetParticleId() == m_particleId ) // but the particle ID matches
                {
                    falseFoundPfo.insert(std::make_pair(pPfo,mcLargestContribution)); // then it's a false found (fake)

                    // take the pfo information here, because no useful MCParticle information is available for fake Pfos
                    PANDORA_MONITORING_API(Fill1DHistogram(m_histMCParticlePurityVsTheta, pfoTheta, 0.f ));
                    PANDORA_MONITORING_API(Fill1DHistogram(m_histMCParticlePurityVsEnergy, pfoEnergy, 0.f ));

                    PANDORA_MONITORING_API(Fill1DHistogram(m_histEntriesForPurityPerThetaBin,pfoTheta));
                    PANDORA_MONITORING_API(Fill1DHistogram(m_histEntriesForPurityPerEnergyBin,pfoEnergy));

                    falseFound_pfoTheta.push_back(pfoTheta);
                    falseFound_pfoPhi.push_back(pfoPhi);
                    falseFound_pfoEnergy.push_back(pfoEnergy);
                    falseFound_pfoP.push_back(pfoPMagnitude);
                    falseFound_pfoCharge.push_back(pfoCharge);
                }
                else // particle id doesn't match
                {
                    otherPfo.insert(std::make_pair(pPfo,mcLargestContribution));

                    other_pfoTheta.push_back(pfoTheta);
                    other_pfoPhi.push_back(pfoPhi);
                    other_pfoEnergy.push_back(pfoEnergy);
                    other_pfoP.push_back(pfoPMagnitude);
                    other_pfoCharge.push_back(pfoCharge);
                }

            }
            else
            {
                const MCParticle* foundMC = (*itFoundMC); // the MC particle found in the selected particles
            
                float mcPMagnitude, mcPhi, mcTheta;
                const CartesianVector& foundMCMomentum = foundMC->GetMomentum();
                foundMCMomentum.GetSphericalCoordinates(mcPMagnitude,mcPhi,mcTheta);

                float mcEnergy = foundMC->GetEnergy();

                foundMCToPfo.insert(std::make_pair(foundMC,pPfo)); // then insert the MCParticle/Pfo pair


                // TO THINK: in case of two PFOs per one MCParticle, now the parameters all of the found MCParticles are filled into the histograms
                PANDORA_MONITORING_API(Fill1DHistogram(m_histMCParticlePurityVsTheta, pfoTheta, 1.f )); 
                PANDORA_MONITORING_API(Fill1DHistogram(m_histMCParticlePurityVsEnergy, pfoEnergy, 1.f ));

                PANDORA_MONITORING_API(Fill1DHistogram(m_histMCParticleEffVsTheta, mcTheta, 1.f ));
                PANDORA_MONITORING_API(Fill1DHistogram(m_histMCParticleEffVsEnergy, mcEnergy, 1.f ));

                PANDORA_MONITORING_API(Fill1DHistogram(m_histEntriesForEffPerThetaBin,mcTheta));
                PANDORA_MONITORING_API(Fill1DHistogram(m_histEntriesForEffPerEnergyBin,mcEnergy));

                PANDORA_MONITORING_API(Fill1DHistogram(m_histEntriesForPurityPerThetaBin,pfoTheta));
                PANDORA_MONITORING_API(Fill1DHistogram(m_histEntriesForPurityPerEnergyBin,pfoEnergy));

                found_pfoTheta.push_back(pfoTheta);
                found_pfoPhi.push_back(pfoPhi);
                found_pfoEnergy.push_back(pfoEnergy);
                found_pfoP.push_back(pfoPMagnitude);
                found_pfoCharge.push_back(pfoCharge);

                found_mcTheta.push_back(mcTheta);
                found_mcPhi.push_back(mcPhi);
                found_mcEnergy.push_back(mcEnergy);
                found_mcP.push_back(mcPMagnitude);
            }
        }
    }

    // erase all MCParticles which have been found
    for (MCParticleToParticleFlowObject::iterator itFoundMC = foundMCToPfo.begin(), itFoundMCEnd = foundMCToPfo.end(); itFoundMC != itFoundMCEnd; ++itFoundMC )
    {
        const MCParticle* mc = itFoundMC->first;
        mcSelectedParticleList.erase(mc);
    }
    
    // loop over all left-over MCParticles (the ones which were not found)
    for (ConstMCParticleList::iterator itMc = mcSelectedParticleList.begin(), itMcEnd = mcSelectedParticleList.end(); itMc != itMcEnd; ++itMc )
    {
        const MCParticle* mcPtcl = (*itMc);

        float mcPMagnitude, mcPhi, mcTheta;
        const CartesianVector& notFoundMCMomentum = mcPtcl->GetMomentum();
        notFoundMCMomentum.GetSphericalCoordinates(mcPMagnitude,mcPhi,mcTheta);

        float mcEnergy = mcPtcl->GetEnergy();

        PANDORA_MONITORING_API(Fill1DHistogram(m_histMCParticleEffVsTheta, mcTheta, 0.f ));
        PANDORA_MONITORING_API(Fill1DHistogram(m_histMCParticleEffVsEnergy, mcEnergy, 0.f ));

        PANDORA_MONITORING_API(Fill1DHistogram(m_histEntriesForEffPerThetaBin,mcTheta));
        PANDORA_MONITORING_API(Fill1DHistogram(m_histEntriesForEffPerEnergyBin,mcEnergy));

        notFound_mcTheta.push_back(mcTheta);
        notFound_mcPhi.push_back(mcPhi);
        notFound_mcEnergy.push_back(mcEnergy);
        notFound_mcP.push_back(mcPMagnitude);
    }



    // compute the numbers
    int truePositive  = foundMCToPfo.size();
    int falsePositive = falseFoundPfo.size();
    int trueNegative  = otherPfo.size();
    int falseNegative = mcSelectedParticleList.size();


    m_numberFoundMCs    += truePositive;  // for MCParticle efficiency (global)
    m_numberNotFoundMCs += falseNegative; // for MCParticle efficiency (global)
    m_numberFakes       += falsePositive; // for MCParticle efficiency (global)

    float eventEfficiency = (findableMCs               ==0) ? 0 : float(truePositive)/float(findableMCs);
    // in case of no pfos, the purity is set to 1, since missing pfos should not increase the impurity.
    float eventPurity     = (truePositive+falsePositive==0) ? 1 : float(truePositive)/float(truePositive+falsePositive); 

    m_eventEfficiency     += eventEfficiency; // for MCParticle efficiency averaged over the events
    m_eventPurity         += eventPurity;     // for MCParticle efficiency averaged over the events

    if (m_print)
    {
        std::cout << "Event: " << m_numberEvents << " --- " << std::endl;
        std::cout << "efficiency: " << eventEfficiency << "  purity: " << eventPurity;
        std::cout << "    found: " << truePositive << "   fakes:" << falsePositive << "  not found: " << falseNegative << "  other: " << trueNegative << std::endl;
    }

    PANDORA_MONITORING_API(SetTreeVariable(m_eventTreeName, "found", truePositive ));
    PANDORA_MONITORING_API(SetTreeVariable(m_eventTreeName, "fakes", falsePositive ));
    PANDORA_MONITORING_API(SetTreeVariable(m_eventTreeName, "other", trueNegative ));
    PANDORA_MONITORING_API(SetTreeVariable(m_eventTreeName, "notFound", falseNegative ));
    PANDORA_MONITORING_API(SetTreeVariable(m_eventTreeName, "eff", eventEfficiency ));
    PANDORA_MONITORING_API(SetTreeVariable(m_eventTreeName, "purity", eventPurity ));

    // now fill the calohit energy MC efficiency plots
    const float epsilon = 1e-7;
    MCParticleToCaloHitMCTypeMap::iterator itCHMC = mcParticleToCaloHitMCTypeMap.begin(), itCHMCEnd = mcParticleToCaloHitMCTypeMap.end();
    for ( ; itCHMC!=itCHMCEnd; ++itCHMC )
    {
        const MCParticle* mc = itCHMC->first;
        float mcEnergy = mc->GetEnergy();
        float mcPMagnitude, mcPhi, mcTheta;
        const CartesianVector& mcMomentum = mc->GetMomentum();
        mcMomentum.GetSphericalCoordinates(mcPMagnitude,mcPhi,mcTheta);
        
        float truePositiveCHEnergy  = itCHMC->second.m_truePositiveEnergy;
        float falseNegativeCHEnergy = itCHMC->second.m_falseNegativeEnergy;

        float caloHitEnergyEfficiency = 0.f;
        if (truePositiveCHEnergy+falseNegativeCHEnergy>epsilon)
            caloHitEnergyEfficiency = truePositiveCHEnergy/(truePositiveCHEnergy+falseNegativeCHEnergy);

        PANDORA_MONITORING_API(Fill2DHistogram(m_histCaloHitMCEffVsTheta, mcTheta, caloHitEnergyEfficiency ));
        PANDORA_MONITORING_API(Fill2DHistogram(m_histCaloHitMCEffVsEnergy, mcEnergy, caloHitEnergyEfficiency ));
        
        mcCaloHitE_truePositive.push_back(truePositiveCHEnergy);
        mcCaloHitE_falseNegative.push_back(falseNegativeCHEnergy);
        mcCaloHitE_efficiency.push_back(caloHitEnergyEfficiency);

        mcCaloHitE_mcTheta.push_back(mcTheta);
        mcCaloHitE_mcPhi.push_back(mcPhi);
        mcCaloHitE_mcEnergy.push_back(mcEnergy);
        mcCaloHitE_mcP.push_back(mcPMagnitude);
    }

    // now fill the calohit energy PFO purity plots
    PFOToCaloHitMCTypeMap::iterator itCHPfo = pfoToCaloHitMCTypeMap.begin(), itCHPfoEnd = pfoToCaloHitMCTypeMap.end();
    for ( ; itCHPfo!=itCHPfoEnd; ++itCHPfo )
    {
        const ParticleFlowObject* pfo = itCHPfo->first;
        float pfoEnergy = pfo->GetEnergy();
        int   pfoCharge = pfo->GetCharge();
        float pfoPMagnitude, pfoPhi, pfoTheta;
        const CartesianVector& pfoMomentum = pfo->GetMomentum();
        pfoMomentum.GetSphericalCoordinates(pfoPMagnitude,pfoPhi,pfoTheta);
        
        float truePositiveCHEnergy  = itCHPfo->second.m_truePositiveEnergy;
        float falsePositiveCHEnergy = itCHPfo->second.m_falsePositiveEnergy;
        float trueNegativeCHEnergy  = itCHPfo->second.m_trueNegativeEnergy;

        float caloHitEnergyPurity = 0.f;
        if (truePositiveCHEnergy+falsePositiveCHEnergy>epsilon)
        {
            caloHitEnergyPurity = truePositiveCHEnergy/(truePositiveCHEnergy+falsePositiveCHEnergy);
        }
        else
            caloHitEnergyPurity = 1.f;

        PANDORA_MONITORING_API(Fill2DHistogram(m_histCaloHitMCPurityVsTheta, pfoTheta, caloHitEnergyPurity ));
        PANDORA_MONITORING_API(Fill2DHistogram(m_histCaloHitMCPurityVsEnergy, pfoEnergy, caloHitEnergyPurity ));

        pfoCaloHitE_truePositive.push_back(truePositiveCHEnergy);
        pfoCaloHitE_falsePositive.push_back(falsePositiveCHEnergy);
        pfoCaloHitE_trueNegative.push_back(trueNegativeCHEnergy);
        pfoCaloHitE_purity.push_back(caloHitEnergyPurity);

        pfoCaloHitE_pfoTheta.push_back(pfoTheta);
        pfoCaloHitE_pfoPhi.push_back(pfoPhi);
        pfoCaloHitE_pfoEnergy.push_back(pfoEnergy);
        pfoCaloHitE_pfoP.push_back(pfoPMagnitude);
        pfoCaloHitE_pfoCharge.push_back(pfoCharge);
    }



    // fill the trees
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "pfo_theta", &found_pfoTheta ));
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "pfo_phi", &found_pfoPhi));
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "pfo_E", &found_pfoEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "pfo_p", &found_pfoP));
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "pfo_charge", &found_pfoCharge));
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "mc_theta", &found_mcTheta));
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "mc_phi", &found_mcPhi));
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "mc_E", &found_mcEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(m_foundTreeName, "mc_p", &found_mcP));

    PANDORA_MONITORING_API(SetTreeVariable(m_fakesTreeName, "pfo_theta", &falseFound_pfoTheta ));
    PANDORA_MONITORING_API(SetTreeVariable(m_fakesTreeName, "pfo_phi", &falseFound_pfoPhi));
    PANDORA_MONITORING_API(SetTreeVariable(m_fakesTreeName, "pfo_E", &falseFound_pfoEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(m_fakesTreeName, "pfo_p", &falseFound_pfoP));
    PANDORA_MONITORING_API(SetTreeVariable(m_fakesTreeName, "pfo_charge", &falseFound_pfoCharge));

    PANDORA_MONITORING_API(SetTreeVariable(m_notFoundTreeName, "mc_theta", &notFound_mcTheta));
    PANDORA_MONITORING_API(SetTreeVariable(m_notFoundTreeName, "mc_phi", &notFound_mcPhi));
    PANDORA_MONITORING_API(SetTreeVariable(m_notFoundTreeName, "mc_E", &notFound_mcEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(m_notFoundTreeName, "mc_p", &notFound_mcP));

    PANDORA_MONITORING_API(SetTreeVariable(m_otherTreeName, "pfo_theta", &other_pfoTheta ));
    PANDORA_MONITORING_API(SetTreeVariable(m_otherTreeName, "pfo_phi", &other_pfoPhi));
    PANDORA_MONITORING_API(SetTreeVariable(m_otherTreeName, "pfo_E", &other_pfoEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(m_otherTreeName, "pfo_p", &other_pfoP));
    PANDORA_MONITORING_API(SetTreeVariable(m_otherTreeName, "pfo_charge", &other_pfoCharge));

    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "found", &pfoCaloHitE_truePositive));
    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "fakes", &pfoCaloHitE_falsePositive));
    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "other", &pfoCaloHitE_trueNegative));
    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "purity", &pfoCaloHitE_purity));

    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "pfo_theta", &pfoCaloHitE_pfoTheta ));
    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "pfo_phi", &pfoCaloHitE_pfoPhi));
    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "pfo_E", &pfoCaloHitE_pfoEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "pfo_p", &pfoCaloHitE_pfoP));
    PANDORA_MONITORING_API(SetTreeVariable(m_pfoCaloHitETreeName, "pfo_charge", &pfoCaloHitE_pfoCharge));

    PANDORA_MONITORING_API(SetTreeVariable(m_mcCaloHitETreeName, "found", &mcCaloHitE_truePositive));
    PANDORA_MONITORING_API(SetTreeVariable(m_mcCaloHitETreeName, "notFound", &mcCaloHitE_falseNegative));
    PANDORA_MONITORING_API(SetTreeVariable(m_mcCaloHitETreeName, "eff", &mcCaloHitE_efficiency));

    PANDORA_MONITORING_API(SetTreeVariable(m_mcCaloHitETreeName, "mc_theta", &mcCaloHitE_mcTheta ));
    PANDORA_MONITORING_API(SetTreeVariable(m_mcCaloHitETreeName, "mc_phi", &mcCaloHitE_mcPhi));
    PANDORA_MONITORING_API(SetTreeVariable(m_mcCaloHitETreeName, "mc_E", &mcCaloHitE_mcEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(m_mcCaloHitETreeName, "mc_p", &mcCaloHitE_mcP));


    PANDORA_MONITORING_API(SetTreeVariable(m_controlTreeName, "energyOfLargestAndMatchingMC", &control_energyOfLargestAndMatchingMC));
    PANDORA_MONITORING_API(SetTreeVariable(m_controlTreeName, "pfoMCCaloHitEFraction", &control_pfoMCCaloHitEFraction));
    PANDORA_MONITORING_API(SetTreeVariable(m_controlTreeName, "largestContribution", &control_largestContribution));
    PANDORA_MONITORING_API(SetTreeVariable(m_controlTreeName, "totalContribution", &control_totalContribution));
    PANDORA_MONITORING_API(SetTreeVariable(m_controlTreeName, "pfoEnergy", &control_pfoEnergy));
    PANDORA_MONITORING_API(SetTreeVariable(m_controlTreeName, "pfoMCEDiffMulByEMC", &control_pfoMCEDiffMulByEMC));
    PANDORA_MONITORING_API(SetTreeVariable(m_controlTreeName, "passedCriteria", &control_passedCriteria));


    PANDORA_MONITORING_API(FillTree(m_foundTreeName));
    PANDORA_MONITORING_API(FillTree(m_fakesTreeName));
    PANDORA_MONITORING_API(FillTree(m_otherTreeName));
    PANDORA_MONITORING_API(FillTree(m_notFoundTreeName));

    PANDORA_MONITORING_API(FillTree(m_pfoCaloHitETreeName));
    PANDORA_MONITORING_API(FillTree(m_mcCaloHitETreeName));
    PANDORA_MONITORING_API(FillTree(m_controlTreeName));

    PANDORA_MONITORING_API(FillTree(m_eventTreeName));


    m_numberEvents      += 1;


    return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EfficiencyMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_particleId = 0;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "ParticleId", m_particleId));

    m_print = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Print", m_print));

    m_monitoringFileName = "effmon.root";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MonitoringFileName", m_monitoringFileName));

    m_foundTreeName = "found";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeNameFound", m_foundTreeName));

    m_notFoundTreeName = "notFound";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeNameNotFound", m_notFoundTreeName));

    m_fakesTreeName = "fakes";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeNameFakes", m_fakesTreeName));

    m_otherTreeName = "other";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeNameOther", m_otherTreeName));

    m_pfoCaloHitETreeName = "caloEPfo";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeNameCaloHitEnergyFromPFOs", m_pfoCaloHitETreeName));

    m_mcCaloHitETreeName = "caloEMC";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeNameCaloHitEnergyFromMCs", m_mcCaloHitETreeName));

    m_controlTreeName = "control";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeNameControl", m_controlTreeName));

    m_eventTreeName = "event";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TreeNameEvent", m_eventTreeName));


    m_minCaloHitEnergyFraction = 50.f; // in %
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCaloHitEnergyFraction", m_minCaloHitEnergyFraction));

    m_calorimeterResolutionStochasticCut = 500.f; // in %, -1 means no cut
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CalorimeterResolutionStochasticCut", m_calorimeterResolutionStochasticCut));

    m_calorimeterResolutionConstantCut = -1.f; // in %, -1 means no cut
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "CalorimeterResolutionConstantCut", m_calorimeterResolutionConstantCut));


    StringVector thetaBinning;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
                                                                                                                  "ThetaBinning", thetaBinning));

    static const float pi(std::acos(-1.f));
    if (thetaBinning.empty() )
    {
        m_thetaBins = 50;
        m_thetaMin  = 0.f;
        m_thetaMax  = pi;
    }
    else
    {
        if (thetaBinning.size() != 3)
            return STATUS_CODE_INVALID_PARAMETER;
        
        std::stringstream sstrTBins(thetaBinning.at(0));
        sstrTBins >> m_thetaBins;

        std::stringstream sstrTMin(thetaBinning.at(1));
        sstrTMin >> m_thetaMin;

        std::stringstream sstrTMax(thetaBinning.at(2));
        sstrTMax >> m_thetaMax;
    }

    StringVector energyBinning;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
                                                                                                                  "EnergyBinning", energyBinning));

    if (energyBinning.empty() )
    {
        m_energyBins = 500;
        m_energyMin  = 0.f;
        m_energyMax  = 1500.f;
    }
    else
    {
        if (energyBinning.size() != 3)
            return STATUS_CODE_INVALID_PARAMETER;
        
        std::stringstream sstrEBins(energyBinning.at(0));
        sstrEBins >> m_energyBins;

        std::stringstream sstrEMin(energyBinning.at(1));
        sstrEMin >> m_energyMin;

        std::stringstream sstrEMax(energyBinning.at(2));
        sstrEMax >> m_energyMax;
    }

    return STATUS_CODE_SUCCESS;
}
