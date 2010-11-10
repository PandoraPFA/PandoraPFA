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

#include "Pandora/PandoraInternal.h"

/**
 *  @brief PerfectFragmentRemovalAlgorithm class
 */
class PerfectFragmentRemovalAlgorithm : public pandora::Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

private:
    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  FragmentRemoval
     * 
     *  @param  pCluster
     *  @param  removedEnergy
     */
    pandora::StatusCode FragmentRemoval(pandora::Cluster *pCluster, float &removedEnergy) const;

    pandora::StringVector   m_clusterListNames;             ///< names of the clusterlists of which fragments should be removed
    bool                    m_debug;                        ///< turn on additional debugging output
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PerfectFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new PerfectFragmentRemovalAlgorithm();
}

#endif // #ifndef PERFECT_FRAGMENT_REMOVAL_ALGORITHM_H
