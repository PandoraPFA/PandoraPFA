/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/PfoCreationAlgorithm.h
 * 
 *  @brief  Header file for the pfo creation algorithm class.
 * 
 *  $Log: $
 */
#ifndef PFO_CREATION_ALGORITHM_H
#define PFO_CREATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  PfoCreationAlgorithm class
 */
class PfoCreationAlgorithm : public pandora::Algorithm
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
    typedef PandoraContentApi::ParticleFlowObject::Parameters PfoParameters;

    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  Create particle flow objects corresponding to charged particles. These pfos contain tracks from the main tracker
     *          and the clusters associated with these tracks.
     */
    StatusCode CreateChargedPfos() const;

    /**
     *  @brief  Add relevant tracks and clusters to a charged pfo
     * 
     *  @param  pTrack address of the track to consider
     *  @param  pfoParameters the pfo parameters to populate
     *  @param  readSiblingInfo whether to read sibling track information (set to false to avoid multiple counting)
     */
    StatusCode PopulateChargedPfo(const pandora::Track *const pTrack, PfoParameters &pfoParameters, const bool readSiblingInfo = true) const;

    /**
     *  @brief  Create particle flow objects corresponding to neutral particles, These pfos consist only of clusters that have no
     *          associated tracks.
     */
    StatusCode CreateNeutralPfos() const;

    bool            m_shouldUsePhotonClusters;              ///< Whether to create neutral pfos from clusters tagged as photons
    std::string     m_photonClusterListName;                ///< The name of the photon cluster list to use

    float           m_minClusterHadronicEnergy;             ///< Min hadronic energy for neutral (non-photon) clusters to be added to pfos
    unsigned int    m_minHitsInCluster;                     ///< Min number of calo hits for neutral cluster to be added to pfos
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PfoCreationAlgorithm::Factory::CreateAlgorithm() const
{
    return new PfoCreationAlgorithm();
}

#endif // #ifndef PFO_CREATION_ALGORITHM_H
