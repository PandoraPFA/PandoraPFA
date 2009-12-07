/**
 *  @file   PandoraPFANew/src/Algorithms/ECalPhotonIdAlgorithm.cc
 * 
 *  @brief  Implementation of the photon clustering algorithm class.
 * 
 *  $Log: $
 */

#include <functional>
#include <algorithm>
#include <iostream>
#include <iomanip>


#include "Algorithms/ECalPhotonIdAlgorithm.h"
#include "Objects/MCParticle.h"

#include <cmath>

using namespace pandora;


PhotonIDLikelihoodCalculator* PhotonIDLikelihoodCalculator::_instance =0;

PhotonIDLikelihoodCalculator* PhotonIDLikelihoodCalculator::Instance(){
    if(_instance==0){
        _instance = new PhotonIDLikelihoodCalculator;
    }

    return _instance;
}



//------------------------------------------------------------------------------------------------------------------------------------------

ECalPhotonIdAlgorithm::ECalPhotonIdAlgorithm()
{
    std::cout << "constructor" << std::endl;

}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonIdAlgorithm::Initialize()
{
    std::cout << "initialize" << std::endl;
    std::cout << "makingphotonid " <<_makingPhotonIDLikelihoodHistograms << std::endl;

    // create monitoring histograms for likelihood
    if(_makingPhotonIDLikelihoodHistograms)
        CreateOrSaveLikelihoodHistograms(true);

    return STATUS_CODE_SUCCESS;
}


//------------------------------------------------------------------------------------------------------------------------------------------

ECalPhotonIdAlgorithm::~ECalPhotonIdAlgorithm()
{

    std::cout << "destructor" << std::endl;
    std::cout << "makingphotonid " <<_makingPhotonIDLikelihoodHistograms << std::endl;
    // create monitoring histograms for likelihood
    if(_makingPhotonIDLikelihoodHistograms)
        CreateOrSaveLikelihoodHistograms(false);

}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonIdAlgorithm::Run()
{


    // Run initial clustering algorithm
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunClusteringAlgorithm(*this, m_clusteringAlgorithmName, pClusterList));

    // load the tracks only for event display
//     const TrackList *pTrackList = NULL;
//     PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentTrackList(*this, pTrackList));

    // set object variables:
    _nEcalLayers = GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers();

    ClusterList photonClusters;
    ClusterList nonPhotonClusters;
    for( ClusterList::const_iterator itCluster = pClusterList->begin(), itClusterEnd = pClusterList->end(); itCluster != itClusterEnd; itCluster++ )
    {
        if( (*itCluster)->GetNCaloHits() < _minimumHitsInCluster )
            continue;
        if( IsPhoton( *itCluster ) )
        {
            std::cout << "is photon cluster? --> YES ";
            photonClusters.insert( *itCluster );
	    (*itCluster)->SetIsPhotonFlag( true );
        }
        else
        {
            std::cout << "is photon cluster? --> NO ";
            nonPhotonClusters.insert( *itCluster );
	    (*itCluster)->SetIsPhotonFlag( false );
        }
        std::cout << std::endl;
    }    



    //Save the clusters tagged as photons
    if( !photonClusters.empty() )
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, m_photonClusterListName, photonClusters));
        //    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterListAndReplaceCurrent(*this, m_photonClusterListName, photonClusters));
    }

//     PANDORA_MONITORING_API(DrawEvent(DETECTOR_VIEW_XZ, pTrackList, pClusterList ) );
//     PANDORA_MONITORING_API(DrawEvent(DETECTOR_VIEW_XZ, pTrackList, &photonClusters ) );

    PANDORA_MONITORING_API(DrawEvent(DETECTOR_VIEW_XZ, &nonPhotonClusters ) );
    PANDORA_MONITORING_API(DrawEvent(DETECTOR_VIEW_XZ, &photonClusters    ) );

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonIdAlgorithm::ReadSettings(TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithm(*this, xmlHandle, "ClusterFormation", m_clusteringAlgorithmName));

//    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "clusterCandidatesListName", m_clusterCandidatesListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle, "photonClusters", m_photonClusterListName));

    _minimumHitsInCluster = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinimumHitsInCluster", _minimumHitsInCluster));

    // debug printing
    _printing = 1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Printing", _printing));

    // make photon ID likelihood histograms
    _makingPhotonIDLikelihoodHistograms = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MakePhotonIDLikelihoodHistograms", _makingPhotonIDLikelihoodHistograms));

    // max. number of layers
    MAX_NUMBER_OF_LAYERS = 150;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "maxNumberOfLayers", MAX_NUMBER_OF_LAYERS));

    // monitoring filename
    m_monitoringFileName = "photonIdMonitoring.root";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MonitoringFileName", m_monitoringFileName));


    return STATUS_CODE_SUCCESS;
}




