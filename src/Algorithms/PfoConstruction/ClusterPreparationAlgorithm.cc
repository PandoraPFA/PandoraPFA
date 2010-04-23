/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstruction/ClusterPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the cluster preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/ClusterPreparationAlgorithm.h"

#include "Helpers/ParticleIdHelper.h"

#include <limits>

using namespace pandora;

StatusCode ClusterPreparationAlgorithm::Run()
{
    // Create pfo cluster list, containing all candidate clusters for use in final pfo creation
    for (StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveClusterList(*this, *iter, m_finalPfoListName));
    }

    // Save the filtered list and set it to be the current list for next algorithms
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentClusterList(*this, m_finalPfoListName));

    const ClusterList *pFinalClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pFinalClusterList));

    //    // Perform full photon id on output clusters
    //    for (ClusterList::const_iterator iter = pFinalClusterList->begin(), iterEnd = pFinalClusterList->end(); iter != iterEnd; ++iter)
    //    {
    //        Cluster *pCluster = *iter;
    //
    //        if (ParticleIdHelper::IsPhotonFull(pCluster))
    //            pCluster->SetIsPhotonFlag(true);
    //    }

    // Set best energy estimates for output clusters
    for (ClusterList::const_iterator iter = pFinalClusterList->begin(), iterEnd = pFinalClusterList->end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;

        // ATTN: Fast photon id defers to any previous algorithm that has flagged the cluster as a photon
        if (pCluster->IsPhotonFast())
        {
            pCluster->SetBestEnergyEstimate(pCluster->GetElectromagneticEnergy());
        }
        else
        {
            pCluster->SetBestEnergyEstimate(pCluster->GetHadronicEnergy());
        }
    }

    // Now make corrections to these energy estimates
    if (m_shouldCleanClusters)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CleanClusters());

    if (m_shouldScaleHotHadrons)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ScaleHotHadronEnergy());

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

        if (pCluster->IsPhotonFast())
            continue;

        const float clusterHadronicEnergy(pCluster->GetHadronicEnergy());

        if (0. == clusterHadronicEnergy)
            return STATUS_CODE_FAILURE;

        bool isOutsideECal(false);
        const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

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
                        pCluster->SetBestEnergyEstimate(pCluster->GetBestEnergyEstimate() + newHitHadronicEnergy - hitHadronicEnergy);
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

        if (pCluster->IsPhotonFast())
            continue;

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

        for (OrderedCaloHitList::const_iterator layerIter = orderedCaloHitList.begin(), layerIterEnd = orderedCaloHitList.end();
            layerIter != layerIterEnd; ++layerIter)
        {
            for (CaloHitList::const_iterator hitIter = layerIter->second->begin(), hitIterEnd = layerIter->second->end();
                hitIter != hitIterEnd; ++hitIter)
            {
                clusterMipEnergy += (*hitIter)->GetMipEquivalentEnergy();
            }
        }

        const float meanMipsPerHit(clusterMipEnergy / static_cast<float>(nHitsInCluster));

        if (0. == meanMipsPerHit)
            continue;

        if (meanMipsPerHit > m_hotHadronMipsPerHit)
        {
            pCluster->SetBestEnergyEstimate(pCluster->GetBestEnergyEstimate() * m_scaledHotHadronMipsPerHit / meanMipsPerHit);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ClusterPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        return STATUS_CODE_INVALID_PARAMETER;

    m_finalPfoListName = "pfoCreation";
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "FinalPfoListName", m_finalPfoListName));

    m_shouldCleanClusters = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldCleanClusters", m_shouldCleanClusters));

    m_shouldScaleHotHadrons = true;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldScaleHotHadrons", m_shouldScaleHotHadrons));

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

    if (0 == m_minHitsForHotHadron)
        return STATUS_CODE_INVALID_PARAMETER;

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

    return STATUS_CODE_SUCCESS;
}
