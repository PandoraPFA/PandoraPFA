/**
 *  @file   PandoraPfaNew/Algoirthms/src/KinkPfoAlgorithm.cc
 * 
 *  @brief  Analyse kink pfos to identify decays to muons and neutral hadrons
 * 
 *  $Log: $
 */

#include "Objects/MCParticle.h"
#include "Algorithms/PfoConstruction/V0PfoCreationAlgorithm.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"
#include "Objects/Cluster.h"
#include "Helpers/ParticleIdHelper.h"
#include "Helpers/XmlHelper.h"
#include "Pandora/PdgTable.h"

using namespace pandora;

V0PfoCreationAlgorithm::V0PfoCreationAlgorithm(){

}


V0PfoCreationAlgorithm::~V0PfoCreationAlgorithm(){

}


StatusCode V0PfoCreationAlgorithm::Run()
{

    // Algorithm code here
  
    const ParticleFlowObjectList *pPfoList = NULL;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentPfoList(*this, pPfoList));


    for (ParticleFlowObjectList::const_iterator iter = pPfoList->begin(), iterEnd = pPfoList->end(); iter != iterEnd; ++iter)
      {
	ParticleFlowObject *pfo = *iter;
	const TrackList pTrackList = pfo->GetTrackList();
	if(pTrackList.size()!=2)continue;
	Track* track1 = *(pTrackList.begin());
	TrackList siblingTrackList  = track1->GetSiblingTrackList();
	if(siblingTrackList.size()!=1)continue;
	Track* track2 = *(siblingTrackList.begin());
	
	const CartesianVector momentumAtStart1(track1->GetTrackStateAtStart().GetMomentum());
	const CartesianVector momentumAtStart2(track2->GetTrackStateAtStart().GetMomentum());
	Cluster *cluster1=NULL;
	track1->GetAssociatedCluster(cluster1);
	Cluster *cluster2=NULL;
	track2->GetAssociatedCluster(cluster2);
	bool electron1 = false;
	if(cluster1!=NULL)ParticleIdHelper::IsElectronFast(cluster1);
	bool electron2 = false;
	if(cluster2!=NULL)ParticleIdHelper::IsElectronFast(cluster2);
	float mass=0;
      
	if(abs(track1->GetParticleId())==abs(E_MINUS) && abs(track2->GetParticleId())==abs(E_MINUS)){
	  PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->V0Mass(momentumAtStart1,momentumAtStart2,PdgTable::GetParticleMass(E_MINUS),PdgTable::GetParticleMass(E_MINUS),mass));
	  pfo->SetParticleId(PHOTON);
	}
	if(abs(track1->GetParticleId())==abs(PI_PLUS) && abs(track2->GetParticleId())==abs(PI_PLUS)){
	  PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->V0Mass(momentumAtStart1,momentumAtStart2,PdgTable::GetParticleMass(PI_MINUS),PdgTable::GetParticleMass(PI_MINUS),mass));
	  pfo->SetParticleId(K_SHORT);
	}
	if(abs(track1->GetParticleId())==abs(PROTON) && abs(track2->GetParticleId())==abs(PI_MINUS)){
	  PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->V0Mass(momentumAtStart1,momentumAtStart2,PdgTable::GetParticleMass(PROTON),PdgTable::GetParticleMass(PI_MINUS),mass));
	  (track1->GetParticleId()==PROTON) ? pfo->SetParticleId(LAMBDA) : pfo->SetParticleId(LAMBDA_BAR);
	}
	if(abs(track1->GetParticleId())==abs(PI_MINUS) && abs(track2->GetParticleId())==abs(PROTON)){
	  PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->V0Mass(momentumAtStart1,momentumAtStart2,PdgTable::GetParticleMass(PI_MINUS),PdgTable::GetParticleMass(PROTON),mass));
	  (track2->GetParticleId()==PROTON) ? pfo->SetParticleId(LAMBDA) : pfo->SetParticleId(LAMBDA_BAR);
	}

	if(m_mcMonitoring)
        {
	  const MCParticle* pMCParticle1 = NULL; 
	  const MCParticle* pMCParticle2 = NULL; 
	  MCParticle* pMCParent1 = NULL; 
	  MCParticle* pMCParent2 = NULL; 
	  try
	    {
	      PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, 
				      track1->GetMCParticle(pMCParticle1));
	      PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, 
				      track2->GetMCParticle(pMCParticle2));
	      const int ipdg1 =  pMCParticle1->GetParticleId();
	      std::cout << " Track 1 : " << " ipdg = " << ipdg1 << " E = " << pMCParticle1->GetEnergy() << std::endl;
	      const int ipdg2 =  pMCParticle2->GetParticleId();
	      std::cout << " Track 2 : " << " ipdg = " << ipdg2 << " E = " << pMCParticle2->GetEnergy() << std::endl;
	      MCParticleList parentList1 = pMCParticle1->GetParentList();
	      MCParticleList parentList2 = pMCParticle2->GetParentList();
	      pMCParent1 = *(parentList1.begin());
	      pMCParent2 = *(parentList2.begin());
	      for (MCParticleList::const_iterator diter = parentList1.begin(), iterEnd = parentList1.end(); diter != iterEnd; ++diter){
		int dipdg =  (*diter)->GetParticleId();
		const float energyD = (*diter)->GetEnergy();
		if(energyD>0.1)std::cout << "    decay parent " << dipdg <<  " E = " << energyD << std::endl;
	      }
	      for (MCParticleList::const_iterator diter = parentList2.begin(), iterEnd = parentList2.end(); diter != iterEnd; ++diter){
		int dipdg =  (*diter)->GetParticleId();
		const float energyD = (*diter)->GetEnergy();
		if(energyD>0.1)std::cout << "    decay parent " << dipdg <<  " E = " << energyD << std::endl;
	      }
	    }
	  catch (StatusCodeException &statusCodeException)
	    {
	      std::cout << "Failed to find MC particle for track " << statusCodeException.ToString() << std::endl;
	      continue;
	    }
	  bool goodV0 = true;
	  if(pMCParent1!=pMCParent2)goodV0=false;
	  if(pMCParent1->GetParticleId()!=pfo->GetParticleId() )goodV0=false;
	  
	  if(goodV0 )std::cout << " Good V0 : " << pfo->GetParticleId() << " " << track1->GetEnergyAtDca() << " " << track2->GetEnergyAtDca() << " eID " << electron1 << " " << electron2 << " Mass = " << mass << std::endl;
	  if(!goodV0)std::cout << " Bad  V0 : " << pfo->GetParticleId() << " " << track1->GetEnergyAtDca() << " " << track2->GetEnergyAtDca() << " eID " << electron1 << " " << electron2 << " Mass = " << mass << std::endl;
	  
	  const TrackState trackState1 = track1->GetTrackStateAtStart();
	  const TrackState trackState2 = track2->GetTrackStateAtStart();
	  const CartesianVector position1 = trackState1.GetPosition();
	  const CartesianVector position2 = trackState2.GetPosition();
	  float rxy1 = sqrt( position1.GetX()*position1.GetX() + position1.GetY()*position1.GetY());
	  float rxy2 = sqrt( position2.GetX()*position2.GetX() + position2.GetY()*position2.GetY());
	  std::cout << "     " << " r = " << position1.GetMagnitude() << " rxy = " << rxy1 << std::endl;
	  std::cout << "     " << " r = " << position2.GetMagnitude() << " rxy = " << rxy2 << std::endl;
	  
	}
 

    }      
  
    return STATUS_CODE_SUCCESS;

}


StatusCode V0PfoCreationAlgorithm::V0Mass(const CartesianVector &momentum1, const CartesianVector &momentum2, float mass1, float mass2, float &V0mass){

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


//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode V0PfoCreationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    // Read settings from xml file here

    m_mcMonitoring = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "McMonitoring", m_mcMonitoring));

    
    return STATUS_CODE_SUCCESS;
}

