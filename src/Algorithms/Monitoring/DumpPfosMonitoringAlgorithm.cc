/**
 *  @file   PandoraPFANew/src/Algorithms/Monitoring/DumpPfosMonitoringAlgorithm.cc
 * 
 *  @brief  Implementation of the energy monitoring algorithm class 
 * 
 *  $Log: $
 */

#include "Algorithms/Monitoring/DumpPfosMonitoringAlgorithm.h"
#include "Pandora/AlgorithmHeaders.h"
#include "Pandora/PdgTable.h"

#include <vector>
#include <iostream>
#include <iomanip>

using namespace pandora;
const int precision = 2;
const int width = 8;
const int widthFloat = 7;
const int widthSmallFloat = 5;
const int widthInt = 5;
const int widthSmallInt = 2;
const int widthInt4 = 4;
const int widthFlag     = 2;


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::Run()
{

    m_trackMcPfoTargets.clear();
    m_trackToErrorTypeMap.clear();
    m_mcParticleToTrackMap.clear();
    m_trackRecoAsTrackEnergy = 0;
    m_trackRecoAsPhotonEnergy = 0;
    m_trackRecoAsNeutralEnergy = 0;
    m_photonRecoAsTrackEnergy = 0;
    m_photonRecoAsPhotonEnergy = 0;
    m_photonRecoAsNeutralEnergy = 0;
    m_neutralRecoAsTrackEnergy = 0;
    m_neutralRecoAsPhotonEnergy = 0;
    m_neutralRecoAsNeutralEnergy = 0;
    m_firstChargedPfoToPrint = true;
    m_firstNeutralPfoToPrint = true;
    m_firstPhotonPfoToPrint = true;

    
    std::cout << std::fixed;
    std::cout << std::setprecision(precision);
    
    const ParticleFlowObjectList *pPfoList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pPfoList));
    
#define FORMATTEDOUTPUTPFOHEADER(N1,E2)				   \
    std::cout  <<						   \
      std::right << std::setw(widthInt)      <<    N1        <<	   \
      std::right << std::setw(widthFloat)    <<    E2
    
#define FORMATTEDOUTPUTPFOHEADERTITLE()				      \
    std::cout  <<						      \
      std::right << std::setw(widthInt)      <<    "Pfo"        <<    \
      std::right << std::setw(widthFloat)    <<    "EPfo"
    
#define FORMATTEDPFOPADDING(TITLE1,TITLE2)			       \
    std::cout  <<						       \
      std::right << std::setw(widthInt)      <<    TITLE1        <<    \
      std::right << std::setw(widthFloat)    <<    TITLE2 
    
#define FORMATTEDOUTPUTCONFUSION(E1,E2,E3,E4,E5,E6,E7,E8,E9)	     \
    std::cout  <<						     \
      std::right << std::setw(16) << " Generated as   " <<		\
      std::right << std::setw(widthFloat)    <<    "track"        <<	\
      std::right << std::setw(widthFloat)    <<    "gamma"        <<	\
      std::right << std::setw(widthFloat)    <<    "hadron"       <<  std::endl << \
      std::right << std::setw(16) << "Reco as track : " <<		\
      std::right << std::setw(widthFloat)    <<    E1        <<		\
      std::right << std::setw(widthFloat)    <<    E2        <<		\
      std::right << std::setw(widthFloat)    <<    E3        <<  std::endl << \
      std::right << std::setw(16) <<  "Reco as gamma : " <<		\
      std::right << std::setw(widthFloat)    <<    E4        <<		\
      std::right << std::setw(widthFloat)    <<    E5        <<		\
      std::right << std::setw(widthFloat)    <<    E6        << std::endl << \
      std::right << std::setw(16) <<  "Reco as hadron: " <<		\
      std::right << std::setw(widthFloat)    <<    E7        <<		\
      std::right << std::setw(widthFloat)    <<    E8        <<		\
      std::right << std::setw(widthFloat)    <<    E9        << std::endl;
   
    
    
