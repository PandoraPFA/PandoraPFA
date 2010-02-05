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
    StatusCode Run();
    StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    pandora::StringVector   m_fragmentRemovalAlgorithms;    ///< The ordered list of fragment removal algorithms to be used
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *FragmentRemovalAlgorithm::Factory::CreateAlgorithm() const
{
    return new FragmentRemovalAlgorithm();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *ClusterContact::GetDaughterCluster() const
{
    return m_pDaughterCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster *ClusterContact::GetParentCluster() const
{
    return m_pParentCluster;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetParentClusterEnergy() const
{
    return m_parentClusterEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetParentTrackEnergy() const
{
    return m_parentTrackEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ClusterContact::GetNContactLayers() const
{
    return m_nContactLayers;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetContactFraction() const
{
    return m_contactFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetDistanceToClosestHit() const
{
    return m_distanceToClosestHit;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetCloseHitFraction1() const
{
    return m_closeHitFraction1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetCloseHitFraction2() const
{
    return m_closeHitFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetConeFraction1() const
{
    return m_coneFraction1;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetConeFraction2() const
{
    return m_coneFraction2;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetConeFraction3() const
{
    return m_coneFraction3;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetMeanDistanceToHelix() const
{
    return m_meanDistanceToHelix;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ClusterContact::GetClosestDistanceToHelix() const
{
    return m_closestDistanceToHelix;
}

#endif // #ifndef FRAGMENT_REMOVAL_ALGORITHM_H
