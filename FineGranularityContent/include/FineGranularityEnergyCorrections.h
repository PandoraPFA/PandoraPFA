/**
 *  @file   PandoraPFANew/FineGranularityContent/include/FineGranularityEnergyCorrections.h
 * 
 *  @brief  Header file for the fine granularity energy corrections class.
 * 
 *  $Log: $
 */
#ifndef FINE_GRANULARITY_ENERGY_CORRECTIONS_H
#define FINE_GRANULARITY_ENERGY_CORRECTIONS_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"
#include "Pandora/StatusCodes.h"

#include "Xml/tinyxml.h"

/**
 *  @brief  FineGranularityEnergyCorrections class
 */
class FineGranularityEnergyCorrections
{
public:
    /**
     *  @brief  Correct cluster energy by searching for constituent calo hits with anomalously high energy. Corrections
     *          are made by examining the energy in adjacent layers of the cluster.
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedHadronicEnergy the current corrected hadronic energy measure, which may be modified by this function
     */
    static void CleanCluster(const pandora::Cluster *const pCluster, float &correctedHadronicEnergy);

    /**
     *  @brief  Correct cluster energy by searching for clusters with anomalously high mip energies per constituent calo hit.
     *          Corrections are made by scaling back the mean number of mips per calo hit.
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedHadronicEnergy the current corrected hadronic energy measure, which may be modified by this function
     */
    static void ScaleHotHadronEnergy(const pandora::Cluster *const pCluster, float &correctedHadronicEnergy);

    /**
     *  @brief  Correct energy of clusters containing muon hits, by addressing issue of energy loss in the coil
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedHadronicEnergy the current corrected hadronic energy measure, which may be modified by this function
     */
    static void ApplyMuonEnergyCorrection(const pandora::Cluster *const pCluster, float &correctedHadronicEnergy);

    /**
     *  @brief  Read the energy corrections helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

private:
    /**
     *  @brief  Get the sum of the hadronic energies of all calo hits in a specified layer of an ordered calo hit list
     * 
     *  @param  orderedCaloHitList the ordered calo hit list
     *  @param  pseudoLayer the specified pseudolayer
     */
    static float GetHadronicEnergyInLayer(const pandora::OrderedCaloHitList &orderedCaloHitList, const pandora::PseudoLayer pseudoLayer);

    static bool             m_shouldCleanClusters;          ///< Whether to make cluster cleaning energy corrections
    static bool             m_shouldScaleHotHadrons;        ///< Whether to make hot hadron energy corrections

    static float            m_minCleanHitEnergy;            ///< Min calo hit hadronic energy to consider cleaning hit/cluster
    static float            m_minCleanHitEnergyFraction;    ///< Min fraction of cluster energy represented by hit to consider cleaning
    static float            m_minCleanCorrectedHitEnergy;   ///< Min value of new hit hadronic energy estimate after cleaning

    static unsigned int     m_minHitsForHotHadron;          ///< Min number of hits in a hot hadron candidate cluster
    static unsigned int     m_maxHitsForHotHadron;          ///< Max number of hits in a hot hadron candidate cluster

    static unsigned int     m_hotHadronInnerLayerCut;       ///< Cut 1 of 3 (must fail all for rejection): Min inner layer for hot hadron
    static float            m_hotHadronMipFractionCut;      ///< Cut 2 of 3 (must fail all for rejection): Min mip fraction for hot hadron
    static unsigned int     m_hotHadronNHitsCut;            ///< Cut 3 of 3 (must fail all for rejection): Max number of hits for hot hadron

    static float            m_hotHadronMipsPerHit;          ///< Min number of mips per hit for a hot hadron cluster
    static float            m_scaledHotHadronMipsPerHit;    ///< Scale factor (new mips per hit value) to correct hot hadron energies

    static float            m_muonHitEnergy;                ///< The energy for a digital muon calorimeter hit, units GeV
    static float            m_coilEnergyLossCorrection;     ///< Energy correction due to missing energy deposited in coil, units GeV
    static unsigned int     m_minMuonHitsInInnerLayer;      ///< Min muon hits in muon inner layer to correct charged cluster energy
    static float            m_coilEnergyCorrectionChi;      ///< Track-cluster chi value used to assess need for coil energy correction
};

#endif // #ifndef FINE_GRANULARITY_ENERGY_CORRECTIONS_H
