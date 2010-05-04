/**
 *  @file   PandoraPFANew/include/Algorithms/PhotonRecoveryAlgorithm.h
 * 
 *  @brief  Header file for the photon recovery algorithm class.
 * 
 *  $Log: $
 */
#ifndef PHOTON_RECOVERY_ALGORITHM_H
#define PHOTON_RECOVERY_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

/**
 *  @brief  PhotonRecoveryAlgorithm class
 */
class PhotonRecoveryAlgorithm : public pandora::Algorithm
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

    /**
     *  @brief  
     * 
     *  @param  
     * 
     *  @return 
     */
    pandora::PseudoLayer GetLayer90(const pandora::Cluster *const pCluster) const;

    /**
     *  @brief  
     * 
     *  @param  
     * 
     *  @return 
     */
    float GetBarrelEndCapEnergySplit(const pandora::Cluster *const pCluster) const;

    std::string     m_trackClusterAssociationAlgName;   ///< The name of the track-cluster association algorithm to run
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonRecoveryAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonRecoveryAlgorithm();
}

#endif // #ifndef PHOTON_RECOVERY_ALGORITHM_H
