/**
 *  @file   PandoraPfaNew/Algoirthms/src/KinkPfoCreationAlgorithm.cc
 * 
 *  @brief  Analyse kink pfos to identify decays to muons and neutral hadrons
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/KinkPfoCreationAlgorithm.h"
#include "Objects/MCParticle.h"
#include "Objects/Track.h"
#include "Objects/Cluster.h"
#include "Objects/ParticleFlowObject.h"
#include "Helpers/ParticleIdHelper.h"
#include "Helpers/ReclusterHelper.h"
#include "Helpers/XmlHelper.h"
#include "PandoraMonitoringApi.h"
#include "Pandora/PdgTable.h"
#include "math.h"
#include <limits>

using namespace pandora;

KinkPfoCreationAlgorithm::KinkPfoCreationAlgorithm(){

  std::cout << " KinkPfoCreationAlgorithm constructor " << std::endl;

}


KinkPfoCreationAlgorithm::~KinkPfoCreationAlgorithm(){

  std::cout << " KinkPfoCreationAlgorithm destructor " << std::endl;

}


StatusCode KinkPfoCreationAlgorithm::Run()
{

    // Algorithm code here
  
  const ParticleFlowObjectList *pPfoList = NULL;
  
  PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pPfoList));
  
  ParticleFlowObjectList lambdaPfos;
  ParticleFlowObjectList kShortPfos;
  
  for (ParticleFlowObjectList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end(); iter != iterEnd; ++iter)
    {
      ParticleFlowObject *pfo = *iter;
      if(pfo->GetParticleId()==K_SHORT)kShortPfos.insert(pfo);
      if(abs(pfo->GetParticleId())==LAMBDA)lambdaPfos.insert(pfo);
    }
  
  
  for (ParticleFlowObjectList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end(); iter != iterEnd; ++iter)
    {
      ParticleFlowObject *pfo = *iter;
      const TrackList pTrackList = pfo->GetTrackList();
      
      // a kink should have a single daughter track
      if(pTrackList.size()<2)continue;
      
      for (TrackList::const_iterator iterTrack = pTrackList.begin(), iterTrackEnd = pTrackList.end(); iterTrack != iterTrackEnd; ++iterTrack)
	{
	  Track* track = *iterTrack;
	  
	  TrackList daughterTrackList = track->GetDaughterTrackList();
	  if(daughterTrackList.size()!=1)continue;
	  
	  TrackList siblingTrackList  = track->GetSiblingTrackList();
	  TrackList parentTrackList   = track->GetParentTrackList();
	  if(parentTrackList.size()!=0 || siblingTrackList.size()!=0)continue;
	  
	  Track* parentTrack = track;
	  Track* daughterTrack = *(daughterTrackList.begin());
	  
	  // check to see if this is a decay kink 
	  if( abs(parentTrack->GetParticleId())==abs(PI_PLUS) && abs(daughterTrack->GetParticleId())==abs(PI_PLUS))continue; 
	  
	  CartesianVector parentMomentumAtEnd(parentTrack->GetTrackStateAtEnd().GetMomentum());
	  const CartesianVector parentPositionAtEnd(parentTrack->GetTrackStateAtEnd().GetPosition());
	  const CartesianVector daughterMomentumAtStart(daughterTrack->GetTrackStateAtStart().GetMomentum());
	  float scale = (track->GetMomentumAtDca()).GetMagnitude()/parentMomentumAtEnd.GetMagnitude();
	  if(scale>1.1 || scale < 0.9){
	    parentMomentumAtEnd*=scale;
	    std::cout << "Scaling parent momentum at end" << std::endl;
	  }
	  const CartesianVector neutralMomentum = parentMomentumAtEnd-daughterMomentumAtStart;
	  const float neutralEnergy = neutralMomentum.GetMagnitude();
	  
	  const float ediff = parentTrack->GetEnergyAtDca() - daughterTrack->GetEnergyAtDca();
	  if(ediff < 0.4 )continue;
	  
	  if(m_mcMonitoring)this->DisplayMcInformation(parentTrack);

	  
	  Cluster *cluster = NULL;
	  daughterTrack->GetAssociatedCluster(cluster);
	  bool muon = false;
	  if(cluster!=NULL){
	    ClusterList c;
	    c.insert(cluster);
	    //if(m_monitoring)PandoraMonitoringApi::AddClusterList(DETECTOR_VIEW_XY, &c, BLUE );
	    muon = ParticleIdHelper::IsMuonFast(cluster);
	  }


	   	    
	  if(m_monitoring){
	    std::cout << " Reco Parent Track : " << parentTrack->GetParticleId() << " E = " << parentTrack->GetEnergyAtDca() << " " << parentMomentumAtEnd.GetMagnitude() << std::endl;
	    std::cout << "   daughter   : " << daughterTrack->GetParticleId() << " E = " << daughterTrack->GetEnergyAtDca() << " " << daughterMomentumAtStart.GetMagnitude() << std::endl;
	    std::cout << "   neutral    : " << " E = " << neutralEnergy << std::endl;
	    if(cluster!=NULL)std::cout << "   cluster    : " << " E = " << cluster->GetCorrectedHadronicEnergy() << std::endl;
	    if(muon)std::cout << " Track is a muon " << std::endl;
	  }

	  
	  
	  
	  TrackList t;
	  t.insert(parentTrack);
	  
	  // find potential neutral clusters
	  PfosOrderedByDistanceMap neutralPfosByDistance;
	  PfosOrderedByDistanceMap photonPfosByDistance;
	  PfosOrderedByDistanceMap chargedPfosByDistance;
	  ClusterList candNeutralClusterList;
	  for (ParticleFlowObjectList::const_iterator neutralIter = pPfoList->begin(), neutralIterEnd = pPfoList->end(); neutralIter != neutralIterEnd; )
	    {
	      ParticleFlowObject *neutralPfo = *neutralIter;
	      ++neutralIter;
	      const TrackList pTrackList = neutralPfo->GetTrackList();
	      const ClusterList pClusterList = neutralPfo->GetClusterList();
	      if(pClusterList.size()!=1)continue;
	      Cluster* neutralCluster = *(pClusterList.begin());
	      const CartesianVector centroid = neutralCluster->GetCentroid(neutralCluster->GetInnerPseudoLayer());
	      const CartesianVector clusterMomentum(neutralPfo->GetMomentum());
	      float scale = centroid.GetMagnitude()/clusterMomentum.GetMagnitude();
	      const CartesianVector radialVector = clusterMomentum*scale + parentPositionAtEnd;
	      float angle = radialVector.GetOpeningAngle(neutralMomentum);
	      float deltaR = angle*centroid.GetMagnitude();
	      if(neutralPfo->GetParticleId()==22)deltaR*=m_photonDistancePenalty;
	      if(pTrackList.size()==1)deltaR = deltaR/2.;
	      if(deltaR < m_maxProjectedDistanceToNeutral){
		if(pTrackList.size()==0){
		  switch(neutralPfo->GetParticleId()){
		  case PHOTON:
		    photonPfosByDistance.insert(PfosOrderedByDistanceMap::value_type(deltaR,neutralPfo));
		    break;
		  case NEUTRON:
		    neutralPfosByDistance.insert(PfosOrderedByDistanceMap::value_type(deltaR,neutralPfo));	
		    break;
		  default:
		    break;
		  }
		  candNeutralClusterList.insert(neutralCluster);
		}else{
		  //Track* associatedTrack = *(pTrackList.begin());
		  chargedPfosByDistance.insert(PfosOrderedByDistanceMap::value_type(deltaR,neutralPfo));		  
		}
	      }
	    }
	  
	  
	  // now apply corrections
	  bool correctedThisPfo = false;
	  
	  if(m_findDecaysWithNeutrinos && ( (abs(parentTrack->GetParticleId())==abs(PI_PLUS)) || (abs(parentTrack->GetParticleId())==abs(K_PLUS)))){
	    float munuMass;
	    float pipiMass;
	    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->KinkMass(parentMomentumAtEnd,daughterMomentumAtStart, PdgTable::GetParticleMass(MU_MINUS), 0.0, munuMass));
	    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->KinkMass(parentMomentumAtEnd,daughterMomentumAtStart, PdgTable::GetParticleMass(PI_MINUS), 0.0, pipiMass));
	    if(m_monitoring){
	      std::cout << " Kink mass (reco) mu nu : " << munuMass << std::endl;
	      std::cout << " Kink mass (reco) pi pi : " << pipiMass << std::endl;
	    }
	    bool piMass=false;
	    if(munuMass>m_lowerCutOnPiToMuNuMass && munuMass<m_upperCutOnPiToMuNuMass)piMass = true;
	    bool kaonMass=false;
	    if(munuMass>m_lowerCutOnKToMuNuMass  && munuMass<m_upperCutOnKToMuNuMass)kaonMass = true;
	    if( fabs(pipiMass-PdgTable::GetParticleMass(K_MINUS)) < 
                fabs(munuMass-PdgTable::GetParticleMass(K_MINUS)))kaonMass = false;
	    if(muon || piMass || kaonMass){
	      int parentId;
	      (pfo->GetCharge()>0) ? parentId = PI_PLUS : parentId = PI_MINUS;
	      if(fabs(munuMass-PdgTable::GetParticleMass(PI_MINUS))>fabs(munuMass-PdgTable::GetParticleMass(K_MINUS)))(pfo->GetCharge()>0) ? parentId = K_PLUS : parentId = K_MINUS;
	      pfo->SetParticleId(parentId);
	      this->SetPfoParametersFromParentTrack(pfo);
	      correctedThisPfo = true;
	      if(m_monitoring)std::cout << " New PFO  " << pfo->GetParticleId() << " energy " << pfo->GetEnergy() << std::endl; 
	    }else{
	      // would like to define daughter track as pion not a muon - don't have a way to do this or propagate it to PFOs
	      // no impact on performance
	    }
	  }




	  //if(m_findDecaysWithLambdas && !correctedThisPfo){
	  // const float targetEnergy = parentTrack->GetEnergyAtDca() - daughterTrack->GetEnergyAtDca();
	  //for (ParticleFlowObjectList::const_iterator lIter = lambdaPfos.begin(), lIterEnd = lambdaPfos.end(); lIter != lIterEnd; ++lIter)
	  //  {
	  //ParticleFlowObject *lambdaPfo = (*lIter);
	  //std::cout << " Lambda E = " << lambdaPfo->GetEnergy() << std::endl;
	  //const CartesianVector lambdaMomentum(lambdaPfo->GetMomentum());
	  //float recoMass;
	  //
	  //PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->V0Mass(daughterMomentumAtStart, lambdaMomentum, PdgTable::GetParticleMass(PI_MINUS), PdgTable::GetParticleMass(LAMBDA), recoMass));
	  //std::cout << " Lamda kink mass = " << recoMass << std::endl;
	  //
	  //  }
	  //}


	  if(m_findDecaysWithNeutrons && !correctedThisPfo){
	    const float targetEnergy = parentTrack->GetEnergyAtDca() - daughterTrack->GetEnergyAtDca();
	    float clusterEnergy(0.);
	    ParticleFlowObjectList pfosToRemove;
	    float chi2Current(std::numeric_limits<float>::max());
	    for (PfosOrderedByDistanceMap::const_iterator cIter = neutralPfosByDistance.begin(), cIterEnd = neutralPfosByDistance.end(); cIter != cIterEnd; ++cIter)
	      {
		ParticleFlowObject *clusterPfo = (*cIter).second;
		clusterEnergy += clusterPfo->GetEnergy();
		const float chi(ReclusterHelper::GetTrackClusterCompatibility(clusterEnergy, targetEnergy));

		float recoMass;
		PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->KinkMass(parentMomentumAtEnd,daughterMomentumAtStart, PdgTable::GetParticleMass(PI_MINUS), PdgTable::GetParticleMass(NEUTRON), recoMass));

		if(m_monitoring){
		  std::cout << " Neutral cluster " << (*cIter).first << " pid " << clusterPfo->GetParticleId() << " E = " << clusterPfo->GetEnergy() << std::endl;
		  std::cout << " Kink mass = " << recoMass << std::endl;
		}
		if(chi*chi<chi2Current){
		  pfosToRemove.insert(clusterPfo);
		  chi2Current = chi*chi;
		}
	      }
	    
	    if(m_monitoring)std::cout << " adding " << pfosToRemove.size() << " clusters, chi2 = " << chi2Current << std::endl; 

	    if( chi2Current < m_chi2CutForMergingPfos){
	      // use the track parent for the energy 
	      (pfo->GetCharge()>0) ?  pfo->SetParticleId(SIGMA_PLUS) : pfo->SetParticleId(SIGMA_MINUS);
	      this->SetPfoParametersFromParentTrack(pfo);
	      correctedThisPfo = true;
	      if(m_monitoring)std::cout << " New PFO  " << pfo->GetParticleId() << " energy " << pfo->GetEnergy() << std::endl; 
	      // add the clusters and delete other pfos
	      this->MergeClustersFromPfoListToPfo(pfosToRemove,pfo);
	    }else{
	      if(clusterEnergy>targetEnergy){
		// cluster lost in larger neutral cluster
		(pfo->GetCharge()>0) ?  pfo->SetParticleId(SIGMA_PLUS) : pfo->SetParticleId(SIGMA_MINUS);
		if(m_monitoring)std::cout << " Setting PFO ID " << pfo->GetParticleId() << " energy " << pfo->GetEnergy() << std::endl; 
		correctedThisPfo = true;
	      }
	    }
	  }

	 

	  //if(m_findDecaysWithPiZeros && !correctedThisPfo){
	  // for (PfosOrderedByDistanceMap::const_iterator cIter = photonPfosByDistance.begin(), cIterEnd = photonPfosByDistance.end(); cIter != cIterEnd; ++cIter)
	  //  {
	  //ParticleFlowObject *pfo = (*cIter).second;
	  //std::cout << " Photon cluster " << (*cIter).first << " pid " << pfo->GetParticleId() << " E = " << pfo->GetEnergy() << std::endl;
	  //  }
	  //}


	  //if(m_findDecaysMergedWithTrackClusters && !correctedThisPfo){
	  //for (PfosOrderedByDistanceMap::const_iterator cIter = chargedPfosByDistance.begin(), cIterEnd = chargedPfosByDistance.end(); cIter != cIterEnd; ++cIter)
	  //{
	  //  //  ParticleFlowObject *pfo = (*cIter).second;
	  ////  const ClusterList pClusterList = pfo->GetClusterList();
	  //if(pClusterList.size()!=1)continue;
	  //  Cluster* neutralCluster = *(pClusterList.begin());
	  //  float eclust = neutralCluster->GetCorrectedHadronicEnergy();
	  //  const float chiEC(ReclusterHelper::GetTrackClusterCompatibility(eclust, pfo->GetEnergy()));
	  //
	  //  std::cout << " Charged cluster " << (*cIter).first << " pid " << " Et = " << pfo->GetEnergy() << " Ec = " << eclust << " chi = " << chiEC <<std::endl;
	  //}
	  //}      
	  

	  //	  TrackList v0s;
	  //for (ParticleFlowObjectList::const_iterator iterV0 = pPfoList->begin(), iterV0End = pPfoList->end(); iterV0 != iterV0End; ++iterV0)
	  //{
	  //  ParticleFlowObject *pfoV0 = *iterV0;
	  //  const TrackList pTrackListV0 = pfoV0->GetTrackList();
	  //  if(pTrackListV0.size()!=2)continue;
	  //  Track* track = (*pTrackListV0.begin());
	  //  TrackList daughterTrackList = track->GetDaughterTrackList();
	  //  if(daughterTrackList.size()!=0)continue;
	  //  TrackList siblingTrackList  = track->GetSiblingTrackList();
	  //  TrackList parentTrackList   = track->GetParentTrackList();
	  //  if(parentTrackList.size()!=0 || siblingTrackList.size()!=1 )continue;
	  //  for (TrackList::const_iterator iterTrackV0 = pTrackListV0.begin(), iterTrackEnd = pTrackListV0.end(); iterTrackV0 != iterTrackEnd; ++iterTrackV0)
	  //{
	  //  Track* track = *iterTrackV0;
	  //  v0s.insert(track);
	  //}
	  //}	  	  
	  //}
	}
    }   
   
  return STATUS_CODE_SUCCESS;

}

  StatusCode KinkPfoCreationAlgorithm::KinkMass(const CartesianVector &parentMomentum, const CartesianVector &daughterMomentum, float daughterMass, float neutralMass, float &mass){

  // Calculate the invariant mass for a decaying charged particle
  mass = 0;
  float  daughterEnergy = sqrt(daughterMomentum.GetMagnitudeSquared() + daughterMass*daughterMass);
  CartesianVector neutralMomentum = parentMomentum-daughterMomentum;
  float neutralEnergy  = sqrt( neutralMomentum.GetMagnitudeSquared() + neutralMass*neutralMass);
  float massSquared =  (daughterEnergy+neutralEnergy)*(daughterEnergy+neutralEnergy)-parentMomentum.GetMagnitudeSquared();
  if(massSquared<=0)return STATUS_CODE_SUCCESS;
  mass = sqrt(massSquared);

  return STATUS_CODE_SUCCESS;

}

StatusCode KinkPfoCreationAlgorithm::V0Mass(const CartesianVector &momentum1, const CartesianVector &momentum2, float mass1, float mass2, float &V0mass){

  // Calculate the invariant mass for a decaying charged particle
  V0mass = 0;
  float  energy1 = sqrt(momentum1.GetMagnitudeSquared() + mass1*mass1);
  float  energy2 = sqrt(momentum2.GetMagnitudeSquared() + mass2*mass2);
  CartesianVector totalMomentum = momentum1 + momentum2;
  float totalEnergy = energy1+energy2;
  float massSquared =  totalEnergy*totalEnergy-totalMomentum.GetMagnitudeSquared();
  if(massSquared<=0)return STATUS_CODE_FAILURE;
  V0mass = sqrt(massSquared);

  return STATUS_CODE_SUCCESS;

}


StatusCode KinkPfoCreationAlgorithm::SetPfoParametersFromParentTrack(ParticleFlowObject *pfo){

  float mass = PdgTable::GetParticleMass(static_cast<pandora::ParticleType>(pfo->GetParticleId()));

  const TrackList pTrackList = pfo->GetTrackList();
  for (TrackList::const_iterator iterTrack = pTrackList.begin(), iterTrackEnd = pTrackList.end(); iterTrack != iterTrackEnd; ++iterTrack)
    {
      Track* track = *iterTrack;
      TrackList daughterTrackList = track->GetDaughterTrackList();
      if(daughterTrackList.size()!=1)continue;
      TrackList siblingTrackList  = track->GetSiblingTrackList();
      TrackList parentTrackList   = track->GetParentTrackList();
      if(parentTrackList.size()!=0 || siblingTrackList.size()!=0)continue;
      CartesianVector pAtDca = track->GetMomentumAtDca(); 
      pfo->SetMomentum(pAtDca);
      float energy = sqrt(pAtDca.GetMagnitudeSquared() + mass*mass);
      pfo->SetEnergy(energy);
      pfo->SetMass(mass);
      return STATUS_CODE_SUCCESS;
    }

  return STATUS_CODE_FAILURE;

}

StatusCode KinkPfoCreationAlgorithm::MergeClustersFromPfoListToPfo(ParticleFlowObjectList &pfosToMerge, ParticleFlowObject *pfo){


  ClusterList clustersToAdd;

  for (ParticleFlowObjectList::const_iterator iter = pfosToMerge.begin(), iterEnd = pfosToMerge.end(); iter != iterEnd; )
    {
      ParticleFlowObject *pfoToMerge = *iter;
      iter++;
      const ClusterList clusterList = pfoToMerge->GetClusterList();
      for (ClusterList::const_iterator clusterIter = clusterList.begin(), clusterIterEnd = clusterList.end(); clusterIter != clusterIterEnd; clusterIter++)
	{ 
	  clustersToAdd.insert(*clusterIter);
	}
      
      ;
      try
	{
	  PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, 
				  PandoraContentApi::DeletePfo(*this, pfoToMerge));
	}
      catch (StatusCodeException &statusCodeException)
	{
	  std::cout << "Failed to delete PFO !!! " << statusCodeException.ToString() << std::endl;
	  return STATUS_CODE_FAILURE;
	}
    }

  for (ClusterList::const_iterator clusterIter = clustersToAdd.begin(), clusterIterEnd = clustersToAdd.end(); clusterIter != clusterIterEnd; clusterIter++)
    {


      try
	{
	  PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, 
				  PandoraContentApi::AddClusterToPfo(*this, pfo,*clusterIter));
	}
      catch (StatusCodeException &statusCodeException)
	{
	  std::cout << "Failed to add cluster to PFO !!! " << statusCodeException.ToString() << std::endl;
	  return STATUS_CODE_FAILURE;
	}
      
    }

  return STATUS_CODE_SUCCESS;

}


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode KinkPfoCreationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Read settings from xml file here

    m_maxProjectedDistanceToNeutral = 150; 
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxProjectedDistanceToNeutral", m_maxProjectedDistanceToNeutral));

    m_photonDistancePenalty = 0.5; 
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonDistancePenalty", m_photonDistancePenalty));


    m_findDecaysMergedWithTrackClusters = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysMergedWithTrackClusters", m_findDecaysMergedWithTrackClusters));   

    m_mcMonitoring = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
	"McMonitoring", m_mcMonitoring));

    m_monitoring = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
	"Monitoring", m_monitoring));

    m_findDecaysWithNeutrinos = true; 
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysWithNeutrinos", m_findDecaysWithNeutrinos));

    m_findDecaysWithPiZeros = false; 
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysWithPiZeros", m_findDecaysWithPiZeros));

    m_findDecaysWithNeutrons = false; 
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysWithNeutrons", m_findDecaysWithNeutrons));

    m_findDecaysWithLambdas = false; 
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FindDecaysWithLambdas", m_findDecaysWithLambdas));

    m_chi2CutForMergingPfos = 6.0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "Chi2CutForMergingPfos", m_chi2CutForMergingPfos));
    
    m_lowerCutOnPiZeroMass = 0.12;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowerCutOnPiZeroMass", m_lowerCutOnPiZeroMass));

    m_upperCutOnPiZeroMass = 0.16;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UpperCutOnPiZeroMass", m_upperCutOnPiZeroMass));


    m_lowerCutOnPiToMuNuMass = 0.1;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowerCutOnPiToMuNuMass", m_lowerCutOnPiToMuNuMass));

    m_upperCutOnPiToMuNuMass = 0.16;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UpperCutOnPiToMuNuMass", m_upperCutOnPiToMuNuMass));

    m_lowerCutOnKToMuNuMass = 0.45;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "LowerCutOnKToMuNuMass", m_lowerCutOnKToMuNuMass));

    m_upperCutOnKToMuNuMass = 0.55;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "UpperCutOnKToMuNuMass", m_upperCutOnKToMuNuMass));

    
    return STATUS_CODE_SUCCESS;
}

 void KinkPfoCreationAlgorithm::DisplayMcInformation(Track* parentTrack){

   const MCParticle* pMCParticle = NULL; 
   try
     {
       PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, 
			       parentTrack->GetMCParticle(pMCParticle));
       const int ipdg =  pMCParticle->GetParticleId();
       std::cout << " MC Parent Track : " << " ipdg = " << ipdg << " E = " << pMCParticle->GetEnergy() << std::endl;
       MCParticleList daughterList = pMCParticle->GetDaughterList();
       for (MCParticleList::const_iterator diter = daughterList.begin(), iterEnd = daughterList.end(); diter != iterEnd; ++diter){
	 int dipdg =  (*diter)->GetParticleId();
	 const float energyD = (*diter)->GetEnergy();
	 if(energyD>0.1)std::cout << "    decay daughter " << dipdg <<  " E = " << energyD << std::endl;
       }
       
       MCParticleList parentList = pMCParticle->GetParentList();
       for (MCParticleList::const_iterator diter = parentList.begin(), iterEnd = parentList.end(); diter != iterEnd; ++diter){
	 int dipdg =  (*diter)->GetParticleId();
	 const float energyD = (*diter)->GetEnergy();
	 if(energyD>0.1)std::cout << "    decay parent " << dipdg <<  " E = " << energyD << std::endl;
       }
       
       
     }
   catch (StatusCodeException &statusCodeException)
     {
       std::cout << "Failed to find MC particle for track " << statusCodeException.ToString() << std::endl;
     }
   
   
   return;
 }
 
