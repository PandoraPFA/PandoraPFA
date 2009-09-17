/**
 *  @file   PandoraPFANew/src/Objects/Cluster.cc
 * 
 *  @brief  Implementation of the cluster class.
 * 
 *  $Log: $
 */

#include "Objects/Cluster.h"
#include "Objects/Track.h"

namespace pandora
{

Cluster::Cluster(CaloHit *pCaloHit)
{
    if (NULL == pCaloHit)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    this->AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(InputCaloHitList *pCaloHitList)
{
    if (NULL == pCaloHitList)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    for (InputCaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
        this->AddCaloHit(*iter);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(Track *pTrack)
{
    if (NULL == pTrack)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);    

    // TODO, case where track specified
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddCaloHit(CaloHit *const pCaloHit)
{
    return m_orderedCaloHitList.AddCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveCaloHit(CaloHit *const pCaloHit)
{
    return m_orderedCaloHitList.RemoveCaloHit(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::UpdateProperties()
{
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddHitsFromSecondCluster(Cluster *const pCluster)
{
    return m_orderedCaloHitList.Add(pCluster->GetOrderedCaloHitList());
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddTrackAssociation(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    m_associatedTrackList.insert(pTrack);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveTrackAssociation(Track *const pTrack)
{
    std::pair<TrackList::const_iterator, TrackList::const_iterator> range = m_associatedTrackList.equal_range(pTrack);

    if (m_associatedTrackList.end() == range.first)
        return STATUS_CODE_NOT_FOUND;

    for (TrackList::iterator iter = range.first, iterEnd = range.second; iter != iterEnd; ++iter)
    {
        if (pTrack == *iter)
        {
            m_associatedTrackList.erase(iter);
            return STATUS_CODE_SUCCESS;
        }
    }

    return STATUS_CODE_NOT_FOUND;
}

} // namespace pandora
