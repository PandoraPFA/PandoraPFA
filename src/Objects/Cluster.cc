/**
 *  @file   PandoraPFANew/src/Objects/Cluster.cc
 * 
 *  @brief  Implementation of the cluster class.
 * 
 *  $Log: $
 */

#include "Helpers/ParticleIdHelper.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/Track.h"

namespace pandora
{

Cluster::Cluster(CaloHit *pCaloHit) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isolatedElectromagneticEnergy(0),
    m_isolatedHadronicEnergy(0),
    m_isPhoton(false),
    m_isMipTrack(false),
    m_pTrackSeed(NULL),
    m_initialDirection(pCaloHit->GetPositionVector().GetUnitVector()),
    m_isFitUpToDate(false)
{
    if (NULL == pCaloHit)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(pCaloHit));
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(CaloHitList *pCaloHitList) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isolatedElectromagneticEnergy(0),
    m_isolatedHadronicEnergy(0),
    m_isPhoton(false),
    m_isMipTrack(false),
    m_pTrackSeed(NULL),
    m_isFitUpToDate(false)
{
    if (NULL == pCaloHitList)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    for (CaloHitList::const_iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(*iter));

    m_initialDirection = (this->GetCentroid(this->GetInnerPseudoLayer())).GetUnitVector();
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(Track *pTrack) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isolatedElectromagneticEnergy(0),
    m_isolatedHadronicEnergy(0),
    m_isPhoton(false),
    m_isMipTrack(true),
    m_isFitUpToDate(false)
{
    if (NULL == pTrack)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetTrackSeed(pTrack));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddCaloHit(CaloHit *const pCaloHit)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Add(pCaloHit));

    this->ResetOutdatedProperties();

    m_nCaloHits++;

