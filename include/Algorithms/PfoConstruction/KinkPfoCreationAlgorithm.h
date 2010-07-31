/**
 *  @file   PandoraPFANew/include/Algorithms/KinkPfoCreationAlgorithm.h
 * 
 *  @brief  Header file for the template algorithm class.
 * 
 *  $Log: $
 */
#ifndef KINK_PFO_CREATION_ALGORITHM_H
#define KINK_PFO_CREATION_ALGORITHM_H 1
#include "Algorithms/Algorithm.h"
#include "Objects/Track.h"
#include "Objects/CartesianVector.h"

typedef std::multimap<float, pandora::ParticleFlowObject*> PfosOrderedByDistanceMap; 

/**
 *  @brief  KinkPfoCreationAlgorithm class
 */
class KinkPfoCreationAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
     

    class Factory : public pandora::AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };


    /**
     *  @brief  Factory class for instantiating algorithm
     */
  KinkPfoCreationAlgorithm();
    /**
     *  @brief  Factory class for instantiating algorithm
     */
  ~KinkPfoCreationAlgorithm();

private:
    StatusCode Run();

    StatusCode ReadSettings(const TiXmlHandle xmlHandle);


/**
 *  @brief  Kink mass calculation
 * 
 *  @param  parentMomentum is cartesian vector of parent track at end 
 *  @param  daughterMomentum is cartesian vector of daughter track at start
 *  @param  daughterMass is assumed mass of daughter track
 *  @param  daughterMass is assumed mass of neutral
 *  @param  mass is the resulting mass of the decay
 */
    StatusCode KinkMass(const pandora::CartesianVector &parentMomentum, const pandora::CartesianVector &daughterMomentum, const float daughterMass, const float neutralMass, float &mass);

/**
 *  @brief  V0 mass calculation
 *  
 *  @param  p1 is cartesian vector of particle 1 at start
 *  @param  p2 is cartesian vector of particle 2 at start
 *  @param  mass1 is assumed mass of particle 1
 *  @param  mass2 is assumed mass of particle 2
 *  @param  mass is the resulting mass of the decay
 */
    StatusCode V0Mass(const pandora::CartesianVector &p1, const pandora::CartesianVector &p2, const float mass1, const float mass2, float &mass);


/**
 *  @brief  Set the PFO parameters to those of the parent
 * 
 *  @param  pfo is the particle flow obeject
 */
    StatusCode SetPfoParametersFromParentTrack(pandora::ParticleFlowObject* pfo);


/**
 *  @brief  Merge the clusters from one Pfo into another and delete list
 * 
 *  @param  pfosToMerge is the list of Pfos to merge with pfo
 *  @param  pfo is the particle flow object accepting the clusters
 */
    StatusCode MergeClustersFromPfoListToPfo(pandora::ParticleFlowObjectList &pfosToMerge, pandora::ParticleFlowObject *pfo);

/**
 *  @brief  Disply MC information for candidate kink
 * 
 *  @param  parentTrack track assumed to be the kink parent
 */
    void DisplayMcInformation(pandora::Track* parentTrack);

 
    // Member variables here

    bool  m_mcMonitoring;                                 ///< printout MC information
    bool  m_monitoring;                                   ///< printout reco information
    float m_maxProjectedDistanceToNeutral;                ///< Max distance from neutral to expected position from decay
    float m_photonDistancePenalty;                        ///< Penalty for photon distances (scale factor)
    float m_chi2CutForMergingPfos;                        ///< Chi2 cut to merge Pfos
    bool  m_findDecaysWithNeutrinos;                      ///< Find decays with neutrinos, k->mu nu, pi->mu nu             
    bool  m_findDecaysWithPiZeros;                        ///< Find decays with pizeros,   k->pi pi0          
    bool  m_findDecaysWithNeutrons;                       ///< Find decays with neutrons   
    bool  m_findDecaysWithLambdas;                        ///< Find decays with lambdas   
    bool  m_findDecaysMergedWithTrackClusters;            ///< Find decays where the neutral is merged in a charged cluster  
    float m_upperCutOnPiZeroMass;                         ///< upper cut on pizero mass
    float m_lowerCutOnPiZeroMass;                         ///< lower cut on pizero mass
    float m_lowerCutOnPiToMuNuMass;                       ///< lower cut on pi decay mass
    float m_upperCutOnPiToMuNuMass;                       ///< upper cut on pi decay mass
    float m_lowerCutOnKToMuNuMass;                        ///< lower cut on K decay mass
    float m_upperCutOnKToMuNuMass;                        ///< upper cut on K decay mass

  
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *KinkPfoCreationAlgorithm::Factory::CreateAlgorithm() const
{
    return new KinkPfoCreationAlgorithm();
}




#endif // #ifndef KINK_PFO_CREATION_ALGORITHM_H
