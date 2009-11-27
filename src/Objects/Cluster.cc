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

StatusCode Cluster::SetTrackSeed(Track *const pTrack)
{
    if (m_associatedTrackList.end() == m_associatedTrackList.find(pTrack))
        return STATUS_CODE_NOT_ALLOWED;

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

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(CaloHit *pCaloHit) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isPhoton(false),
    m_isMipTrack(false),
    m_pTrackSeed(NULL),
    m_sumX(0), m_sumY(0), m_sumZ(0),
    m_sumXX(0), m_sumYY(0), m_sumZZ(0),
    m_sumXY(0), m_sumXZ(0), m_sumYZ(0),
    m_initialDirection(pCaloHit->GetPositionVector().GetUnitVector()),
    m_isUpToDate(false)
{
    if (NULL == pCaloHit)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(pCaloHit));
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(CaloHitVector *pCaloHitVector) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isPhoton(false),
    m_isMipTrack(false),
    m_pTrackSeed(NULL),
    m_sumX(0), m_sumY(0), m_sumZ(0),
    m_sumXX(0), m_sumYY(0), m_sumZZ(0),
    m_sumXY(0), m_sumXZ(0), m_sumYZ(0),
    m_isUpToDate(false)
{
    if (NULL == pCaloHitVector)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    for (CaloHitVector::const_iterator iter = pCaloHitVector->begin(), iterEnd = pCaloHitVector->end(); iter != iterEnd; ++iter)
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddCaloHit(*iter));

    m_initialDirection = (this->GetCentroid(this->GetInnerPseudoLayer())).GetUnitVector();
}

//------------------------------------------------------------------------------------------------------------------------------------------

