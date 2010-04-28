/**
 *  @file   PandoraPFANew/include/Algorithms/TopologicalAssociation/MipPhotonSeparationAlgorithm.h
 * 
 *  @brief  Header file for the mip-photon separation algorithm class.
 * 
 *  $Log: $
 */
#ifndef MIP_PHOTON_SEPARATION_ALGORITHM_H
#define MIP_PHOTON_SEPARATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

using namespace pandora;

/**
 *  @brief  MipPhotonSeparationAlgorithm class
 */
class MipPhotonSeparationAlgorithm : public pandora::Algorithm
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
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  Whether to attempt to fragment a cluster into a mip-like part and a photon-like part
     * 
     *  @param  pCluster address of the cluster
     *  @param  showerStartLayer to receive the shower start layer for the original cluster
     *  @param  showerEndLayer to receive the shower end layer for the original cluster
     * 
     *  @return boolean
     */
    bool ShouldFragmentCluster(Cluster *const pCluster, PseudoLayer &showerStartLayer, PseudoLayer &showerEndLayer) const;

    /**
     *  @brief  Make mip-like and photon-like fragments from a cluster
     * 
     *  @param  showerStartLayer the shower start layer
     *  @param  showerEndLayer the shower end layer
     *  @param  pOriginalCluster address of the original cluster
     *  @param  pMipCluster to receive the address of the mip-like cluster fragment
     *  @param  pPhotonCluster to receive the address of the photon-like cluster fragment
     */
    StatusCode MakeClusterFragments(const PseudoLayer showerStartLayer, const PseudoLayer showerEndLayer, Cluster *const pOriginalCluster,
        Cluster *&pMipCluster, Cluster *&pPhotonCluster) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the ecal surface, ONLY calculated if:
     *          1) the calo hit is within the first m_maxLayersToTrackSeed OR
     *          2) cluster contains a hit consistent with the track seed within the last m_maxLayersToTrackLikeHit
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  the pseudolayer currently being examined
     *  @param  to receive the distance
     */
    StatusCode GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, PseudoLayer searchLayer, float &distance) const;

    /**
     *  @brief  Get the distance between a calo hit and the track seed (projected) position at the ecal surface
     * 
     *  @param  pCluster address of the cluster
     *  @param  pCaloHit address of the calo hit
     *  @param  distance to receive the distance
     */
    StatusCode GetDistanceToTrackSeed(Cluster *const pCluster, CaloHit *const pCaloHit, float &distance) const;

    static const float FLOAT_MAX;
    static const unsigned int LAYER_MAX;

    std::string     m_trackClusterAssociationAlgName;///< The name of the track-cluster association algorithm to run

    unsigned int    m_minMipRegion2Span;            ///< Minimum number of layers spanned by mip 2 region to allow fragmentation
    unsigned int    m_minShowerRegionSpan;          ///< Minimum number of layers spanned by shower region to allow fragmentation
    unsigned int    m_maxShowerStartLayer;          ///< Maximum shower start layer to allow fragmentation

    float           m_nonPhotonDeltaChi2Cut;        ///< Delta chi2 cut for case when photon cluster fragment fails photon id
    float           m_photonDeltaChi2Cut;           ///< Delta chi2 cut for case when photon cluster fragment passes photon id
    unsigned int    m_minHitsInPhotonCluster;       ///< Minimum number of hit in photon cluster fragment

    float           m_genericDistanceCut;           ///< Generic distance cut to apply throughout algorithm

    unsigned int    m_maxLayersToTrackSeed;         ///< Max number of layers to the track seed for trackSeedDistance to be calculated
    unsigned int    m_maxLayersToTrackLikeHit;      ///< Max number of layers to a "track-like" hit for trackSeedDistance to be calculated
    float           m_trackPathWidth;               ///< Track path width, used to determine whether hits are associated with seed track
    float           m_maxTrackSeedSeparation;       ///< Maximum distance between a calo hit and track seed
    float           m_additionalPadWidthsECal;      ///< ECal adjacent pad widths used to calculate cone approach distance
    float           m_additionalPadWidthsHCal;      ///< HCal adjacent pad widths used to calculate cone approach distance
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *MipPhotonSeparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new MipPhotonSeparationAlgorithm();
}

#endif // #ifndef MIP_PHOTON_SEPARATION_ALGORITHM_H
