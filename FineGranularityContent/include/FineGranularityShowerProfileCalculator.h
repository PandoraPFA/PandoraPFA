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
    void ReadSettings(const TiXmlHandle *const pXmlHandle);
    void CalculateShowerProfile(const pandora::Cluster *const pCluster, float &profileStart, float &profileDiscrepancy) const;

private:
    static float        m_showerProfileBinWidth;            ///< Bin width used to construct shower profiles, units radiation lengths
    static unsigned int m_showerProfileNBins;               ///< Number of bins used to construct shower profiles
    static float        m_showerProfileMinCosAngle;         ///< Min angular correction used to adjust radiation length measures
    static float        m_showerProfileCriticalEnergy;      ///< Critical energy, used to calculate argument for gamma function
    static float        m_showerProfileParameter0;          ///< Parameter0, used to calculate argument for gamma function
    static float        m_showerProfileParameter1;          ///< Parameter1, used to calculate argument for gamma function
    static float        m_showerProfileMaxDifference;       ///< Max difference between current and best shower profile comparisons
};

#endif // #ifndef FINE_GRANULARITY_SHOWER_PROFILE_CALCULATOR_H