Cluster::Cluster(Track *pTrack) :
    m_nCaloHits(0),
    m_nPossibleMipHits(0),
    m_electromagneticEnergy(0),
    m_hadronicEnergy(0),
    m_isPhoton(false),
    m_isMipTrack(true),
    m_sumX(0), m_sumY(0), m_sumZ(0),
    m_sumXX(0), m_sumYY(0), m_sumZZ(0),
    m_sumXY(0), m_sumXZ(0), m_sumYZ(0),
    m_isUpToDate(false)
{
    if (NULL == pTrack)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddTrackAssociation(pTrack));
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetTrackSeed(pTrack));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddCaloHit(CaloHit *const pCaloHit)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.AddCaloHit(pCaloHit));

    m_isUpToDate = false;
    m_nCaloHits++;

    if (pCaloHit->IsPossibleMip())
        m_nPossibleMipHits++;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());
    const float electromagneticEnergy(pCaloHit->GetElectromagneticEnergy());
    const float hadronicEnergy(pCaloHit->GetHadronicEnergy());

    m_sumX += x; m_sumY += y; m_sumZ += z;
    m_sumXX += x * x; m_sumYY += y * y; m_sumZZ += z * z;
    m_sumXY += x * y; m_sumXZ += x * z; m_sumYZ += y * z;

    m_electromagneticEnergy += electromagneticEnergy;
    m_hadronicEnergy += hadronicEnergy;

    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());

    if (m_orderedCaloHitList[pseudoLayer]->size() > 1)
    {
        m_sumXByPseudoLayer[pseudoLayer] += x;
        m_sumYByPseudoLayer[pseudoLayer] += y;
        m_sumZByPseudoLayer[pseudoLayer] += z;

        m_emEnergyByPseudoLayer[pseudoLayer] += electromagneticEnergy;
        m_hadEnergyByPseudoLayer[pseudoLayer] += hadronicEnergy;
    }
    else
    {
        m_sumXByPseudoLayer[pseudoLayer] = x;
        m_sumYByPseudoLayer[pseudoLayer] = y;
        m_sumZByPseudoLayer[pseudoLayer] = z;

        m_emEnergyByPseudoLayer[pseudoLayer] = electromagneticEnergy;
        m_hadEnergyByPseudoLayer[pseudoLayer] = hadronicEnergy;
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

    if (pCaloHit->IsPossibleMip())
        m_nPossibleMipHits--;

    const float x(pCaloHit->GetPositionVector().GetX());
    const float y(pCaloHit->GetPositionVector().GetY());
    const float z(pCaloHit->GetPositionVector().GetZ());
    const float electromagneticEnergy(pCaloHit->GetElectromagneticEnergy());
    const float hadronicEnergy(pCaloHit->GetHadronicEnergy());

    m_sumX -= x; m_sumY -= y; m_sumZ -= z;
    m_sumXX -= x * x; m_sumYY -= y * y; m_sumZZ -= z * z;
    m_sumXY -= x * y; m_sumXZ -= x * z; m_sumYZ -= y * z;

    m_electromagneticEnergy -= pCaloHit->GetElectromagneticEnergy();
    m_hadronicEnergy -= pCaloHit->GetHadronicEnergy();

    const PseudoLayer pseudoLayer(pCaloHit->GetPseudoLayer());

    if (m_orderedCaloHitList.end() != m_orderedCaloHitList.find(pseudoLayer))
    {
        m_sumXByPseudoLayer[pseudoLayer] -= x;
        m_sumYByPseudoLayer[pseudoLayer] -= y;
        m_sumZByPseudoLayer[pseudoLayer] -= z;

        m_emEnergyByPseudoLayer[pseudoLayer] -= electromagneticEnergy;
        m_hadEnergyByPseudoLayer[pseudoLayer] -= hadronicEnergy;
    }
    else
    {
        m_sumXByPseudoLayer.erase(pseudoLayer);
        m_sumYByPseudoLayer.erase(pseudoLayer);
        m_sumZByPseudoLayer.erase(pseudoLayer);

        m_emEnergyByPseudoLayer.erase(pseudoLayer);
        m_hadEnergyByPseudoLayer.erase(pseudoLayer);
    }

    if (pseudoLayer < m_innerPseudoLayer.Get())
        m_innerPseudoLayer = m_orderedCaloHitList.begin()->first;

    if (pseudoLayer > m_outerPseudoLayer.Get())
        m_outerPseudoLayer = m_orderedCaloHitList.rbegin()->first;

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

PseudoLayer Cluster::GetShowerMaxLayer() const
{
    PseudoLayer showerMaxLayer(0);
    float maxEnergyInLayer(0);
    bool showerMaxFound(false);

    for (ValueByPseudoLayerMap::const_iterator iter = m_emEnergyByPseudoLayer.begin(), iterEnd = m_emEnergyByPseudoLayer.end();
        iter != iterEnd; ++iter)
    {
        if (iter->second > maxEnergyInLayer)
        {
            maxEnergyInLayer = iter->second;
            showerMaxLayer = iter->first;
            showerMaxFound = true;
        }
    }

    if (!showerMaxFound)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return showerMaxLayer;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::UpdateProperties()
{
    if (m_isUpToDate)
        return STATUS_CODE_SUCCESS;

    (void) ClusterHelper::FitPoints(this, m_fitToAllHitsResult);

    m_isUpToDate = true;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::ResetProperties()
{
    if (!m_orderedCaloHitList.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Reset());

    m_nCaloHits = 0;
    m_nPossibleMipHits = 0;

    m_sumX = m_sumY = m_sumZ = 0;
    m_sumXX = m_sumYY = m_sumZZ = 0;
    m_sumXY = m_sumXZ = m_sumYZ = 0;

    m_sumXByPseudoLayer.clear();
    m_sumYByPseudoLayer.clear();
    m_sumZByPseudoLayer.clear();

    m_electromagneticEnergy = 0;
    m_hadronicEnergy = 0;

    m_bestEnergyEstimate.Reset();
    m_innerPseudoLayer.Reset();
    m_outerPseudoLayer.Reset();

    m_currentFitResult.Reset();
    m_fitToAllHitsResult.Reset();

    m_isUpToDate = true;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Cluster::AddHitsFromSecondCluster(Cluster *const pCluster)
{
    const OrderedCaloHitList &orderedCaloHitList = pCluster->GetOrderedCaloHitList();
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_orderedCaloHitList.Add(orderedCaloHitList));

    m_isUpToDate = false;
    m_nCaloHits += pCluster->GetNCaloHits();
    m_nPossibleMipHits += pCluster->GetNPossibleMipHits();

    m_electromagneticEnergy += pCluster->GetElectromagneticEnergy();
    m_hadronicEnergy += pCluster->GetHadronicEnergy();

    const float x(pCluster->m_sumX);
    const float y(pCluster->m_sumY);
    const float z(pCluster->m_sumZ);

    m_sumX += x; m_sumY += y; m_sumZ += z;
    m_sumXX += x * x; m_sumYY += y * y; m_sumZZ += z * z;
    m_sumXY += x * y; m_sumXZ += x * z; m_sumYZ += y * z;

    // Loop over pseudo layers in second cluster
    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        const PseudoLayer pseudoLayer(iter->first);

        if (m_orderedCaloHitList[pseudoLayer]->size() > 1)
        {
            m_sumXByPseudoLayer[pseudoLayer] += pCluster->m_sumXByPseudoLayer[pseudoLayer];
            m_sumYByPseudoLayer[pseudoLayer] += pCluster->m_sumYByPseudoLayer[pseudoLayer];
            m_sumZByPseudoLayer[pseudoLayer] += pCluster->m_sumZByPseudoLayer[pseudoLayer];

            m_emEnergyByPseudoLayer[pseudoLayer] += pCluster->m_emEnergyByPseudoLayer[pseudoLayer];
            m_hadEnergyByPseudoLayer[pseudoLayer] += pCluster->m_hadEnergyByPseudoLayer[pseudoLayer];
        }
        else
        {
            m_sumXByPseudoLayer[pseudoLayer] = pCluster->m_sumXByPseudoLayer[pseudoLayer];
            m_sumYByPseudoLayer[pseudoLayer] = pCluster->m_sumYByPseudoLayer[pseudoLayer];
            m_sumZByPseudoLayer[pseudoLayer] = pCluster->m_sumZByPseudoLayer[pseudoLayer];

            m_emEnergyByPseudoLayer[pseudoLayer] = pCluster->m_emEnergyByPseudoLayer[pseudoLayer];
            m_hadEnergyByPseudoLayer[pseudoLayer] = pCluster->m_hadEnergyByPseudoLayer[pseudoLayer];
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

} // namespace pandora
