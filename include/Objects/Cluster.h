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
     *  @brief  Get the list of tracks associated with the cluster
     * 
     *  @return Address of the list of associated tracks
     */
    const TrackList &GetAssociatedTrackList() const;

    /**
     *  @brief  Get calo hits in specified pseudo layer
     * 
     *  @param  pseudoLayer the pseudo layer
     *  @param  pCaloHitList to receive the address of the relevant calo hit list
     */
    StatusCode GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *&pCaloHitList) const;

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
     *  @param  pCaloHitList calo hits with which to create cluster
     */
    Cluster(InputCaloHitList *pCaloHitList);

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
    unsigned int            m_nMipLikeCaloHits;         ///< The number of calo hits that have been flagged as mip-like

    InputFloat              m_electromagneticEnergy;    ///< The sum of electromagnetic energy measures of constituent calo hits, units GeV
    InputFloat              m_hadronicEnergy;           ///< The sum of hadronic energy measures of constituent calo hits, units GeV
    InputFloat              m_bestEnergy;               ///< The best estimate of the cluster energy, units GeV

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

    InputCartesianVector    m_energyWeightedCentroid;   ///< The energy weighted centroid
    InputCartesianVector    m_initialDirection;         ///< The initial direction of the cluster
    InputCartesianVector    m_currentDirection;         ///< The current direction of the cluster

    InputFloat              m_radialDirectionCosine;    ///< The direction cosine (of a straight-line fit) wrt to the radial direction
    InputFloat              m_clusterRMS;               ///< The cluster rms wrt to a straight-line fit to the cluster

    InputPseudoLayer        m_showerMax;                ///< The pseudo layer at which the cluster energy deposition is greatest

    TrackList               m_associatedTrackList;      ///< The list of tracks associated with the cluster

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

inline const TrackList &Cluster::GetAssociatedTrackList() const
{
    return m_associatedTrackList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode Cluster::GetCaloHitsInPseudoLayer(const PseudoLayer pseudoLayer, CaloHitList *&pCaloHitList) const
{
    return m_orderedCaloHitList.GetCaloHitsInPseudoLayer(pseudoLayer, pCaloHitList);
}

} // namespace pandora

#endif // #ifndef CLUSTER_H
