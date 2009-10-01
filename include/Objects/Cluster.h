/**
 *  @file   PandoraPFANew/include/Objects/Cluster.h
 * 
 *  @brief  Header file for the cluster class.
 * 
 *  $Log: $
 */
#ifndef CLUSTER_H
#define CLUSTER_H 1

#include "Api/PandoraContentApi.h"

#include "Objects/OrderedCaloHitList.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

class CaloHit;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  Cluster class
 */
class Cluster
{
public:
    /**
     *  @brief  Get the ordered calo hit list
     * 
     *  @return Address of the ordered calo hit list
     */    
    const OrderedCaloHitList &GetOrderedCaloHitList() const;

    /**
     *  @brief  Get calo hits in specified pseudo layer
     * 
     *  @param  pseudoLayer the pseudo layer
     *  @param  pCaloHitList to receive the address of the relevant calo hit list
     */
    StatusCode GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *&pCaloHitList) const;

    /**
     *  @brief  Get the number of calo hits in the cluster
     * 
     *  @return The number of calo hits
     */
    unsigned int GetNCaloHits() const;

    /**
     *  @brief  Get fraction of constituent calo hits that have been flagged as part of a mip track
     * 
     *  @return The mip fraction
     */
    float GetMipFraction() const;

    /**
     *  @brief  Get the sum of electromagnetic energy measures of constituent calo hits, units GeV
     * 
     *  @return The electromagnetic energy measure
     */
    float GetElectromagneticEnergy() const;

    /**
     *  @brief  Get the sum of hadronic energy measures of constituent calo hits, units GeV
     * 
     *  @return The hadronic energy measure
     */
    float GetHadronicEnergy() const;

    /**
     *  @brief  Whether the cluster has been flagged as a photon cluster
     * 
     *  @return boolean 
     */
    bool IsPhoton() const;

    /**
     *  @brief  Get the initial direction of the cluster
     * 
     *  @return The initial direction of the cluster
     */
    const CartesianVector &GetInitialDirection() const;

    /**
     *  @brief  Get the current direction of the cluster
     * 
     *  @return The current direction of the cluster
     */
    const CartesianVector &GetCurrentDirection();

    /**
     *  @brief  Get the energy weighted centroid for the cluster
     * 
     *  @return The energy weighted centroid
     */
    const CartesianVector &GetEnergyWeightedCentroid();

    /**
     *  @brief  Get the best estimate of the cluster energy, units GeV
     * 
     *  @return The best energy estimate
     */
    float GetBestEnergyEstimate();

    /**
     *  @brief  Get the direction cosine (of a straight-line fit) wrt to the radial direction
     * 
     *  @return The direction cosine wrt to the radial direction
     */
    float GetRadialDirectionCosine();

    /**
     *  @brief  Get cluster rms wrt to a straight-line fit to the cluster
     * 
     *  @return The cluster rms 
     */
    float GetRMS();

    /**
     *  @brief  Get the pseudo layer at which the cluster energy deposition is greatest
     * 
     *  @return The pseudo layer at which the cluster energy deposition is greatest
     */
    PseudoLayer GetShowerMax();

    /**
     *  @brief  Get the innermost pseudo layer in the cluster
     * 
     *  @return The innermost pseudo layer in the cluster
     */
    PseudoLayer GetInnerPseudoLayer() const;

    /**
     *  @brief  Get the outermost pseudo layer in the cluster
     * 
     *  @return The outermost pseudo layer in the cluster
     */
    PseudoLayer GetOuterPseudoLayer() const;

    /**
     *  @brief  Get the list of tracks associated with the cluster
     * 
     *  @return Address of the list of associated tracks
     */
    const TrackList &GetAssociatedTrackList() const;

    /**
     *  @brief  Set the best estimate of the cluster energy, units GeV
     * 
     *  @param  bestEnergyEstimate the best energy estimate
     */
    StatusCode SetBestEnergyEstimate(float bestEnergyEstimate);

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  pCaloHit address of calo hit with which initialize cluster
     */
    Cluster(CaloHit *pCaloHit);

    /**
     *  @brief  Constructor
     * 
     *  @param  pCaloHitVector calo hits with which to create cluster
     */
    Cluster(CaloHitVector *pCaloHitVector);

    /**
     *  @brief  Constructor
     * 
     *  @param  pTrack address of track with which to seed cluster
     */
    Cluster(Track *pTrack);

    /**
     *  @brief  Add a calo hit to the cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode AddCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Remove a calo hit from the cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     */
    StatusCode RemoveCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Update the cluster properties
     */
    StatusCode UpdateProperties();

    /**
     *  @brief  Reset the cluster properties
     */
    StatusCode ResetProperties();

    /**
     *  @brief  Add the calo hits from a second cluster to this
     * 
     *  @param  pCluster the address of the second cluster
     */
    StatusCode AddHitsFromSecondCluster(Cluster *const pCluster);

    /**
     *  @brief  Add an association between the cluster and a track
     * 
     *  @param  pTrack the address of the track with which the cluster is associated
     */
    StatusCode AddTrackAssociation(Track *const pTrack);

    /**
     *  @brief  Remove an association between the cluster and a track
     * 
     *  @param  pTrack the address of the track with which the cluster is no longer associated
     */
    StatusCode RemoveTrackAssociation(Track *const pTrack);

    typedef std::map<PseudoLayer, float> ValueByPseudoLayerMap; ///< The value by pseudo layer typedef

    OrderedCaloHitList      m_orderedCaloHitList;       ///< The ordered calo hit list

    unsigned int            m_nCaloHits;                ///< The number of calo hits
    unsigned int            m_nMipTrackHits;            ///< The number of calo hits that have been flagged as part of a mip track

    float                   m_electromagneticEnergy;    ///< The sum of electromagnetic energy measures of constituent calo hits, units GeV
    float                   m_hadronicEnergy;           ///< The sum of hadronic energy measures of constituent calo hits, units GeV

    bool                    m_isPhoton;                 ///< Whether the cluster has been flagged as a photon cluster

    float                   m_sumX;                     ///< The sum of the x coordinates of the constituent calo hits
    float                   m_sumY;                     ///< The sum of the y coordinates of the constituent calo hits
    float                   m_sumZ;                     ///< The sum of the z coordinates of the constituent calo hits

    float                   m_sumXX;                    ///< The sum of the coordinates x*x for the constituent calo hits
    float                   m_sumYY;                    ///< The sum of the coordinates y*y for the constituent calo hits
    float                   m_sumZZ;                    ///< The sum of the coordinates z*z for the constituent calo hits

    float                   m_sumXY;                    ///< The sum of the coordinates x*y for the constituent calo hits
    float                   m_sumXZ;                    ///< The sum of the coordinates x*z for the constituent calo hits
    float                   m_sumYZ;                    ///< The sum of the coordinates y*z for the constituent calo hits

    ValueByPseudoLayerMap   m_sumXByPseudoLayer;        ///< The sum of the x coordinates of the calo hits, stored by pseudo layer
    ValueByPseudoLayerMap   m_sumYByPseudoLayer;        ///< The sum of the y coordinates of the calo hits, stored by pseudo layer
    ValueByPseudoLayerMap   m_sumZByPseudoLayer;        ///< The sum of the z coordinates of the calo hits, stored by pseudo layer

    InputCartesianVector    m_initialDirection;         ///< The initial direction of the cluster
    InputCartesianVector    m_currentDirection;         ///< The current direction of the cluster
    InputCartesianVector    m_energyWeightedCentroid;   ///< The energy weighted centroid

    InputFloat              m_bestEnergy;               ///< The best estimate of the cluster energy, units GeV
    InputFloat              m_radialDirectionCosine;    ///< The direction cosine (of a straight-line fit) wrt to the radial direction
    InputFloat              m_clusterRMS;               ///< The cluster rms wrt to a straight-line fit to the cluster

    InputPseudoLayer        m_showerMax;                ///< The pseudo layer at which the cluster energy deposition is greatest
    InputPseudoLayer        m_innerPseudoLayer;         ///< The innermost pseudo layer in the cluster
    InputPseudoLayer        m_outerPseudoLayer;         ///< The outermost pseudo layer in the cluster

    TrackList               m_associatedTrackList;      ///< The list of tracks associated with the cluster

    bool                    m_isUpToDate;               ///< Whether the cluster properties are up to date

    // TODO add track segment properties

    friend class PandoraContentApiImpl;
    friend class ClusterManager;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const OrderedCaloHitList &Cluster::GetOrderedCaloHitList() const
{
    return m_orderedCaloHitList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Cluster::GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *&pCaloHitList) const
{
    return m_orderedCaloHitList.GetCaloHitsInPseudoLayer(pseudoLayer, pCaloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetNCaloHits() const
{
    return m_nCaloHits;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetMipFraction() const
{
    float mipFraction = 0;

    if (0 != m_nMipTrackHits)
        mipFraction = float (m_nCaloHits) / float (m_nMipTrackHits);

    return mipFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetElectromagneticEnergy() const
{
    return m_electromagneticEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetHadronicEnergy() const
{
    return m_hadronicEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsPhoton() const
{
    return m_isPhoton;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Cluster::GetInitialDirection() const
{
    return m_initialDirection.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Cluster::GetCurrentDirection()
{
    if (!m_isUpToDate)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->UpdateProperties());

    return m_currentDirection.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &Cluster::GetEnergyWeightedCentroid()
{
    if (!m_isUpToDate)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->UpdateProperties());

    return m_energyWeightedCentroid.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetBestEnergyEstimate()
{
    return m_bestEnergy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetRadialDirectionCosine()
{
    if (!m_isUpToDate)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->UpdateProperties());

    return m_radialDirectionCosine.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetRMS()
{
    if (!m_isUpToDate)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->UpdateProperties());

    return m_clusterRMS.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayer Cluster::GetShowerMax()
{
    if (!m_isUpToDate)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->UpdateProperties());

    return m_showerMax.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayer Cluster::GetInnerPseudoLayer() const
{
    return m_innerPseudoLayer.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayer Cluster::GetOuterPseudoLayer() const
{
    return m_outerPseudoLayer.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackList &Cluster::GetAssociatedTrackList() const
{
    return m_associatedTrackList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Cluster::SetBestEnergyEstimate(float bestEnergyEstimate)
{
    if (!(m_bestEnergy = bestEnergyEstimate))
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef CLUSTER_H
