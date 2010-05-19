/**
 *  @file   PandoraPFANew/include/Algorithms/Monitoring/VisualMonitoringAlgorithm.h
 * 
 *  @brief  Header file for the energy monitoring algorithm
 * 
 *  $Log: $
 */
#ifndef VISUAL_MONITORING_ALGORITHM_H
#define VISUAL_MONITORING_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"


/**
 *  @brief VisualMonitoringAlgorithm class
 */
class VisualMonitoringAlgorithm : public pandora::Algorithm
{
private:
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

    pandora::StringVector m_clusterListNames;            ///< list of strings denoting clusternames 
    bool         m_hits;                                 ///< show current ordered calohitlist
    bool         m_tracks;                               ///< show current tracklist
    bool         m_onlyAvailable;                        ///< show only available  (i.e. non-clustered) calohits and tracks

    bool         m_eve;                                  ///< use ROOT Eve

    bool         m_show;                                 ///< show the output

    std::string  m_detectorView;                         ///< for "normal" display
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *VisualMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new VisualMonitoringAlgorithm();
}

#endif // #ifndef VISUAL_MONITORING_ALGORITHM_H