#define FORMATTEDOUTPUTTRACKTITLE()				      \
    std::cout  <<						      \
      std::right << std::setw(widthInt)      <<    "Track"      <<    \
      std::right << std::setw(widthFlag)     <<    "("       <<	      \
      std::right << std::setw(widthInt)      <<    "mc"         <<    \
      std::left  << std::setw(widthFlag)     <<    ")"       <<	      \
      std::right << std::setw(widthFlag)     <<    "C" <<  \
      std::right << std::setw(widthFlag)     <<    "R" <<  \
      std::right << std::setw(widthFloat)    <<    "Mom"        <<    \
      std::right << std::setw(widthFlag)     <<    "("       <<	      \
      std::right << std::setw(widthFloat)    <<    "mc"         <<    \
      std::left  << std::setw(widthFlag)     <<    ")"       <<	      \
      std::right << std::setw(widthFloat)    <<    "Eclust"     <<    \
      std::right << std::setw(widthFloat)    <<    "chi"        <<    \
      std::right << std::setw(widthFlag)     <<    "L" <<  \
      std::right << std::setw(widthSmallFloat)    <<    "fC"        <<	\
      std::right << std::setw(widthSmallFloat)    <<    "fP"        <<	\
      std::right << std::setw(widthSmallFloat)    <<    "fN"        <<  \
      std::endl
    
#define FORMATTEDOUTPUTTRACK(N1,N2,FLAG1,FLAG2,E1,E2,E3,E4,FLAG3,E5,E6,E7) \
    std::cout  <<						   \
      std::right << std::setw(widthInt)      <<    N1        <<	   \
      std::right << std::setw(widthFlag)     <<    "("       <<    \
      std::right << std::setw(widthInt)      <<    N2        <<    \
      std::left  << std::setw(widthFlag)     <<    ")"       <<    \
      std::right << std::setw(widthFlag)     <<    FLAG1     <<	   \
      std::right << std::setw(widthFlag)     <<    FLAG2     <<	   \
      std::right << std::setw(widthFloat)    <<    E1        <<	   \
      std::right << std::setw(widthFlag)     <<    "("       <<    \
      std::right << std::setw(widthFloat)    <<    E2        <<    \
      std::left  << std::setw(widthFlag)     <<    ")"       <<    \
      std::right << std::setw(widthFloat)    <<    E3        <<	    \
      std::right << std::setw(widthFloat)    <<    E4        <<		\
      std::right << std::setw(widthFlag)     <<    FLAG3	<<	\
      std::right << std::setw(widthSmallFloat)    <<    E5        <<	\
      std::right << std::setw(widthSmallFloat)    <<    E6        <<	\
      std::right << std::setw(widthSmallFloat)    <<    E7
    
#define FORMATTEDOUTPUTNEUTRAL(E1,E2,E3,E4,N1,N2,E5,E6)			\
    std::cout  <<							\
      std::right << std::setw(widthFloat)         <<    E1        <<	\
      std::right << std::setw(widthInt4)          <<    "     "   <<	\
      std::right << std::setw(widthSmallFloat)    <<    E2        <<	\
      std::right << std::setw(widthSmallFloat)    <<    E3        <<	\
      std::right << std::setw(widthSmallFloat)    <<    E4        <<    \
      std::right << std::setw(widthInt4)          <<    N1        <<    \
      std::left  << std::setw(widthFlag)          <<    "-"       <<    \
      std::left  << std::setw(widthInt4)          <<    N2        <<    \
      std::right << std::setw(widthFloat)          <<   E5        <<    \
      std::right << std::setw(widthFloat)          <<   E6  

    
