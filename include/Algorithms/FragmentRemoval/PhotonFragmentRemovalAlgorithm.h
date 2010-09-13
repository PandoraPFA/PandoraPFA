/**
 *  @file   PandoraPFANew/include/Algorithms/FragmentRemoval/PhotonFragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the photon fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef PHOTON_FRAGMENT_REMOVAL_ALGORITHM_H
#define PHOTON_FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

using namespace pandora;

/**
 *  @brief  PhotonClusterContact class, describing the interactions and proximity between parent and daughter candidate clusters
 */
class PhotonClusterContact
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  pParentCluster address of the parent candidate cluster
     */
    PhotonClusterContact(Cluster *const pDaughterCluster, Cluster *const pParentCluster);

    /**
     *  @brief  Get the address of the daughter candidate cluster
     * 
     *  @return The address of the daughter candidate cluster
     */
    Cluster *GetDaughterCluster() const;

    /**
     *  @brief  Get the address of the parent candidate cluster
     * 
     *  @return The address of the parent candidate cluster
     */
    Cluster *GetParentCluster() const;

    /**
     *  @brief  Get the number of contact layers for parent and daughter clusters two clusters
     * 
     *  @return The number of contact layers
     */
    unsigned int GetNContactLayers() const;

    /**
     *  @brief  Get the ratio of the number of contact layers to the number of overlap layers
     * 
     *  @return The ratio of contact layers to overlap layers
     */
    float GetContactFraction() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within specified cone along parent direction
     * 
     *  @return The daughter cone fraction
     */
    float GetConeFraction() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within sepcified distance 1 of parent cluster
     * 
     *  @return The daughter close hit fraction
     */
    float GetCloseHitFraction1() const;

    /**
     *  @brief  Get the fraction of daughter hits that lie within sepcified distance 2 of parent cluster
     * 
     *  @return The daughter close hit fraction
     */
    float GetCloseHitFraction2() const;

    /**
     *  @brief  Distance between closest hits in parent and daughter clusters, units mm
     * 
     *  @return The distance between closest hits
     */
    float GetDistanceToClosestHit() const;

private:
    /**
     *  @brief  Compare hits in daughter cluster with those in parent cluster to calculate minimum hit separation
     *          and close hit fractions. Calculate these properties in a single loop, for efficiency.
     * 
     *  @param  pDaughterCluster address of the daughter candidate cluster
     *  @param  pParentCluster address of the parent candidate cluster
     */
    void HitDistanceComparison(Cluster *const pDaughterCluster, Cluster *const pParentCluster);

    Cluster                *m_pDaughterCluster;             ///< Address of the daughter candidate cluster
    Cluster                *m_pParentCluster;               ///< Address of the parent candidate cluster

    unsigned int            m_nContactLayers;               ///< The number of contact layers for parent and daughter clusters two clusters
    float                   m_contactFraction;              ///< The ratio of the number of contact layers to the number of overlap layers
    float                   m_coneFraction;                 ///< Fraction of daughter hits that lie within specified cone along parent direction
    float                   m_closeHitFraction1;            ///< Fraction of daughter hits that lie within sepcified distance 1 of parent cluster
    float                   m_closeHitFraction2;            ///< Fraction of daughter hits that lie within sepcified distance 2 of parent cluster
    float                   m_distanceToClosestHit;         ///< Distance between closest hits in parent and daughter clusters, units mm

    static float            m_distanceThreshold;            ///< Number of calorimeter cell-widths used to identify cluster contact layers
    static float            m_coneCosineHalfAngle;          ///< Cosine half angle for cone comparison in cluster contact object
    static float            m_closeHitDistance1;            ///< First distance used to identify close hits in cluster contact object
    static float            m_closeHitDistance2;            ///< Second distance used to identify close hits in cluster contact object

    friend class PhotonFragmentRemovalAlgorithm;
};

typedef std::vector<PhotonClusterContact> PhotonClusterContactVector;
typedef std::map<Cluster *, PhotonClusterContactVector> PhotonClusterContactMap;

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PhotonFragmentRemovalAlgorithm class
 */
