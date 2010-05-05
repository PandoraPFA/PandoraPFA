/**
 *  @file   PandoraPFANew/src/Algorithms/PhotonRecoveryAlgorithm.cc
 * 
 *  @brief  Implementation of the photon recovery algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PhotonRecoveryAlgorithm.h"

#include "Helpers/ParticleIdHelper.h"

#include <limits>

using namespace pandora;

StatusCode PhotonRecoveryAlgorithm::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        // Consider only plausible photon candidates, currently identified as hadrons
        if (!pCluster->GetAssociatedTrackList().empty())
            continue;

        if (pCluster->IsPhotonFast())
            continue;

        // Apply simple initial cuts
        const float electroMagneticEnergy(pCluster->GetElectromagneticEnergy());
        const PseudoLayer layer90(this->GetLayer90(pCluster));
        const PseudoLayer innerPseudoLayer(pCluster->GetInnerPseudoLayer());

        // TODO remove hardcoded numbers
        if ((electroMagneticEnergy < 1.5f) || (layer90 >= nECalLayers + 5) || (innerPseudoLayer >= nECalLayers / 2))
            continue;

        // Cut on cluster shower profile properties
        bool isPhoton(false);
        const float showerProfileStart(pCluster->GetShowerProfileStart());
        const float showerProfileDiscrepancy(pCluster->GetShowerProfileDiscrepancy());

        float profileStartCut(4.1f);

        if (electroMagneticEnergy > 5.f)
            profileStartCut = 5.1f;

        if (layer90 > nECalLayers)
            profileStartCut = 2.f;

        float profileDiscrepancyCut(0.4);

        if (electroMagneticEnergy > 2.5f)
            profileDiscrepancyCut = 0.5f - 0.02f * showerProfileStart;

        if ((showerProfileStart < profileStartCut) && (showerProfileDiscrepancy < profileDiscrepancyCut) && (showerProfileDiscrepancy > 0.f))
        {
            isPhoton = true;
        }
        else if ((showerProfileDiscrepancy > 0.f) && (showerProfileDiscrepancy < 0.5f) && (showerProfileStart < 2.75f))
        {
            isPhoton = true;
        }

        // Check barrel-endcap overlap
        if (!isPhoton && (innerPseudoLayer < 10) && (pCluster->GetMipFraction() - 0.5f < std::numeric_limits<float>::epsilon()) &&
            (this->GetBarrelEndCapEnergySplit(pCluster) < 0.9f) && (pCluster->GetCurrentFitResult().GetRadialDirectionCosine() > 0.9f))
        {
            isPhoton = true;
        }

        // Tag the cluster as a fixed photon
        if (isPhoton)
        {
            pCluster->SetIsPhotonFlag(true);
        }
    }

    // Soft photon id
    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (ParticleIdHelper::IsPhotonFull(pCluster))
        {
            pCluster->SetIsPhotonFlag(true);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer PhotonRecoveryAlgorithm::GetLayer90(const Cluster *const pCluster) const
{
    float electromagneticEnergy90(0.f);
    const float totalElectromagneticEnergy(pCluster->GetElectromagneticEnergy() - pCluster->GetIsolatedElectromagneticEnergy());
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            electromagneticEnergy90 += (*hitIter)->GetElectromagneticEnergy();

        if (electromagneticEnergy90 > 0.9 * totalElectromagneticEnergy)
            return iter->first;
    }

    throw StatusCodeException(STATUS_CODE_NOT_FOUND);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float PhotonRecoveryAlgorithm::GetBarrelEndCapEnergySplit(const Cluster *const pCluster) const
{
    float barrelEnergy(0.f), endCapEnergy(0.f);
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            const DetectorRegion detectorRegion((*hitIter)->GetDetectorRegion());

            if (detectorRegion == BARREL)
            {
                barrelEnergy += (*hitIter)->GetElectromagneticEnergy();
            }
            else if (detectorRegion == ENDCAP)
            {
                endCapEnergy += (*hitIter)->GetElectromagneticEnergy();
            }
        }
    }

    const float totalEnergy(barrelEnergy + endCapEnergy);

    if (totalEnergy > 0.f)
    {
        return std::max(barrelEnergy / totalEnergy, endCapEnergy / totalEnergy);
    }

    throw StatusCodeException(STATUS_CODE_NOT_FOUND);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PhotonRecoveryAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_trackClusterAssociationAlgName));

    return STATUS_CODE_SUCCESS;
}