#define FORMATTEDOUTPUTNEUTRALTITLE()					\
    std::cout  <<							\
      std::right << std::setw(widthFloat     )    <<    "Eclust"    <<	\
      std::right << std::setw(widthInt4)          <<    "     "     <<	\
      std::right << std::setw(widthSmallFloat)    <<    "fC"        <<	\
      std::right << std::setw(widthSmallFloat)    <<    "fP"        <<	\
      std::right << std::setw(widthSmallFloat)    <<    "fN"        <<	\
      std::left  << std::setw(widthInt4+widthFlag+widthInt4) <<  " Layers " << \
      std::right << std::setw(widthFloat)          <<   "sStart"    <<	\
      std::right << std::setw(widthFloat)          <<   "sDisc"     <<	\
      std::endl
    
      
    ParticleFlowObjectList chargedPfos;
    ParticleFlowObjectList photonPfos;
    ParticleFlowObjectList neutralHadronPfos;
    
    float totalPfoEnergy(0.);
    // First loop over pfos to make collections and save track mc pfo list
    for (ParticleFlowObjectList::const_iterator pfoIter = pPfoList->begin(); pfoIter != pPfoList->end(); ++pfoIter)
      {
        ParticleFlowObject *pPfo = *pfoIter;
	totalPfoEnergy+=pPfo->GetEnergy();
	const int pfoPid    = pPfo->GetParticleId();
        const TrackList   &trackList(pPfo->GetTrackList());
	if(trackList.size()>0)chargedPfos.insert(pPfo);
	if(trackList.size()==0)(pfoPid == 22) ? photonPfos.insert(pPfo) : neutralHadronPfos.insert(pPfo);
	for (TrackList::const_iterator trackIter = trackList.begin(); trackIter != trackList.end(); ++trackIter)
          {
	    const Track *pTrack = *trackIter;
	    const MCParticle* pMcParticle(NULL); 
	    pTrack->GetMCParticle(pMcParticle);
	    if(pMcParticle!=NULL){
	      m_trackMcPfoTargets.insert(pMcParticle);

	      const TrackList daughterTracks = pTrack->GetDaughterTrackList();
	      if(daughterTracks.size()==0){
		std::map<const MCParticle*,const Track*>::iterator it = m_mcParticleToTrackMap.find(pMcParticle);
		if(it==m_mcParticleToTrackMap.end())
		  {
		    m_mcParticleToTrackMap.insert(std::map<const MCParticle*,const Track*>::value_type(pMcParticle,pTrack));
		  }
		else
		  {
		    const TrackList siblingTracks = pTrack->GetSiblingTrackList();
		    if(siblingTracks.size()==0){
		      switch(pMcParticle->GetParticleId()){
		      case 22:
			m_trackToErrorTypeMap.insert(std::map<const Track*,TrackErrorTypes_t>::value_type((*it).second,MISSED_CONVERSION));
			m_trackToErrorTypeMap.insert(std::map<const Track*,TrackErrorTypes_t>::value_type(pTrack,MISSED_CONVERSION));
			std::cout << " Track appears twice in list - conversion " << pMcParticle->GetEnergy() << std::endl;
			break;	
		      case 310:
			m_trackToErrorTypeMap.insert(std::map<const Track*,TrackErrorTypes_t>::value_type((*it).second,MISSED_KSHORT));
			m_trackToErrorTypeMap.insert(std::map<const Track*,TrackErrorTypes_t>::value_type(pTrack,MISSED_KSHORT));
			std::cout << " Track appears twice in list - ks " << pMcParticle->GetEnergy() << std::endl;
			break;
		      default:
			m_trackToErrorTypeMap.insert(std::map<const Track*,TrackErrorTypes_t>::value_type((*it).second,SPLIT_TRACK));
			m_trackToErrorTypeMap.insert(std::map<const Track*,TrackErrorTypes_t>::value_type(pTrack,SPLIT_TRACK));
			std::cout << " Track appears twice in list - split " << pMcParticle->GetEnergy() << std::endl;
			break;
		      }
		    }
		  }

	      }
	    }	
	    
	  }
      }	
    if(totalPfoEnergy>m_totalPfoEnergyDisplayLessThan && totalPfoEnergy<m_totalPfoEnergyDisplayGreaterThan)return STATUS_CODE_SUCCESS;
    
    
    std::cout << " <---------------------------------DumpPfosMonitoringAlgorithm----------------------------------------->" <<  std::endl;
    std::cout << " Total RECO PFO Energy = " << totalPfoEnergy << std::endl;
    
    // now order the lists
    
    std::vector<ParticleFlowObject*> sortedChargedPfos(chargedPfos.begin(), chargedPfos.end());
    std::vector<ParticleFlowObject*> sortedPhotonPfos(photonPfos.begin(), photonPfos.end());
    std::vector<ParticleFlowObject*> sortedNeutralHadronPfos(neutralHadronPfos.begin(), neutralHadronPfos.end());
    std::sort(sortedChargedPfos.begin(), sortedChargedPfos.end(), ParticleFlowObject::SortByEnergy);
    std::sort(sortedPhotonPfos.begin(),  sortedPhotonPfos.end(), ParticleFlowObject::SortByEnergy);
    std::sort(sortedNeutralHadronPfos.begin(), sortedNeutralHadronPfos.end(), ParticleFlowObject::SortByEnergy);

    for (std::vector<ParticleFlowObject*>::const_iterator pfoIter = sortedChargedPfos.begin(); pfoIter != sortedChargedPfos.end(); ++pfoIter)
      {
        const ParticleFlowObject *pPfo = *pfoIter;
	DumpChargedPfo(pPfo);
      }



    // Second loop to dump photon pfos
    for (std::vector<ParticleFlowObject*>::const_iterator pfoIter = sortedPhotonPfos.begin(); pfoIter != sortedPhotonPfos.end(); ++pfoIter)
      {
        ParticleFlowObject *pPfo = *pfoIter;
	DumpPhotonPfo(pPfo);
      }


    // Second loop to dump neutral hadron pfos
    for (std::vector<ParticleFlowObject*>::const_iterator pfoIter = sortedNeutralHadronPfos.begin(); pfoIter != sortedNeutralHadronPfos.end(); ++pfoIter)
      {
        ParticleFlowObject *pPfo = *pfoIter;
	DumpNeutralPfo(pPfo);
      }

    const float confmat[3][3] = {
      {m_trackRecoAsTrackEnergy,m_photonRecoAsTrackEnergy,m_neutralRecoAsTrackEnergy},    
      {m_trackRecoAsPhotonEnergy,m_photonRecoAsPhotonEnergy,m_neutralRecoAsPhotonEnergy},
      {m_trackRecoAsNeutralEnergy,m_photonRecoAsNeutralEnergy,m_neutralRecoAsNeutralEnergy}
    };

    const float sumCal = m_trackRecoAsTrackEnergy   + m_photonRecoAsTrackEnergy   + m_neutralRecoAsTrackEnergy  +
		   m_trackRecoAsPhotonEnergy  + m_photonRecoAsPhotonEnergy  + m_neutralRecoAsPhotonEnergy +
		   m_trackRecoAsNeutralEnergy + m_photonRecoAsNeutralEnergy + m_neutralRecoAsNeutralEnergy;
    const float nSumCal=sumCal/100.;

    const float econfmat[3][3] = {
      {m_trackRecoAsTrackEnergy/nSumCal,m_photonRecoAsTrackEnergy/nSumCal,m_neutralRecoAsTrackEnergy/nSumCal},    
      {m_trackRecoAsPhotonEnergy/nSumCal,m_photonRecoAsPhotonEnergy/nSumCal,m_neutralRecoAsPhotonEnergy/nSumCal},
      {m_trackRecoAsNeutralEnergy/nSumCal,m_photonRecoAsNeutralEnergy/nSumCal,m_neutralRecoAsNeutralEnergy/nSumCal}
    };
	
    std::cout << std::endl;
    FORMATTEDOUTPUTCONFUSION(confmat[0][0],confmat[0][1],confmat[0][2],confmat[1][0],confmat[1][1],confmat[1][2],confmat[2][0],confmat[2][1],confmat[2][2]);

    //    std::cout << endl;
    // FORMATTEDOUTPUTCONFUSION(econfmat[0][0],econfmat[0][1],econfmat[0][2],econfmat[1][0],econfmat[1][1],econfmat[1][2],econfmat[2][0],econfmat[2][1],econfmat[2][2]);

    return STATUS_CODE_SUCCESS;

}

