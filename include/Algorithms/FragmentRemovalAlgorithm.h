/**
 *  @file   PandoraPFANew/include/Algorithms/FragmentRemovalAlgorithm.h
 * 
 *  @brief  Header file for the fragment removal algorithm class.
 * 
 *  $Log: $
 */
#ifndef FRAGMENT_REMOVAL_ALGORITHM_H
#define FRAGMENT_REMOVAL_ALGORITHM_H 1

#include "Algorithms/Algorithm.h"

using namespace pandora;

/**
 *  @brief  FragmentRemovalAlgorithm class
 */
class FragmentRemovalAlgorithm : public Algorithm
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
    /**
     *  @brief  ClusterContact class
     */
    class ClusterContact
    {
    public:
        /**
         *  @brief  Constructor
         * 
         *  @param  pDaughterCluster
         *  @param  pParentCluster
         */
        ClusterContact(Cluster *const pDaughterCluster, Cluster *const pParentCluster);

        /**
         *  @brief  
         * 
         *  @return  
         */
        Cluster *GetDaughterCluster() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        Cluster *GetParentCluster() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetParentClusterEnergy() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetParentTrackEnergy() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        unsigned int GetNContactLayers() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetContactFraction() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetDistanceToClosestHit() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetCloseHitFraction1() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetCloseHitFraction2() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetConeFraction1() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetConeFraction2() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetConeFraction3() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetMeanDistanceToHelix() const;

        /**
         *  @brief  
         * 
         *  @return  
         */
        float GetClosestDistanceToHelix() const;

    private:
        Cluster        *m_pDaughterCluster;             ///< 
        Cluster        *m_pParentCluster;               ///< 

        float           m_parentClusterEnergy;          ///< 
        float           m_parentTrackEnergy;            ///< 

        unsigned int    m_nContactLayers;               ///< 
        float           m_contactFraction;              ///< 

        float           m_distanceToClosestHit;         ///< 
        float           m_closeHitFraction1;            ///< 
        float           m_closeHitFraction2;            ///< 

        float           m_coneFraction1;                ///< 
        float           m_coneFraction2;                ///< 
        float           m_coneFraction3;                ///< 

        float           m_meanDistanceToHelix;          ///< 
        float           m_closestDistanceToHelix;       ///< 
    };

    typedef std::vector<ClusterContact> ClusterContactVector;
    typedef std::map<Cluster *, ClusterContactVector> ClusterContactMap;

    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     */
    StatusCode GetClusterContactMap(bool &isFirstPass, const ClusterList &affectedClusters, ClusterContactMap &clusterContactMap,
        const Cluster *const pBestParentCluster, const Cluster *const pBestDaughterCluster) const;

    /**
     *  @brief  
     * 
     *  @param  
     */
    bool PassesClusterContactCuts(const ClusterContact &clusterContact) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     */
    StatusCode GetClusterMergingCandidates(const ClusterContactMap &clusterContactMap, Cluster *&pBestParentCluster,
        Cluster *&pBestDaughterCluster) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     * 
     *  @return boolean
     */
    bool PassesPreselection(Cluster *const pDaughterCluster, const ClusterContactVector &clusterContactVector) const;

    /**
     *  @brief  
     * 
     *  @param  
     * 
     *  @return
     */
    float GetTotalEvidenceForMerge(const ClusterContact &clusterContact) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     * 
     *  @return
     */
    float GetRequiredEvidenceForMerge(const float daughterClusterEnergy, const PseudoLayer correctionLayer, const ClusterContact &clusterContact) const;

    /**
     *  @brief  
     * 
     *  @param  
     * 
     *  @return
     */
    PseudoLayer GetClusterCorrectionLayer(const Cluster *const pDaughterCluster) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     * 
     *  @return 
     */
    float GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy) const;

    /**
     *  @brief  
     * 
     *  @param  
     *  @param  
     *  @param  
     *  @param  
     */
    StatusCode GetAffectedClusters(const ClusterContactMap &clusterContactMap, Cluster *const pBestParentCluster,
        Cluster *const pBestDaughterCluster, ClusterList &affectedClusters) const;

    float           m_contactWeight;                ///< 
    float           m_coneWeight;                   ///< 
    float           m_distanceWeight;               ///< 
    float           m_trackExtrapolationWeight;     ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *FragmentRemovalAlgorithm::ClusterContact::GetDaughterCluster() const
{
    return m_pDaughterCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *FragmentRemovalAlgorithm::ClusterContact::GetParentCluster() const
{
    return m_pParentCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetParentClusterEnergy() const
{
    return m_parentClusterEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetParentTrackEnergy() const
{
    return m_parentTrackEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int FragmentRemovalAlgorithm::ClusterContact::GetNContactLayers() const
{
    return m_nContactLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetContactFraction() const
{
    return m_contactFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetDistanceToClosestHit() const
{
    return m_distanceToClosestHit;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetCloseHitFraction1() const
{
    return m_closeHitFraction1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetCloseHitFraction2() const
{
    return m_closeHitFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetConeFraction1() const
{
    return m_coneFraction1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetConeFraction2() const
{
    return m_coneFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetConeFraction3() const
{
    return m_coneFraction3;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetMeanDistanceToHelix() const
{
    return m_meanDistanceToHelix;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float FragmentRemovalAlgorithm::ClusterContact::GetClosestDistanceToHelix() const
{
    return m_closestDistanceToHelix;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline Algorithm *FragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new FragmentRemovalAlgorithm();
}

#endif // #ifndef FRAGMENT_REMOVAL_ALGORITHM_H