//------------------------------------------------------------------------------------------------------------------------------------------

void ECalPhotonIdAlgorithm::CreateOrSaveLikelihoodHistograms(bool create)
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
            PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_sighistrms,  m_monitoringFileName, "UPDATE"));
            PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_backhistrms, m_monitoringFileName, "UPDATE"));
            PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_sighistfrac, m_monitoringFileName, "UPDATE"));
            PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_backhistfrac, m_monitoringFileName, "UPDATE"));
            PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_sighiststart, m_monitoringFileName, "UPDATE"));
            PANDORA_MONITORING_API(SaveAndCloseHistogram(hName_backhiststart, m_monitoringFileName, "UPDATE"));
        }
    }

    if( create )
    {
        PANDORA_MONITORING_API(Create2DHistogram("energyVsPhotonE", "E vs. photonE", 100, 0.0, 100.0, 100, 0.0 ,100.0));
        PANDORA_MONITORING_API(Create2DHistogram("pidVsPhotonEFraction", "pid vs. photonEFraction", 100, 0.0, 1.0, 100, 0.0 ,10.0));
        
    }
    else
    {
        PANDORA_MONITORING_API(SaveAndCloseHistogram("energyVsPhotonE", m_monitoringFileName, "UPDATE" ));
        PANDORA_MONITORING_API(SaveAndCloseHistogram("pidVsPhotonEFraction", m_monitoringFileName, "UPDATE" ));

        PANDORA_MONITORING_API(PrintTree("photonId"));
        PANDORA_MONITORING_API(SaveTree("photonId", m_monitoringFileName, "UPDATE" ));
    }

}




//------------------------------------------------------------------------------------------------------------------------------------------

