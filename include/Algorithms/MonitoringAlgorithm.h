/**
 *  @file   PandoraPFANew/include/Algorithms/MonitoringAlgorithm.h
 * 
 *  @brief  Header file for the monitoring algorithm class.
 * 
 *  $Log: $
 */
#ifndef MONITORING_ALGORITHM_H
#define MONITORING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  MonitoringAlgorithm class
 */
class MonitoringAlgorithm : public pandora::Algorithm
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

inline pandora::Algorithm *MonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new MonitoringAlgorithm();
}

#endif // #ifndef MONITORING_ALGORITHM_H
