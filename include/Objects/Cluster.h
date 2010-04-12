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

#include "Helpers/ClusterHelper.h"

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
     *  @brief  Sort clusters by ascending inner layer, and by descending mip fraction within a layer
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortByInnerLayer(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    /**
     *  @brief  Sort clusters by ascending inner layer, and by descending mip fraction within a layer, using
     *          hadronic energy as a final discrimant to enable a unique ordering
     * 
     *  @param  pLhs address of first cluster
     *  @param  pRhs address of second cluster
     */
    static bool SortByInnerLayerIncEnergy(const pandora::Cluster *const pLhs, const pandora::Cluster *const pRhs);

    /**
     *  @brief  Get the ordered calo hit list
     * 
     *  @return The ordered calo hit list by reference
     */
    const OrderedCaloHitList &GetOrderedCaloHitList() const;

    /**
     *  @brief  Get the isolated calo hit list
     * 
     *  @return The isolated calo hit list by reference
     */
    const CaloHitList &GetIsolatedCaloHitList() const;

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
     *  @brief  Get the number of isolated calo hits in the cluster
     * 
     *  @return The number of isolated calo hits
     */
    unsigned int GetNIsolatedCaloHits() const;

    /**
     *  @brief  Get the number of calo hits in the cluster that have been flagged as possible mip hits
     * 
     *  @return The number of possible mip hits
     */
    unsigned int GetNPossibleMipHits() const;

    /**
     *  @brief  Get fraction of constituent calo hits that have been flagged as possible mip hits
     * 
     *  @return The mip fraction
     */
    float GetMipFraction() const;

    /**
     *  @brief  Get the sum of electromagnetic energy measures of all constituent calo hits, units GeV
     * 
     *  @return The electromagnetic energy measure
     */
    float GetElectromagneticEnergy() const;

    /**
     *  @brief  Get the sum of hadronic energy measures of all constituent calo hits, units GeV
     * 
     *  @return The hadronic energy measure
     */
    float GetHadronicEnergy() const;

    /**
     *  @brief  Get the sum of electromagnetic energy measures of isolated constituent calo hits, units GeV
     * 
     *  @return The electromagnetic energy measure
     */
    float GetIsolatedElectromagneticEnergy() const;

    /**
     *  @brief  Get the sum of hadronic energy measures of isolated constituent calo hits, units GeV
     * 
     *  @return The hadronic energy measure
     */
    float GetIsolatedHadronicEnergy() const;

    /**
     *  @brief  Whether the cluster has been flagged as a photon
     * 
     *  @return boolean
     */
    bool IsPhoton() const;

    /**
     *  @brief  Whether the cluster has been flagged as a section of mip track
     * 
     *  @return boolean
     */
    bool IsMipTrack() const;

    /**
     *  @brief  Whether the cluster is track seeded
     * 
     *  @return boolean 
     */
    bool IsTrackSeeded() const;

    /**
     *  @brief  Get the address of the track with which the cluster is seeded
     * 
     *  @return address of the track seed
     */
    const Track *const GetTrackSeed() const;

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
     *  @brief  Whether the cluster contains a calo hit flagged as being in an outer sampling layer
     * 
     *  @return boolean
     */
    bool ContainsHitInOuterSamplingLayer() const;

    /**
     *  @brief  Get the centroid for the cluster at a particular pseudo layer
     * 
     *  @param  pseudoLayer the pseudo layer of interest
     * 
     *  @return The centroid, returned by value
     */
    const CartesianVector GetCentroid(PseudoLayer pseudoLayer) const;

    /**
     *  @brief  Get the initial direction of the cluster
     * 
     *  @return The initial direction of the cluster
     */
    const CartesianVector &GetInitialDirection() const;

    /**
     *  @brief  Get the current linear fit result, usually set by a clustering algorithm, as cluster grows
     * 
     *  @return The cluster fit result
     */
    const ClusterHelper::ClusterFitResult &GetCurrentFitResult() const;

    /**
     *  @brief  Get the result of a linear fit to all calo hits in the cluster
     * 
     *  @return The cluster fit result
     */
    const ClusterHelper::ClusterFitResult &GetFitToAllHitsResult();

    /**
     *  @brief  Get the best estimate of the cluster energy, units GeV
     * 
     *  @return The best energy estimate
     */
    float GetBestEnergyEstimate() const;

    /**
     *  @brief  Whether the cluster has been flagged as a photon by fast photon id function
     * 
     *  @return boolean
     */
    bool IsPhotonFast();

    /**
     *  @brief  Get the pseudo layer at which shower commences
     * 
     *  @return The pseudo layer at which shower commences
     */
    PseudoLayer GetShowerStartLayer();

    /**
     *  @brief  Get the cluster shower profile start, units radiation lengths
     * 
     *  @return The cluster shower profile start
     */
    float GetShowerProfileStart();

    /**
     *  @brief  Get the cluster shower profile discrepancy
     * 
     *  @return The cluster shower profile discrepancy
     */
    float GetShowerProfileDiscrepancy();

    /**
     *  @brief  Get the list of tracks associated with the cluster
     * 
     *  @return Address of the list of associated tracks
     */
    const TrackList &GetAssociatedTrackList() const;

    /**
     *  @brief  Set the is photon flag for the cluster
     * 
     *  @param  isPhotonFlag the is photon flag
     */
    void SetIsPhotonFlag(bool isPhotonFlag);

    /**
     *  @brief  Set the is mip track for the cluster
     * 
     *  @param  isMipTrackFlag the is mip track flag
     */
    void SetIsMipTrackFlag(bool isMipTrackFlag);

    /**
     *  @brief  Set the best estimate of the cluster energy, units GeV
     * 
     *  @param  bestEnergyEstimate the best energy estimate
     */
    void SetBestEnergyEstimate(float bestEnergyEstimate);

    /**
     *  @brief  Set the result of the current linear fit to the cluster. This function is usually called by a clustering
     *          algorithm, as the cluster grows
     * 
     *  @param  currentFitResult the current fit result
     */
    void SetCurrentFitResult(const ClusterHelper::ClusterFitResult &currentFitResult);

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
    Cluster(CaloHitList *pCaloHitList);

    /**
     *  @brief  Constructor
     * 
     *  @param  pTrack address of track with which to seed cluster
     */
    Cluster(Track *pTrack);

    /**
     *  @brief  Destructor
     */
    ~Cluster();

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
     *  @brief  Add an isolated calo hit to the cluster.
     * 
     *  @param  pCaloHit the address of the isolated calo hit
     */
    StatusCode AddIsolatedCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Remove an isolated calo hit from the cluster
     * 
     *  @param  pCaloHit the address of the isolated calo hit
     */
    StatusCode RemoveIsolatedCaloHit(CaloHit *const pCaloHit);

    /**
     *  @brief  Calculate the fast photon flag
     */
    void CalculateFastPhotonFlag();

    /**
     *  @brief  Calculate the pseudo layer at which shower commences
     */
    void CalculateShowerStartLayer();

    /**
     *  @brief  Calculate shower profile and compare it with the expected profile for a photon
     */
    void CalculateShowerProfile();

    /**
     *  @brief  Calculate result of a linear fit to all calo hits in the cluster
     */
    void CalculateFitToAllHitsResult();

    /**
     *  @brief  Reset all cluster properties
     */
    StatusCode ResetProperties();

    /**
     *  @brief  Reset those cluster properties that must be recalculated upon addition/removal of a calo hit
     */
    void ResetOutdatedProperties();

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

    /**
     *  @brief  Set the track with which the cluster is seeded, updating the initial direction measurement.
     * 
     *  @param  pTrack address of the track seed
     */
    StatusCode SetTrackSeed(Track *const pTrack);

    /**
     *  @brief  Remove the track seed, changing the initial direction measurement.
     */
    void RemoveTrackSeed();

    typedef std::map<PseudoLayer, double> ValueByPseudoLayerMap; ///< The value by pseudo layer typedef
    typedef ClusterHelper::ClusterFitResult ClusterFitResult;   ///< The cluster fit result typedef

    OrderedCaloHitList      m_orderedCaloHitList;           ///< The ordered calo hit list
    CaloHitList             m_isolatedCaloHitList;          ///< The list of isolated hits, which contribute only towards cluster energy

    unsigned int            m_nCaloHits;                    ///< The number of calo hits
    unsigned int            m_nPossibleMipHits;             ///< The number of calo hits that have been flagged as possible mip hits

    double                  m_electromagneticEnergy;        ///< The sum of electromagnetic energy measures of constituent calo hits, units GeV
    double                  m_hadronicEnergy;               ///< The sum of hadronic energy measures of constituent calo hits, units GeV
    double                  m_isolatedElectromagneticEnergy;///< Sum of electromagnetic energy measures of isolated calo hits, units GeV
    double                  m_isolatedHadronicEnergy;       ///< Sum of hadronic energy measures of isolated calo hits, units GeV

    bool                    m_isPhoton;                     ///< Whether the cluster has been flagged as a photon cluster
    bool                    m_isMipTrack;                   ///< Whether the cluster has been flagged as a section of mip track
    const Track            *m_pTrackSeed;                   ///< Address of the track with which the cluster is seeded

    ValueByPseudoLayerMap   m_sumXByPseudoLayer;            ///< The sum of the x coordinates of the calo hits, stored by pseudo layer
    ValueByPseudoLayerMap   m_sumYByPseudoLayer;            ///< The sum of the y coordinates of the calo hits, stored by pseudo layer
    ValueByPseudoLayerMap   m_sumZByPseudoLayer;            ///< The sum of the z coordinates of the calo hits, stored by pseudo layer

    CartesianVector         m_initialDirection;             ///< The initial direction of the cluster
    ClusterFitResult        m_currentFitResult;             ///< The current fit result, usually set by clustering algorithm, as cluster grows

    ClusterFitResult        m_fitToAllHitsResult;           ///< The result of a linear fit to all calo hits in the cluster
    bool                    m_isFitUpToDate;                ///< Whether the fit to all calo hits is up to date

    InputPseudoLayer        m_innerPseudoLayer;             ///< The innermost pseudo layer in the cluster
    InputPseudoLayer        m_outerPseudoLayer;             ///< The outermost pseudo layer in the cluster

    InputFloat              m_bestEnergyEstimate;           ///< The best estimate of the cluster energy, units GeV

    InputBool               m_isPhotonFast;                 ///< Whether the cluster is flagged as a photon by fast photon id function
    InputPseudoLayer        m_showerStartLayer;             ///< The pseudo layer at which shower commences
    InputFloat              m_showerProfileStart;           ///< The cluster shower profile start, units radiation lengths
    InputFloat              m_showerProfileDiscrepancy;     ///< The cluster shower profile discrepancy

    TrackList               m_associatedTrackList;          ///< The list of tracks associated with the cluster

    friend class PandoraContentApiImpl;
    friend class ClusterManager;

    ADD_TEST_CLASS_FRIENDS;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::SortByInnerLayer(const Cluster *const pLhs, const Cluster *const pRhs)
{
    const PseudoLayer innerLayerLhs(pLhs->GetInnerPseudoLayer()), innerLayerRhs(pRhs->GetInnerPseudoLayer());

    if (innerLayerLhs != innerLayerRhs)
        return (innerLayerLhs < innerLayerRhs);

    const float mipFractionLhs(pLhs->GetMipFraction()), mipFractionRhs(pRhs->GetMipFraction());

    if (mipFractionLhs != mipFractionRhs)
        return (mipFractionLhs > mipFractionRhs);

    return (pLhs->GetNCaloHits() > pRhs->GetNCaloHits());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::SortByInnerLayerIncEnergy(const Cluster *const pLhs, const Cluster *const pRhs)
{
    const PseudoLayer innerLayerLhs(pLhs->GetInnerPseudoLayer()), innerLayerRhs(pRhs->GetInnerPseudoLayer());

    if (innerLayerLhs != innerLayerRhs)
        return (innerLayerLhs < innerLayerRhs);

    const float mipFractionLhs(pLhs->GetMipFraction()), mipFractionRhs(pRhs->GetMipFraction());

    if (mipFractionLhs != mipFractionRhs)
        return (mipFractionLhs > mipFractionRhs);

    const unsigned int nCaloHitsLhs(pLhs->GetNCaloHits()), nCaloHitsRhs(pRhs->GetNCaloHits());

    if (nCaloHitsLhs != nCaloHitsRhs)
        return (nCaloHitsLhs > nCaloHitsRhs);

    return (pLhs->GetHadronicEnergy() > pRhs->GetHadronicEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const OrderedCaloHitList &Cluster::GetOrderedCaloHitList() const
{
    return m_orderedCaloHitList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CaloHitList &Cluster::GetIsolatedCaloHitList() const
{
    return m_isolatedCaloHitList;
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

inline unsigned int Cluster::GetNIsolatedCaloHits() const
{
    return m_isolatedCaloHitList.size();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Cluster::GetNPossibleMipHits() const
{
    return m_nPossibleMipHits;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetMipFraction() const
{
    float mipFraction = 0;

    if (0 != m_nCaloHits)
        mipFraction = static_cast<float> (m_nPossibleMipHits) / static_cast<float> (m_nCaloHits);

    return mipFraction;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetElectromagneticEnergy() const
{
    return static_cast<float>(m_electromagneticEnergy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetHadronicEnergy() const
{
    return static_cast<float>(m_hadronicEnergy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetIsolatedElectromagneticEnergy() const
{
    return static_cast<float>(m_isolatedElectromagneticEnergy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetIsolatedHadronicEnergy() const
{
    return static_cast<float>(m_isolatedHadronicEnergy);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsPhoton() const
{
    return m_isPhoton;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsMipTrack() const
{
    return m_isMipTrack;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsTrackSeeded() const
{
    return (NULL != m_pTrackSeed);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const Track *const Cluster::GetTrackSeed() const
{
    if (NULL == m_pTrackSeed)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pTrackSeed;
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

inline const CartesianVector &Cluster::GetInitialDirection() const
{
    return m_initialDirection;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ClusterHelper::ClusterFitResult &Cluster::GetCurrentFitResult() const
{
    return m_currentFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const ClusterHelper::ClusterFitResult &Cluster::GetFitToAllHitsResult()
{
    if (!m_isFitUpToDate)
        this->CalculateFitToAllHitsResult();

    return m_fitToAllHitsResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetBestEnergyEstimate() const
{
    return m_bestEnergyEstimate.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool Cluster::IsPhotonFast()
{
    if (!m_isPhotonFast.IsInitialized())
        this->CalculateFastPhotonFlag();

    return m_isPhotonFast.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline PseudoLayer Cluster::GetShowerStartLayer()
{
    if (!m_showerStartLayer.IsInitialized())
        this->CalculateShowerStartLayer();

    return m_showerStartLayer.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetShowerProfileStart()
{
    if (!m_showerProfileStart.IsInitialized())
        this->CalculateShowerProfile();

    return m_showerProfileStart.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Cluster::GetShowerProfileDiscrepancy()
{
    if (!m_showerProfileDiscrepancy.IsInitialized())
        this->CalculateShowerProfile();

    return m_showerProfileDiscrepancy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const TrackList &Cluster::GetAssociatedTrackList() const
{
    return m_associatedTrackList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetIsPhotonFlag(bool isPhotonFlag)
{
    m_isPhoton = isPhotonFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetIsMipTrackFlag(bool isMipTrackFlag)
{
    m_isMipTrack = isMipTrackFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetBestEnergyEstimate(float bestEnergyEstimate)
{
    if (!(m_bestEnergyEstimate = bestEnergyEstimate))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::SetCurrentFitResult(const ClusterHelper::ClusterFitResult &currentFitResult)
{
    m_currentFitResult = currentFitResult;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline Cluster::~Cluster()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void Cluster::ResetOutdatedProperties()
{
    m_isFitUpToDate = false;
    m_fitToAllHitsResult.Reset();
    m_showerStartLayer.Reset();
    m_isPhotonFast.Reset();
    m_showerProfileStart.Reset();
    m_showerProfileDiscrepancy.Reset();
}

} // namespace pandora

#endif // #ifndef CLUSTER_H