    if (pCaloHit->IsPossibleMip())
        m_nPossibleMipHits++;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());

    m_electromagneticEnergy += pCaloHit->GetElectromagneticEnergy();
    m_hadronicEnergy += pCaloHit->GetHadronicEnergy();

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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Remove(pCaloHit));

    if (m_orderedCaloHitList.empty())
        return this->ResetProperties();

    this->ResetOutdatedProperties();

    m_nCaloHits--;

    if (pCaloHit->IsPossibleMip())
        m_nPossibleMipHits--;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());

    m_electromagneticEnergy -= pCaloHit->GetElectromagneticEnergy();
    m_hadronicEnergy -= pCaloHit->GetHadronicEnergy();

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
        m_outerPseudoLayer = m_orderedCaloHitList.rbegin()->first;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddIsolatedCaloHit(CaloHit *const pCaloHit)
{
    if (!m_isolatedCaloHitList.insert(pCaloHit).second)
        return STATUS_CODE_ALREADY_PRESENT;

    const float electromagneticEnergy(pCaloHit->GetElectromagneticEnergy());
    const float hadronicEnergy(pCaloHit->GetHadronicEnergy());

    m_electromagneticEnergy += electromagneticEnergy;
    m_hadronicEnergy += hadronicEnergy;
    m_isolatedElectromagneticEnergy += electromagneticEnergy;
    m_isolatedHadronicEnergy += hadronicEnergy;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveIsolatedCaloHit(CaloHit *const pCaloHit)
{
    CaloHitList::iterator iter = m_isolatedCaloHitList.find(pCaloHit);

    if (m_isolatedCaloHitList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_isolatedCaloHitList.erase(iter);

    const float electromagneticEnergy(pCaloHit->GetElectromagneticEnergy());
    const float hadronicEnergy(pCaloHit->GetHadronicEnergy());

    m_electromagneticEnergy -= electromagneticEnergy;
    m_hadronicEnergy -= hadronicEnergy;
    m_isolatedElectromagneticEnergy -= electromagneticEnergy;
    m_isolatedHadronicEnergy -= hadronicEnergy;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const CartesianVector Cluster::GetCentroid(PseudoLayer pseudoLayer) const
{
    OrderedCaloHitList::const_iterator iter = m_orderedCaloHitList.find(pseudoLayer);

    if (m_orderedCaloHitList.end() == iter)
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    const float nHitsInLayer(static_cast<float>(iter->second->size()));

    if (0 == nHitsInLayer)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    ValueByPseudoLayerMap::const_iterator xValueIter = m_sumXByPseudoLayer.find(pseudoLayer);
    ValueByPseudoLayerMap::const_iterator yValueIter = m_sumYByPseudoLayer.find(pseudoLayer);
    ValueByPseudoLayerMap::const_iterator zValueIter = m_sumZByPseudoLayer.find(pseudoLayer);

    if ((m_sumXByPseudoLayer.end() == xValueIter) || (m_sumYByPseudoLayer.end() == yValueIter) || (m_sumZByPseudoLayer.end() == zValueIter))
        throw StatusCodeException(STATUS_CODE_FAILURE);

    return CartesianVector(xValueIter->second / nHitsInLayer, yValueIter->second / nHitsInLayer, zValueIter->second / nHitsInLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateFastPhotonFlag()
{
    const bool fastPhotonFlag(ParticleIdHelper::IsPhotonFast(this));

    if (!(m_isPhotonFast = fastPhotonFlag))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateShowerStartLayer()
{
    const PseudoLayer showerStartLayer(ClusterHelper::GetShowerStartLayer(this));

    if (!(m_showerStartLayer = showerStartLayer))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateShowerProfile()
{
    float showerProfileStart(0.), showerProfileDiscrepancy(0.);

    PANDORA_THROW_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, ParticleIdHelper::CalculateShowerProfile(this,
        showerProfileStart, showerProfileDiscrepancy));

    if (!(m_showerProfileStart = showerProfileStart) || !(m_showerProfileDiscrepancy = showerProfileDiscrepancy))
        throw StatusCodeException(STATUS_CODE_FAILURE);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::CalculateFitToAllHitsResult()
{
    (void) ClusterHelper::FitPoints(this, m_fitToAllHitsResult);
    m_isFitUpToDate = true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::ResetProperties()
{
    if (!m_orderedCaloHitList.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Reset());

    m_isolatedCaloHitList.clear();

    m_nCaloHits = 0;
    m_nPossibleMipHits = 0;

    m_sumXByPseudoLayer.clear();
    m_sumYByPseudoLayer.clear();
    m_sumZByPseudoLayer.clear();

    m_electromagneticEnergy = 0;
    m_hadronicEnergy = 0;

    m_bestEnergyEstimate.Reset();
    m_innerPseudoLayer.Reset();
    m_outerPseudoLayer.Reset();

    m_currentFitResult.Reset();
    this->ResetOutdatedProperties();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddHitsFromSecondCluster(Cluster *const pCluster)
{
    if (this == pCluster)
        return STATUS_CODE_NOT_ALLOWED;

    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Add(orderedCaloHitList));

    const CaloHitList &isolatedCaloHitList(pCluster->GetIsolatedCaloHitList());
    for (CaloHitList::const_iterator iter = isolatedCaloHitList.begin(), iterEnd = isolatedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        if (!m_isolatedCaloHitList.insert(*iter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    this->ResetOutdatedProperties();

    m_nCaloHits += pCluster->GetNCaloHits();
    m_nPossibleMipHits += pCluster->GetNPossibleMipHits();

    m_electromagneticEnergy += pCluster->GetElectromagneticEnergy();
    m_hadronicEnergy += pCluster->GetHadronicEnergy();

    // Loop over pseudo layers in second cluster
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer pseudoLayer(iter->first);

        if (m_orderedCaloHitList[pseudoLayer]->size() > 1)
        {
            m_sumXByPseudoLayer[pseudoLayer] += pCluster->m_sumXByPseudoLayer[pseudoLayer];
            m_sumYByPseudoLayer[pseudoLayer] += pCluster->m_sumYByPseudoLayer[pseudoLayer];
            m_sumZByPseudoLayer[pseudoLayer] += pCluster->m_sumZByPseudoLayer[pseudoLayer];
        }
        else
        {
            m_sumXByPseudoLayer[pseudoLayer] = pCluster->m_sumXByPseudoLayer[pseudoLayer];
            m_sumYByPseudoLayer[pseudoLayer] = pCluster->m_sumYByPseudoLayer[pseudoLayer];
            m_sumZByPseudoLayer[pseudoLayer] = pCluster->m_sumZByPseudoLayer[pseudoLayer];
        }
    }

    m_innerPseudoLayer = m_orderedCaloHitList.begin()->first;
    m_outerPseudoLayer = m_orderedCaloHitList.rbegin()->first;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddTrackAssociation(Track *const pTrack)
{
    if (NULL == pTrack)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_associatedTrackList.insert(pTrack).second)
        return STATUS_CODE_FAILURE;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::RemoveTrackAssociation(Track *const pTrack)
{
    TrackList::iterator iter = m_associatedTrackList.find(pTrack);

    if (m_associatedTrackList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_associatedTrackList.erase(iter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::SetTrackSeed(Track *const pTrack)
{
    m_pTrackSeed = pTrack;
    m_initialDirection = pTrack->GetTrackStateAtECal().GetMomentum().GetUnitVector();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Cluster::RemoveTrackSeed()
{
    m_pTrackSeed = NULL;

    if (!m_orderedCaloHitList.empty())
    {
        m_initialDirection = (this->GetCentroid(this->GetInnerPseudoLayer())).GetUnitVector();
    }
    else
    {
        m_initialDirection.Reset();
    }
}

} // namespace pandora