class PhotonFragmentRemovalAlgorithm : public Algorithm
{
public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public AlgorithmFactory
    {
    public:
        Algorithm *CreateAlgorithm() const;
    };

private:
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  Get cluster contact map, linking each daughter candidate cluster to a list of parent candidates and describing
     *          the proximity/contact between each pairing
     * 
     *  @param  isFirstPass whether this is the first call to GetClusterContactMap
     *  @param  affectedClusters list of those clusters affected by previous cluster merging, for which contact details must be updated
     *  @param  clusterContactMap to receive the populated cluster contact map
     */
    StatusCode GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters, PhotonClusterContactMap &clusterContactMap) const;

    /**
     *  @brief  Whether candidate daughter cluster can be considered as photon-like
     * 
     *  @param  pDaughterCluster address of the candidate daughter cluster
     * 
     *  @return boolean
     */
    bool IsPhotonLike(Cluster *const pDaughterCluster) const;

    /**
     *  @brief  Whether candidate parent and daughter clusters are sufficiently in contact to warrant further investigation
     * 
     *  @param  clusterContact
     * 
     *  @return boolean
     */
    bool PassesClusterContactCuts(const PhotonClusterContact &clusterContact) const;

    /**
     *  @brief  Find the best candidate parent and daughter clusters for fragment removal merging
     * 
     *  @param  clusterContactMap the populated cluster contact map
     *  @param  pBestParentCluster to receive the address of the best parent cluster candidate
     *  @param  pBestDaughterCluster to receive the address of the best daughter cluster candidate
     */
    StatusCode GetClusterMergingCandidates(const PhotonClusterContactMap &clusterContactMap, Cluster *&pBestParentCluster,
        Cluster *&pBestDaughterCluster) const;

    /**
     *  @brief  Get a measure of the evidence for merging the parent and daughter candidate clusters
     * 
     *  @param  clusterContact the cluster contact details for parent/daughter candidate merge
     * 
     *  @return the evidence
     */
    float GetEvidenceForMerge(const PhotonClusterContact &clusterContact) const;

    /**
     *  @brief  Get the list of clusters for which cluster contact information will be affected by a specified cluster merge
     * 
     *  @param  clusterContactMap the cluster contact map
     *  @param  pBestParentCluster address of the parent cluster to be merged
     *  @param  pBestDaughterCluster address of the daughter cluster to be merged
     *  @param  affectedClusters to receive the list of affected clusters
     */
    StatusCode GetAffectedClusters(const PhotonClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
        Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const;

    unsigned int        m_nMaxPasses;                               ///< Maximum number of passes over cluster contact information

    unsigned int        m_minDaughterCaloHits;                      ///< Min number of calo hits in daughter candidate clusters
    float               m_minDaughterHadronicEnergy;                ///< Min hadronic energy for daughter candidate clusters
    unsigned int        m_innerLayerTolerance;                      ///< Max number of layers by which daughter can exceed parent inner layer
    float               m_minCosOpeningAngle;                       ///< Min cos opening angle between candidate cluster initial directions

    unsigned int        m_photonLikeMaxInnerLayer;                  ///< Max inner layer to identify daughter cluster as photon-like
    float               m_photonLikeMinDCosR;                       ///< Max radial direction cosine to identify daughter as photon-like
    float               m_photonLikeMaxShowerStart;                 ///< Max shower profile start to identify daughter as photon-like
    float               m_photonLikeMaxProfileDiscrepancy;          ///< Max shower profile discrepancy to identify daughter as photon-like

    float               m_contactCutMaxDistance;                    ///< Max distance between closest hits to store cluster contact info
    unsigned int        m_contactCutNLayers;                        ///< Number of contact layers to store cluster contact info
    float               m_contactCutConeFraction;                   ///< Cone fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction1;              ///< Close hit fraction 1 value to store cluster contact info
    float               m_contactCutCloseHitFraction2;              ///< Close hit fraction 2 value to store cluster contact info

    unsigned int        m_contactEvidenceNLayers;                   ///< Contact layers required for contact evidence contribution
    float               m_contactEvidenceFraction;                  ///< Contact fraction required for contact evidence contribution
    float               m_coneEvidenceFraction;                     ///< Cone fraction value required for cone evidence contribution
    float               m_distanceEvidence1;                        ///< Offset for distance evidence contribution 1
    float               m_distanceEvidence1d;                       ///< Denominator for distance evidence contribution 1
    float               m_distanceEvidenceCloseFraction1Multiplier; ///< Distance evidence multiplier for close hit fraction 1
    float               m_distanceEvidenceCloseFraction2Multiplier; ///< Distance evidence multiplier for close hit fraction 2

    float               m_contactWeight;                            ///< Weight for layers in contact evidence
    float               m_coneWeight;                               ///< Weight for cone extrapolation evidence
    float               m_distanceWeight;                           ///< Weight for distance of closest approach evidence

    float               m_minEvidence;                              ///< Min evidence before parent/daughter candidates can be merged
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *PhotonClusterContact::GetDaughterCluster() const
{
    return m_pDaughterCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *PhotonClusterContact::GetParentCluster() const
{
    return m_pParentCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int PhotonClusterContact::GetNContactLayers() const
{
    return m_nContactLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PhotonClusterContact::GetContactFraction() const
{
    return m_contactFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PhotonClusterContact::GetConeFraction() const
{
    return m_coneFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PhotonClusterContact::GetCloseHitFraction1() const
{
    return m_closeHitFraction1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PhotonClusterContact::GetCloseHitFraction2() const
{
    return m_closeHitFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float PhotonClusterContact::GetDistanceToClosestHit() const
{
    return m_distanceToClosestHit;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *PhotonFragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new PhotonFragmentRemovalAlgorithm();
}

#endif // #ifndef PHOTON_FRAGMENT_REMOVAL_ALGORITHM_H
