/**
 *  @file   PandoraPFANew/Framework/include/Helpers/ReclusterHelper.h
 * 
 *  @brief  Header file for the recluster helper class.
 * 
 *  $Log: $
 */
#ifndef RECLUSTER_HELPER_H
#define RECLUSTER_HELPER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

#include "Xml/tinyxml.h"

namespace pandora
{

/**
 *  @brief  ReclusterHelper class
 */
class ReclusterHelper
{
public:
    /**
     *  @brief  ReclusterResult class
     */
    class ReclusterResult
    {
    public:
        /**
         *  @brief  Get the total chi value for the suitability of all track/cluster associations
         * 
         *  @return the total chi value
         */
        float GetChi() const;

        /**
         *  @brief  Get the total chi2 value for the suitability of all track/cluster associations
         * 
         *  @return the total chi2 value
         */
        float GetChi2() const;

        /**
         *  @brief  Get the chi per degree of freedom value for suitability of track/cluster associations
         * 
         *  @return the chi per degree of freedom value
         */
        float GetChiPerDof() const;

        /**
         *  @brief  Get the chi2 per degree of freedom value for suitability of track/cluster associations
         * 
         *  @return the chi2 per degree of freedom value
         */
        float GetChi2PerDof() const;

        /**
         *  @brief  Get the total hadronic energy in clusters that have no track associations
         * 
         *  @return the total hadronic energy in clusters that have no track associations
         */
        float GetUnassociatedEnergy() const;

        /**
         *  @brief  Get the minimum energy of a cluster associated with a track
         * 
         *  @return the minimum energy of a cluster associated with a track
         */
        float GetMinTrackAssociationEnergy() const;

        /**
         *  @brief  Get the number of excess track-cluster associations
         * 
         *  @return the number of excess track-cluster associations
         */
        unsigned int GetNExcessTrackAssociations() const;

        /**
         *  @brief  Set the total chi value for the suitability of all track/cluster associations
         * 
         *  @param  chi the total chi value
         */
        void SetChi(float chi);

        /**
         *  @brief  Set the total chi2 value for the suitability of all track/cluster associations
         * 
         *  @param  chi the total chi2 value
         */
        void SetChi2(float chi2);

        /**
         *  @brief  Set the chi per degree of freedom value for suitability of track/cluster associations
         * 
         *  @param  chiPerDof the chi per degree of freedom value
         */
        void SetChiPerDof(float chiPerDof);

        /**
         *  @brief  Set the chi2 per degree of freedom value for suitability of track/cluster associations
         * 
         *  @param  chi2PerDof the chi2 per degree of freedom value
         */
        void SetChi2PerDof(float chi2PerDof);

        /**
         *  @brief  Set the total hadronic energy in clusters that have no track associations
         * 
         *  @param  unassociatedEnergy the total hadronic energy in clusters that have no track associations
         */
        void SetUnassociatedEnergy(float unassociatedEnergy);

        /**
         *  @brief  Set the minimum energy of a cluster associated with a track
         * 
         *  @param  minTrackAssociationEnergy the minimum energy of a cluster associated with a track
         */
        void SetMinTrackAssociationEnergy(float minTrackAssociationEnergy);

        /**
         *  @brief  Set the number of excess track-cluster associations
         * 
         *  @param  nExcessTrackAssociations the number of excess track-cluster associations
         */
        void SetNExcessTrackAssociations(unsigned int nExcessTrackAssociations);

    private:
        InputFloat      m_chi;                          ///< Total chi value for the suitability of all track/cluster associations
        InputFloat      m_chi2;                         ///< Total chi2 value for the suitability of all track/cluster associations
        InputFloat      m_chiPerDof;                    ///< Chi per degree of freedom value for suitability of track/cluster associations
        InputFloat      m_chi2PerDof;                   ///< Chi2 per degree of freedom value for suitability of track/cluster associations
        InputFloat      m_unassociatedEnergy;           ///< Total hadronic energy in clusters that have no track associations
        InputFloat      m_minTrackAssociationEnergy;    ///< Minimum energy of a cluster associated with a track
        InputUInt       m_nExcessTrackAssociations;     ///< Number of excess track-cluster associations
    };

    /**
     *  @brief  Evaluate the compatibility of a cluster with its associated tracks. Reclustering can be used to split up a
     *          cluster and produce more favourable track/cluster matches.
     *
     *  @param  pCluster address of the cluster
     *  @param  trackList address of the list of tracks associated with the cluster
     * 
     *  @return the chi value for the suitability of the track/cluster associations
     */
    static float GetTrackClusterCompatibility(const Cluster *const pCluster, const TrackList &trackList);

    /**
     *  @brief  Evaluate the compatibility of a cluster with its associated tracks. Reclustering can be used to split up a
     *          cluster and produce more favourable track/cluster matches.
     * 
     *  @param  clusterEnergy the cluster energy
     *  @param  trackEnergy the sum of the energies of the associated tracks
     * 
     *  @return the chi value for the suitability of the track/cluster associations
     */
    static float GetTrackClusterCompatibility(const float clusterEnergy, const float trackEnergy);

    /**
     *  @brief  Extract recluster results, indicating suitability of recluster candidates as a replacement for the original clusters
     * 
     *  @param  pReclusterCandidatesList address of the recluster candidates list
     *  @param  reclusterResult to receive the recluster results
     */
    static StatusCode ExtractReclusterResults(const ClusterList *const pReclusterCandidatesList, ReclusterResult &reclusterResult);

private:
    /**
     *  @brief  Read the recluster helper settings
     * 
     *  @param  xmlHandle the relevant xml handle
     */
    static StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    friend class PandoraSettings;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterHelper::ReclusterResult::GetChi() const
{
    return m_chi.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterHelper::ReclusterResult::GetChi2() const
{
    return m_chi2.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterHelper::ReclusterResult::GetChiPerDof() const
{
    return m_chiPerDof.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterHelper::ReclusterResult::GetChi2PerDof() const
{
    return m_chi2PerDof.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterHelper::ReclusterResult::GetUnassociatedEnergy() const
{
    return m_unassociatedEnergy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float ReclusterHelper::ReclusterResult::GetMinTrackAssociationEnergy() const
{
    return m_minTrackAssociationEnergy.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int ReclusterHelper::ReclusterResult::GetNExcessTrackAssociations() const
{
    return m_nExcessTrackAssociations.Get();
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterHelper::ReclusterResult::SetChi(float chi)
{
    if (!(m_chi = chi))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterHelper::ReclusterResult::SetChi2(float chi2)
{
    if (!(m_chi2 = chi2))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterHelper::ReclusterResult::SetChiPerDof(float chiPerDof)
{
    if (!(m_chiPerDof = chiPerDof))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterHelper::ReclusterResult::SetChi2PerDof(float chi2PerDof)
{
    if (!(m_chi2PerDof = chi2PerDof))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterHelper::ReclusterResult::SetUnassociatedEnergy(float unassociatedEnergy)
{
    if (!(m_unassociatedEnergy = unassociatedEnergy))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterHelper::ReclusterResult::SetMinTrackAssociationEnergy(float minTrackAssociationEnergy)
{
    if (!(m_minTrackAssociationEnergy = minTrackAssociationEnergy))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void ReclusterHelper::ReclusterResult::SetNExcessTrackAssociations(unsigned int nExcessTrackAssociations)
{
    if (!(m_nExcessTrackAssociations = nExcessTrackAssociations))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
}

} // namespace pandora

#endif // #ifndef RECLUSTER_HELPER_H
