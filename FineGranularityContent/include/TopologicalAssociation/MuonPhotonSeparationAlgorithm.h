/**
 *  @file   PandoraPFANew/FineGranularityContent/include/TopologicalAssociation/MipPhotonSeparationAlgorithm.h
 * 
 *  @brief  Header file for the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */
#ifndef MUON_PHOTON_SEPARATION_ALGORITHM_H
#define MUON_PHOTON_SEPARATION_ALGORITHM_H 1

#include "TopologicalAssociation/MipPhotonSeparationAlgorithm.h"

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
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    pandora::StatusCode PerformFragmentation(pandora::Cluster *const pOriginalCluster, pandora::Track *const pTrack,
        pandora::PseudoLayer showerStartLayer, pandora::PseudoLayer showerEndLayer) const;

    pandora::StatusCode MakeClusterFragments(const pandora::PseudoLayer showerStartLayer, const pandora::PseudoLayer showerEndLayer,
        pandora::Cluster *const pOriginalCluster, pandora::Cluster *&pMipCluster, pandora::Cluster *&pPhotonCluster) const;

    float           m_highEnergyMuonCut;            ///< Cut for muon to be considered high energy
    unsigned int    m_nTransitionLayers;            ///< Number of transition layers, treated more flexibly, between shower and mip-region
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MuonPhotonSeparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new MuonPhotonSeparationAlgorithm();
}

#endif // #ifndef MUON_PHOTON_SEPARATION_ALGORITHM_H
