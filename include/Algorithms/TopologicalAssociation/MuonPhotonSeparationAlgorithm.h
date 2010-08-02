/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/MipPhotonSeparationAlgorithm.h
 * 
 *  @brief  Header file for the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */
#ifndef MUON_PHOTON_SEPARATION_ALGORITHM_H
#define MUON_PHOTON_SEPARATION_ALGORITHM_H 1

#include "Algorithms/TopologicalAssociation/MipPhotonSeparationAlgorithm.h"

/**
 *  @brief  MuonPhotonSeparationAlgorithm class
 */
class MuonPhotonSeparationAlgorithm : public MipPhotonSeparationAlgorithm
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

private:
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    StatusCode PerformFragmentation(pandora::Cluster *const pOriginalCluster, pandora::Track *const pTrack,
        pandora::PseudoLayer showerStartLayer, pandora::PseudoLayer showerEndLayer) const;

    StatusCode MakeClusterFragments(const pandora::PseudoLayer showerStartLayer, const pandora::PseudoLayer showerEndLayer,
        pandora::Cluster *const pOriginalCluster, pandora::Cluster *&pMipCluster, pandora::Cluster *&pPhotonCluster) const;

    float           m_highEnergyMuonCut;            ///< Cut for muon to be considered high energy
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MuonPhotonSeparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new MuonPhotonSeparationAlgorithm();
}

#endif // #ifndef MUON_PHOTON_SEPARATION_ALGORITHM_H
