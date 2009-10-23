/**
 *  @file   PandoraPFANew/src/Managers/CaloHitManager.cc
 * 
 *  @brief  Implementation of the calo hit manager class.
 * 
 *  $Log: $
 */

#include "Helpers/GeometryHelper.h"

#include "Managers/CaloHitManager.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>

namespace pandora
{

const std::string CaloHitManager::INPUT_LIST_NAME = "input";

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::~CaloHitManager()
{
    (void) this->ResetForNextEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateCaloHit(const PandoraApi::CaloHitParameters &caloHitParameters)
{
    try
    {
        CaloHit *pCaloHit = NULL;
        pCaloHit = new CaloHit(caloHitParameters);

        if (NULL == pCaloHit)
            return STATUS_CODE_FAILURE;

        m_inputCaloHitVector.push_back(pCaloHit);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create calo hit: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::OrderInputCaloHits()
{
    OrderedCaloHitList orderedCaloHitList;

    for (CaloHitVector::iterator iter = m_inputCaloHitVector.begin(), iterEnd = m_inputCaloHitVector.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AssignToPseudoLayer(*iter));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.AddCaloHit(*iter));
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, SaveList(orderedCaloHitList, INPUT_LIST_NAME));
    m_currentListName = INPUT_LIST_NAME;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::AssignToPseudoLayer(CaloHit *const pCaloHit) const
{
    static const GeometryHelper *const pGeometryHelper = GeometryHelper::GetInstance();
    static const GeometryHelper::SubDetectorParameters eCalBarrelParameters = pGeometryHelper->GetECalBarrelParameters();
    static const GeometryHelper::SubDetectorParameters eCalEndCapParameters = pGeometryHelper->GetECalEndCapParameters();

    const CartesianVector positionVector = pCaloHit->GetPositionVector();
    const float zCoordinate = std::fabs(positionVector.GetZ());
    const float radius = pGeometryHelper->GetMaximumRadius(positionVector.GetX(), positionVector.GetY());

    PseudoLayer pseudoLayer;

    if (zCoordinate < eCalEndCapParameters.GetInnerZCoordinate())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pGeometryHelper->FindBarrelLayer(radius, pseudoLayer));
    }
    else if (radius < eCalBarrelParameters.GetInnerRCoordinate())
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pGeometryHelper->FindEndCapLayer(zCoordinate, pseudoLayer));
    }
    else
    {
        PseudoLayer bestBarrelLayer;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pGeometryHelper->FindBarrelLayer(radius, bestBarrelLayer));

        PseudoLayer bestEndCapLayer;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pGeometryHelper->FindEndCapLayer(zCoordinate, bestEndCapLayer));

        pseudoLayer = std::max(bestBarrelLayer, bestEndCapLayer);
    }

    return pCaloHit->SetPseudoLayer(pseudoLayer);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CalculateCaloHitProperties() const
{
    NameToOrderedCaloHitListMap::const_iterator iter = m_nameToOrderedCaloHitListMap.find(INPUT_LIST_NAME);

    if (m_nameToOrderedCaloHitListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    static const PseudoLayer densityWeightNLayers(PandoraSettings::GetInstance()->GetDensityWeightNLayers());
    static const PseudoLayer isolationNLayers(PandoraSettings::GetInstance()->GetIsolationNLayers());
    static const bool shouldUseSimpleIsolationScheme(PandoraSettings::GetInstance()->ShouldUseSimpleIsolationScheme());

    for (OrderedCaloHitList::const_iterator pseudoLayerIter = iter->second->begin(), pseudoLayerIterEnd = iter->second->end();
        pseudoLayerIter != pseudoLayerIterEnd; ++pseudoLayerIter)
    {
        const PseudoLayer pseudoLayer(pseudoLayerIter->first);

        for (CaloHitList::iterator caloHitIter = pseudoLayerIter->second->begin(), caloHitIterEnd = pseudoLayerIter->second->end();
            caloHitIter != caloHitIterEnd; ++caloHitIter)
        {
            CaloHit *pCaloHit = *caloHitIter;

            // Calculate number of adjacent pseudolayers to examine
            const PseudoLayer densityWeightMaxLayer(pseudoLayer + densityWeightNLayers);
            const PseudoLayer densityWeightMinLayer((pseudoLayer < densityWeightNLayers) ? 0 : pseudoLayer - densityWeightNLayers);

            const PseudoLayer isolationMaxLayer(pseudoLayer + isolationNLayers);
            const PseudoLayer isolationMinLayer((pseudoLayer < isolationNLayers) ? 0 : pseudoLayer - isolationNLayers);

            float densityWeight = 0;
            bool isIsolated = true;

            // Loop over adjacent pseudolayers
            for(PseudoLayer iPseudoLayer = std::min(densityWeightMinLayer, isolationMinLayer);
                iPseudoLayer <= std::max(densityWeightMaxLayer, isolationMaxLayer); ++iPseudoLayer)
            {
                OrderedCaloHitList::iterator adjacentPseudoLayerIter = iter->second->find(iPseudoLayer);

                if (iter->second->end() == adjacentPseudoLayerIter)
                    continue;

                CaloHitList *pCaloHitList = adjacentPseudoLayerIter->second;

                if ((densityWeightMinLayer <= iPseudoLayer) && (densityWeightMaxLayer >= iPseudoLayer))
                    this->CalculateDensityWeight(pCaloHit, pCaloHitList, densityWeight);

                if (!shouldUseSimpleIsolationScheme && isIsolated && (isolationMinLayer <= iPseudoLayer) && (isolationMaxLayer >= iPseudoLayer))
                    this->IdentifyIsolatedHits(pCaloHit, pCaloHitList, isIsolated);

                if (pseudoLayer == iPseudoLayer)
                {
                    this->IdentifyPossibleMipHits(pCaloHit, pCaloHitList);
                    this->CalculateSurroundingEnergy(pCaloHit, pCaloHitList);
                }
            }

            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCaloHit->SetDensityWeight(densityWeight));

            if (!shouldUseSimpleIsolationScheme && isIsolated)
                pCaloHit->SetIsolatedFlag(true);
        }
    }

    if (shouldUseSimpleIsolationScheme)
        this->IdentifyIsolatedHits();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitManager::CalculateDensityWeight(CaloHit *const pCaloHit, CaloHitList *const pCaloHitList, float &densityWeight) const
{
    static const float caloHitMaxSeparation(PandoraSettings::GetInstance()->GetCaloHitMaxSeparation());
    static const unsigned int densityWeightPower(PandoraSettings::GetInstance()->GetDensityWeightPower());

    const CartesianVector positionVector(pCaloHit->GetPositionVector());
    const float positionMagnitude(positionVector.GetMagnitude());

    for(CaloHitList::iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference = positionVector - (*iter)->GetPositionVector();
        const CartesianVector crossProduct = positionVector.GetCrossProduct(positionDifference);

        if (positionDifference.GetMagnitude() > caloHitMaxSeparation)
            continue;

        const float rN(pow(crossProduct.GetMagnitude() / positionMagnitude, densityWeightPower));

        if (0 == rN)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        densityWeight += (100. / rN);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitManager::IdentifyIsolatedHits(CaloHit *const pCaloHit, CaloHitList *const pCaloHitList, bool &isIsolated) const
{
    static const float caloHitMaxSeparation(PandoraSettings::GetInstance()->GetCaloHitMaxSeparation());
    static const float isolationCutDistanceECal(PandoraSettings::GetInstance()->GetIsolationCutDistanceECal());
    static const float isolationCutDistanceHCal(PandoraSettings::GetInstance()->GetIsolationCutDistanceHCal());
    static const float isolationMaxNearbyHits(PandoraSettings::GetInstance()->GetIsolationMaxNearbyHits());

    unsigned int nearbyHitsFound = 0;
    const CartesianVector positionVector(pCaloHit->GetPositionVector());
    const float positionMagnitude(positionVector.GetMagnitude());
    const float isolationCutDistance((pCaloHit->GetHitType() == ECAL) ? isolationCutDistanceECal : isolationCutDistanceHCal);

    for(CaloHitList::iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference = positionVector - (*iter)->GetPositionVector();
        const CartesianVector crossProduct = positionVector.GetCrossProduct(positionDifference);

        if (positionDifference.GetMagnitude() > caloHitMaxSeparation)
            continue;

        if((crossProduct.GetMagnitude() / positionMagnitude) < isolationCutDistance)
        {
            ++nearbyHitsFound;

            if(nearbyHitsFound >= isolationMaxNearbyHits)
            {
                isIsolated = false;
                return;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitManager::IdentifyIsolatedHits() const
{
    static const float isolationDensityWeightCutECal(PandoraSettings::GetInstance()->GetIsolationDensityWeightCutECal());
    static const float isolationDensityWeightCutHCal(PandoraSettings::GetInstance()->GetIsolationDensityWeightCutHCal());

    for (CaloHitVector::const_iterator iter = m_inputCaloHitVector.begin(), iterEnd = m_inputCaloHitVector.end(); iter != iterEnd; ++iter)
    {
        CaloHit *pCaloHit = *iter;
        const float isolationDensityWeightCut((pCaloHit->GetHitType() == ECAL) ? isolationDensityWeightCutECal : isolationDensityWeightCutHCal);

        if (pCaloHit->GetDensityWeight() < isolationDensityWeightCut)
            pCaloHit->SetIsolatedFlag(true);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitManager::IdentifyPossibleMipHits(CaloHit *const pCaloHit, CaloHitList *const pCaloHitList) const
{
    static const float caloHitMaxSeparation(PandoraSettings::GetInstance()->GetCaloHitMaxSeparation());
    static const float mipLikeMipCut(PandoraSettings::GetInstance()->GetMipLikeMipCut());
    static const float mipNCellsForNearbyHit(PandoraSettings::GetInstance()->GetMipNCellsForNearbyHit() + 0.5);
    static const unsigned int mipMaxNearbyHits(PandoraSettings::GetInstance()->GetMipMaxNearbyHits());

    if (pCaloHit->GetMipEquivalentEnergy() > mipLikeMipCut)
        return;

    unsigned int nearbyHitCount = 0;
    const CartesianVector positionVector(pCaloHit->GetPositionVector());
    const bool isHitInBarrelRegion(pCaloHit->GetDetectorRegion() == BARREL);

    for(CaloHitList::iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference = positionVector - (*iter)->GetPositionVector();

        if (positionDifference.GetMagnitude() > caloHitMaxSeparation)
            continue;

        if(isHitInBarrelRegion)
        {
            const float dZ(fabs(positionDifference.GetZ()));
            const float dPhi(std::sqrt(pow(positionDifference.GetX(), 2) + pow(positionDifference.GetY(), 2)));

            if( (dZ < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeZ())) && (dPhi < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeV())) )
                ++nearbyHitCount;
        }
        else
        {
            const float dX(fabs(positionDifference.GetX()));
            const float dY(fabs(positionDifference.GetY()));

            if( (dX < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeZ())) && (dY < (mipNCellsForNearbyHit * pCaloHit->GetCellSizeV())) )
                ++nearbyHitCount;
        }
    }

    if (nearbyHitCount <= mipMaxNearbyHits)
        pCaloHit->SetPossibleMipFlag(true);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitManager::CalculateSurroundingEnergy(CaloHit *const pCaloHit, CaloHitList *const pCaloHitList) const
{
    static const float caloHitMaxSeparation(PandoraSettings::GetInstance()->GetCaloHitMaxSeparation());

    const CartesianVector positionVector(pCaloHit->GetPositionVector());
    const bool isHitInBarrelRegion(pCaloHit->GetDetectorRegion() == BARREL);

    for(CaloHitList::iterator iter = pCaloHitList->begin(), iterEnd = pCaloHitList->end(); iter != iterEnd; ++iter)
    {
        if (pCaloHit == *iter)
            continue;

        const CartesianVector positionDifference = positionVector - (*iter)->GetPositionVector();

        if (positionDifference.GetMagnitude() > caloHitMaxSeparation)
            continue;

        if(isHitInBarrelRegion)
        {
            const float dZ(fabs(positionDifference.GetZ()));
            const float dPhi(std::sqrt(pow(positionDifference.GetX(), 2) + pow(positionDifference.GetY(), 2)));

            if( (dZ < (1.5 * pCaloHit->GetCellSizeZ())) && (dPhi < (1.5 * pCaloHit->GetCellSizeV())) )
                pCaloHit->AddSurroundingEnergy((*iter)->GetHadronicEnergy());
        }
        else
        {
            const float dX(fabs(positionDifference.GetX()));
            const float dY(fabs(positionDifference.GetY()));

            if( (dX < (1.5 * pCaloHit->GetCellSizeZ())) && (dY < (1.5 * pCaloHit->GetCellSizeV())) )
                pCaloHit->AddSurroundingEnergy((*iter)->GetHadronicEnergy());
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::GetList(const std::string &listName, const OrderedCaloHitList *&pOrderedCaloHitList) const
{
    NameToOrderedCaloHitListMap::const_iterator iter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    pOrderedCaloHitList = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &orderedCaloHitListName)
{
    if (m_nameToOrderedCaloHitListMap.end() == m_nameToOrderedCaloHitListMap.find(orderedCaloHitListName))
        return STATUS_CODE_NOT_FOUND;

    if ((m_algorithmInfoMap.size() > 1) || (m_savedLists.end() == m_savedLists.find(orderedCaloHitListName)))
        return STATUS_CODE_NOT_ALLOWED;

    m_currentListName = orderedCaloHitListName;

    AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    iter->second.m_parentListName = orderedCaloHitListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const OrderedCaloHitList &orderedCaloHitList,
    std::string &temporaryListName)
{
    if (orderedCaloHitList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

    if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
        return STATUS_CODE_ALREADY_PRESENT;

    m_nameToOrderedCaloHitListMap[temporaryListName] = new OrderedCaloHitList(orderedCaloHitList);
    m_currentListName = temporaryListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ClusterList &clusterList,
    std::string &temporaryListName)
{
    if (clusterList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    const OrderedCaloHitList orderedCaloHitList = (*clusterList.begin())->GetOrderedCaloHitList();
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CreateTemporaryListAndSetCurrent(pAlgorithm, orderedCaloHitList, temporaryListName));

    for (ClusterList::const_iterator iter = ++(clusterList.begin()), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_nameToOrderedCaloHitListMap[temporaryListName]->Add((*iter)->GetOrderedCaloHitList()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::SaveList(const OrderedCaloHitList &orderedCaloHitList, const std::string &newListName)
{
    if (m_nameToOrderedCaloHitListMap.end() != m_nameToOrderedCaloHitListMap.find(newListName))
        return STATUS_CODE_ALREADY_PRESENT;

    if (!m_nameToOrderedCaloHitListMap.insert(NameToOrderedCaloHitListMap::value_type(newListName, new OrderedCaloHitList)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    *(m_nameToOrderedCaloHitListMap[newListName]) = orderedCaloHitList;
    m_savedLists.insert(newListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::AddCaloHitsToList(const std::string &listName, const OrderedCaloHitList &orderedCaloHitList)
{
    NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, listIter->second->Add(orderedCaloHitList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::AddCaloHitsToList(const std::string &listName, const ClusterList &clusterList)
{
    if (clusterList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, listIter->second->Add((*iter)->GetOrderedCaloHitList()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::RemoveCaloHitsFromList(const std::string &listName, const OrderedCaloHitList &orderedCaloHitList)
{
    NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, listIter->second->Remove(orderedCaloHitList));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::RemoveCaloHitsFromList(const std::string &listName, const ClusterList &clusterList)
{
    if (clusterList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.find(listName);

    if (m_nameToOrderedCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, listIter->second->Remove((*iter)->GetOrderedCaloHitList()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::MatchCaloHitsToMCPfoTargets(const UidToMCParticleMap &caloHitToPfoTargetMap)
{
    if (caloHitToPfoTargetMap.empty())
        return STATUS_CODE_SUCCESS;

    for (CaloHitVector::iterator iter = m_inputCaloHitVector.begin(), iterEnd = m_inputCaloHitVector.end(); iter != iterEnd; ++iter)
    {
        UidToMCParticleMap::const_iterator pfoTargetIter = caloHitToPfoTargetMap.find((*iter)->GetParentCaloHitAddress());

        if (caloHitToPfoTargetMap.end() != pfoTargetIter)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetMCParticle(pfoTargetIter->second));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::RegisterAlgorithm(const Algorithm *const pAlgorithm)
{
    if (m_algorithmInfoMap.end() != m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_ALREADY_PRESENT;

    AlgorithmInfo algorithmInfo;
    algorithmInfo.m_parentListName = m_currentListName;
    algorithmInfo.m_numberOfListsCreated = 0;

    if (!m_algorithmInfoMap.insert(AlgorithmInfoMap::value_type(pAlgorithm, algorithmInfo)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ResetAlgorithmInfo(const Algorithm *const pAlgorithm, bool isAlgorithmFinished)
{
    AlgorithmInfoMap::iterator algorithmListIter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == algorithmListIter)
        return STATUS_CODE_NOT_FOUND;

    for (StringSet::const_iterator listNameIter = algorithmListIter->second.m_temporaryListNames.begin(),
        listNameIterEnd = algorithmListIter->second.m_temporaryListNames.end(); listNameIter != listNameIterEnd; ++listNameIter)
    {
        NameToOrderedCaloHitListMap::iterator iter = m_nameToOrderedCaloHitListMap.find(*listNameIter);

        if (m_nameToOrderedCaloHitListMap.end() == iter)
            return STATUS_CODE_FAILURE;

        delete iter->second;
        m_nameToOrderedCaloHitListMap.erase(iter);
    }

    algorithmListIter->second.m_temporaryListNames.clear();
    m_currentListName = algorithmListIter->second.m_parentListName;

    if (isAlgorithmFinished)
        m_algorithmInfoMap.erase(algorithmListIter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ResetForNextEvent()
{
    for (CaloHitVector::iterator iter = m_inputCaloHitVector.begin(), iterEnd = m_inputCaloHitVector.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_inputCaloHitVector.clear();

    for (NameToOrderedCaloHitListMap::iterator listIter = m_nameToOrderedCaloHitListMap.begin();
        listIter != m_nameToOrderedCaloHitListMap.end(); ++listIter)
    {
        delete listIter->second;
        m_nameToOrderedCaloHitListMap.erase(listIter);
    }

    m_nameToOrderedCaloHitListMap.clear();
    m_savedLists.clear();
    m_currentListName.clear();

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
