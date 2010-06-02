/**
 *  @file   PandoraPFANew/include/Helpers/EnergyCorrectionsHelper.h
 * 
 *  @brief  Header file for the calo hit helper class.
 * 
 *  $Log: $
 */
#ifndef ENERGY_CORRECTIONS_HELPER_H
#define ENERGY_CORRECTIONS_HELPER_H 1

#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraInputTypes.h"

#include "StatusCodes.h"

class TiXmlHandle;

//------------------------------------------------------------------------------------------------------------------------------------------

namespace pandora
{

/**
 *  @brief  EnergyCorrectionsHelper class
 */
class EnergyCorrectionsHelper
{
public:
    /**
     *  @brief  Make energy corrections to a cluster
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedElectromagneticEnergy to receive the corrected electromagnetic energy
     *  @param  correctedHadronicEnergy to receive the corrected hadronic energy
     */
    static StatusCode EnergyCorrection(const Cluster *const pCluster, float &correctedElectromagneticEnergy, float &correctedHadronicEnergy);

private:
    /**
     *  @brief  Correct cluster energy by searching for constituent calo hits with anomalously high energy. Corrections
     *          are made by examining the energy in adjacent layers of the cluster.
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedHadronicEnergy the current corrected hadronic energy measure, which may be modified by this function
     */
    static void CleanCluster(const Cluster *const pCluster, float &correctedHadronicEnergy);

    /**
     *  @brief  Correct cluster energy by searching for clusters with anomalously high mip energies per constituent calo hit.
     *          Corrections are made by scaling back the mean number of mips per calo hit.
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedHadronicEnergy the current corrected hadronic energy measure, which may be modified by this function
     */
    static void ScaleHotHadronEnergy(const Cluster *const pCluster, float &correctedHadronicEnergy);

    /**
     *  @brief  Get the sum of the hadronic energies of all calo hits in a specified layer of an ordered calo hit list
     * 
     *  @param  orderedCaloHitList the ordered calo hit list
     *  @param  pseudoLayer the specified pseudolayer
     */
    static float GetHadronicEnergyInLayer(const OrderedCaloHitList &orderedCaloHitList, const PseudoLayer pseudoLayer);

    /**
     *  @brief  Correct energy of clusters containing muon hits, by addressing issue of energy loss in the coil
     * 
     *  @param  pCluster address of the cluster
     *  @param  correctedHadronicEnergy the current corrected hadronic energy measure, which may be modified by this function
     */
    static void ApplyMuonEnergyCorrection(const Cluster *const pCluster, float &correctedHadronicEnergy);

    /**
     *  @brief  Read the energy corrections helper settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle *const pXmlHandle);

    static EnergyCorrectionFunctionVector m_hadEnergyCorrectionFunctions;   ///< The hadronic energy correction function vector
    static EnergyCorrectionFunctionVector m_emEnergyCorrectionFunctions;    ///< The electromagnetic energy correction function vector

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

    friend class PandoraSettings;
    friend class PluginManager;
};

} // namespace pandora

#endif // #ifndef ENERGY_CORRECTIONS_HELPER_H
