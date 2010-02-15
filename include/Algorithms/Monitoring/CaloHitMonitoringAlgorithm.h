/**
 *  @file   PandoraPFANew/include/Algorithms/Monitoring/CaloHitMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the calo hit monitoring algorithm class.
 * 
 *  $Log: $
 */
#ifndef CALO_HIT_MONITORING_ALGORITHM_H
#define CALO_HIT_MONITORING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  CaloHitMonitoringAlgorithm class
 */
class CaloHitMonitoringAlgorithm : public pandora::Algorithm
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

    std::string     m_monitoringFileName;       ///< The name of the file in which to save the monitoring histograms
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *CaloHitMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new CaloHitMonitoringAlgorithm();
}

#endif // #ifndef CALO_HIT_MONITORING_ALGORITHM_H