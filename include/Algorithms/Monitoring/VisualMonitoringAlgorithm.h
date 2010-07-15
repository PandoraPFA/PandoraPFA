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

    pandora::StringVector m_clusterListNames;            ///< List of strings denoting clusternames 
    bool         m_particleFlowObjects;                  ///< Whether to show current particle flow object list
    bool         m_clusters;                             ///< Whether to show current cluster list
    bool         m_hits;                                 ///< Whether to show current ordered calohitlist
    bool         m_tracks;                               ///< Whether to show current tracklist
    bool         m_onlyAvailable;                        ///< Whether to show only available  (i.e. non-clustered) calohits and tracks
    bool         m_displayEvent;                         ///< Whether to display the event
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *VisualMonitoringAlgorithm::Factory::CreateAlgorithm() const
{
    return new VisualMonitoringAlgorithm();
}

#endif // #ifndef VISUAL_MONITORING_ALGORITHM_H
