/**
 *  @file   PandoraPFANew/include/Algorithms/PfoConstruction/ClusterPreparationAlgorithm.h
 * 
 *  @brief  Header file for the cluster preparation algorithm class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_PREPARATION_ALGORITHM_H
#define CLUSTER_PREPARATION_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ClusterPreparationAlgorithm class
 */
class ClusterPreparationAlgorithm : public pandora::Algorithm
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
     *  @brief  Correct cluster energies by searching for constituent calo hits with anomalously high energy. Corrections
     *          are made by examining the energy in adjacent layers of the cluster.
     */
    StatusCode CleanClusters() const;

    /**
     *  @brief  Get the sum of the hadronic energies of all calo hits in a specified layer of an ordered calo hit list
     * 
     *  @param  orderedCaloHitList the ordered calo hit list
     *  @param  pseudoLayer the specified pseudolayer
     */
    float GetHadronicEnergyInLayer(const pandora::OrderedCaloHitList &orderedCaloHitList, const pandora::PseudoLayer pseudoLayer) const;

    /**
     *  @brief  Correct cluster energies by searching for clusters with anomalously high mip energies per constituent calo hit.
     *          Corrections are made by scaling back the mean number of mips per calo hit.
     */
    StatusCode ScaleHotHadronEnergy() const;

    pandora::StringVector   m_candidateListNames;           ///< The list of cluster list names to use
    std::string             m_finalPfoListName;             ///< The name of the output cluster list

    float                   m_minCleanHitEnergy;            ///< Min calo hit hadronic energy to consider cleaning hit/cluster
    float                   m_minCleanHitEnergyFraction;    ///< Min fraction of cluster energy represented by hit to consider cleaning
    float                   m_minCleanCorrectedHitEnergy;   ///< Min value of new hit hadronic energy estimate after cleaning

    unsigned int            m_minHitsForHotHadron;          ///< Min number of hits in a hot hadron candidate cluster
    unsigned int            m_maxHitsForHotHadron;          ///< Max number of hits in a hot hadron candidate cluster

    unsigned int            m_hotHadronInnerLayerCut;       ///< Cut 1 of 3 (must fail all for rejection): Min inner layer for hot hadron
    float                   m_hotHadronMipFractionCut;      ///< Cut 2 of 3 (must fail all for rejection): Min mip fraction for hot hadron
    unsigned int            m_hotHadronNHitsCut;            ///< Cut 3 of 3 (must fail all for rejection): Max number of hits for hot hadron

    float                   m_hotHadronMipsPerHit;          ///< Min number of mips per hit for a hot hadron cluster
    float                   m_scaledHotHadronMipsPerHit;    ///< Scale factor (new mips per hit value) to correct hot hadron energies
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ClusterPreparationAlgorithm::Factory::CreateAlgorithm() const
{
    return new ClusterPreparationAlgorithm();
}

#endif // #ifndef CLUSTER_PREPARATION_ALGORITHM_H