bool ECalPhotonIdAlgorithm::IsPhoton( Cluster* photonCandidateCluster )
{
    if( _printing ) std::cout << "=============== IsPhoton? ===================" << std::endl;

//     if(photonCandidateCluster==NULL)
//         return false;

    bool returnValue = false;

    // now perform photon ID
    pandora::protoClusterPeaks_t peak;
    if(photonCandidateCluster->GetElectromagneticEnergy()<0.2)
        return false;

    TransverseProfile(photonCandidateCluster, peak,_nEcalLayers); // tweaked: instead of searching for peaks, it only fills the properties of the "peak"(=cluster)

    peak.energy = photonCandidateCluster->GetElectromagneticEnergy();
//     try
//     {
//         const ClusterHelper::ClusterFitResult& fitResult = cluster->GetFitToAllHitsResult();
//         peak.rms = fitResult.GetRms();
//         peaks.push_back( peak );
//     }
//     catch(StatusCodeException &statusCodeException)
//     {
//         std::cout << "fit exception" << std::endl;
//         std::cout << "size: " << cluster->GetNCaloHits() << std::endl;
//         return false;
//     }
//     catch(...)
//     {
//         std::cout << "unkown exception" << std::endl;
//         return false;
//     }


    PhotonIdProperties photonIdProperties;
    try
    {
        PhotonProfileID(photonCandidateCluster, photonIdProperties);
    }
    catch(StatusCodeException &statusCodeException)
    {
        std::cout << "IsPhoton/statusCodeException" << std::endl;
        return false; // it's not a photon then
    }
    catch(...)
    {
        std::cout << "PhotonProfileID/unknown exception" << std::endl;
        throw;
    }

    float showerStart   = photonIdProperties.GetLongProfileShowerStart();
    float gammaFraction = photonIdProperties.GetLongProfileGammaFraction();
    float truePhotonE = GetTrueEnergyContribution(photonCandidateCluster, 22); // get true photon energy contribution
    float trueE       = GetTrueEnergyContribution(photonCandidateCluster);     // get true energy contribution
    float electromagneticE = photonCandidateCluster->GetElectromagneticEnergy();
    float fraction = truePhotonE / trueE;

    ClusterProperties clusterProperties;
    GetClusterProperties( photonCandidateCluster, clusterProperties );

    float closest = 999.;
    float cclosest = 999.;
    float c10closest = 999.;
    float c20closest = 999.;


    // loop through all tracks
    //
    const TrackList& trackList = photonCandidateCluster->GetAssociatedTrackList();
    for( TrackList::iterator itTrack = trackList.begin(), itTrackEnd = trackList.end(); itTrack != itTrackEnd; ++itTrack )
    {
        const TrackState& trackStateAtECal = (*itTrack)->GetTrackStateAtECal();
                
        float longitudinalComponent;
        float approach;

        // ToDo: treat overlap-region barrel endcap as mark does (protocluster: DistanceToTrack)
        CartesianVector hitMean(clusterProperties.hitMean[0],clusterProperties.hitMean[1],clusterProperties.hitMean[2] );
        DistanceToPositionAndDirection(hitMean,
                                       trackStateAtECal.GetPosition(), 
                                       trackStateAtECal.GetMomentum().GetUnitVector(),
                                       longitudinalComponent,
                                       approach  );
        if( approach < closest )
        {
            closest = approach;
            CartesianVector centroid(clusterProperties.centroid[0],clusterProperties.centroid[1],clusterProperties.centroid[2] );
            DistanceToPositionAndDirection(centroid,trackStateAtECal.GetPosition(), trackStateAtECal.GetMomentum().GetUnitVector(),
                                           longitudinalComponent,cclosest  );
            if( cclosest > 999 ) cclosest = 0.0;
            CartesianVector centroid10(clusterProperties.centroid10[0],clusterProperties.centroid10[1],clusterProperties.centroid10[2] );
            DistanceToPositionAndDirection(centroid10,trackStateAtECal.GetPosition(), trackStateAtECal.GetMomentum().GetUnitVector(),
                                           longitudinalComponent,c10closest  );
            if( c10closest> 999 ) c10closest = 0.0;
            CartesianVector centroid20(clusterProperties.centroid20[0],clusterProperties.centroid20[1],clusterProperties.centroid20[2] );
            DistanceToPositionAndDirection(centroid20,trackStateAtECal.GetPosition(), trackStateAtECal.GetMomentum().GetUnitVector(),
                                           longitudinalComponent,c20closest  );
            if( c20closest> 999 ) c20closest = 0.0;
        }
    }

    float dist = std::min(c10closest,c20closest);
    if(cclosest<dist)dist = closest;

    unsigned int nhits = photonCandidateCluster->GetNCaloHits();
    float pid = (PhotonIDLikelihoodCalculator::Instance())->PID( peak.energy, peak.rms, gammaFraction,showerStart);     

    bool accept = false;

    // cuts for case where there is no pointing track
    float pidCut = 0.5;
    //if(closest>10)pidCut=0.45;
    //if(closest>20)pidCut=0.40;
    //if(closest>30)pidCut=0.35;
    //if(closest>40)pidCut=0.30;
    //if(closest>50)pidCut=0.25;

    PANDORA_MONITORING_API(Fill2DHistogram("pidVsPhotonEFraction", pid, fraction ));
    
    

//         float fracE = photonCandidateCluster->GetElectromagneticEnergy()/photonCandidateCluster->GetElectromagneticEnergy();
    if(nhits>=_minimumHitsInCluster && peak.energy>0.2 && pid > pidCut && showerStart<10 && gammaFraction < 1.0 &&peak.rms<5.0 && closest > 2.0){
        accept = true;
    }
    // cluster which is very close to a nearby track
//             if(nhits>=_minimumHitsInCluster && peaks[ipeak].energy>0.2 && pid > 0.5 && showerStart<10 && gammaFraction < 1.0 &&peaks[ipeak].rms<5.0 && closest <= 2.0){
    if(nhits>=_minimumHitsInCluster && peak.energy>0.2 && pid > 0.5 && showerStart<10 && gammaFraction < 1.0 &&peak.rms<5.0 && closest <= 2.0){
        if(dist >  5.0 && pid > 0.9)accept = true;
        if(dist >  7.5 && pid > 0.8)accept = true;
        if(dist > 10.0 && pid > 0.7)accept = true;
    }


    PANDORA_MONITORING_API(SetTreeVariable("photonId", "pid", pid ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "fraction", fraction ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "EtruePhot", truePhotonE ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "Etrue",     trueE ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "Eem", electromagneticE ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "accept", int(accept) ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakRms", peak.rms ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakE", peak.energy ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakE", peak.energy ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakShStart", peak.showerStartDepth ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "peakShDepth", peak.showerDepth90 ));
    PANDORA_MONITORING_API(SetTreeVariable("photonId", "gammaFraction", gammaFraction ));

    PANDORA_MONITORING_API(FillTree("photonId"));


    if(_printing>0&& fraction>0.5){
        std::cout << "fraction " << fraction << " --> should be identified as photon  | true photon E " << truePhotonE << " elm E "  << electromagneticE  << std::endl;
    }


    PANDORA_MONITORING_API(Fill2DHistogram("energyVsPhotonE", electromagneticE, truePhotonE ));


    if(_printing>0&& peak.energy>1.0){
        std::cout << " PEAK  : " <<  peak.du << "," << peak.dv << "  E = " << peak.energy << " dmin : " << peak.dmin <<  " d25/d90 : " << peak.showerDepth25 << "/" << peak.showerDepth90  << " start : " << peak.showerStartDepth << " rms = " << peak.rms << " PhotonID : " << showerStart << " " << gammaFraction << " TRUE FRACTION = " << fraction << " pid " << pid << " d = " << closest << " c= " << dist;
        if(fraction<0.5 &&  accept)std::cout << " <---A*****************";
        if(fraction>0.5 && !accept) std::cout << " <---B*****************";
        std::cout << std::endl;    
    }

    //********** code to make the likelihood histograms ************
    if(_makingPhotonIDLikelihoodHistograms){
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
        if(ihist>=0 && gammaFraction<1.0 && showerStart<10. && peak.rms<5.){
            const std::string iHist(TypeToString(ihist));
            if(fraction>0.5){
                const std::string hName_sighistrms("sighistrms" + iHist);
                const std::string hName_sighistfrac("sighistfrac" + iHist);
                const std::string hName_sighiststart("sighiststart" + iHist);
                PANDORA_MONITORING_API(Fill1DHistogram(hName_sighistrms, peak.rms));
                PANDORA_MONITORING_API(Fill1DHistogram(hName_sighistfrac, gammaFraction));
                PANDORA_MONITORING_API(Fill1DHistogram(hName_sighiststart, showerStart));
            }else{
                const std::string hName_backhistrms("backhistrms" + iHist);
                const std::string hName_backhistfrac("backhistfrac" + iHist);
                const std::string hName_backhiststart("backhiststart" + iHist);
                PANDORA_MONITORING_API(Fill1DHistogram(hName_backhistrms, peak.rms));
                PANDORA_MONITORING_API(Fill1DHistogram(hName_backhistfrac, gammaFraction));
                PANDORA_MONITORING_API(Fill1DHistogram(hName_backhiststart, showerStart));
            }
        }
    }

    if(accept){
        // changed: change returnValue to true
        std::cout << "accepted as photon" << std::endl;
        returnValue = true;

    }



    return returnValue;
}



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ECalPhotonIdAlgorithm::TransverseProfile(const Cluster* cluster, protoClusterPeaks_t &peak, int maxLayers){

    int nbins   = 41;
    int ioffset = nbins/2; 

//  int ifirst = 0; // this->FirstLayer(); // ???
    int ifirst = (int)cluster->GetInnerPseudoLayer();
  

    CartesianVector centroid = cluster->GetCentroid(ifirst);
    float x0   = centroid.GetX();
    float y0   = centroid.GetY();
    float z0   = centroid.GetZ();
    float r0   = sqrt(x0*x0+y0*y0+z0*z0);
    float ux   = x0/r0;
    float uy   = y0/r0;
    float uz   = z0/r0;
    float utx  = uy/sqrt(ux*ux+uy*uy);
    float uty  = -ux/sqrt(ux*ux+uy*uy);
    float utz  = 0;
    float vtx=0;
    float vty=0;
    float vtz=0;
    if(utx!=0&&uz!=0){
        vtx  = -uty/utx;
        vtz  = (uty*ux-uy*utx)/utx/uz;
        vty  = 1;
        float v    = sqrt(vtx*vtx+vty*vty+vtz*vtz);
        vtx = vtx/v;
        vty = vty/v;
        vtz = vtz/v;
    }
    if(utx==0&&uz!=0){
        vtx  = 1;
        vtz  = ux/uz;
        vty  = 0;
        float v    = sqrt(vtx*vtx+vty*vty+vtz*vtz);
        vtx = vtx/v;
        vty = vty/v;
        vtz = vtz/v;
    }
    if(utx==0&&uz==0){
        vtx  = 0;
        vtz  = 1;
        vty  = 0;
    }

    int  done[nbins][nbins];
    bool assigned[nbins][nbins];
    float tprofile[nbins][nbins];
    float tlprofile[nbins][nbins][MAX_NUMBER_OF_LAYERS];
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
//              pixelsize  = hiti->getHitSizeZ();
                pixelsize  = caloHit->GetCellSizeV(); // in barrel: perpendicular to beam and pixel thickness; in endcap: perp. to thickness and up
            }
            const CartesianVector& position = caloHit->GetPositionVector();
            float dx = (position.GetX() -x0)/pixelsize;
            float dy = (position.GetY() -y0)/pixelsize;
            float dz = (position.GetZ() -z0)/pixelsize;
            float dut = dx*utx+dy*uty+dz*utz;
            float dvt = dx*vtx+dy*vty+dz*vtz;
            int idut  = static_cast<int>(dut+0.5+ioffset);
            int idvt  = static_cast<int>(dvt+0.5+ioffset);
            if(idut>=0&&idut<nbins&&idvt>=0&&idvt<nbins){
                tprofile[idut][idvt] += caloHit->GetElectromagneticEnergy();  
                tlprofile[idut][idvt][i] += caloHit->GetElectromagneticEnergy();  
            }
        }
    }