//------------------------------------------------------------------------------------------------------------------------------------------


StatusCode DumpPfosMonitoringAlgorithm::DumpChargedPfo(const ParticleFlowObject* pPfo)
{

  const TrackList   &trackList(pPfo->GetTrackList());
  const int pfoPid    = pPfo->GetParticleId();
  const float pfoEnergy = pPfo->GetEnergy();
  bool printedHeader(false);
  bool printThisPfo(false);

  for (TrackList::const_iterator trackIter = trackList.begin(); trackIter != trackList.end(); ++trackIter)
    {
      Track *pTrack = *trackIter;
      TrackErrorTypes_t trackStatus = OK; 
      std::map<const Track*,TrackErrorTypes_t>::iterator it = m_trackToErrorTypeMap.find(pTrack);
      if(it!=m_trackToErrorTypeMap.end())trackStatus = (*it).second;
      
      const float trackEnergy(pTrack->GetEnergyAtDca());
      const int   trackId = pTrack->GetParticleId();
      float clusterEnergy(0);
      Cluster* pCluster(NULL);
      if(pTrack->HasAssociatedCluster()){
	pTrack->GetAssociatedCluster(pCluster);
	clusterEnergy += pCluster->GetTrackComparisonEnergy();
      }	      	    
      const MCParticle* pMcParticle(NULL);
      pTrack->GetMCParticle(pMcParticle);
      int mcId(0);
      float mcEnergy(0.);
      if(pMcParticle!=NULL)
	{
	  mcId = pMcParticle->GetParticleId();
	  mcEnergy = pMcParticle->GetEnergy();
	}
      
 
      std::string leaving(" ");
      bool isLeaving = (pCluster!=NULL && ClusterHelper::IsClusterLeavingDetector(pCluster));
      if(isLeaving)leaving= "L";

      const float chi(ReclusterHelper::GetTrackClusterCompatibility(clusterEnergy, trackEnergy));

      const TrackList &daughterTrackList(pTrack->GetDaughterTrackList());
      bool isParent = (daughterTrackList.size()>0);
      bool badChi = (chi>m_minAbsChiToDisplay || (chi<-m_minAbsChiToDisplay && !isLeaving && !isParent) );

 
      float fCharged(0.f);
      float fPhoton(0.f);
      float fNeutral(0.f);
      const MCParticle *bestMcMatch(NULL);
      if(pCluster!=NULL)DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(pCluster, fCharged, fPhoton, fNeutral, bestMcMatch);

      // fix for conversions (where both tracks are ided)
      if(trackStatus==MISSED_CONVERSION){
	fCharged+= fPhoton;
	fPhoton  = 0;
      }

      m_trackRecoAsTrackEnergy   += clusterEnergy*fCharged; 
      m_photonRecoAsTrackEnergy  += clusterEnergy*fPhoton;
      m_neutralRecoAsTrackEnergy += clusterEnergy*fNeutral;


      bool badConfusion = (clusterEnergy*(fPhoton+fNeutral)>m_minConfusionEnergyToDisplay);

      // decide whether to print
      if(pfoEnergy>m_minPfoEnergyToDisplay)printThisPfo=true;
      if(badConfusion||badChi)printThisPfo=true;

      if(printThisPfo)
	{
	  if(m_firstChargedPfoToPrint)
	    {
	      // First loop to dump charged pfos
	      std::cout << std::endl;
	      FORMATTEDOUTPUTPFOHEADERTITLE();
	      FORMATTEDOUTPUTTRACKTITLE();
	      m_firstChargedPfoToPrint = false;
	    }
	  if(!printedHeader)
	    {
	      FORMATTEDOUTPUTPFOHEADER(pfoPid, pfoEnergy);
	      if(trackList.size()>1)std::cout << std::endl;
	      printedHeader = true;
	    }
	  if(trackList.size()>1)FORMATTEDOUTPUTPFOHEADER("", "");
	  (pCluster!=NULL) ? FORMATTEDOUTPUTTRACK(trackId, mcId, pTrack->CanFormPfo(), pTrack->ReachesECal(),trackEnergy, mcEnergy, clusterEnergy, chi, leaving, fCharged, fPhoton, fNeutral) : FORMATTEDOUTPUTTRACK(trackId, mcId,pTrack->CanFormPfo(),pTrack->ReachesECal(), trackEnergy, mcEnergy, " ", chi, " ", " ", " ", " ");

	  if(badChi){
	    std::cout << " <-- Bad E-P consistency : " << chi << std::endl;
	    continue;
	  }
	  if(badConfusion){
	    std::cout << " <-- confusion : " << clusterEnergy*(fPhoton+fNeutral) << " GeV " << std::endl;
	    continue;
	  }
	  if(trackStatus==SPLIT_TRACK){
	    std::cout << " <-- split track" << std::endl;
	    continue;
	  }
	  if(trackStatus==MISSED_CONVERSION){
	    std::cout << " <-- missed conversion" << std::endl;
	    continue;
	  }
	  if(trackStatus==MISSED_KSHORT){
	    std::cout << " <-- missed kshort" << std::endl;
	    continue;
	  }
	  std::cout << std::endl;

	}
    }



  
  return STATUS_CODE_SUCCESS;

}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::DumpPhotonPfo(const ParticleFlowObject* pPfo)
{

  float fCharged(0.f);
  float fPhoton(0.f);
  float fNeutral(0.f);
  bool  printThisPfo(false);
  bool printedHeader(false);
    
  const ClusterList   &clusterList(pPfo->GetClusterList());
  const int pfoPid    = pPfo->GetParticleId();
  const float pfoEnergy = pPfo->GetEnergy();
  for (ClusterList::const_iterator clusterIter = clusterList.begin(); clusterIter != clusterList.end(); ++clusterIter)
    {
      Cluster *pCluster = *clusterIter;
      float clusterEnergy(pCluster->GetHadronicEnergy());
      const MCParticle *bestMcMatch(NULL);
      DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(pCluster, fCharged, fPhoton, fNeutral,bestMcMatch);
      m_trackRecoAsPhotonEnergy   += clusterEnergy*fCharged; 
      m_photonRecoAsPhotonEnergy  += clusterEnergy*fPhoton;
      m_neutralRecoAsPhotonEnergy += clusterEnergy*fNeutral;
      float  showerProfileStart(0.);
      float  showerProfileDiscrepancy(0.);
      ParticleIdHelper::CalculateShowerProfile(pCluster, showerProfileStart, showerProfileDiscrepancy);
      bool badConfusion = (clusterEnergy*fCharged>m_minConfusionEnergyToDisplay);
      bool badFragment  = (fCharged>0.80 && clusterEnergy*fCharged>m_fragmentEnergyToDisplay);
      bool badTrackMatch  = (fCharged>0.95 && clusterEnergy*fCharged>m_fragmentEnergyToDisplay);
      if(badTrackMatch){
	std::map<const MCParticle*,const Track*>::iterator it = m_mcParticleToTrackMap.find(bestMcMatch);
	if(it!=m_mcParticleToTrackMap.end()){
	  if(((*it).second)->HasAssociatedCluster())badTrackMatch=false;
	}
      }
      bool badPhotonId  = (fNeutral>0.80 && clusterEnergy*fNeutral>m_photonIdEnergyToDisplay);
      if(pfoEnergy>m_minPfoEnergyToDisplay)printThisPfo=true;
      if(badConfusion||badFragment||badPhotonId)printThisPfo=true;
      if(printThisPfo)
	{
	  if(m_firstPhotonPfoToPrint){
	    std::cout << std::endl;
	    FORMATTEDOUTPUTPFOHEADERTITLE();
	    FORMATTEDOUTPUTNEUTRALTITLE();
	    m_firstPhotonPfoToPrint = false;
	  }
	  if(!printedHeader)
	    {
	      FORMATTEDOUTPUTPFOHEADER(pfoPid, pfoEnergy);
	      printedHeader = true;
	    }	 
	  if(pCluster!=NULL)FORMATTEDOUTPUTNEUTRAL(clusterEnergy,fCharged,fPhoton,fNeutral,pCluster->GetInnerPseudoLayer(),pCluster->GetOuterPseudoLayer(),showerProfileStart,showerProfileDiscrepancy);
	  if(badTrackMatch){
	    std::cout << " <-- bad track match  : " << clusterEnergy*fCharged << " GeV ";
	    if(bestMcMatch!=NULL)std::cout << "(" << bestMcMatch->GetEnergy() << " ) ";
	    std::cout << std::endl;
	    continue;
	  }
	  if(badFragment){
	    std::cout << " <-- fragment  : " << clusterEnergy*fCharged << " GeV ";
	    if(bestMcMatch!=NULL)std::cout << "(" << bestMcMatch->GetEnergy() << " ) ";
	    std::cout << std::endl;
	    continue;
	  }
	  if(badConfusion){
	    std::cout << " <-- confusion : " << clusterEnergy*fCharged << " GeV " << std::endl;
	    continue;
	  }
	  if(badPhotonId){
	    std::cout << " <-- neutral hadron : " << clusterEnergy*fNeutral << " GeV " << std::endl;
	    continue;
	  }

	  std::cout << std::endl;
	}
    }	
  
  // decide whether to print



  return STATUS_CODE_SUCCESS;

}



