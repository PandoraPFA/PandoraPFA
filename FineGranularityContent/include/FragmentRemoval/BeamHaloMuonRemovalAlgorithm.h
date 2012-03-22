/**
 *  @file   PandoraPFANew/FineGranularityContent/include/FragmentRemoval/BeamHaloMuonRemovalAlgorithm.h
 * 
 *  @brief  Header file for the beam halo muon removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef BEAM_HALO_MUON_REMOVAL_ALGORITHM_H
#define BEAM_HALO_MUON_REMOVAL_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  BeamHaloRemovalAlgorithm class
 */
class BeamHaloMuonRemovalAlgorithm : public pandora::Algorithm
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
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

   /**
     *  @brief  Whether a cluster is a candidate beam halo muon based on inertia measurements
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    bool IsBeamHaloMuonInertia(pandora::Cluster *pCluster) const;

    /**
     *  @brief  Whether a cluster is a candidate beam halo muon
     * 
     *  @param  pCluster address of the cluster
     * 
     *  @return boolean
     */
    bool IsBeamHaloMuon(pandora::Cluster *pCluster) const;

    bool            m_monitoring;                                   ///< Whether to display monitoring information
    bool            m_displayRetainedClusters;                      ///< Whether to display monitoring information concerning selected clusters
    bool            m_displayRejectedClusters;                      ///< Whether to display monitoring information concerning rejected clusters
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *BeamHaloMuonRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new BeamHaloMuonRemovalAlgorithm();
}

#endif // #ifndef BEAM_HALO_MUON_REMOVAL_ALGORITHM_H
