/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstruction/ClusterPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the cluster preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/ClusterPreparationAlgorithm.h"

#include <limits>

using namespace pandora;

StatusCode ClusterPreparationAlgorithm::Run()
{
    // Set best energy estimates for input clusters
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        if (!(*iter)->IsPhoton())
            (*iter)->SetBestEnergyEstimate((*iter)->GetHadronicEnergy());
    }

    // Now make corrections to these energy estimates
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CleanClusters());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ScaleHotHadronEnergy());

    // Perform last-minute photon identification
    if (m_shouldPerformPhotonId)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->IdentifyPhotons());
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterPreparationAlgorithm::CleanClusters() const
{
    static const PseudoLayer firstPseudoLayer(TRACK_PROJECTION_LAYER + 1);

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

        const float clusterHadronicEnergy(pCluster->GetHadronicEnergy());

        if (0. == clusterHadronicEnergy)
            return STATUS_CODE_FAILURE;

        bool isOutsideECal(false);

        // Loop over all constituent ecal hits, looking for 
        for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end();
            (layerIter != layerIterEnd) && !isOutsideECal; ++layerIter)
        {
            const PseudoLayer pseudoLayer(layerIter->first);

            for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end();
                hitIter != hitIterEnd; ++hitIter)
            {
                CaloHit *pCaloHit = *hitIter;

                if (ECAL != pCaloHit->GetHitType())
                {
                    isOutsideECal = true;
                    break;
                }

                const float hitHadronicEnergy(pCaloHit->GetHadronicEnergy());

                if ((hitHadronicEnergy > m_minCleanHitEnergy) && (hitHadronicEnergy / clusterHadronicEnergy > m_minCleanHitEnergyFraction))
                {
                    // Calculate new energy from surrounding layers
                    float energyInPreviousLayer(0.);

                    if (pseudoLayer > firstPseudoLayer)
                        energyInPreviousLayer = this->GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer - 1);

                    float energyInNextLayer(0.);

                    if (pseudoLayer < std::numeric_limits<unsigned int>::max())
                        energyInNextLayer = this->GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer + 1);

                    const float energyInCurrentLayer = this->GetHadronicEnergyInLayer(orderedCaloHitList, pseudoLayer);

                    // Calculate new energy estimate for hit and update cluster best energy estimate
                    float energyInAdjacentLayers(energyInPreviousLayer + energyInNextLayer);

                    if (pseudoLayer > firstPseudoLayer)
                        energyInAdjacentLayers /= 2.f;

                    float newHitHadronicEnergy(energyInAdjacentLayers - energyInCurrentLayer + hitHadronicEnergy);
                    newHitHadronicEnergy = std::max(newHitHadronicEnergy, m_minCleanCorrectedHitEnergy);

                    if (newHitHadronicEnergy < hitHadronicEnergy)
                    {
                        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCluster->SetBestEnergyEstimate(pCluster->GetHadronicEnergy() +
                            newHitHadronicEnergy - hitHadronicEnergy));
                    }
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float ClusterPreparationAlgorithm::GetHadronicEnergyInLayer(const OrderedCaloHitList &orderedCaloHitList, const PseudoLayer pseudoLayer) const
{
    OrderedCaloHitList::const_iterator iter = orderedCaloHitList.find(pseudoLayer);

    float hadronicEnergy(0.);

    if (iter != orderedCaloHitList.end())
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            hadronicEnergy += (*hitIter)->GetHadronicEnergy();
        }
    }

    return hadronicEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterPreparationAlgorithm::ScaleHotHadronEnergy() const
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        // Perform hit hadron identification
        const unsigned int nHitsInCluster(pCluster->GetNCaloHits());

        if ((nHitsInCluster < m_minHitsForHotHadron) || (nHitsInCluster > m_maxHitsForHotHadron))
            continue;

        if ((pCluster->GetInnerPseudoLayer() < m_hotHadronInnerLayerCut) && (pCluster->GetMipFraction() < m_hotHadronMipFractionCut) &&
            (nHitsInCluster > m_hotHadronNHitsCut))
        {
            continue;
        }

        // Finally, check the number of mips per hit
        float clusterMipEnergy(0.);
        const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

        for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end(); layerIter != layerIterEnd; ++layerIter)
        {
            for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                clusterMipEnergy += (*hitIter)->GetMipEquivalentEnergy();
            }
        }

        const float meanMipsPerHit(clusterMipEnergy / static_cast<float>(nHitsInCluster));

        if (0. == meanMipsPerHit)
            continue;

        if (meanMipsPerHit > m_hotHadronMipsPerHit)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCluster->SetBestEnergyEstimate(pCluster->GetBestEnergyEstimate() *
                m_scaledHotHadronMipsPerHit / meanMipsPerHit));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterPreparationAlgorithm::IdentifyPhotons() const
{
    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    for (ClusterList::const_iterator iter = pClusterList->begin(), iterEnd = pClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        if (pCluster->IsPhoton())
            continue;

        // Cluster with associated tracks is not a photon
        if (!pCluster->GetAssociatedTrackList().empty())
            continue;

        // Reject clusters starting outside ecal
        static const unsigned int nECalLayers(GeometryHelper::GetInstance()->GetECalBarrelParameters().GetNLayers());
        const PseudoLayer innerLayer(pCluster->GetInnerPseudoLayer());

        if (innerLayer > nECalLayers)
            continue;

        // Cut on cluster mip fraction
        const float totalElectromagneticEnergy(pCluster->GetElectromagneticEnergy());

        float mipCut(m_photonIdMipCut_0);

        if (totalElectromagneticEnergy > m_photonIdMipCutEnergy_1)
        {
            mipCut = m_photonIdMipCut_1;
        }
        else if (totalElectromagneticEnergy > m_photonIdMipCutEnergy_2)
        {
            mipCut = m_photonIdMipCut_2;
        }
        else if (totalElectromagneticEnergy > m_photonIdMipCutEnergy_3)
        {
            mipCut = m_photonIdMipCut_3;
        }
        else if (totalElectromagneticEnergy > m_photonIdMipCutEnergy_4)
        {
            mipCut = m_photonIdMipCut_4;
        }

        if (pCluster->GetMipFraction() > mipCut)
            continue;

        // Cut on results of fit to all hits in cluster
        float dCosR(0.f);
        float clusterRms(0.f);

        const ClusterHelper::ClusterFitResult &clusterFitResult(pCluster->GetFitToAllHitsResult());
        const CartesianVector innerLayerCentroid(pCluster->GetCentroid(innerLayer));

        if (clusterFitResult.IsFitSuccessful())
        {
            dCosR = innerLayerCentroid.GetUnitVector().GetDotProduct(clusterFitResult.GetDirection());
            clusterRms = clusterFitResult.GetRms();
        }

        const float dCosRCut(totalElectromagneticEnergy < m_photonIdDCosRCutEnergy ? m_photonIdDCosRLowECut : m_photonIdDCosRHighECut);

        if (dCosR < dCosRCut)
            continue;

        const float rmsCut(totalElectromagneticEnergy < m_photonIdRmsCutEnergy ? m_photonIdRmsLowECut : m_photonIdRmsHighECut);

        if (clusterRms > rmsCut)
            continue;

        // Compare initial cluster direction with normal to ecal layers
        static const float eCalEndCapInnerZ(GeometryHelper::GetInstance()->GetECalEndCapParameters().GetInnerZCoordinate());

        const float cosTheta(std::fabs(innerLayerCentroid.GetZ()) / innerLayerCentroid.GetMagnitude());
        const float rDotN((std::fabs(innerLayerCentroid.GetZ()) > eCalEndCapInnerZ - m_photonIdEndCapZSeparation) ? cosTheta : std::sqrt(1. - cosTheta * cosTheta));

        if (0 == rDotN)
            return STATUS_CODE_FAILURE;

        // Find number of radiation lengths in front of cluster first layer
        const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());
        OrderedCaloHitList::const_iterator firstLayerIter = orderedCaloHitList.find(innerLayer);

        if (orderedCaloHitList.end() == firstLayerIter)
            return STATUS_CODE_FAILURE;

        const unsigned int nHitsInFirstLayer(firstLayerIter->second->size());

        if (0 == nHitsInFirstLayer)
            return STATUS_CODE_FAILURE;

        float nRadiationLengths(0.f);

        for (CaloHitList::const_iterator iter = firstLayerIter->second->begin(), iterEnd = firstLayerIter->second->end(); iter != iterEnd; ++iter)
        {
            nRadiationLengths += (*iter)->GetNRadiationLengths();
        }

        const float firstLayerInRadiationLengths(nRadiationLengths / (static_cast<float>(nHitsInFirstLayer) * rDotN));

        if (firstLayerInRadiationLengths > m_photonIdRadiationLengthsCut)
            continue;

        // Cut on position of shower start layer
        const int showerStartLayer(static_cast<int>(pCluster->GetShowerStartLayer()));

        float showerStartCut1(m_photonIdShowerStartCut1_0);
        const float showerStartCut2(m_photonIdShowerStartCut2);

        if (totalElectromagneticEnergy > m_photonIdShowerStartCut1Energy_1)
        {
            showerStartCut1 = m_photonIdShowerStartCut1_1;
        }
        else if (totalElectromagneticEnergy > m_photonIdShowerStartCut1Energy_2)
        {
            showerStartCut1 = m_photonIdShowerStartCut1_2;
        }

        if ((showerStartLayer - innerLayer < showerStartCut1 * rDotN) || (showerStartLayer - innerLayer > showerStartCut2 * rDotN))
            continue;

        // Cut on layer by which 90% of cluster energy has been deposited
        float electromagneticEnergy90(0.f);
        int layer90(std::numeric_limits<int>::max());

        for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
        {
            for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                electromagneticEnergy90 += (*hitIter)->GetElectromagneticEnergy();
            }

            if (electromagneticEnergy90 > 0.9 * totalElectromagneticEnergy)
            {
                layer90 = static_cast<int>(iter->first);
                break;
            }
        }

        const float layer90Cut1(m_photonIdLayer90Cut1);
        const float layer90Cut2(totalElectromagneticEnergy < m_photonIdLayer90Cut2Energy ? m_photonIdLayer90LowECut2 : m_photonIdLayer90HighECut2);

        if ((layer90 - innerLayer < layer90Cut1 * rDotN) || (layer90 - innerLayer > layer90Cut2 * rDotN))
            continue;

        if (layer90 > static_cast<int>(nECalLayers) + m_photonIdLayer90MaxLayersFromECal)
            continue;

        // Anything remaining at this point is classed as a photon
        pCluster->SetIsPhotonFlag(true);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_minCleanHitEnergy = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCleanHitEnergy", m_minCleanHitEnergy));

    m_minCleanHitEnergyFraction = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCleanHitEnergyFraction", m_minCleanHitEnergyFraction));

    m_minCleanCorrectedHitEnergy = 0.2f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinCleanCorrectedHitEnergy", m_minCleanCorrectedHitEnergy));

    m_minHitsForHotHadron = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MinHitsForHotHadron", m_minHitsForHotHadron));

    m_maxHitsForHotHadron = 100;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "MaxHitsForHotHadron", m_maxHitsForHotHadron));

    m_hotHadronInnerLayerCut = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HotHadronInnerLayerCut", m_hotHadronInnerLayerCut));

    m_hotHadronMipFractionCut = 0.4f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HotHadronMipFractionCut", m_hotHadronMipFractionCut));

    m_hotHadronNHitsCut = 50;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HotHadronNHitsCut", m_hotHadronNHitsCut));

    m_hotHadronMipsPerHit = 15;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "HotHadronMipsPerHit", m_hotHadronMipsPerHit));

    m_scaledHotHadronMipsPerHit = 5;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ScaledHotHadronMipsPerHit", m_scaledHotHadronMipsPerHit));

    m_shouldPerformPhotonId = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldPerformPhotonId", m_shouldPerformPhotonId));

    m_photonIdMipCut_0 = 0.9f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_0", m_photonIdMipCut_0));

    m_photonIdMipCutEnergy_1 = 15.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCutEnergy_1", m_photonIdMipCutEnergy_1));

    m_photonIdMipCut_1 = 0.3f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_1", m_photonIdMipCut_1));

    m_photonIdMipCutEnergy_2 = 7.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCutEnergy_2", m_photonIdMipCutEnergy_2));

    m_photonIdMipCut_2 = 0.4f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_2", m_photonIdMipCut_2));

    m_photonIdMipCutEnergy_3 = 3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCutEnergy_3", m_photonIdMipCutEnergy_3));

    m_photonIdMipCut_3 = 0.6f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_3", m_photonIdMipCut_3));

    m_photonIdMipCutEnergy_4 = 1.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCutEnergy_4", m_photonIdMipCutEnergy_4));

    m_photonIdMipCut_4 = 0.7f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdMipCut_4", m_photonIdMipCut_4));

    m_photonIdDCosRCutEnergy = 1.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdDCosRCutEnergy", m_photonIdDCosRCutEnergy));

    m_photonIdDCosRLowECut = 0.94f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdDCosRLowECut", m_photonIdDCosRLowECut));

    m_photonIdDCosRHighECut = 0.95f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdDCosRHighECut", m_photonIdDCosRHighECut));

    m_photonIdRmsCutEnergy = 40.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdRmsCutEnergy", m_photonIdRmsCutEnergy));

    m_photonIdRmsLowECut = 40.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdRmsLowECut", m_photonIdRmsLowECut));

    m_photonIdRmsHighECut = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdRmsHighECut", m_photonIdRmsHighECut));

    m_photonIdEndCapZSeparation = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdEndCapZSeparation", m_photonIdEndCapZSeparation));

    m_photonIdRadiationLengthsCut = 10.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdRadiationLengthsCut", m_photonIdRadiationLengthsCut));

    m_photonIdShowerStartCut1_0 = 0.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerStartCut1_0", m_photonIdShowerStartCut1_0));

    m_photonIdShowerStartCut2 = 40.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerStartCut2", m_photonIdShowerStartCut2));

    m_photonIdShowerStartCut1Energy_1 = 3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerStartCut1Energy_1", m_photonIdShowerStartCut1Energy_1));

    m_photonIdShowerStartCut1_1 = 3.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerStartCut1_1", m_photonIdShowerStartCut1_1));

    m_photonIdShowerStartCut1Energy_2 = 1.5f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerStartCut1Energy_2", m_photonIdShowerStartCut1Energy_2));

    m_photonIdShowerStartCut1_2 = 1.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdShowerStartCut1_2", m_photonIdShowerStartCut1_2));

    m_photonIdLayer90Cut1 = 5.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90Cut1", m_photonIdLayer90Cut1));

    m_photonIdLayer90Cut2Energy = 40.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90Cut2Energy", m_photonIdLayer90Cut2Energy));

    m_photonIdLayer90LowECut2 = 40.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90LowECut2", m_photonIdLayer90LowECut2));

    m_photonIdLayer90HighECut2 = 50.f;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90HighECut2", m_photonIdLayer90HighECut2));

    m_photonIdLayer90MaxLayersFromECal = 10;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "PhotonIdLayer90MaxLayersFromECal", m_photonIdLayer90MaxLayersFromECal));

    return STATUS_CODE_SUCCESS;
}