//   bool first = true;
//   float pixelsize = 10.; 
//   for(int i=0; i<maxLayers; i++){
//     for(int ihit =0; ihit < this->hitsInLayer(i);++ihit){
//       MyCaloHitExtended* hiti = this->hitInLayer(i,ihit);
//       if(first){
// 	pixelsize  = hiti->getHitSizeZ();
//       }
//       float dx = (hiti->getPosition()[0] -x0)/pixelsize;
//       float dy = (hiti->getPosition()[1] -y0)/pixelsize;
//       float dz = (hiti->getPosition()[2] -z0)/pixelsize;
//       float dut = dx*utx+dy*uty+dz*utz;
//       float dvt = dx*vtx+dy*vty+dz*vtz;
//       int idut  = static_cast<int>(dut+0.5+ioffset);
//       int idvt  = static_cast<int>(dvt+0.5+ioffset);
//       if(idut>=0&&idut<nbins&&idvt>=0&&idvt<nbins){
// 	tprofile[idut][idvt] += hiti->getEnergyEM();  
// 	tlprofile[idut][idvt][i] += hiti->getEnergyEM();  
//       }
//     }
//   }


    // mask low ph region
    float threshold = 0.025;
    for(int i=0; i<nbins; i++){
        for(int j=0; j<nbins; j++){
            if(tprofile[i][j]<threshold)assigned[i][j]=true;
        }
    }
    // fill peak values
    // 
    float dmin=9999.;

    float peakheight = 0.;
    int   ipeak =0;
    int   jpeak =0;
    float peakenergy=0;
    float xbar=0;
    float ybar=0;
    float xxbar=0;
    float yybar=0;
    float longitudinalProfile[MAX_NUMBER_OF_LAYERS];
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
    dmin = sqrt(   (ipeak-ioffset)*(ipeak-ioffset)+
                   (jpeak-ioffset)*(jpeak-ioffset));
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
                                    float d = sqrt((is-ioffset)*(is-ioffset)+(js-ioffset)*(js-ioffset));
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

