/**
 *  @file   PandoraPFANew/FineGranularityContent/src/FragmentRemoval/MergeSplitPhotonsAlgorithm.cc
 * 
 *  @brief  Implementation of the merge split photons algorithm class.
 * 
 *  $Log: $
 */

#include "FragmentRemoval/MergeSplitPhotonsAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode MergeSplitPhotonsAlgorithm::Run()
{
    // TODO consider any need for rechecking track-cluster associations here

    const ClusterList *pClusterList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentClusterList(*this, pClusterList));

    // Create a vector of input clusters, ordered by inner layer
    ClusterVector clusterVector(pClusterList->begin(), pClusterList->end());
    std::sort(clusterVector.begin(), clusterVector.end(), Cluster::SortByInnerLayer);

    // Loop over photon candidate clusters
    for (ClusterVector::iterator iterI = clusterVector.begin(), iterIEnd = clusterVector.end(); iterI != iterIEnd; ++iterI)
    {
        Cluster *pParentCluster = *iterI;

        if (NULL == pParentCluster)
            continue;

        if (!pParentCluster->GetAssociatedTrackList().empty())
            continue;

        if (GeometryHelper::GetHitTypeGranularity(pParentCluster->GetOuterLayerHitType()) > FINE)
            continue;

        const CartesianVector parentShowerMaxCentroid(pParentCluster->GetCentroid(this->GetShowerMaxLayer(pParentCluster)));
        const bool isParentPhoton(pParentCluster->IsPhotonFast());

        // Find daughter photon candidate clusters
        for (ClusterVector::iterator iterJ = iterI + 1, iterJEnd = clusterVector.end(); iterJ != iterJEnd; ++iterJ)
        {
            Cluster *pDaughterCluster = *iterJ;

            if (NULL == pDaughterCluster)
                continue;

            if (!pDaughterCluster->GetAssociatedTrackList().empty())
                continue;

            if (GeometryHelper::GetHitTypeGranularity(pDaughterCluster->GetOuterLayerHitType()) > FINE)
                continue;

            const CartesianVector daughterShowerMaxCentroid(pDaughterCluster->GetCentroid(this->GetShowerMaxLayer(pDaughterCluster)));
            const bool isDaughterPhoton(pDaughterCluster->IsPhotonFast());

            // Look for compatible parent/daughter pairings
            if (!isParentPhoton && !isDaughterPhoton)
                continue;

            if (parentShowerMaxCentroid.GetCosOpeningAngle(daughterShowerMaxCentroid) <= 0.98f)
                continue;

            // TODO some kind of cos(theta) cut here???
            unsigned int nContactLayers(0);
            float contactFraction(0.f);

            const StatusCode statusCode(FragmentRemovalHelper::GetClusterContactDetails(pParentCluster, pDaughterCluster, 2.f,
                nContactLayers, contactFraction));

            if ((STATUS_CODE_SUCCESS == statusCode) && (nContactLayers > 2) && (contactFraction > 0.5f))
            {
                // Initialize fragmentation to compare merged cluster with original
                ClusterList clusterList;
                clusterList.insert(pParentCluster); clusterList.insert(pDaughterCluster);

                std::string originalClusterListName, mergedClusterListName;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::InitializeFragmentation(*this, clusterList,
                    originalClusterListName, mergedClusterListName));

                CaloHitList caloHitList;
                pParentCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);
                pDaughterCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);

                Cluster *pMergedCluster = NULL;
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, &caloHitList, pMergedCluster));

                // Look for peaks in cluster transverse shower profile
                ParticleIdHelper::ShowerPeakList showerPeakList;
                ParticleIdHelper::GetShowerPeaks(pMergedCluster, 30, showerPeakList);

                const float subsidiaryPeakEnergy((showerPeakList.size() > 1) ? showerPeakList[1].GetPeakEnergy() : 0.f);
                const float minFragmentEnergy(std::min(pDaughterCluster->GetElectromagneticEnergy(), pParentCluster->GetElectromagneticEnergy()));
                const float maxFragmentEnergy(std::max(pDaughterCluster->GetElectromagneticEnergy(), pParentCluster->GetElectromagneticEnergy()));

                // Decide whether merged cluster is better than individual fragments
                bool acceptMerge(false);

                if (minFragmentEnergy < 0.2f)
                {
                    acceptMerge = true;
                }
                else if (subsidiaryPeakEnergy < 0.5f)
                {
                    if (minFragmentEnergy < 0.05f * maxFragmentEnergy)
                    {
                        acceptMerge = true;
                    }
                    else if (subsidiaryPeakEnergy < 0.1f * minFragmentEnergy)
                    {
                        acceptMerge = true;
                    }
                }

                // If merging hard photons, check for early peaks in transverse profile
                if (acceptMerge && (minFragmentEnergy > 0.2f))
                {
                    // Look for peaks in cluster transverse shower profile
                    ParticleIdHelper::ShowerPeakList earlyShowerPeakList;
                    ParticleIdHelper::GetShowerPeaks(pMergedCluster, 20, earlyShowerPeakList);

                    const float earlySubsidiaryPeakEnergy((earlyShowerPeakList.size() > 1) ? earlyShowerPeakList[1].GetPeakEnergy() : 0.f);

                    if (earlySubsidiaryPeakEnergy > 0.5f)
                        acceptMerge = false;
                }

                // Tidy up
                const std::string clusterListToSaveName(acceptMerge ? mergedClusterListName : originalClusterListName);
                const std::string clusterListToDeleteName(acceptMerge ? originalClusterListName : mergedClusterListName);

                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::EndFragmentation(*this, clusterListToSaveName,
                    clusterListToDeleteName));

                if (acceptMerge)
                {
                    *iterI = NULL;
                    *iterJ = NULL;
                    break;
                }
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

PseudoLayer MergeSplitPhotonsAlgorithm::GetShowerMaxLayer(const Cluster *const pCluster) const
{
    const OrderedCaloHitList &orderedCaloHitList(pCluster->GetOrderedCaloHitList());

    float maxEnergyInLayer(0.f);
    PseudoLayer showerMaxLayer(0);
    bool isLayerFound(false);

    for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
    {
        float energyInLayer(0.f);

        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            energyInLayer += (*hitIter)->GetElectromagneticEnergy();
        }

        if (energyInLayer > maxEnergyInLayer)
        {
            maxEnergyInLayer = energyInLayer;
            showerMaxLayer = iter->first;
            isLayerFound = true;
        }
    }

    if (!isLayerFound)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return showerMaxLayer;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MergeSplitPhotonsAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}