//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::DumpNeutralPfo(const ParticleFlowObject* pPfo)
{

  float fCharged(0.f);
  float fPhoton(0.f);
  float fNeutral(0.f);
  bool  printThisPfo(false);
  bool printedHeader(false);
  
  
  const ClusterList   &clusterList(pPfo->GetClusterList());
  const int pfoPid    = pPfo->GetParticleId();
  const float pfoEnergy = pPfo->GetEnergy();
  for (ClusterList::const_iterator clusterIter = clusterList.begin(); clusterIter != clusterList.end(); ++clusterIter)
    {
      Cluster *pCluster = *clusterIter;
      float clusterEnergy(pCluster->GetHadronicEnergy());
      const MCParticle *bestMcMatch(NULL);
      DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(pCluster, fCharged, fPhoton, fNeutral, bestMcMatch);
      m_trackRecoAsNeutralEnergy   += clusterEnergy*fCharged; 
      m_photonRecoAsNeutralEnergy  += clusterEnergy*fPhoton;
      m_neutralRecoAsNeutralEnergy += clusterEnergy*fNeutral;
      float  showerProfileStart(0.);
      float  showerProfileDiscrepancy(0.);
      ParticleIdHelper::CalculateShowerProfile(pCluster, showerProfileStart, showerProfileDiscrepancy);
      bool badConfusion   = (clusterEnergy*fCharged>m_minConfusionEnergyToDisplay);
      bool badFragment    = (fCharged>0.80 && clusterEnergy*fCharged>m_fragmentEnergyToDisplay);
      bool badTrackMatch  = (fCharged>0.95 && clusterEnergy*fCharged>m_fragmentEnergyToDisplay);
      if(badTrackMatch){
	std::map<const MCParticle*,const Track*>::iterator it = m_mcParticleToTrackMap.find(bestMcMatch);
	if(it!=m_mcParticleToTrackMap.end()){
	  if(((*it).second)->HasAssociatedCluster())badTrackMatch=false;
	}
      }

      bool badPhotonId  = (fPhoton>0.80 && clusterEnergy*fPhoton>m_photonIdEnergyToDisplay);
      


      if(pfoEnergy>m_minPfoEnergyToDisplay)printThisPfo=true;
      if(badConfusion || badFragment || badPhotonId)printThisPfo=true;
      if(printThisPfo)
	{
	  if(m_firstNeutralPfoToPrint)
	    {
	      // First loop to dump charged pfos
	      std::cout << std::endl;
	      FORMATTEDOUTPUTPFOHEADERTITLE();
	      FORMATTEDOUTPUTNEUTRALTITLE();
	      m_firstNeutralPfoToPrint = false;
	    }
	  if(!printedHeader)
	    {
	      FORMATTEDOUTPUTPFOHEADER(pfoPid, pfoEnergy);
	      printedHeader = true;
	    }	 

	  if(pCluster!=NULL)FORMATTEDOUTPUTNEUTRAL(clusterEnergy,fCharged,fPhoton,fNeutral,pCluster->GetInnerPseudoLayer(),pCluster->GetOuterPseudoLayer(),showerProfileStart,showerProfileDiscrepancy);
	  if(badTrackMatch){
	    std::cout << " <-- bad track match  : " << clusterEnergy*fCharged << " GeV ";
	    if(bestMcMatch!=NULL)std::cout << "(" << bestMcMatch->GetEnergy() << " ) ";
	    std::cout << std::endl;
	    continue;
	  }
	  if(badFragment){
	    std::cout << " <-- fragment  : " << clusterEnergy*fCharged << " GeV ";
	    if(bestMcMatch!=NULL)std::cout << "(" << bestMcMatch->GetEnergy() << " ) ";
	    std::cout << std::endl;
	    continue;
	  }
	  if(badConfusion){
	    std::cout << " <-- confusion : " << clusterEnergy*fCharged << " GeV " << std::endl;
	    continue;
	  }
	  if(badPhotonId){
	    std::cout << " <-- photon    : " << clusterEnergy*fPhoton << " GeV " << std::endl;
	    continue;
	  }
	  std::cout << std::endl;
	}
    }	
  
  // decide whether to print
  
  return STATUS_CODE_SUCCESS;

}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    


    m_minPfoEnergyToDisplay = 0.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinPfoEnergyToDisplay", m_minPfoEnergyToDisplay));

    m_minAbsChiToDisplay    = 3.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinAbsChiToDisplay", m_minAbsChiToDisplay));

    m_minConfusionEnergyToDisplay    = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinConfusionEnergyToDisplay", m_minConfusionEnergyToDisplay));

    m_minFragmentEnergyToDisplay    = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinFragmentEnergyToDisplay", m_minFragmentEnergyToDisplay));

    m_totalPfoEnergyDisplayLessThan = 1000000.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TotalPfoEnergyDisplayLessThan", m_totalPfoEnergyDisplayLessThan));
 
    m_totalPfoEnergyDisplayGreaterThan = 0.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "TotalPfoEnergyDisplayGreaterThan", m_totalPfoEnergyDisplayGreaterThan));
 
    m_fragmentEnergyToDisplay = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FragmentEnergyToDisplay", m_fragmentEnergyToDisplay));
 
    m_photonIdEnergyToDisplay = 5.0f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdEnergyToDisplay", m_photonIdEnergyToDisplay));
 

    return STATUS_CODE_SUCCESS;

}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode DumpPfosMonitoringAlgorithm::ClusterEnergyFractions(const Cluster* pCluster, float &fCharged, float &fPhoton, float &fNeutral, const MCParticle* &pMcBest)
{
    
  pMcBest = NULL;
  float totEnergy(0.f);
  float neutralEnergy(0.f);
  float photonEnergy(0.f);
  float chargedEnergy(0.f);
  
  std::map<const MCParticle*,float> mcContribs;

  const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
  for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
      for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
	  CaloHit *pCaloHit = *hitIter;
	  const MCParticle* mcpart(NULL);
	  pCaloHit->GetMCParticle(mcpart);
	  if(mcpart==NULL)continue;
	  const MCParticle* mcPfoTarget(NULL);
	  mcpart->GetPfoTarget(mcPfoTarget);
	  if(mcPfoTarget==NULL)continue;
	  totEnergy += pCaloHit->GetHadronicEnergy();
	  std::map<const MCParticle*,float>::iterator it = mcContribs.find(mcPfoTarget);

	  if(it != mcContribs.end())
	    {
	      (*it).second+=pCaloHit->GetHadronicEnergy();
	    }
	  else
	    {
	      mcContribs.insert(std::map<const MCParticle*,float>::value_type(mcPfoTarget,pCaloHit->GetHadronicEnergy()));
	    }
	  
	  const int pdgCode = mcPfoTarget->GetParticleId();
	  try
	    {
	      const int charge  = PdgTable::GetParticleCharge(pdgCode);
	      if(charge!=0 || abs(pdgCode)==3122 || abs(pdgCode)==310)
		{
		  if(m_trackMcPfoTargets.count(mcpart)==0)
		    {
		      neutralEnergy += pCaloHit->GetHadronicEnergy();
		    }
		  else
		    {
		      chargedEnergy += pCaloHit->GetHadronicEnergy();
		    }
		}
	      else
		{
		  (mcpart->GetParticleId() == 22)? photonEnergy+=pCaloHit->GetHadronicEnergy(): neutralEnergy+=pCaloHit->GetHadronicEnergy();
		}
	      
		}
	  catch (StatusCodeException &statusCodeException)
	    {
	      std::cout << "Failed to get charge  " << pdgCode << std::endl;
	    }
	}
    }
  
  if(totEnergy>0){
    fCharged = chargedEnergy/totEnergy;
    fPhoton  = photonEnergy/totEnergy;
    fNeutral = neutralEnergy/totEnergy;
  }

  // find mc particle with largest associated energy
  float emax(0.);
  std::map<const MCParticle*,float>::iterator iter = mcContribs.begin();
  std::map<const MCParticle*,float>::iterator iter_end = mcContribs.end();
  while(iter != iter_end){
    if( (*iter).second > emax){
      emax = (*iter).second;
      pMcBest = (*iter).first;
    }
    ++iter;
  }
  
  return STATUS_CODE_SUCCESS;
  
}