//}

//    if(npeaks>3)stillfindingpeaks=false;

// NOTE:
// select the first peak only --> differs to marks code

//     if(npeaks>0)stillfindingpeaks=false;


//   }


return STATUS_CODE_SUCCESS;

}








//------------------------------------------------------------------------------------------------------------------------------------------


void ECalPhotonIdAlgorithm::PhotonProfileID( Cluster* cluster, PhotonIdProperties& photonIdProperties, bool truncate){

    float X0BinSize = 0.5;
    const unsigned int X0Bins = 100;
//    float X0Max     = X0Bins*X0BinSize;
    float X0Profile[X0Bins];
    float expectedX0Profile[X0Bins];
    float eProfile[MAX_NUMBER_OF_LAYERS];
    for(unsigned int i=0;i<MAX_NUMBER_OF_LAYERS;i++)eProfile[i]=0.;
    for(unsigned int i=0;i<X0Bins;i++)X0Profile[i]=0.;
    for(unsigned int i=0;i<X0Bins;i++)expectedX0Profile[i]=0.;


    // it should be able to make this more general and easier with the new code
    std::cout << "in photon profile id" << std::endl;

    float radLenLayers[MAX_NUMBER_OF_LAYERS]; // radiation lengths per layer seen by the cluster (in the direction of the cluster
    try
    {
        CartesianVector clusterDirection = cluster->GetFitToAllHitsResult().GetDirection().GetUnitVector();

    // --- loop through all the hits and do stuff mark has put into AddHit 
    unsigned int maxSetIndex = 0;
    float lastRadLen =0.1;

    OrderedCaloHitList pOrderedCaloHitList = cluster->GetOrderedCaloHitList();
    for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList.begin(), itLyrEnd = pOrderedCaloHitList.end(); itLyr != itLyrEnd; itLyr++ )
    {
        unsigned int pseudoLayer = itLyr->first;

        CaloHitList::iterator itCaloHit    = itLyr->second->begin();
        CaloHitList::iterator itCaloHitEnd = itLyr->second->end();

        int n = 0; // count number of cells in layer
        
        radLenLayers[pseudoLayer] = 0.0; // initialisation
        eProfile[pseudoLayer] = 0.0; // initialisation
        for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
        {
            ++n;
            eProfile[pseudoLayer] += (*itCaloHit)->GetElectromagneticEnergy(); // sum up the energies in each pseudo-layer

            // radiationlengths
            float angleClusterDirectionWrtPseudoLayerNormalVector = (*itCaloHit)->GetNormalVector().GetUnitVector().GetOpeningAngle( clusterDirection );
            float cosOpeningAngle = fabs( cos( angleClusterDirectionWrtPseudoLayerNormalVector ) );

            if( cosOpeningAngle < 0.3 )
            {
                std::cout << "Cluster has large angle to pseudo layer normal vector (cos = " << cosOpeningAngle << ", which is an angle of " 
                          << angleClusterDirectionWrtPseudoLayerNormalVector << ") --> reset cos(angle) to 1.0" << std::endl;
                cosOpeningAngle = 1.0;
            }
            radLenLayers[pseudoLayer] += (*itCaloHit)->GetNRadiationLengths()/cosOpeningAngle; // gives the radiation lengths per layer in the clusterDirection

            //            std::cout << "radlen " <<  (*itCaloHit)->GetNRadiationLengths() << "  normalvect " << (*itCaloHit)->GetNormalVector().GetUnitVector() << "  clusterdir " << clusterDirection << std::endl;
        }
        if( n>0 ) radLenLayers[pseudoLayer] /= n;

        // set lengths in units of radiation length for all pseudo layers before the given hit (if not set already)
        for( ; maxSetIndex < pseudoLayer; maxSetIndex++ )
        {
            radLenLayers[maxSetIndex] = radLenLayers[pseudoLayer];
        }
        maxSetIndex = pseudoLayer +1;

//         std::cout << "radlenlayers[] " << radLenLayers[pseudoLayer] << " pseudo layer " << pseudoLayer << " n " << n << " eprofile[] " << eProfile[pseudoLayer] << std::endl;
        lastRadLen = radLenLayers[pseudoLayer];

    }    
    for( ; maxSetIndex <= _nEcalLayers; maxSetIndex++ )
    {
        radLenLayers[maxSetIndex] = lastRadLen;
    }



    // ---

    float x0 = 3.5;                     // oops: hardcoded radiation length (W)
    float E0 = cluster->GetElectromagneticEnergy();
    // Effective critical energy
    float EC = 0.08;                    // oops: critical energy hardcoded (  (thickness-Si*40.19MeV+thickness-W*93.11MeV)/sumThickness in GeV )
    double a = 1.25+0.5*log(E0/EC);
    double lngammaa = lgamma(a);
    double gammaa = exp(lngammaa);

    float dist=0.;
    int   X0ProfileEnd=0;
    // step through ECAL making profile in radiation lengths
    float ecalE = 0.;
    int maxLayer = _nEcalLayers;
    if(truncate)maxLayer = 35;

    for(unsigned int i=1; i<=_nEcalLayers; i++){
        ecalE += eProfile[i];
//         float dr = sep[i]/cost[i];
        float dr = radLenLayers[i];
        float dt = dr/x0;
        //        float dt = radLenLayers[i];
        float distStart = dist;
        dist+= dt;
        float deltaX0Bins = dt/X0BinSize;
        float bStart = distStart/X0BinSize;
        float bEnd   = dist/X0BinSize;
    
//         std::cout << "=== dr " << dr << " dt " << dt << " distStart " << distStart << " dist " << dist << " X0BinSize " << X0BinSize << " bEnd " << bEnd << " layer " << i << std::endl;

        if(bEnd>100)bEnd=100.;
        unsigned int iStart = static_cast<unsigned int>(bStart);
        unsigned int iEnd = static_cast<unsigned int>(bEnd);
        float deltaStart = bStart-iStart;
        float deltaEnd   = 1.0-bEnd+iEnd;
        if(iStart<=100){
            for(unsigned int ibin=iStart;ibin<=iEnd;++ibin){
                float delta = 1.;
                if(ibin==iStart)delta = delta - deltaStart;
                if(ibin==iEnd)delta   = delta - deltaEnd;
                float frac = delta/deltaX0Bins;
                if(ibin<100)X0Profile[ibin] += eProfile[i]*frac; 
//                 std::cout << "=== === X0Profile[ibin="<<ibin<<"] " << X0Profile[ibin] << " eProfile[i="<<i<<"] " << eProfile[i] << "  frac " << frac << std::endl;
            }
        }
        X0ProfileEnd = iEnd;
        if(X0ProfileEnd>=100)X0ProfileEnd=100;
    }
    if(truncate)E0=ecalE;
    float t =0.;
    for(int i=0; i<100; i++){
        t+= X0BinSize;
        float de = E0/2.0*pow(t/2,a-1)*exp(-t/2)*X0BinSize/gammaa;
        expectedX0Profile[i] = de;
    }

    float diffEmin = 9999.;
    int   ioffmin  = 0;
    bool  stillgoing = true;
    for(int ioffset=0; stillgoing && ioffset<static_cast<int>(_nEcalLayers); ioffset++){
        float diffE = 0.;
        for(int i=0; i<X0ProfileEnd; i++){
            if(i-ioffset<0){
                diffE += X0Profile[i];
            }else{
                diffE += fabs(expectedX0Profile[i-ioffset]-X0Profile[i]);
            }

//             if( i < X0ProfileEnd )
//                 std::cout << "=== === X0Profile[i="<<i<<"] " << X0Profile[i] << "     expectedX0Profile[i="<<i<<"] " << expectedX0Profile[i]  << "      diffE " << diffE << std::endl;

        }
        if(diffE<diffEmin){
            diffEmin = diffE;
            ioffmin = ioffset;
        }
        if(diffE-diffEmin>0.1)stillgoing = false;
    }
  
    if(ecalE>0){
        photonIdProperties._photonLongProfileFraction =  diffEmin/ecalE;
        photonIdProperties._photonLongShowerStart     =  ioffmin*X0BinSize ;
//         std::cout << "=== longprofilefraction   " << photonIdProperties._photonLongProfileFraction << std::endl;
//         std::cout << "=== photonLongShowerStart " << photonIdProperties._photonLongShowerStart << std::endl;
    }
    else
    {
        std::cout << "ecalE <= 0 " << std::endl;
    }
    }
    catch(StatusCodeException &statusCodeException)
    {
        std::cout << "PhotonProfileID/statusCodeException" << std::endl;
        throw;
    }
    catch(...)
    {
        std::cout << "PhotonProfileID/unknown exception" << std::endl;
        throw;
    }

}






