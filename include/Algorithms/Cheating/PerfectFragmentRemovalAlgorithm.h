/**
 *  @file   PandoraPFANew/include/Algorithms/Cheating/PerfectFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the cheating clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H
#define PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief PerfectFragmentRemovalAlgorithm class
 */
class PerfectFragmentRemovalAlgorithm : public pandora::Algorithm
{
private:
public:


    class pandora::MCParticle;


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

    StatusCode FragmentRemoval( pandora::Cluster* pCluster, float& removedEnergy );

    typedef std::vector<std::string> STRINGVECTOR;

    STRINGVECTOR    m_clusterListNames;                 ///< names of the clusterlists of which fragments should be removed

    bool            m_debug;                            ///< turn on additional debugging output
};


//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectFragmentRemovalAlgorithm();
}

#endif // #ifndef PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H
