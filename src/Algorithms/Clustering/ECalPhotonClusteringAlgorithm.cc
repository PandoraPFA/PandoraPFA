/**
 *  @file   PandoraPFANew/src/Algorithms/ECalPhotonClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the photon clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/Clustering/ECalPhotonClusteringAlgorithm.h"
#include "Objects/MCParticle.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <iomanip>
#include <limits>

#include <sstream>

#include <assert.h>

using namespace pandora;

PhotonIDLikelihoodCalculator* PhotonIDLikelihoodCalculator::_instance = 0;
bool PhotonIDLikelihoodCalculator::fromXml = false;

const unsigned int ECalPhotonClusteringAlgorithm::m_maximumNumberOfLayers = 150; // TODO remove this, as we no longer fix number of layers

//------------------------------------------------------------------------------------------------------------------------------------------

PhotonIDLikelihoodCalculator* PhotonIDLikelihoodCalculator::Instance()
{
    if(_instance==0)
    {
        _instance = new PhotonIDLikelihoodCalculator();
    }

    return _instance;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ECalPhotonClusteringAlgorithm::ECalPhotonClusteringAlgorithm()
{
    if (m_producePrintoutStatements > 0)
        std::cout << "constructor" << std::endl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonClusteringAlgorithm::Initialize()
{
    if (m_producePrintoutStatements > 0)
    {
        std::cout << "initialize" << std::endl;
        std::cout << "makingphotonid " <<m_makingPhotonIdLikelihoodHistograms << std::endl;
    }

    // create monitoring histograms for likelihood
    if(m_makingPhotonIdLikelihoodHistograms)
        CreateOrSaveLikelihoodHistograms(true);


    // set object variables:
    m_nECalLayers = GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers();



    if( m_produceConfigurationFiles <= -1 )
    {
        PhotonIDLikelihoodCalculator::Instance()->LoadXml( m_configurationFileNameSig, m_configurationFileNameBkg );
    }
    else
    {
        std::vector<float> eBinBorders;
        eBinBorders.push_back(0.2);
        eBinBorders.push_back(0.5);
        eBinBorders.push_back(1.0);
        eBinBorders.push_back(1.5);
        eBinBorders.push_back(2.5);
        eBinBorders.push_back(5.0);
        eBinBorders.push_back(10.0);
        eBinBorders.push_back(20.0);
        eBinBorders.push_back(50.0);

        PhotonIDLikelihoodCalculator* plc = PhotonIDLikelihoodCalculator::Instance();
        plc->energySig.SetDimensions( "energySig", eBinBorders );
        plc->rmsSig.SetDimensions  ( "rmsSig",     eBinBorders, 20, 0.f, 5.f  );
        plc->fracSig.SetDimensions ( "fracSig",    eBinBorders, 20, 0.f, 1.f  );
        plc->startSig.SetDimensions( "startSig",   eBinBorders, 20, 0.f, 10.f );

        plc->energyBkg.SetDimensions( "energyBkg", eBinBorders );
        plc->rmsBkg.SetDimensions  ( "rmsBkg",     eBinBorders, 20, 0.f, 5.f  );
        plc->fracBkg.SetDimensions ( "fracBkg",    eBinBorders, 20, 0.f, 1.f  );
        plc->startBkg.SetDimensions( "startBkg",   eBinBorders, 20, 0.f, 10.f );
    }




    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

ECalPhotonClusteringAlgorithm::~ECalPhotonClusteringAlgorithm()
{
    if (m_producePrintoutStatements > 0)
    {
        std::cout << "destructor" << std::endl;
        std::cout << "makingphotonid " <<m_makingPhotonIdLikelihoodHistograms << std::endl;
    }

    // create monitoring histograms for likelihood
    if(m_makingPhotonIdLikelihoodHistograms)
        CreateOrSaveLikelihoodHistograms(false);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonClusteringAlgorithm::Run()
{
    ClusterList photonClusters;


    // Run initial clustering algorithm
    const ClusterList *pClusterList = NULL;
    if( STATUS_CODE_SUCCESS != PandoraContentApi::GetClusterList(*this, m_clusterListName, pClusterList))
        return STATUS_CODE_SUCCESS;

    if( pClusterList != NULL )
    {
//        PANDORA_MONITORING_API(AddClusterList(DETECTOR_VIEW_XZ,pClusterList, BLUE  ) );
//        PANDORA_MONITORING_API(AddClusterList(DETECTOR_VIEW_XY,pClusterList, BLUE  ) );
//        PANDORA_MONITORING_API(VisualizeClusters(pClusterList, "all", BLUE  ) );

        std::vector<Cluster*> temporaryClusterList(pClusterList->begin(), pClusterList->end() );

        for( std::vector<Cluster*>::const_iterator itCluster = temporaryClusterList.begin(), itClusterEnd = temporaryClusterList.end(); itCluster != itClusterEnd; ++itCluster )
        {
            Cluster* pCluster = (*itCluster);

            if (m_producePrintoutStatements > 0)
                std::cout << "*** main cluster cells : " << pCluster->GetNCaloHits() << std::endl;
            if( pCluster->GetElectromagneticEnergy()<=0.2 || pCluster->GetNCaloHits() < m_minimumHitsInClusters ) 
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS,!=,PandoraContentApi::DeleteCluster(*this, pCluster, m_clusterListName));
                if (m_producePrintoutStatements > 0)
                    std::cout << "is photon cluster? --> NO / electromagnetic energy too small (<=0.2)" << std::endl;
                continue;
            }

            std::vector<protoClusterPeaks_t> peaks;
            TransverseProfile( pCluster, peaks,m_nECalLayers); 

            // get the ordered calohits of the cluster

            ClusterProperties clusterProperties;
            GetClusterProperties( pCluster, clusterProperties );

            const OrderedCaloHitList pOrderedCaloHitList( pCluster->GetOrderedCaloHitList() );
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS,!=,PandoraContentApi::DeleteCluster(*this, pCluster, m_clusterListName));


            // cluster these hits differently ==============================

            int peakForProtoCluster = 0;
            bool useOriginalCluster = false;
            if( peaks.size() == 1 ) // if only one peak and this peak is identified as photon, use the original cluster
                useOriginalCluster = true;

            for( std::vector<pandora::protoClusterPeaks_t>::iterator itPeak = peaks.begin(), itPeakEnd = peaks.end(); 
                 itPeak != itPeakEnd; ++itPeak )
            {
                Cluster* pPhotonCandidateCluster = TransverseProfile( clusterProperties, pOrderedCaloHitList, peakForProtoCluster, m_nECalLayers);

                if( pPhotonCandidateCluster != NULL )
                {
                    if (m_producePrintoutStatements > 0)
                        std::cout << "*** sub  cluster size : " << pPhotonCandidateCluster->GetNCaloHits() << std::endl;

                    if( IsPhoton( pPhotonCandidateCluster, pOrderedCaloHitList, (*itPeak), clusterProperties, useOriginalCluster ) )
                    {
                        pPhotonCandidateCluster->SetIsPhotonFlag( true );
                        if (m_producePrintoutStatements > 0)
                            std::cout << "is photon cluster? --> YES " << std::endl;

                        photonClusters.insert( pPhotonCandidateCluster );
                        if( useOriginalCluster ) // if the original cluster is used
                        {
                            break;
//                            itPeak = itPeakEnd; // end this loop
                        }
                    }
                    else
                    {
                        if (m_producePrintoutStatements > 0)
                            std::cout << "is photon cluster? --> NO ";

                        PANDORA_RETURN_RESULT_IF( STATUS_CODE_SUCCESS, !=, PandoraContentApi::DeleteCluster(*this, pPhotonCandidateCluster ) );
                    }

                    if (m_producePrintoutStatements > 0)
                        std::cout << std::endl;

                }
                ++peakForProtoCluster;
            }
        }
    }


//    PANDORA_MONITORING_API(AddClusterList(DETECTOR_VIEW_XZ,&photonClusters, DARKYELLOW  ) );
//    PANDORA_MONITORING_API(AddClusterList(DETECTOR_VIEW_XY,&photonClusters, DARKYELLOW  ) );
//    PANDORA_MONITORING_API(ViewEvent() );
//     PANDORA_MONITORING_API(VisualizeClusters(&photonClusters, "photons", DARKYELLOW  ) );

//     const OrderedCaloHitList *pOrderedCaloHitList = NULL;
//     PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

//     PANDORA_MONITORING_API(VisualizeCaloHits(pOrderedCaloHitList, "currentHits", BLUE  ) );
//     PANDORA_MONITORING_API(View());


    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonClusteringAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "ClusterListName", m_clusterListName));

    m_minimumHitsInClusters = 5; 
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                                                                                         "MinimumHitsInCluster", m_minimumHitsInClusters));

    // debug printing
    m_producePrintoutStatements = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                                                                                         "Print", m_producePrintoutStatements));

    // make photon ID likelihood histograms
    m_makingPhotonIdLikelihoodHistograms = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                                                                                         "MakePhotonIDLikelihoodHistograms", m_makingPhotonIdLikelihoodHistograms));

    // monitoring filename
    m_monitoringFileName = "";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                                                                                         "MonitoringFileName", m_monitoringFileName));
    // xml configuration input filename background
    m_configurationFileNameBkg = "photonClusteringConfiguration_Bkg.xml";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                                                                                         "ConfigurationFileNameBkg", m_configurationFileNameBkg));

    // xml configuration input filename signal
    m_configurationFileNameSig = "photonClusteringConfiguration_Sig.xml";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                                                                                         "ConfigurationFileNameSig", m_configurationFileNameSig));

    // produce configuration file
    // 0... signal events, 1 ... background events, 2 ... signal and background events (to be split by "fraction" always : >=0.5 for signal, < 0.5 for background )
    m_produceConfigurationFiles = -1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
                                                                                                         "ProduceConfiguration", m_produceConfigurationFiles));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ECalPhotonClusteringAlgorithm::CreateOrSaveLikelihoodHistograms(bool create)
{
    int nHistograms = 8;

    for( int i = 0; i <= nHistograms; ++i )
    {
        const std::string iHist(TypeToString(i));
        const std::string hName_sighistrms("sighistrms" + iHist);
        const std::string hName_backhistrms("backhistrms" + iHist);
        const std::string hName_sighistfrac("sighistfrac" + iHist);
        const std::string hName_backhistfrac("backhistfrac" + iHist);
        const std::string hName_sighiststart("sighiststart" + iHist);
        const std::string hName_backhiststart("backhiststart" + iHist);
        const std::string hTitle_rms(" rms  ");
        const std::string hTitle_frac(" frac  ");
        const std::string hTitle_start(" start  ");

        if( create )
        {
            PANDORA_MONITORING_API(Create1DHistogram(hName_sighistrms, hTitle_rms, 20, 0.0, 5.0));
            PANDORA_MONITORING_API(Create1DHistogram(hName_backhistrms, hTitle_rms, 20, 0.0, 5.0));
            PANDORA_MONITORING_API(Create1DHistogram(hName_sighistfrac, hTitle_frac, 20, 0.0, 1.0));
            PANDORA_MONITORING_API(Create1DHistogram(hName_backhistfrac, hTitle_frac, 20, 0.0, 1.0));
            PANDORA_MONITORING_API(Create1DHistogram(hName_sighiststart, hTitle_start, 20, 0.0, 10.0));
            PANDORA_MONITORING_API(Create1DHistogram(hName_backhiststart, hTitle_start, 20, 0.0, 10.0));
        }
        else // if not create --> save them
        {
            if( !m_monitoringFileName.empty() )
            {
                PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_sighistrms,  m_monitoringFileName, "UPDATE"));
                PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_backhistrms, m_monitoringFileName, "UPDATE"));
                PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_sighistfrac, m_monitoringFileName, "UPDATE"));
                PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_backhistfrac, m_monitoringFileName, "UPDATE"));
                PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_sighiststart, m_monitoringFileName, "UPDATE"));
                PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_backhiststart, m_monitoringFileName, "UPDATE"));
            }
        }
    }

    if( create )
    {
        PANDORA_MONITORING_API(Create2DHistogram("energyVsPhotonE", "E vs. photonE", 100, 0.0, 100.0, 100, 0.0 ,100.0));
        PANDORA_MONITORING_API(Create2DHistogram("pidVsPhotonEFraction", "pid vs. photonEFraction", 100, 0.0, 1.0, 100, 0.0 ,10.0));
        
    }
    else
    {
        if( !m_monitoringFileName.empty() )
        {
            PANDORA_MONITORING_API(SaveAndCloseHistogram("energyVsPhotonE", m_monitoringFileName, "UPDATE" ));
            PANDORA_MONITORING_API(SaveAndCloseHistogram("pidVsPhotonEFraction", m_monitoringFileName, "UPDATE" ));

            try
            {
                PANDORA_MONITORING_API(SaveTree("photonId", m_monitoringFileName, "UPDATE" ));
            }
            catch(...)
            {
                std::cout << "Tree 'photonId' could not be saved!" << std::endl;
            }
        }
    }


    if( !create )
    {
        PhotonIDLikelihoodCalculator* plc = PhotonIDLikelihoodCalculator::Instance();
        if( m_produceConfigurationFiles==0 || m_produceConfigurationFiles==2 ) // write signal file
        {
            //            plc->energySig.Scale( plc->energySig.GetSumOfEntries() );
            plc->rmsSig.Scale( plc->rmsSig.GetSumOfEntries() );
            plc->fracSig.Scale( plc->fracSig.GetSumOfEntries() );
            plc->startSig.Scale( plc->startSig.GetSumOfEntries() );

            plc->WriteXmlSig( m_configurationFileNameSig );
        }

        if( m_produceConfigurationFiles==1 || m_produceConfigurationFiles==2 ) // write background file
        {
            //            plc->energyBkg.Scale( plc->energyBkg.GetSumOfEntries() );
            plc->rmsBkg.Scale( plc->rmsBkg.GetSumOfEntries() );
            plc->fracBkg.Scale( plc->fracBkg.GetSumOfEntries() );
            plc->startBkg.Scale( plc->startBkg.GetSumOfEntries() );

            plc->WriteXmlBkg( m_configurationFileNameBkg );
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ECalPhotonClusteringAlgorithm::IsPhoton( Cluster* &pPhotonCandidateCluster, const OrderedCaloHitList& pOriginalOrderedCaloHitList, 
                                      protoClusterPeaks_t& peak, ClusterProperties& originalClusterProperties, bool& useOriginalCluster )
{
    if(m_producePrintoutStatements > 0)
        std::cout << "=============== IsPhoton? ===================" << std::endl;

    bool returnValue = false;


    peak.energy = pPhotonCandidateCluster->GetElectromagneticEnergy();


    PhotonIdProperties photonIdProperties;
    try
    {
        photonIdProperties.m_photonLongShowerStart = pPhotonCandidateCluster->GetShowerProfileStart();
        photonIdProperties.m_photonLongProfileFraction = pPhotonCandidateCluster->GetShowerProfileDiscrepancy();
    }
    catch(StatusCodeException &statusCodeException)
    {
        if(m_producePrintoutStatements > 0)
        {
            std::cout << "IsPhoton/statusCodeException " << StatusCodeToString(statusCodeException.GetStatusCode()) << std::endl;
            std::cout << "it's not a photon then" << std::endl;
        }
        return false; // it's not a photon then
    }
    catch(...)
    {
        std::cout << "PhotonProfileID/unknown exception" << std::endl;
        throw;
    }

    float showerStart   = photonIdProperties.GetLongProfileShowerStart();
    float photonFraction = photonIdProperties.GetLongProfilePhotonFraction();
    float electromagneticPhotonEContribution = 0.0;
    float truePhotonE = GetTrueEnergyContribution(pPhotonCandidateCluster, electromagneticPhotonEContribution, 22); // get true photon energy contribution
    float electromagneticEContribution = 0.0;
    float trueE       = GetTrueEnergyContribution(pPhotonCandidateCluster, electromagneticEContribution);     // get true energy contribution
    float electromagneticE = pPhotonCandidateCluster->GetElectromagneticEnergy();

    if (m_producePrintoutStatements > 0)
    {
        std::cout << "electromagneticE " << electromagneticE << "  electromagneticEContrib " << electromagneticEContribution
                  << " truePhotonE " << truePhotonE << " trueE " << trueE << std::endl;
    }

    assert( electromagneticE - electromagneticEContribution < 0.0001 );

    //    float fraction = electromagneticPhotonEContribution / electromagneticEContribution;
    float fraction = electromagneticPhotonEContribution / electromagneticE; 


//    float fraction = truePhotonE / trueE;

    ClusterProperties clusterProperties;
    GetClusterProperties( pPhotonCandidateCluster, clusterProperties );

    float closest = 999.;
    float cclosest = 999.;
    float c10closest = 999.;
    float c20closest = 999.;


    // loop through all tracks
    //
    const TrackList& trackList = pPhotonCandidateCluster->GetAssociatedTrackList();
    for( TrackList::const_iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack )
    {
        const TrackState& trackStateAtECal = (*itTrack)->GetTrackStateAtECal();
                
        float longitudinalComponent;
        float approach;

        // ToDo: treat overlap-region barrel endcap as mark does (protocluster: DistanceToTrack)
        CartesianVector hitMean(clusterProperties.m_hitMean[0],clusterProperties.m_hitMean[1],clusterProperties.m_hitMean[2] );
        DistanceToPositionAndDirection(hitMean,
                                       trackStateAtECal.GetPosition(), 
                                       trackStateAtECal.GetMomentum().GetUnitVector(),
                                       longitudinalComponent,
                                       approach  );
        if( approach < closest )
        {
            closest = approach;
            CartesianVector centroid(clusterProperties.m_centroid[0],clusterProperties.m_centroid[1],clusterProperties.m_centroid[2] );
            DistanceToPositionAndDirection(centroid,trackStateAtECal.GetPosition(), trackStateAtECal.GetMomentum().GetUnitVector(),
                                           longitudinalComponent,cclosest  );
            if( cclosest > 999 ) cclosest = 0.0;
            CartesianVector centroid10(clusterProperties.m_centroid10[0],clusterProperties.m_centroid10[1],clusterProperties.m_centroid10[2] );
            DistanceToPositionAndDirection(centroid10,trackStateAtECal.GetPosition(), trackStateAtECal.GetMomentum().GetUnitVector(),
                                           longitudinalComponent,c10closest  );
            if( c10closest> 999 ) c10closest = 0.0;
            CartesianVector centroid20(clusterProperties.m_centroid20[0],clusterProperties.m_centroid20[1],clusterProperties.m_centroid20[2] );
            DistanceToPositionAndDirection(centroid20,trackStateAtECal.GetPosition(), trackStateAtECal.GetMomentum().GetUnitVector(),
                                           longitudinalComponent,c20closest  );
            if( c20closest> 999 ) c20closest = 0.0;
        }
    }

    float dist = std::min(c10closest,c20closest);
    if(cclosest<dist)dist = closest;

    unsigned int nhits = pPhotonCandidateCluster->GetNCaloHits();
    
    float pid = 0;
    pid = (PhotonIDLikelihoodCalculator::Instance())->PID( peak.energy, peak.rms, photonFraction,showerStart );


    bool accept = false;

    // cuts for case where there is no pointing track
    float pidCut = 0.5;
    //if(closest>10)pidCut=0.45;
    //if(closest>20)pidCut=0.40;
    //if(closest>30)pidCut=0.35;
    //if(closest>40)pidCut=0.30;
    //if(closest>50)pidCut=0.25;

    if(m_makingPhotonIdLikelihoodHistograms)
    {
        PANDORA_MONITORING_API(Fill2DHistogram("pidVsPhotonEFraction", pid, fraction ));
    }

    float fracE = pPhotonCandidateCluster->GetElectromagneticEnergy()/originalClusterProperties.electromagneticEnergy;
    
    if(nhits>=m_minimumHitsInClusters && peak.energy>0.2 && pid > pidCut && showerStart<10 && photonFraction < 1.0 &&peak.rms<5.0 && closest > 2.0){
        accept = true;
    }

    if(nhits>=m_minimumHitsInClusters && peak.energy>0.2 && pid > pidCut && showerStart<10 && photonFraction < 1.0 &&peak.rms<5.0 && closest > 2.0){
        accept = true;
        // OK found photon cluster - use this sub-cluster (peak) or the whole thing 
        float diffE = originalClusterProperties.electromagneticEnergy - electromagneticE;
        if(fracE>0.95 || useOriginalCluster )useOriginalCluster=true;
        if(fracE>0.90 && diffE < 2.0)useOriginalCluster=true;
        if(fracE>0.80 && diffE < 1.0)useOriginalCluster=true;
        if(fracE>0.5 && diffE<0.5)useOriginalCluster=true;
        if(peak.peakNumber!=0)useOriginalCluster = false; // the original cluster can only be taken if the FIRST peak is identified as photon
    }


    // cluster which is very close to a nearby track
//             if(nhits>=m_minimumHitsInClusters && peaks[ipeak].energy>0.2 && pid > 0.5 && showerStart<10 && photonFraction < 1.0 &&peaks[ipeak].rms<5.0 && closest <= 2.0){
    if(nhits>=m_minimumHitsInClusters && peak.energy>0.2 && pid > 0.5 && showerStart<10 && photonFraction < 1.0 &&peak.rms<5.0 && closest <= 2.0){
        if(dist >  5.0 && pid > 0.9)accept = true;
        if(dist >  7.5 && pid > 0.8)accept = true;
        if(dist > 10.0 && pid > 0.7)accept = true;
    }

    if( nhits>=m_minimumHitsInClusters )
    {    
        // Debugging info
        float radius(std::numeric_limits<float>::max());
        float phi(std::numeric_limits<float>::max());
        float theta(std::numeric_limits<float>::max());

        // compute fitresults of cluster --> to get the position
        const ClusterHelper::ClusterFitResult &fitResult(pPhotonCandidateCluster->GetFitToAllHitsResult());

        if (fitResult.IsFitSuccessful())
        {
            const CartesianVector &clusterDirection(fitResult.GetDirection());
            clusterDirection.GetSphericalCoordinates( radius, phi, theta );
        }

        PANDORA_MONITORING_API(SetTreeVariable("photonId", "pid", pid ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "fraction", fraction ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "EemPhot", electromagneticPhotonEContribution ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "EtruePhot", truePhotonE ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "Etrue",     trueE ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "Eem", electromagneticE ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "accept", float(accept) ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakRms", peak.rms ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakE", peak.energy ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "shStart", showerStart ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakShStart", peak.showerStartDepth ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakShDepth", peak.showerDepth90 ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "photonFraction", photonFraction ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "phi", phi ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "theta", theta ));
        PANDORA_MONITORING_API(SetTreeVariable("photonId", "nhits", static_cast<int>(nhits) ));

        PANDORA_MONITORING_API(FillTree("photonId"));

    }

    if(m_producePrintoutStatements > 0 && fraction > 0.5)
    {
        std::cout << "fraction " << fraction << " --> should be identified as photon  | true photon E " << truePhotonE << " elm E "  << electromagneticE  << std::endl;
    }

    if(m_makingPhotonIdLikelihoodHistograms)
    {
        PANDORA_MONITORING_API(Fill2DHistogram("energyVsPhotonE", electromagneticE, truePhotonE));
    }

    if(m_producePrintoutStatements > 0 && peak.energy > 1.0)
    {
        std::cout << " PEAK  : " <<  peak.du << "," << peak.dv << "  E = " << peak.energy << " dmin : " << peak.dmin <<  " d25/d90 : " << peak.showerDepth25 << "/" << peak.showerDepth90  << " start : " << peak.showerStartDepth << " rms = " << peak.rms << " PhotonID : " << showerStart << " " << photonFraction << " TRUE FRACTION = " << fraction << " pid " << pid << " d = " << closest << " c= " << dist;
        if(fraction<0.5 &&  accept)std::cout << " <---A*****************";
        if(fraction>0.5 && !accept) std::cout << " <---B*****************";
        std::cout << std::endl;    
    }

    if(useOriginalCluster){
        if (m_producePrintoutStatements > 0)
            std::cout << "Use original cluster " << std::endl;

        // we don't need the photon candidate any more
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS,!=,PandoraContentApi::DeleteCluster(*this, pPhotonCandidateCluster));

        // re-create the cluster which get's all the hits from the original cluster
        CaloHitList caloHitList;
        pOriginalOrderedCaloHitList.GetCaloHitList( caloHitList );
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, &caloHitList, pPhotonCandidateCluster ));
    }


    if(m_produceConfigurationFiles >= 0)
    {
        PhotonIDLikelihoodCalculator* plc = PhotonIDLikelihoodCalculator::Instance();
        if( (peak.energy>0.2) && photonFraction<1.0 && showerStart<10. && peak.rms<5.)
        {
            if( fraction>0.5 ) 
            {
                plc->energySig.Fill( peak.energy );
                plc->rmsSig.Fill( peak.energy, peak.rms );
                plc->fracSig.Fill( peak.energy, photonFraction );
                plc->startSig.Fill( peak.energy, showerStart );
            }
            else
            {
                plc->energyBkg.Fill( peak.energy );
                plc->rmsBkg.Fill( peak.energy, peak.rms );
                plc->fracBkg.Fill( peak.energy, photonFraction );
                plc->startBkg.Fill( peak.energy, showerStart );
            }
        }
    }


    //********** code to make the likelihood histograms ************
    if(m_makingPhotonIdLikelihoodHistograms)
    {
        int ihist = -999;
        if( peak.energy> 0.2 && peak.energy <=  0.5)ihist=0;
        if( peak.energy> 0.5 && peak.energy <=  1.0)ihist=1;
        if( peak.energy> 1.0 && peak.energy <=  1.5)ihist=2;
        if( peak.energy> 1.5 && peak.energy <=  2.5)ihist=3;
        if( peak.energy> 2.5 && peak.energy <=  5.0)ihist=4;
        if( peak.energy> 5.0 && peak.energy <= 10.0)ihist=5;
        if( peak.energy>10.0 && peak.energy <= 20.0)ihist=6;
        if( peak.energy>20.0 && peak.energy <= 50.0)ihist=7;
        if( peak.energy>50.0)ihist=8;
        if(ihist>=0 && photonFraction<1.0 && showerStart<10. && peak.rms<5.)
        {
            const std::string iHist(TypeToString(ihist));
            if(fraction>0.5)
            {
                const std::string hName_sighistrms("sighistrms" + iHist);
                const std::string hName_sighistfrac("sighistfrac" + iHist);
                const std::string hName_sighiststart("sighiststart" + iHist);
                PANDORA_MONITORING_API(Fill1DHistogram(hName_sighistrms, peak.rms));
                PANDORA_MONITORING_API(Fill1DHistogram(hName_sighistfrac, photonFraction));
                PANDORA_MONITORING_API(Fill1DHistogram(hName_sighiststart, showerStart));
            }
            else
            {
                const std::string hName_backhistrms("backhistrms" + iHist);
                const std::string hName_backhistfrac("backhistfrac" + iHist);
                const std::string hName_backhiststart("backhiststart" + iHist);
                PANDORA_MONITORING_API(Fill1DHistogram(hName_backhistrms, peak.rms));
                PANDORA_MONITORING_API(Fill1DHistogram(hName_backhistfrac, photonFraction));
                PANDORA_MONITORING_API(Fill1DHistogram(hName_backhiststart, showerStart));
            }
        }
    }

    if(accept)
    {
        if (m_producePrintoutStatements > 0)
            std::cout << "accepted as photon" << std::endl;

        // changed: change returnValue to true
        returnValue = true;
    }

    return returnValue;
}


//------------------------------------------------------------------------------------------------------------------------------------------

void ECalPhotonClusteringAlgorithm::ComputeTransverseVectors( const pandora::CartesianVector& direction, 
                                                              pandora::CartesianVector& transverseUnitVectorU, 
                                                              pandora::CartesianVector& transverseUnitVectorV )
{

    // calculate unit vector
    CartesianVector unitVector = direction.GetUnitVector();
    float ux = unitVector.GetX();
    float uy = unitVector.GetY();
    float uz = unitVector.GetZ();

    transverseUnitVectorU = CartesianVector( uy, -ux, 0 ).GetUnitVector();
    float utx = transverseUnitVectorU.GetX();
//    float uty = transverseUnitVectorU.GetY();

    #define EPSILON 0.00001
    if(fabs(utx-0)>EPSILON && 
       fabs(uz -0)>EPSILON )
    {
        float x2 = ux*ux;
        float y2 = uy*uy;
        transverseUnitVectorV.SetValues( ux / uy,
                                          1,
                                          -(x2+y2)/(uz*uy) );
//         transverseUnitVectorV.SetValues( -uty / utx,
//                                          1,
//                                          (uty*ux-uy*utx)/utx/uz );
        transverseUnitVectorV = transverseUnitVectorV.GetUnitVector();
    }else if( fabs(utx-0) < EPSILON &&
              fabs(uz -0) > EPSILON )
    {
        transverseUnitVectorV.SetValues( 1,
                                         0,
                                         ux/uz );
        transverseUnitVectorV = transverseUnitVectorV.GetUnitVector();
    }else if( fabs(utx-0)<EPSILON &&
              fabs(uz -0)<EPSILON )
    {
        transverseUnitVectorV.SetValues( 0,
                                         0,
                                         1 );
    }else
    {
        transverseUnitVectorV.SetValues( 0,0,0 );
    }



//     float x0   = clusterProperties.m_centroidFirstLayer.GetX();
//     float y0   = clusterProperties.m_centroidFirstLayer.GetY();
//     float z0   = clusterProperties.m_centroidFirstLayer.GetZ();
//     float r0   = sqrt(x0*x0+y0*y0+z0*z0);
//     float ux   = x0/r0;
//     float uy   = y0/r0;
//     float uz   = z0/r0;
//     float utx  = uy/sqrt(ux*ux+uy*uy);
//     float uty  = -ux/sqrt(ux*ux+uy*uy);
//     float utz  = 0;
//     float vtx=0;
//     float vty=0;
//     float vtz=0;
//     if(utx!=0&&uz!=0){
//         vtx  = -uty/utx;
//         vtz  = (uty*ux-uy*utx)/utx/uz;
//         vty  = 1;
//         float v    = sqrt(vtx*vtx+vty*vty+vtz*vtz);
//         vtx = vtx/v;
//         vty = vty/v;
//         vtz = vtz/v;
//     }
//     if(utx==0&&uz!=0){
//         vtx  = 1;
//         vtz  = ux/uz;
//         vty  = 0;
//         float v    = sqrt(vtx*vtx+vty*vty+vtz*vtz);
//         vtx = vtx/v;
//         vty = vty/v;
//         vtz = vtz/v;
//     }
//     if(utx==0&&uz==0){
//         vtx  = 0;
//         vtz  = 1;
//         vty  = 0;
//     }
    
}



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonClusteringAlgorithm::TransverseProfile(const Cluster* cluster, std::vector<protoClusterPeaks_t> &peaks, int maxLayers)
{
    const int nbins(41);
    const int ioffset(nbins / 2);

    int ifirst = (int)cluster->GetInnerPseudoLayer();

    const CartesianVector centroid = cluster->GetCentroid(ifirst);
    float x0   = centroid.GetX();
    float y0   = centroid.GetY();
    float z0   = centroid.GetZ();


    CartesianVector transverseUnitVectorU(-1,-1,-1);
    CartesianVector transverseUnitVectorV(-1,-1,-1);
    ComputeTransverseVectors( centroid,
                              transverseUnitVectorU, 
                              transverseUnitVectorV );


    int  done[nbins][nbins];
    bool assigned[nbins][nbins];
    float tprofile[nbins][nbins];
    float tlprofile[nbins][nbins][m_maximumNumberOfLayers];
    for(int i=0; i<nbins; ++i){
        for(int j=0; j<nbins; ++j){
            tprofile[i][j]=0;
            done[i][j]=false;
            assigned[i][j]=false;
            for(int k=0;k<=maxLayers ; k++)tlprofile[i][j][k]=0.;
        }
    }


    bool first = true;
    float pixelsize = 10.; 
    for(int i=0; i<maxLayers; i++){
        CaloHitList* caloHitList;
        if( STATUS_CODE_SUCCESS != cluster->GetCaloHitsInPseudoLayer( PseudoLayer(i), caloHitList ) ) continue;
        for( CaloHitList::iterator itCaloHit = caloHitList->begin(), itCaloHitEnd = caloHitList->end(); itCaloHit != itCaloHitEnd; ++itCaloHit )
        {
            CaloHit* caloHit = (*itCaloHit);
            if(first){
                pixelsize  = caloHit->GetCellLengthScale(); // in barrel: perpendicular to beam and pixel thickness; in endcap: perp. to thickness and up
            }
            const CartesianVector& position = caloHit->GetPositionVector();
            float dx = (position.GetX() -x0)/pixelsize; // TODO: this could maybe be replaced by GetCellSizeU()
            float dy = (position.GetY() -y0)/pixelsize; // TODO: this could maybe be replaced by GetCellSizeV()
            float dz = (position.GetZ() -z0)/pixelsize; // TODO: this could maybe be replaced by GetCellSizeThickness()
//             float dut = dx*utx+dy*uty+dz*utz;
//             float dvt = dx*vtx+dy*vty+dz*vtz;
            float dut = 
                dx*transverseUnitVectorU.GetX()+
                dy*transverseUnitVectorU.GetY()+
                dz*transverseUnitVectorU.GetZ();
            float dvt = 
                dx*transverseUnitVectorV.GetX()+
                dy*transverseUnitVectorV.GetY()+
                dz*transverseUnitVectorV.GetZ();
            int idut  = static_cast<int>(dut+0.5+ioffset);
            int idvt  = static_cast<int>(dvt+0.5+ioffset);
            if(idut>=0&&idut<nbins&&idvt>=0&&idvt<nbins){
                tprofile[idut][idvt] += caloHit->GetElectromagneticEnergy();  
                tlprofile[idut][idvt][i] += caloHit->GetElectromagneticEnergy();  
            }
        }
    }

    // mask low ph region
    float threshold = 0.025f;
    for(int i=0; i<nbins; i++){
        for(int j=0; j<nbins; j++){
            if(tprofile[i][j]<threshold)assigned[i][j]=true;
        }
    }
    // fill peak values
    // 

    bool stillfindingpeaks=true;



    int  npeaks=0;
    float dmin=9999.;

    while(stillfindingpeaks){
        if (m_producePrintoutStatements > 0)
            std::cout << "PEAK SEARCH" << std::endl;

        float peakheight = 0.;
        int   ipeak =0;
        int   jpeak =0;
        float peakenergy=0;
        float xbar=0;
        float ybar=0;
        float xxbar=0;
        float yybar=0;
        float longitudinalProfile[m_maximumNumberOfLayers];

        for(int i=0;i<=maxLayers;i++)longitudinalProfile[i]=0.;
    
        for(int i=1; i<nbins-1; i++){
            for(int j=1; j<nbins-1; j++){
                if(!assigned[i][j]){
                    if(tprofile[i][j]>peakheight){
                        peakheight = tprofile[i][j];
                        ipeak = i;
                        jpeak = j;
                    }
                }
            }
        }
    
        if(peakheight<threshold){
            stillfindingpeaks=false;
        }else{
            int point[1000][2];
            int pstart = 0;
            int pend   = 0;
            int pcurrent = pend;
            point[0][0] = ipeak;
            point[0][1] = jpeak;
            peakenergy=tprofile[ipeak][jpeak];
            for(int i=0;i<=maxLayers;++i)longitudinalProfile[i]+=tlprofile[ipeak][jpeak][i];
            xbar=(ipeak-ioffset)*tprofile[ipeak][jpeak];
            ybar=(jpeak-ioffset)*tprofile[ipeak][jpeak];
            xxbar=(ipeak-ioffset)*(ipeak-ioffset)*tprofile[ipeak][jpeak];
            yybar=(jpeak-ioffset)*(jpeak-ioffset)*tprofile[ipeak][jpeak];
            assigned[ipeak][jpeak]=true;
            dmin = std::sqrt(static_cast<float>((ipeak-ioffset)*(ipeak-ioffset)+(jpeak-ioffset)*(jpeak-ioffset)));
            
            npeaks++;
            float stillgoing = true;
            while(stillgoing){
                for(int ip=pstart;ip<=pend;ip++){
                    int i = point[ip][0];
                    int j = point[ip][1];
                    float height = tprofile[i][j];
                    for(int ii=-1; ii<2; ii++){
                        int is = ii+i;
                        for(int jj=-1; jj<2; jj++){
                            int js = jj+j;
                            if(is>=0&&is<nbins&&js>=0&&js<nbins){
                                if(!assigned[is][js]){
                                    if(tprofile[is][js]<height*2.0){
                                        assigned[is][js]=true;
                                        peakenergy+=tprofile[is][js];
                                        for(int i=0;i<=maxLayers;++i)longitudinalProfile[i]+=tlprofile[is][js][i];
                                        xbar+= (is-ioffset)*tprofile[is][js];
                                        ybar+= (js-ioffset)*tprofile[is][js];
                                        xxbar+= (is-ioffset)*(is-ioffset)*tprofile[is][js];
                                        yybar+= (js-ioffset)*(js-ioffset)*tprofile[is][js];
                                        pcurrent++;
                                        point[pcurrent][0] = is;
                                        point[pcurrent][1] = js;
                                        if(tprofile[is][js]/tprofile[ipeak][jpeak]>0.1){
                                            float d = std::sqrt(static_cast<float>((is-ioffset)*(is-ioffset)+(js-ioffset)*(js-ioffset)));
                                            if(d<dmin)dmin=d;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if(pcurrent==pend)stillgoing=false;
                pstart = pend+1;
                pend=pcurrent;
            }
            xbar = xbar/peakenergy;
            ybar = ybar/peakenergy;
            xxbar = xxbar/peakenergy;
            yybar = yybar/peakenergy;

            protoClusterPeaks_t peak;
            peak.peakNumber = npeaks-1;
            peak.du = ipeak -ioffset;
            peak.dv = jpeak -ioffset;
            peak.energy  = peakenergy;
            peak.dmin    = dmin;
            float dr2 = xxbar+yybar -xbar*xbar-ybar*ybar;
            if(dr2>0)peak.rms  = sqrt(xxbar+yybar -xbar*xbar-ybar*ybar);
            if(dr2<=0)peak.rms = -999.;
            float sum = 0;
            bool stillGoing=true;

            peak.showerDepth90    = maxLayers;
            peak.showerDepth25    = maxLayers;
            for(int i=0;i<=maxLayers && stillGoing;i++){
                sum+= longitudinalProfile[i];
                if(sum>0.25*peakenergy && peak.showerDepth25==maxLayers){
                    peak.showerDepth25= i;
                }
                if(sum>0.9*peakenergy){
                    peak.showerDepth90= i;
                    stillGoing = false;
                }
            }

            stillGoing=true;
            peak.showerStartDepth    = maxLayers;
            for(int i=0;i<=maxLayers && stillGoing;i++){
                if(longitudinalProfile[i]>0.1){
                    peak.showerStartDepth= i;
                    stillGoing = false;
                }
            }

            // ****************************************************
            // IDEALLY WOULD INCLUDE SOME EM PROFILE CHI2 HERE
            // NOT INCLUDED AT THIS STAGE AS TOO DETECTOR SPECIFIC
            // ****************************************************
            // From v2.0 the code exists to do this - but isn't used yet

            peaks.push_back(peak);
        }
        if(npeaks>3)stillfindingpeaks=false;
    }

    return STATUS_CODE_SUCCESS;
}











//------------------------------------------------------------------------------------------------------------------------------------------

pandora::Cluster* ECalPhotonClusteringAlgorithm::TransverseProfile( ClusterProperties& clusterProperties, const pandora::OrderedCaloHitList& pOrderedCaloHitList, int peakForProtoCluster, unsigned int maxLayers, int extraLayers){

    pandora::Cluster* newCluster=NULL;

    const int nbins(41);
    const int ioffset(nbins / 2);

    float x0   = clusterProperties.m_centroidFirstLayer.GetX();
    float y0   = clusterProperties.m_centroidFirstLayer.GetY();
    float z0   = clusterProperties.m_centroidFirstLayer.GetZ();
    const CartesianVector centroid( x0, y0, z0 );

    CartesianVector transverseUnitVectorU(-1,-1,-1);
    CartesianVector transverseUnitVectorV(-1,-1,-1);
    ComputeTransverseVectors( centroid,
                              transverseUnitVectorU, 
                              transverseUnitVectorV );



    int  done[nbins][nbins];
    bool assigned[nbins][nbins];
    float tprofile[nbins][nbins];
    std::vector<CaloHit*> tlprofile[nbins][nbins][m_maximumNumberOfLayers];


    for(int i=0; i<nbins; i++){
        for(int j=0; j<nbins; j++){
            tprofile[i][j]=0;
            done[i][j]=assigned[i][j]=false;
        }
    }



    bool first = true;
    float pixelsize = 10.; 

    unsigned int  endLayer = static_cast<unsigned int>(maxLayers);
    if(extraLayers>0)endLayer+=extraLayers;
    for(unsigned int i=0; i<=endLayer; i++){

        CaloHitList* caloHitList;
        if( STATUS_CODE_SUCCESS != pOrderedCaloHitList.GetCaloHitsInPseudoLayer( PseudoLayer(i), caloHitList ) ) 
        {
            continue;
        }

        for( CaloHitList::iterator itCaloHit = caloHitList->begin(), itCaloHitEnd = caloHitList->end(); itCaloHit != itCaloHitEnd; ++itCaloHit )
        {
            CaloHit* caloHit = (*itCaloHit);
            if(first){
                pixelsize  = caloHit->GetCellLengthScale();
            }
            const CartesianVector& position = caloHit->GetPositionVector();
            float dx = (position.GetX() -x0)/pixelsize; // TODO: this could maybe be replaced by GetCellSizeU()
            float dy = (position.GetY() -y0)/pixelsize; // TODO: this could maybe be replaced by GetCellSizeV()
            float dz = (position.GetZ() -z0)/pixelsize; // TODO: this could maybe be replaced by GetCellSizeThickness()
//             float dut = dx*utx+dy*uty+dz*utz;
//             float dvt = dx*vtx+dy*vty+dz*vtz;
            float dut = 
                dx*transverseUnitVectorU.GetX()+
                dy*transverseUnitVectorU.GetY()+
                dz*transverseUnitVectorU.GetZ();
            float dvt = 
                dx*transverseUnitVectorV.GetX()+
                dy*transverseUnitVectorV.GetY()+
                dz*transverseUnitVectorV.GetZ();
            int idut  = static_cast<int>(dut+0.5+ioffset);
            int idvt  = static_cast<int>(dvt+0.5+ioffset);
            if(idut>=0&&idut<nbins&&idvt>=0&&idvt<nbins){
                if(i<=maxLayers)tprofile[idut][idvt] += caloHit->GetElectromagneticEnergy();  
//                 std::cout << "BBB tprofile["<<idut<<"]["<<idvt<<"] = " << tprofile[idut][idvt] << "  EM energy : " << caloHit->GetElectromagneticEnergy() << std::endl;
                if(i<=maxLayers+extraLayers)tlprofile[idut][idvt][i].push_back(caloHit);  
            }
        }
    }



    // mask low ph region
    float threshold = 0.025f;
    for(int i=0; i<nbins; i++){
        for(int j=0; j<nbins; j++){
            if(tprofile[i][j]<threshold)assigned[i][j]=true;
        }
    }
    // Search for peaks in profile
    // 
    bool stillfindingpeaks=true;
    int  npeaks=0;
    float dmin=9999.;

    while(stillfindingpeaks){
        float peakheight = 0.;
        int   ipeak =0;
        int   jpeak =0;
        float peakenergy=0;
        float xbar=0;
        float ybar=0;
        float xxbar=0;
        float yybar=0;
        std::vector<CaloHit*> longitudinalProfile[m_maximumNumberOfLayers];
    
        for(int i=1; i<nbins-1; i++){
            for(int j=1; j<nbins-1; j++){
                if(!assigned[i][j]){
                    if(tprofile[i][j]>peakheight){
                        peakheight = tprofile[i][j];
                        ipeak = i;
                        jpeak = j;
                    }
                }
            }
        }
    
        if(peakheight<threshold){
            stillfindingpeaks=false;
        }else{
            int point[1000][2];
            int pstart = 0;
            int pend   = 0;
            int pcurrent = pend;
            point[0][0] = ipeak;
            point[0][1] = jpeak;
            peakenergy=tprofile[ipeak][jpeak];
      
            for(unsigned int i=0;i<=endLayer;i++){
                for(unsigned int ihit=0;ihit<tlprofile[ipeak][jpeak][i].size();ihit++)
                    longitudinalProfile[i].push_back(tlprofile[ipeak][jpeak][i][ihit]);
            }

            xbar=(ipeak-ioffset)*tprofile[ipeak][jpeak];
            ybar=(jpeak-ioffset)*tprofile[ipeak][jpeak];
            xxbar=(ipeak-ioffset)*(ipeak-ioffset)*tprofile[ipeak][jpeak];
            yybar=(jpeak-ioffset)*(jpeak-ioffset)*tprofile[ipeak][jpeak];
            assigned[ipeak][jpeak]=true;
            dmin = std::sqrt(static_cast<float>((ipeak-ioffset)*(ipeak-ioffset)+ (jpeak-ioffset)*(jpeak-ioffset)));
            npeaks++;
            float stillgoing = true;
            while(stillgoing){
                for(int ip=pstart;ip<=pend;ip++){
                    int i = point[ip][0];
                    int j = point[ip][1];
                    float height = tprofile[i][j];
                    for(int ii=-1; ii<2; ii++){
                        int is = ii+i;
                        for(int jj=-1; jj<2; jj++){
                            int js = jj+j;
                            if(is>=0&&is<nbins&&js>=0&&js<nbins){
                                if(!assigned[is][js]){
                                    if(tprofile[is][js]<height*1.5){
                                        assigned[is][js]=true;
                                        peakenergy+=tprofile[is][js];
                                        for(unsigned int i=0;i<=endLayer;i++){
                                            for(unsigned int ihit=0;ihit<tlprofile[is][js][i].size();ihit++)
                                                longitudinalProfile[i].push_back(tlprofile[is][js][i][ihit]);
                                        }
                                        xbar+= (is-ioffset)*tprofile[is][js];
                                        ybar+= (js-ioffset)*tprofile[is][js];
                                        xxbar+= (is-ioffset)*(is-ioffset)*tprofile[is][js];
                                        yybar+= (js-ioffset)*(js-ioffset)*tprofile[is][js];
                                        pcurrent++;
                                        point[pcurrent][0] = is;
                                        point[pcurrent][1] = js;
                                        float d = std::sqrt(static_cast<float>((is-ioffset)*(is-ioffset)+(js-ioffset)*(js-ioffset)));
                                        if(d<dmin)dmin=d;
                                    }
                                }
                            }
                        }
                    }
                }
                if(pcurrent==pend)stillgoing=false;
                pstart = pend+1;
                pend=pcurrent;
            }

            // for the peak we actually want make a protocluster
            if(npeaks==peakForProtoCluster+1){

                CaloHitList* pCaloHitList = new CaloHitList();

                for(unsigned int i=0;i<=maxLayers+extraLayers;i++){
                    for(unsigned int ihit = 0; ihit<longitudinalProfile[i].size();ihit++){
                        CaloHit* caloHit = longitudinalProfile[i][ihit];
                        pCaloHitList->insert( caloHit );
                    }
                }

                // create the new cluster here
                if (m_producePrintoutStatements > 0)
                    std::cout << "create new cluster " << std::endl;

                PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHitList, newCluster ));

            }
        }
        if(npeaks>3)stillfindingpeaks=false;
    }

    return newCluster;

}






//------------------------------------------------------------------------------------------------------------------------------------------

float ECalPhotonClusteringAlgorithm::GetTrueEnergyContribution(const Cluster* cluster, float& electromagneticEnergyContribution, int pid )
{
#define PHOTONID 22

    typedef std::set< const MCParticle* > MCPARTICLESET;
    typedef std::map< int, double > ENERGYPIDMAP;
    typedef ENERGYPIDMAP::iterator ENERGYPIDMAPITERATOR;

    ENERGYPIDMAP trueEnergyPerMCParticleId;
    ENERGYPIDMAPITERATOR itTrueEnergyPerMCParticleId;

    ENERGYPIDMAP electromagneticEnergyPerMCParticleId;
    ENERGYPIDMAPITERATOR itElectromagneticEnergyPerMCParticleId;

    MCPARTICLESET countedMcParticles;

    float inputEnergy = 0.0;
    float sumTrueEnergy = 0.0;
    float sumElectromagneticEnergy = 0.0;
        
    OrderedCaloHitList pOrderedCaloHitList = cluster->GetOrderedCaloHitList();

    for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList.begin(), itLyrEnd = pOrderedCaloHitList.end(); itLyr != itLyrEnd; itLyr++ )
    {
        // int pseudoLayer = itLyr->first;
        CaloHitList::const_iterator itCaloHit    = itLyr->second->begin();
        CaloHitList::const_iterator itCaloHitEnd = itLyr->second->end();

        for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
        {

            // sum up the true energies from the MC-particles (don't double count the MCParticles)
            const MCParticle* mc = NULL; 
            (*itCaloHit)->GetMCParticle( mc );
            int particleId = 0;
            if( mc != NULL )             // else --> special case: sometimes some CalorimeterHits don't have a MCParticle (e.g. noise)
                particleId = mc->GetParticleId();


            double electromagneticEnergy  = (*itCaloHit)->GetElectromagneticEnergy();

            // add up the electromagnetic energy
            itElectromagneticEnergyPerMCParticleId = electromagneticEnergyPerMCParticleId.find( particleId );
            if( itElectromagneticEnergyPerMCParticleId == electromagneticEnergyPerMCParticleId.end() )
            {
                electromagneticEnergyPerMCParticleId.insert( std::make_pair<int,double>( particleId, electromagneticEnergy ) );
            }
            else
            {
                itElectromagneticEnergyPerMCParticleId->second += electromagneticEnergy;
            }
            sumElectromagneticEnergy += static_cast<float>(electromagneticEnergy);

            // sum up the input energy
            inputEnergy += (*itCaloHit)->GetInputEnergy();

            if( countedMcParticles.find( mc ) == countedMcParticles.end() ) 
            {
                double trueEnergy             = 0.0;
                if( mc != NULL )
                    trueEnergy = mc->GetEnergy();

                // add up the true energy
                itTrueEnergyPerMCParticleId = trueEnergyPerMCParticleId.find( particleId );
                if( itTrueEnergyPerMCParticleId == trueEnergyPerMCParticleId.end() )
                {
                    trueEnergyPerMCParticleId.insert( std::make_pair<int,double>( particleId, trueEnergy ) );
                }
                else
                {
                    itTrueEnergyPerMCParticleId->second += trueEnergy;
                }

                sumTrueEnergy            += static_cast<float>(trueEnergy);
                countedMcParticles.insert( mc );
            }
        }
    }

    if (m_producePrintoutStatements > 0)
    {
        for( ENERGYPIDMAP::iterator it = trueEnergyPerMCParticleId.begin(), itEnd = trueEnergyPerMCParticleId.end(); it != itEnd; ++it )
        {
            std::cout << "pid " << it->first 
                      << "  e-contrib " << electromagneticEnergyPerMCParticleId.find(it->first)->second
                      << "  true e-contrib " << it->second 
                      << "  of electromagnetic energy: " << sumElectromagneticEnergy 
                      << "  and input energy: " << inputEnergy 
                      << "  true E: " << sumTrueEnergy << std::endl;
        }
    }

    if (m_producePrintoutStatements > 0)
        std::cout << "energy-sum : " << sumElectromagneticEnergy << std::endl;

    if( pid == 0 )
    {
        electromagneticEnergyContribution = sumElectromagneticEnergy;
        return sumTrueEnergy;
    }

    itTrueEnergyPerMCParticleId            = trueEnergyPerMCParticleId.find( pid );
    itElectromagneticEnergyPerMCParticleId = electromagneticEnergyPerMCParticleId.find( pid );

    if( (itTrueEnergyPerMCParticleId == trueEnergyPerMCParticleId.end()) 
        || (itElectromagneticEnergyPerMCParticleId == electromagneticEnergyPerMCParticleId.end() ) )
    {
        electromagneticEnergyContribution = 0.0;
        return 0.0;
    }

        
    electromagneticEnergyContribution = static_cast<float>(itElectromagneticEnergyPerMCParticleId->second);
    return static_cast<float>(itTrueEnergyPerMCParticleId->second); // return the energy contribution of photons
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ECalPhotonClusteringAlgorithm::GetClusterProperties(const Cluster* cluster, ClusterProperties& clusterProperties )
{
    RunningMeanRMS hitMean[3];    // [3] for the coordinates x,y and z
    RunningMeanRMS centroid[3];
    RunningMeanRMS centroid10[3]; // centroid of the first 10 layers
    RunningMeanRMS centroid20[3]; // centroid of the first 20 layers

    float centroidEnergy = 0.0;
    float centroid10Energy = 0.0;
    float centroid20Energy = 0.0;


    clusterProperties.electromagneticEnergy = cluster->GetElectromagneticEnergy();


    // loop over all layers
    OrderedCaloHitList pOrderedCaloHitList = cluster->GetOrderedCaloHitList();
    for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList.begin(), itLyrEnd = pOrderedCaloHitList.end(); itLyr != itLyrEnd; itLyr++ )
    {
        // int pseudoLayer = itLyr->first;
        CaloHitList::iterator itCaloHit    = itLyr->second->begin();
        CaloHitList::iterator itCaloHitEnd = itLyr->second->end();

        unsigned int layerNumber = itLyr->first;

        // loop over al calohits
        for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
        {
            const CartesianVector& position  = (*itCaloHit)->GetPositionVector();
            // mean position of hits
            hitMean[0].Fill( position.GetX(), 1.0 );
            hitMean[1].Fill( position.GetY(), 1.0 );
            hitMean[2].Fill( position.GetZ(), 1.0 );

            // centroid
            float electromagneticEnergy = (*itCaloHit)->GetElectromagneticEnergy();
            centroid[0].Fill( position.GetX(), electromagneticEnergy );
            centroid[1].Fill( position.GetY(), electromagneticEnergy );
            centroid[2].Fill( position.GetZ(), electromagneticEnergy );
            centroidEnergy += electromagneticEnergy;

            if( layerNumber>0 && layerNumber<=10 ) // centroid 10
            {
                centroid10[0].Fill( position.GetX(), electromagneticEnergy );
                centroid10[1].Fill( position.GetY(), electromagneticEnergy );
                centroid10[2].Fill( position.GetZ(), electromagneticEnergy );
                centroid10Energy += electromagneticEnergy;
            }
            else if( layerNumber>10 && layerNumber<=20 )
            {
                centroid20[0].Fill( position.GetX(), electromagneticEnergy );
                centroid20[1].Fill( position.GetY(), electromagneticEnergy );
                centroid20[2].Fill( position.GetZ(), electromagneticEnergy );
                centroid20Energy += electromagneticEnergy;
            }
        }
    }
    
    // set the values in clusterProperties
    for( int i=0; i<3; ++i )
    {
        clusterProperties.m_hitMean[i]    = static_cast<float>(hitMean[i].GetMean());
        clusterProperties.m_centroid[i]   = static_cast<float>(centroid[i].GetMean());
        clusterProperties.m_centroid10[i] = static_cast<float>(centroid10[i].GetMean());
        clusterProperties.m_centroid20[i] = static_cast<float>(centroid20[i].GetMean());
    }
    clusterProperties.m_centroidEnergy   = centroidEnergy;
    clusterProperties.m_centroid10Energy = centroid10Energy;
    clusterProperties.m_centroid20Energy = centroid20Energy;

    clusterProperties.m_innerPseudoLayer = (int)cluster->GetInnerPseudoLayer();

    clusterProperties.m_centroidFirstLayer = cluster->GetCentroid(clusterProperties.m_innerPseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ECalPhotonClusteringAlgorithm::DistanceToPositionAndDirection(const CartesianVector &position, const CartesianVector &referencePosition,
                                                           const CartesianVector &referenceDirection, float &longitudinalComponent, float &perpendicularComponent )
{
    const CartesianVector  relativePosition = position - referencePosition;
    longitudinalComponent  = referenceDirection.GetUnitVector().GetDotProduct( relativePosition );
    perpendicularComponent = (relativePosition - referenceDirection*longitudinalComponent).GetMagnitude();
}


//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonIDLikelihoodCalculator::WriteXmlSig( const std::string& fileName )
{
     TiXmlDocument doc("PhotonLikelihoodData_Sig");
     TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
     doc.LinkEndChild( decl );

     TiXmlElement * element = NULL;
     energySig.WriteToXml( element );
     doc.LinkEndChild( element );

     rmsSig.WriteToXml( element );
     doc.LinkEndChild( element );
     
     fracSig.WriteToXml( element );
     doc.LinkEndChild( element );
     
     startSig.WriteToXml( element );
     doc.LinkEndChild( element );

     doc.SaveFile( fileName );
}


//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonIDLikelihoodCalculator::WriteXmlBkg( const std::string& fileName )
{
     TiXmlDocument doc("PhotonLikelihoodData_Bkg");
     TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
     doc.LinkEndChild( decl );

     TiXmlElement * element = NULL;
     energyBkg.WriteToXml( element );
     doc.LinkEndChild( element );

     rmsBkg.WriteToXml( element );
     doc.LinkEndChild( element );
     
     fracBkg.WriteToXml( element );
     doc.LinkEndChild( element );
     
     startBkg.WriteToXml( element );
     doc.LinkEndChild( element );

     doc.SaveFile( fileName );
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonIDLikelihoodCalculator::LoadXml( const std::string& fileNameSig, const std::string& fileNameBkg )
{
    std::cout << "Load photon clustering configuration / loadXml" << std::endl;
    ReadXmlSignal( fileNameSig );
    ReadXmlBackground( fileNameBkg );

    PhotonIDLikelihoodCalculator::fromXml = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonIDLikelihoodCalculator::ReadXmlSignal( const std::string& fileNameSig )
{
    std::cout << "Load photon clustering signal data" << std::endl;
    TiXmlDocument docSig(fileNameSig);
    docSig.LoadFile();

    TiXmlElement * loadElement = docSig.FirstChildElement();

    energySig.ReadFromXml( *loadElement );
    loadElement = loadElement->NextSiblingElement();
    rmsSig.ReadFromXml   ( *loadElement );
    loadElement = loadElement->NextSiblingElement();
    fracSig.ReadFromXml  ( *loadElement );
    loadElement = loadElement->NextSiblingElement();
    startSig.ReadFromXml ( *loadElement );

    energySig.Print( std::cout );

    rmsSig.Print(std::cout);
    fracSig.Print(std::cout);
    startSig.Print(std::cout);
}


//------------------------------------------------------------------------------------------------------------------------------------------

void PhotonIDLikelihoodCalculator::ReadXmlBackground( const std::string& fileNameBkg )
{
    std::cout << "Load photon clustering background data" << std::endl;
    TiXmlDocument docBkg(fileNameBkg);
    docBkg.LoadFile();
    TiXmlElement * loadElement = docBkg.FirstChildElement();
    
    energyBkg.ReadFromXml( *loadElement );
    loadElement = loadElement->NextSiblingElement();
    rmsBkg.ReadFromXml   ( *loadElement );
    loadElement = loadElement->NextSiblingElement();
    fracBkg.ReadFromXml  ( *loadElement );
    loadElement = loadElement->NextSiblingElement();
    startBkg.ReadFromXml ( *loadElement );

    energyBkg.Print( std::cout );

    rmsBkg.Print(std::cout);
    fracBkg.Print(std::cout);
    startBkg.Print(std::cout);
}


//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonIDLikelihoodCalculator::PID(float E, float rms, float frac, float start )
{
    float pid = 0;
    float yes = 0, no = 0;

    if( PhotonIDLikelihoodCalculator::fromXml )
    {
        PhotonIDLikelihoodCalculator* plc = PhotonIDLikelihoodCalculator::Instance();

        float lhSig      = plc->energySig.Get(E);

        float lhRmsSig   = plc->rmsSig.Get  (E, rms  );
        float lhFracSig  = plc->fracSig.Get (E, frac );
        float lhStartSig = plc->startSig.Get(E, start);
        
        float lhBkg      = plc->energyBkg.Get(E);
        float lhRmsBkg   = plc->rmsBkg.Get  (E, rms  );
        float lhFracBkg  = plc->fracBkg.Get (E, frac );
        float lhStartBkg = plc->startBkg.Get(E, start);
        
        yes = lhSig*lhRmsSig*lhFracSig*lhStartSig;
        no  = lhBkg*lhRmsBkg*lhFracBkg*lhStartBkg;
    }
    else
    {
        int ien = 0;
        if( E >  0.2 && E <=  0.5)ien=0;
        if( E >  0.5 && E <=  1.0)ien=1;
        if( E >  1.0 && E <=  1.5)ien=2;
        if( E >  1.5 && E <=  2.5)ien=3;
        if( E >  2.5 && E <=  5.0)ien=4;
        if( E >  5.0 && E <= 10.0)ien=5;
        if( E > 10.0 && E <= 20.0)ien=6;
        if( E > 20.0 && E <= 50.0)ien=7;
        if( E > 50.0)ien=8;
        int irmsbin = int(rms*4)+1;
        if(irmsbin<0)irmsbin  =  0;
        if(irmsbin>21)irmsbin = 21;
        int ifracbin = int(frac*20)+1;
        if(ifracbin<0)ifracbin  =  0;
        if(ifracbin>21)ifracbin = 21;
        int istartbin = int(start*2)+1;
        if(istartbin<0)istartbin  =  0;
        if(istartbin>21)istartbin = 21;

        if(0) // TODO make m_producePrintoutStatements from main algorithm (public and) static so can access here
        {
            std::cout << " E     = " << E     << " -> " << ien       << std::endl;
            std::cout << " rms   = " << rms   << " -> " << irmsbin   << std::endl;
            std::cout << " frac  = " << frac  << " -> " << ifracbin  << std::endl;
            std::cout << " start = " << start << " -> " << istartbin << std::endl;
            std::cout << " likeSig[ie]  = " << likeSig[ien] << std::endl;
            std::cout << " likeBack[ie] = " << likeBack[ien] << std::endl;

            std::cout << " likesrms[ie][irmsbin] = " << likesrms[ien][irmsbin] << std::endl;
            std::cout << " likebrms[ie][irmsbin] = " << likebrms[ien][irmsbin] << std::endl;

            std::cout << " likesfrac[ie][irmsbin] = " << likesfrac[ien][ifracbin] << std::endl;
            std::cout << " likebfrac[ie][irmsbin] = " << likebfrac[ien][ifracbin] << std::endl;

            std::cout << " likesstart[ie][irmsbin] = " << likesstart[ien][istartbin] << std::endl;
            std::cout << " likebstart[ie][irmsbin] = " << likebstart[ien][istartbin] << std::endl;
        }


        yes = static_cast<float>(likeSig[ien]*likesrms[ien][irmsbin]*likesfrac[ien][ifracbin]*likesstart[ien][istartbin]);
        no  = static_cast<float>(likeBack[ien]*likebrms[ien][irmsbin]*likebfrac[ien][ifracbin]*likebstart[ien][istartbin]);
    }
    
    if(0)
    {
        std::cout << "yes " << yes << std::endl;
        std::cout << "no  " << no << std::endl;
    }

    if(yes+no>0)pid = yes/(yes+no);

    return pid;
}





//------------------------------------------------------------------------------------------------------------------------------------------

Axis::Axis( int bins, float from, float to )
{
    SetDimensions( bins, from, to );
}

//------------------------------------------------------------------------------------------------------------------------------------------

 Axis::Axis( const std::vector<float>& binBorders )
{
    SetDimensions( binBorders );
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Axis::SetDimensions( int bins, float from, float to )
{
    numberBins = bins;
    minValue = from;
    maxValue = to;

    if( numberBins <= 0 )
        throw WrongBinNumber();
    if( maxValue <= minValue )
        throw WrongLimits();

    k = numberBins/(maxValue-minValue);

    regularBins = true;
}


//------------------------------------------------------------------------------------------------------------------------------------------

void Axis::SetDimensions( const std::vector<float>& binBorders ) 
{
    std::vector<float> tmpBorders;
    tmpBorders.assign( binBorders.begin(), binBorders.end() );
    std::sort( tmpBorders.begin(), tmpBorders.end() );

    numberBins = tmpBorders.size();
    minValue   = tmpBorders.front();
    maxValue   = tmpBorders.back();
    
    int idx = -1;
    for( std::vector<float>::const_iterator itBinBorder = tmpBorders.begin(), itBinBorderEnd = tmpBorders.end(); itBinBorder != itBinBorderEnd; ++itBinBorder )
    {
        const float border = (*itBinBorder);
        bins.insert( std::make_pair(border, idx) );

        ++idx;
    }

    regularBins = false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

Axis::Axis()
    : numberBins(0),
      minValue(0),
      maxValue(0)
{
}


//------------------------------------------------------------------------------------------------------------------------------------------

int Axis::GetBinForValue( float value )
{
    if( regularBins )
    {
        int bin = 0;
        if( value < minValue )
            return -1;
        if( value >= maxValue )
            return numberBins;

        bin = int(k*(value-minValue));
        return bin;
    }

    BinMap::iterator itBin = bins.upper_bound( value );
    if( itBin == bins.end() ) // overflow
    {
        return numberBins;
    }

    return itBin->second;
}
	
//------------------------------------------------------------------------------------------------------------------------------------------

int Axis::GetNumberBins()
{
    return numberBins;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Axis::WriteToXml( TiXmlElement *xmlElement ) 
{
    TiXmlElement * pXmlElementAxis = new TiXmlElement("Axis");

    if( regularBins )
    {
        pXmlElementAxis->SetAttribute( "RegularBins", 1 );

        pXmlElementAxis->SetAttribute( "Bins", GetNumberBins() );
        pXmlElementAxis->SetDoubleAttribute( "From", GetMinValue() );
        pXmlElementAxis->SetDoubleAttribute( "To",   GetMaxValue() );
    }
    else
    {
        pXmlElementAxis->SetAttribute( "RegularBins", 0 );

        std::stringstream sstr;
        for( BinMap::iterator itBin = bins.begin(), itBinEnd = bins.end(); itBin != itBinEnd; ++itBin )
        {
            float value = itBin->first;

            if( itBin != bins.begin() )
                sstr << " ";
            sstr << value;
        }
        pXmlElementAxis->SetAttribute( "Bins", sstr.str() );
    }

    if( !xmlElement->LinkEndChild(pXmlElementAxis) )
        throw Axis::XmlError();

}

//------------------------------------------------------------------------------------------------------------------------------------------

void Axis::ReadFromXml( const TiXmlElement &xmlElement ) 
{
    std::string value = xmlElement.ValueStr();

    if( value != "Axis" )
        throw NotAxis();

    int rb = 1;
    xmlElement.QueryIntAttribute( "RegularBins", &rb );
    regularBins = false;
    if( rb == 1 )
        regularBins = true;

    if( regularBins )
    {
        xmlElement.QueryIntAttribute  ( "Bins", &numberBins );
        xmlElement.QueryFloatAttribute( "From", &minValue );
        xmlElement.QueryFloatAttribute( "To",   &maxValue );

        SetDimensions( numberBins, minValue, maxValue );
    }
    else
    {
        std::string binsString = "";
        binsString = xmlElement.Attribute( "Bins" );

        StringVector binsStringVector;

        TokenizeString( binsString, binsStringVector, " " );

        // transform the strings to float and fill the bins
        std::vector<float> binBorders;
        for( StringVector::iterator itBin = binsStringVector.begin(), itBinEnd = binsStringVector.end(); itBin != itBinEnd; ++itBin )
        {
            std::stringstream binStr;
            binStr << (*itBin);
            float binValue;
            binStr >> binValue;
            binBorders.push_back( binValue );
        }
        std::sort( binBorders.begin(), binBorders.end() );
        SetDimensions( binBorders );
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Axis::Print( std::ostream& os )
{
    os << "Axis : ";
    if( regularBins )
        os << "bins = " << numberBins << " from = " << minValue <<  " to = " << maxValue << std::endl;
    else
    {
        std::stringstream sstr;
        for( BinMap::iterator itBin = bins.begin(), itBinEnd = bins.end(); itBin != itBinEnd; ++itBin )
        {
            float value = itBin->first;
            if( itBin != bins.begin() )
                sstr << ", ";
            sstr << value;
        }
        os << "Bin-borders = " << sstr.str() << std::endl;
    }
} 

//------------------------------------------------------------------------------------------------------------------------------------------

void Axis::TokenizeString(const std::string &inputString, StringVector &tokens, const std::string &delimiter)
{
    // tokenize the string
    std::string::size_type lastPos = inputString.find_first_not_of(delimiter, 0);
    std::string::size_type pos     = inputString.find_first_of(delimiter, lastPos);

    while ((std::string::npos != pos) || (std::string::npos != lastPos))
    {
        tokens.push_back(inputString.substr(lastPos, pos - lastPos));
        lastPos = inputString.find_first_not_of(delimiter, pos);
        pos = inputString.find_first_of(delimiter, lastPos);
    }
}









//------------------------------------------------------------------------------------------------------------------------------------------

Histogram1D::Histogram1D( const std::string& histogramName, int numberBins, float from, float to )
{
    // create empty bins

    SetDimensions( histogramName, numberBins, from, to );

    CreateEmptyBins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram1D::SetDimensions( const std::string& histogramName, int numberBins, float from, float to )
{
    // create empty bins
    name = histogramName;
    axis.SetDimensions( numberBins, from, to );

    CreateEmptyBins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram1D::SetDimensions( const std::string& histogramName, const std::vector<float>& binBorders )
{
    // create empty bins
    name = histogramName;
    axis.SetDimensions( binBorders );

    CreateEmptyBins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram1D::CreateEmptyBins()
{
    for( int ibin = -1; ibin <= axis.GetNumberBins(); ++ibin ) // add -1 as underflow and numberBins as overflow bin
    {
        bins.insert( std::make_pair<int,float>(ibin,0.f) );
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------

Histogram1D::Histogram1D( const TiXmlElement &xmlElement )
{
    ReadFromXml( xmlElement );
}

//------------------------------------------------------------------------------------------------------------------------------------------

Histogram1D::Histogram1D()
    : axis()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram1D::Fill( float value, float weight )
{
    bins[axis.GetBinForValue(value)] += weight;
}


//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram1D::GetBinContent( int bin )
{
    MapOfBins::iterator itBin = bins.find( bin );
    if( itBin == bins.end() )
        throw Axis::WrongBinNumber();

    return itBin->second;
}


//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram1D::Get( float value )
{
    int bin = axis.GetBinForValue(value);
    
    return GetBinContent( bin );
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram1D::GetSumOfEntries()
{
    float sum = 0.f;
    for( MapOfBins::iterator itBin = bins.begin(), itBinEnd = bins.end(); itBin != itBinEnd; ++itBin )
    {
        float value = itBin->second;
        sum += value;
    }
    return sum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram1D::Scale( float value )
{
    for( MapOfBins::iterator itBin = bins.begin(), itBinEnd = bins.end(); itBin != itBinEnd; ++itBin )
    {
        float binValue = itBin->second;
        itBin->second = binValue/value;
    }
}




//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram1D::WriteToXml ( TiXmlElement * &xmlElement ) 
{
    xmlElement = new TiXmlElement( name );
    xmlElement->SetAttribute( "Type", "Histogram1D" );

    axis.WriteToXml( xmlElement );

    TiXmlElement * pXmlElementBins = new TiXmlElement("Bins");
    TiXmlText *    pXmlText        = new TiXmlText("Bins");

    std::stringstream sstr;
    for( MapOfBins::iterator itBin = bins.begin(), itBinEnd = bins.end(); itBin != itBinEnd; ++itBin )
    {
        float value = itBin->second;
        if( itBin != bins.begin() )
            sstr << " ";
        sstr << value;
    }
    pXmlText->SetValue( sstr.str() );
    pXmlElementBins->LinkEndChild(pXmlText);

    if( !xmlElement->LinkEndChild(pXmlElementBins) )
        throw Histogram1D::XmlError();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram1D::ReadFromXml( const TiXmlElement&  xmlElement )
{
    name = xmlElement.Value();

    std::string type = "";
    if( (type = xmlElement.Attribute( "Type" )) != "Histogram1D" )
    {
        std::cout << "Error at reading Xml file: You tried to load a Histogram1D, but Xml element is of type '" << type << "'." << std::endl;
        std::cout << "The name of the Xml element is : " << name << std::endl;
        throw NotHistogram1D();
    }

    const TiXmlElement* pXmlAxisElement = xmlElement.FirstChildElement();

    axis.ReadFromXml( *(pXmlAxisElement) );

    const TiXmlElement* pXmlBins = pXmlAxisElement->NextSiblingElement();
    if( !pXmlBins )
        throw XmlError();

    std::string binsString = pXmlBins->GetText();

    StringVector binsStringVector;

    Axis::TokenizeString( binsString, binsStringVector, " " );

    // transform the strings to float and fill the bins
    bins.clear();
    int bin = -1; // start with the underflow bin
    for( StringVector::iterator itBin = binsStringVector.begin(), itBinEnd = binsStringVector.end(); itBin != itBinEnd; ++itBin )
    {
        std::stringstream binStr;
        binStr << (*itBin);
        float binValue;
        binStr >> binValue;
        bins.insert( std::make_pair(bin,binValue) );
        ++bin;
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram1D::Print( std::ostream& os )
{
    std::cout << "=== Histogram1D === [" << name << "] " << std::endl;
    axis.Print( os );

    std::stringstream binValuesString;
    for( MapOfBins::iterator itBin = bins.begin(), itBinEnd = bins.end(); itBin != itBinEnd; ++itBin )
    {
        int   bin   = itBin->first;
        float value = itBin->second;
        if( itBin != bins.begin() )
            binValuesString << "  ";
        binValuesString << "[" << bin << "]=" << value << " ";
    }
    os << binValuesString.str() << std::endl;
}















//------------------------------------------------------------------------------------------------------------------------------------------

Histogram2D::Histogram2D( const std::string& histogramName, int numberBinsX, float fromX, float toX, int numberBinsY, float fromY, float toY )
{
    // create empty bins
    SetDimensions( histogramName, numberBinsX, fromX, toX, numberBinsY, fromY, toY );
}

//------------------------------------------------------------------------------------------------------------------------------------------

Histogram2D::Histogram2D( const std::string& histogramName, const std::vector<float>& binBorders, int numberBinsY, float fromY, float toY )
{
    SetDimensions( histogramName, binBorders, numberBinsY, fromY, toY );
}

//------------------------------------------------------------------------------------------------------------------------------------------

Histogram2D::Histogram2D( const std::string& histogramName, int numberBinsX, float fromX, float toX, const std::vector<float>& binBorders )
{
    SetDimensions( histogramName, numberBinsX, fromX, toX, binBorders );
}



//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::SetDimensions( const std::string& histogramName, int numberBinsX, float fromX, float toX, int numberBinsY, float fromY, float toY )
{
    // create empty bins
    name = histogramName;
    axisX.SetDimensions( numberBinsX, fromX, toX );
    axisY.SetDimensions( numberBinsY, fromY, toY );

    CreateEmptyBins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::SetDimensions( const std::string& histogramName, const std::vector<float>& binBorders, int numberBinsY, float fromY, float toY )
{
    // create empty bins
    name = histogramName;
    axisX.SetDimensions( binBorders );
    axisY.SetDimensions( numberBinsY, fromY, toY );

    CreateEmptyBins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::SetDimensions( const std::string& histogramName, int numberBinsX, float fromX, float toX, const std::vector<float>& binBorders )
{
    // create empty bins
    name = histogramName;
    axisX.SetDimensions( numberBinsX, fromX, toX );
    axisY.SetDimensions( binBorders );

    CreateEmptyBins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::SetDimensions( const std::string& histogramName, const std::vector<float>& binBordersX, const std::vector<float>& binBordersY )
{
    // create empty bins
    name = histogramName;
    axisX.SetDimensions( binBordersX );
    axisY.SetDimensions( binBordersY );

    CreateEmptyBins();
}



//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::CreateEmptyBins()
{
    for( int ibinY = -1; ibinY <= axisY.GetNumberBins(); ++ibinY ) // add -1 as underflow and numberBins as overflow bin
    {
        bins.insert( std::make_pair(ibinY,MapOfBins()) );
        
        MapOfMapOfBins::iterator itMoMoB = bins.find( ibinY );
        if( itMoMoB == bins.end() )
            throw DataStructureError();

        for( int ibinX = -1; ibinX <= axisX.GetNumberBins(); ++ibinX ) // add -1 as underflow and numberBins as overflow bin
        {
            itMoMoB->second.insert( std::make_pair<int,float>(ibinX,0.f) );
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------

Histogram2D::Histogram2D( const TiXmlElement &xmlElement )
{
    ReadFromXml( xmlElement );
}

//------------------------------------------------------------------------------------------------------------------------------------------

Histogram2D::Histogram2D()
    : axisX(),
      axisY()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::Fill( float valueX, float valueY, float weight )
{
    bins[axisY.GetBinForValue(valueY)][axisX.GetBinForValue(valueX)] += weight;
}


//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram2D::GetBinContent( int binX, int binY )
{
    MapOfMapOfBins::iterator itBinY = bins.find( binY );
    if( itBinY == bins.end() )
        throw Axis::WrongBinNumber();

    MapOfBins& binXMap = itBinY->second;
    MapOfBins::iterator itBinX = binXMap.find( binX );
    if( itBinX == binXMap.end() )
        throw Axis::WrongBinNumber();

    return itBinX->second;
}


//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram2D::Get( float valueX, float valueY )
{
    int binX = axisX.GetBinForValue(valueX);
    int binY = axisY.GetBinForValue(valueY);

    return GetBinContent(binX, binY);
}


//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram2D::GetSumOfEntries()
{
    float sum = 0.f;
    for( MapOfMapOfBins::iterator itBinY = bins.begin(), itBinYEnd = bins.end(); itBinY != itBinYEnd; ++itBinY )
    {
        MapOfBins& binsX = itBinY->second;
        
        for( MapOfBins::iterator itBinX = binsX.begin(), itBinXEnd = binsX.end(); itBinX != itBinXEnd; ++itBinX )
        {
            float value = itBinX->second;
            sum += value;
        }
    }

    return sum;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::Scale( float value )
{
    for( MapOfMapOfBins::iterator itBinY = bins.begin(), itBinYEnd = bins.end(); itBinY != itBinYEnd; ++itBinY )
    {
        MapOfBins& binsX = itBinY->second;
        
        for( MapOfBins::iterator itBinX = binsX.begin(), itBinXEnd = binsX.end(); itBinX != itBinXEnd; ++itBinX )
        {
            float binValue = itBinX->second;
            itBinX->second = binValue/value;
        }
    }
}





//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::WriteToXml ( TiXmlElement * &xmlElement ) 
{
    xmlElement = new TiXmlElement( name );
    xmlElement->SetAttribute( "Type", "Histogram2D" );

    axisX.WriteToXml( xmlElement );
    axisY.WriteToXml( xmlElement );

    TiXmlElement * pXmlElementBins;
    TiXmlText *    pXmlText;

    for( MapOfMapOfBins::iterator itBinY = bins.begin(), itBinYEnd = bins.end(); itBinY != itBinYEnd; ++itBinY )
    {
        std::stringstream sstr;

        int        iBinY = itBinY->first;
        MapOfBins& binsX = itBinY->second;
        
        pXmlElementBins = new TiXmlElement("Bins");
        pXmlText = new TiXmlText("Bins");
        pXmlElementBins->SetAttribute( "BinY", iBinY );

        for( MapOfBins::iterator itBinX = binsX.begin(), itBinXEnd = binsX.end(); itBinX != itBinXEnd; ++itBinX )
        {
            float value = itBinX->second;

            if( itBinX != binsX.begin() )
                sstr << " ";
            sstr << value;
        }
        pXmlText->SetValue( sstr.str() );
        pXmlElementBins->LinkEndChild(pXmlText);
        if( !xmlElement->LinkEndChild(pXmlElementBins) )
            throw Histogram2D::XmlError();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::ReadFromXml( const TiXmlElement&  xmlElement )
{
    name = xmlElement.Value();
    
    std::string type = "";
    if( (type = xmlElement.Attribute( "Type" )) != "Histogram2D" )
    {
        std::cout << "Error at reading Xml file: You tried to load a Histogram2D, but Xml element is of type '" << type << "'." << std::endl;
        std::cout << "The name of the Xml element is : " << name << std::endl;
        throw NotHistogram2D();
    }

    const TiXmlElement* pXmlAxisXElement = xmlElement.FirstChildElement();
    axisX.ReadFromXml( *(pXmlAxisXElement) );

    const TiXmlElement* pXmlAxisYElement = pXmlAxisXElement->NextSiblingElement();
    axisY.ReadFromXml( *(pXmlAxisYElement) );

    const TiXmlElement* pXmlBins = pXmlAxisYElement;

    bins.clear();
    while( (pXmlBins = pXmlBins->NextSiblingElement()) != NULL )
    {
        if( !pXmlBins )
            throw XmlError();

        int iBinY = 0;
        pXmlBins->QueryIntAttribute( "BinY", &iBinY );

        std::string binsString = pXmlBins->GetText();

        StringVector binsStringVector;

        // tokenize the string
        Axis::TokenizeString( binsString, binsStringVector, " " );

        // transform the strings to float and fill the bins
        int iBinX = -1; // start with the underflow bin
        
        for( StringVector::iterator itBinX = binsStringVector.begin(), itBinXEnd = binsStringVector.end(); itBinX != itBinXEnd; ++itBinX )
        {
            std::stringstream binStr;
            binStr << (*itBinX);
            float binValue;
            binStr >> binValue;
            bins[iBinY][iBinX] = binValue; 
            ++iBinX;
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram2D::Print( std::ostream& os )
{
    std::cout << "=== Histogram2D === [" << name << "] " << std::endl;
    axisX.Print( os );
    axisY.Print( os );

    for( MapOfMapOfBins::iterator itBinY = bins.begin(), itBinYEnd = bins.end(); itBinY != itBinYEnd; ++itBinY )
    {
        std::stringstream binValuesString;

        int        iBinY = itBinY->first;
        MapOfBins& binsX = itBinY->second;

        for( MapOfBins::iterator itBinX = binsX.begin(), itBinXEnd = binsX.end(); itBinX != itBinXEnd; ++itBinX )
        {
            int   iBinX = itBinX->first;
            float value = itBinX->second;

            if( itBinX != binsX.begin() )
                binValuesString << "  ";
            binValuesString << "[" << iBinX << "," << iBinY << "]=" << value;
        }
        os << binValuesString.str() << std::endl;
    }
}







