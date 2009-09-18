/**
 *  @file   PandoraPFANew/src/Objects/Cluster.cc
 * 
 *  @brief  Implementation of the cluster class.
 * 
 *  $Log: $
 */

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/Track.h"

namespace pandora
{

Cluster::Cluster(CaloHit *pCaloHit) :
    m_nCaloHits(0),
    m_nMipTrackHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isPhoton(false),
    m_sumX(0), m_sumY(0), m_sumZ(0),
    m_sumXX(0), m_sumYY(0), m_sumZZ(0),
    m_sumXY(0), m_sumXZ(0), m_sumYZ(0),
    m_isUpToDate(false)
{
    if (NULL == pCaloHit)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
    // TODO set initial direction
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(pCaloHit));
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(InputCaloHitList *pCaloHitList) :
    m_nCaloHits(0),
    m_nMipTrackHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isPhoton(false),
    m_sumX(0), m_sumY(0), m_sumZ(0),
    m_sumXX(0), m_sumYY(0), m_sumZZ(0),
    m_sumXY(0), m_sumXZ(0), m_sumYZ(0),
    m_isUpToDate(false)
{
    if (NULL == pCaloHitList)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    for (InputCaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(*iter));
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(Track *pTrack) :
    m_nCaloHits(0),
    m_nMipTrackHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isPhoton(false),
    m_sumX(0), m_sumY(0), m_sumZ(0),
    m_sumXX(0), m_sumYY(0), m_sumZZ(0),
    m_sumXY(0), m_sumXZ(0), m_sumYZ(0),
    m_isUpToDate(false)
{
    if (NULL == pTrack)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddTrackAssociation(pTrack));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddCaloHit(CaloHit *const pCaloHit)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.AddCaloHit(pCaloHit));

    m_isUpToDate = false;
    m_nCaloHits++;

    if (pCaloHit->IsMipTrack())
        m_nMipTrackHits++;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());

    m_sumX += x; m_sumY += y; m_sumZ += z;
    m_sumXX += x * x; m_sumYY += y * y; m_sumZZ += z * z;
    m_sumXY += x * y; m_sumXZ += x * z; m_sumYZ += y * z;

// TODO, can add back in after calo hit calibration is completed
//    m_electromagneticEnergy += pCaloHit->GetElectromagneticEnergy();
//    m_hadronicEnergy += pCaloHit->GetHadronicEnergy();

    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());

    if (m_orderedCaloHitList[pseudoLayer]->size() > 1)
    {
        m_sumXByPseudoLayer[pseudoLayer] += x;
        m_sumYByPseudoLayer[pseudoLayer] += y;
        m_sumZByPseudoLayer[pseudoLayer] += z;
    }
    else
    {
        m_sumXByPseudoLayer[pseudoLayer] = x;
        m_sumYByPseudoLayer[pseudoLayer] = y;
        m_sumZByPseudoLayer[pseudoLayer] = z;
    }

    if (!m_innerPseudoLayer.IsInitialized() || (pseudoLayer < m_innerPseudoLayer.Get()))
        m_innerPseudoLayer = pseudoLayer;

    if (!m_outerPseudoLayer.IsInitialized() || (pseudoLayer > m_outerPseudoLayer.Get()))
        m_outerPseudoLayer = pseudoLayer;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveCaloHit(CaloHit *const pCaloHit)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.RemoveCaloHit(pCaloHit));

    if (m_orderedCaloHitList.empty())
        return this->ResetProperties();

    m_isUpToDate = false;
    m_nCaloHits--;

    if (pCaloHit->IsMipTrack())
        m_nMipTrackHits--;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());

    m_sumX -= x; m_sumY -= y; m_sumZ -= z;
    m_sumXX -= x * x; m_sumYY -= y * y; m_sumZZ -= z * z;
    m_sumXY -= x * y; m_sumXZ -= x * z; m_sumYZ -= y * z;

// TODO, can add back in after calo hit calibration is completed
//    m_electromagneticEnergy -= pCaloHit->GetElectromagneticEnergy();
//    m_hadronicEnergy -= pCaloHit->GetHadronicEnergy();

    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());

    if (m_orderedCaloHitList.end() != m_orderedCaloHitList.find(pseudoLayer))
    {
        m_sumXByPseudoLayer[pseudoLayer] -= x;
        m_sumYByPseudoLayer[pseudoLayer] -= y;
        m_sumZByPseudoLayer[pseudoLayer] -= z;
    }
    else
    {
        m_sumXByPseudoLayer.erase(pseudoLayer);
        m_sumYByPseudoLayer.erase(pseudoLayer);
        m_sumZByPseudoLayer.erase(pseudoLayer);
    }

    if (pseudoLayer < m_innerPseudoLayer.Get())
        m_innerPseudoLayer = m_orderedCaloHitList.begin()->first;

    if (pseudoLayer > m_outerPseudoLayer.Get())
        m_outerPseudoLayer = m_orderedCaloHitList.end()->first;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::UpdateProperties()
{
    if (m_isUpToDate)
        return STATUS_CODE_SUCCESS;

    m_isUpToDate = true;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::ResetProperties()
{
    if (!m_orderedCaloHitList.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Reset());

    m_nCaloHits = 0;
    m_nMipTrackHits = 0;

    m_sumX = m_sumY = m_sumZ = 0;
    m_sumXX = m_sumYY = m_sumZZ = 0;
    m_sumXY = m_sumXZ = m_sumYZ = 0;

    m_sumXByPseudoLayer.clear();
    m_sumYByPseudoLayer.clear();
    m_sumZByPseudoLayer.clear();

    m_electromagneticEnergy = 0;
    m_hadronicEnergy = 0;

    m_innerPseudoLayer.Reset();
    m_outerPseudoLayer.Reset();
    m_showerMax.Reset();

    m_currentDirection.Reset();
    m_energyWeightedCentroid.Reset();

    m_bestEnergy.Reset();
    m_radialDirectionCosine.Reset();
    m_clusterRMS.Reset();

    m_isUpToDate = true;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddHitsFromSecondCluster(Cluster *const pCluster)
{
    // TODO sort out cluster merging
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