//------------------------------------------------------------------------------------------------------------------------------------------


double ECalPhotonIdAlgorithm::GetTrueEnergyContribution(const Cluster* cluster, int pid )
{

    #define PHOTONID 22

    typedef std::set< MCParticle* > MCPARTICLESET;
    typedef std::map< int, double > ENERGYPIDMAP;
    typedef ENERGYPIDMAP::iterator ENERGYPIDMAPITERATOR;
    ENERGYPIDMAP energyPerMCParticleId;
    ENERGYPIDMAPITERATOR itEnergyPerMCParticleId;
    MCPARTICLESET countedMcParticles;

    float electromagneticEnergy = 0.0;
    float inputEnergy = 0.0;
    float trueEnergy = 0.0;
        
    OrderedCaloHitList pOrderedCaloHitList = cluster->GetOrderedCaloHitList();

    for( OrderedCaloHitList::const_iterator itLyr = pOrderedCaloHitList.begin(), itLyrEnd = pOrderedCaloHitList.end(); itLyr != itLyrEnd; itLyr++ )
    {
        // int pseudoLayer = itLyr->first;
        CaloHitList::iterator itCaloHit    = itLyr->second->begin();
        CaloHitList::iterator itCaloHitEnd = itLyr->second->end();

        for( ; itCaloHit != itCaloHitEnd; itCaloHit++ )
        {
            MCParticle* mc = NULL; 
            (*itCaloHit)->GetMCParticle( mc );
            if( mc == NULL ) continue; // has to be continue, since sometimes some CalorimeterHits don't have a MCParticle (e.g. noise)

            if( countedMcParticles.find( mc ) == countedMcParticles.end() ) 
            {
                int particleId = mc->GetParticleId();
                double energy  = mc->GetEnergy();
                itEnergyPerMCParticleId = energyPerMCParticleId.find( particleId );
                if( itEnergyPerMCParticleId == energyPerMCParticleId.end() )
                {
                    energyPerMCParticleId.insert( std::make_pair<int,double>( particleId, energy ) );
                }
                else
                {
                    itEnergyPerMCParticleId->second += energy;
                }
                trueEnergy += mc->GetEnergy();
                countedMcParticles.insert( mc );
            }
            electromagneticEnergy += (*itCaloHit)->GetElectromagneticEnergy();
            inputEnergy += (*itCaloHit)->GetInputEnergy();
        }
    }

    float energySum = 0.0;
    for( ENERGYPIDMAP::iterator it = energyPerMCParticleId.begin(), itEnd = energyPerMCParticleId.end(); it != itEnd; ++it )
    {
        std::cout << "pid " << it->first 
                  << "  e-contrib " << it->second 
                  << "  of electromagnetic energy: " << electromagneticEnergy 
                  << "  and input energy: " << inputEnergy 
                  << " true E: " << trueEnergy << std::endl;
        energySum += it->second;
    }
    std::cout << "energy-sum : " << energySum << std::endl;

    if( pid == 0 )
        return energySum;

    itEnergyPerMCParticleId = energyPerMCParticleId.find( pid );
    if( itEnergyPerMCParticleId == energyPerMCParticleId.end() )
        return 0.0;
    return itEnergyPerMCParticleId->second; // return the energy contribution of photons
}



