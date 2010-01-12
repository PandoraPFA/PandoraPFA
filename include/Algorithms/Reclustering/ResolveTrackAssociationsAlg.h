/**
 *  @file   PandoraPFANew/include/Algorithms/Reclustering/ResolveTrackAssociationsAlg.h
 * 
 *  @brief  Header file for the resolve track associations algorithm class.
 * 
 *  $Log: $
 */
#ifndef RESOLVE_TRACK_ASSOCIATIONS_ALGORITHM_H
#define RESOLVE_TRACK_ASSOCIATIONS_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  ResolveTrackAssociationsAlg class
 */
class ResolveTrackAssociationsAlg : public pandora::Algorithm
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

    typedef std::vector<unsigned int> UIntVector;

    pandora::StringVector   m_clusteringAlgorithms;                 ///< The ordered list of clustering algorithms to be used
    std::string             m_associationAlgorithmName;             ///< The name of the topological association algorithm to run
    std::string             m_trackClusterAssociationAlgName;       ///< The name of the track-cluster association algorithm to run
    std::string             m_splitAssociationAlgName;              ///< 

    unsigned int            m_nTrackAssociationsToResolve;          ///< The exact number of associated tracks required for reclustering
    float                   m_chiToAttemptReclustering;             ///< The min track/cluster chi value required to start reclustering

    float                   m_coneCosineHalfAngle;                  ///< 
    float                   m_minConeFraction;                      ///< 

    float                   m_minClusterEnergyForTrackAssociation;  ///< Energy threshold for recluster candidates with track associations
    float                   m_chi2ForAutomaticClusterSelection;     ///< Chi2 below which recluster candidates are automatically selected
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ResolveTrackAssociationsAlg::Factory::CreateAlgorithm() const
{
    return new ResolveTrackAssociationsAlg();
}

#endif // #ifndef RESOLVE_TRACK_ASSOCIATIONS_ALGORITHM_H
