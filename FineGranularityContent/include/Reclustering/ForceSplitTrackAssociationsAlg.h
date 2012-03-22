/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Reclustering/ForceSplitTrackAssociationsAlg.h
 * 
 *  @brief  Header file for the force split track associations algorithm class.
 * 
 *  $Log: $
 */
#ifndef FORCE_SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H
#define FORCE_SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

/**
 *  @brief  ForceSplitTrackAssociationsAlg class
 */
class ForceSplitTrackAssociationsAlg : public pandora::Algorithm
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
    pandora::StatusCode ReadSettings(const pandora::TiXmlHandle xmlHandle);

    typedef std::map<pandora::Track *, pandora::Cluster *> TrackToClusterMap;

    unsigned int    m_minTrackAssociations;         ///< The minimum number of track associations to forcibly resolve
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ForceSplitTrackAssociationsAlg::Factory::CreateAlgorithm() const
{
    return new ForceSplitTrackAssociationsAlg();
}

#endif // #ifndef FORCE_SPLIT_TRACK_ASSOCIATIONS_ALGORITHM_H