//------------------------------------------------------------------------------------------------------------------------------------------


void ECalPhotonIdAlgorithm::GetClusterProperties(const Cluster* cluster, ClusterProperties& clusterProperties )
{
    RunningMeanRMS hitMean[3];    // [3] for the coordinates x,y and z
    RunningMeanRMS centroid[3];
    RunningMeanRMS centroid10[3]; // centroid of the first 10 layers
    RunningMeanRMS centroid20[3]; // centroid of the first 20 layers

    float centroidEnergy = 0.0;
    float centroid10Energy = 0.0;
    float centroid20Energy = 0.0;


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
        clusterProperties.hitMean[i]    = hitMean[i].GetMean();
        clusterProperties.centroid[i]   = centroid[i].GetMean();
        clusterProperties.centroid10[i] = centroid10[i].GetMean();
        clusterProperties.centroid20[i] = centroid20[i].GetMean();
    }
    clusterProperties.centroidEnergy   = centroidEnergy;
    clusterProperties.centroid10Energy = centroid10Energy;
    clusterProperties.centroid20Energy = centroid20Energy;
}



//------------------------------------------------------------------------------------------------------------------------------------------


void ECalPhotonIdAlgorithm::DistanceToPositionAndDirection(const CartesianVector& position, 
                                                                     const CartesianVector& referencePosition,
                                                                     const CartesianVector& referenceDirection,
                                                                     float& longitudinalComponent,
                                                                     float& perpendicularComponent )
{
    const CartesianVector  relativePosition = position - referencePosition;
    longitudinalComponent  = referenceDirection.GetUnitVector().GetDotProduct( relativePosition );
    perpendicularComponent = (relativePosition - referenceDirection*longitudinalComponent).GetMagnitude();
    
}



//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonIDLikelihoodCalculator::PID(float E, float rms, float frac, float start){

    float pid = 0;

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

    if(1==1){
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
    float yes = likeSig[ien]*likesrms[ien][irmsbin]*likesfrac[ien][ifracbin]*likesstart[ien][istartbin];
    float no  = likeBack[ien]*likebrms[ien][irmsbin]*likebfrac[ien][ifracbin]*likebstart[ien][istartbin];
    
    if( 1 == 1 )
    {
        std::cout << "yes " << yes << std::endl;
        std::cout << "no  " << no << std::endl;
    }

    if(yes+no>0)pid = yes/(yes+no);

    return pid;
}
