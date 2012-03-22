/**
 *  @file   PandoraPFANew/FineGranularityContent/include/FineGranularityShowerProfileCalculator.h
 * 
 *  @brief  Header file for the fine granularity pseudo layer calculator class.
 * 
 *  $Log: $
 */
#ifndef FINE_GRANULARITY_SHOWER_PROFILE_CALCULATOR_H
#define FINE_GRANULARITY_SHOWER_PROFILE_CALCULATOR_H 1

#include "Utilities/ShowerProfileCalculator.h"

/**
 *  @brief  FineGranularityShowerProfileCalculator class
 */
class FineGranularityShowerProfileCalculator : public pandora::ShowerProfileCalculator
{
public:
    void ReadSettings(const pandora::TiXmlHandle *const pXmlHandle);
    void CalculateLongitudinalProfile(const pandora::Cluster *const pCluster, float &profileStart, float &profileDiscrepancy) const;
    void CalculateTransverseProfile(const pandora::Cluster *const pCluster, const pandora::PseudoLayer maxPseudoLayer, pandora::ParticleIdHelper::ShowerPeakList &showerPeakList) const;

private:
    /**
     *  @brief  ShowerProfileEntry class
     */
    class ShowerProfileEntry
    {
    public:
        /**
         *  @brief  Default constructor
         */
        ShowerProfileEntry();

        bool                        m_isAvailable;          ///< Whether shower profile entry is available (prevent double counting)
        float                       m_energy;               ///< The energy associated with the shower profile entry
        pandora::CaloHitList        m_caloHitList;          ///< The list of calo hits associated with the shower profile entry
    };

    typedef std::vector<ShowerProfileEntry> ShowerProfile;  ///< The shower profile typedef
    typedef std::vector<ShowerProfile> TwoDShowerProfile;   ///< The two dimensional shower profile typedef

    static float        m_longProfileBinWidth;              ///< Bin width used to construct longitudinal profile, units radiation lengths
    static unsigned int m_longProfileNBins;                 ///< Number of bins used to construct longitudinal profile
    static float        m_longProfileMinCosAngle;           ///< Min angular correction used to adjust radiation length measures
    static float        m_longProfileCriticalEnergy;        ///< Critical energy, used to calculate argument for gamma function
    static float        m_longProfileParameter0;            ///< Parameter0, used to calculate argument for gamma function
    static float        m_longProfileParameter1;            ///< Parameter1, used to calculate argument for gamma function
    static float        m_longProfileMaxDifference;         ///< Max difference between current and best longitudinal profile comparisons

    static int          m_transProfileNBins;                ///< Number of bins used to construct transverse profile
    static float        m_transProfilePeakThreshold;        ///< Minimum electrogmagnetic energy for profile peak bin, units GeV
    static float        m_transProfileNearbyEnergyRatio;    ///< Max ratio of bin energy to nearby bin energy; used to identify peak extent
    static unsigned int m_transProfileMaxPeaksToFind;       ///< Maximum number of peaks to identify in transverse profile
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline FineGranularityShowerProfileCalculator::ShowerProfileEntry::ShowerProfileEntry() :
    m_isAvailable(true),
    m_energy(0.f)
{
}

#endif // #ifndef FINE_GRANULARITY_SHOWER_PROFILE_CALCULATOR_H
