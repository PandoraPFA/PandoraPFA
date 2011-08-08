/**
 *  @file   PandoraPFANew/Framework/src/Managers/CaloHitManager.cc
 * 
 *  @brief  Implementation of the calo hit manager class.
 * 
 *  $Log: $
 */

#include "Helpers/CaloHitHelper.h"
#include "Helpers/GeometryHelper.h"

#include "Managers/CaloHitManager.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"

#include "Pandora/PandoraSettings.h"

#include <cmath>

namespace pandora
{

const std::string CaloHitManager::NULL_LIST_NAME = "NullList";
const std::string CaloHitManager::INPUT_LIST_NAME = "Input";

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::CaloHitManager() :
    m_nReclusteringProcesses(0),
    m_pCurrentReclusterMetadata(NULL),
    m_currentListName(NULL_LIST_NAME)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::~CaloHitManager()
{
    (void) this->EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::IsCaloHitAvailable(CaloHit *const pCaloHit) const
{
    if (0 == m_nReclusteringProcesses)
        return pCaloHit->m_isAvailable;

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->IsCaloHitAvailable(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::AreCaloHitsAvailable(const CaloHitList &caloHitList) const
{
    if (0 == m_nReclusteringProcesses)
    {
        for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
        {
            if (!(*iter)->m_isAvailable)
                return false;
        }
        return true;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->AreCaloHitsAvailable(caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
    {
        pCaloHit->m_isAvailable = isAvailable;
        return STATUS_CODE_SUCCESS;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->SetCaloHitAvailability(pCaloHit, isAvailable);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::SetCaloHitAvailability(CaloHitList &caloHitList, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
    {
        for (CaloHitList::iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
        {
            (*iter)->m_isAvailable = isAvailable;
        }
        return STATUS_CODE_SUCCESS;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->SetCaloHitAvailability(caloHitList, isAvailable);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::InitializeReclustering(const Algorithm *const pAlgorithm, const ClusterList &clusterList,
    const std::string &originalReclusterListName)
{
    std::string caloHitListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateTemporaryListAndSetCurrent(pAlgorithm, clusterList, caloHitListName));
    CaloHitList *pCaloHitList = m_nameToCaloHitListMap[caloHitListName];

    m_pCurrentReclusterMetadata = new ReclusterMetadata(pCaloHitList);
    m_reclusterMetadataList.push_back(m_pCurrentReclusterMetadata);

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->CreateCaloHitMetadata(pCaloHitList, caloHitListName,
        originalReclusterListName, false));

    ++m_nReclusteringProcesses;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::PrepareForClustering(const Algorithm *const pAlgorithm, const std::string &newReclusterListName)
{
    if (0 == m_nReclusteringProcesses)
        return STATUS_CODE_SUCCESS;

    const CaloHitList &caloHitList(m_pCurrentReclusterMetadata->GetCaloHitList());

    std::string caloHitListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateTemporaryListAndSetCurrent(pAlgorithm, caloHitList, caloHitListName));
    CaloHitList *pCaloHitList = m_nameToCaloHitListMap[caloHitListName];

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->CreateCaloHitMetadata(pCaloHitList, caloHitListName,
        newReclusterListName, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::EndReclustering(const Algorithm *const pAlgorithm, const std::string &selectedReclusterListName)
{
    if (0 == m_nReclusteringProcesses)
        return STATUS_CODE_SUCCESS;

    CaloHitMetadata *pSelectedCaloHitMetaData = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->ExtractCaloHitMetadata(selectedReclusterListName,
        pSelectedCaloHitMetaData));

    m_reclusterMetadataList.pop_back();
    delete m_pCurrentReclusterMetadata;

    if (--m_nReclusteringProcesses > 0)
    {
        m_pCurrentReclusterMetadata = m_reclusterMetadataList.back();
        CaloHitMetadata *pCurrentCaloHitMetaData = m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata();
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pCurrentCaloHitMetaData->Update(*pSelectedCaloHitMetaData));
    }
    else
    {
        m_pCurrentReclusterMetadata = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(*pSelectedCaloHitMetaData));
    }

    pSelectedCaloHitMetaData->Clear();
    delete pSelectedCaloHitMetaData;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Update(const CaloHitManager::CaloHitMetadata &caloHitMetadata)
{
    const CaloHitReplacementList &caloHitReplacementList(caloHitMetadata.GetCaloHitReplacementList());

    for (CaloHitReplacementList::const_iterator iter = caloHitReplacementList.begin(), iterEnd = caloHitReplacementList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(*(*iter)));
    }

    const CaloHitUsageMap &caloHitUsageMap(caloHitMetadata.GetCaloHitUsageMap());

    for (CaloHitUsageMap::const_iterator iter = caloHitUsageMap.begin(), iterEnd = caloHitUsageMap.end(); iter != iterEnd; ++iter)
    {
        iter->first->m_isAvailable = iter->second;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Update(const CaloHitManager::CaloHitReplacement &caloHitReplacement)
{
    for (NameToCaloHitListMap::const_iterator listIter = m_nameToCaloHitListMap.begin(), listIterEnd = m_nameToCaloHitListMap.end();
        listIter != listIterEnd; ++listIter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(listIter->second, caloHitReplacement));
    }

    for (CaloHitList::const_iterator hitIter = caloHitReplacement.m_oldCaloHits.begin(), hitIterEnd = caloHitReplacement.m_oldCaloHits.end();
        hitIter != hitIterEnd; ++hitIter)
    {
        delete *hitIter;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::Update(CaloHitList *pCaloHitList, const CaloHitManager::CaloHitReplacement &caloHitReplacement)
{
    if (caloHitReplacement.m_newCaloHits.empty() || caloHitReplacement.m_oldCaloHits.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    bool replacementFound(false), allReplacementsFound(true);

    for (CaloHitList::const_iterator hitIter = caloHitReplacement.m_oldCaloHits.begin(), hitIterEnd = caloHitReplacement.m_oldCaloHits.end();
        hitIter != hitIterEnd; ++hitIter)
    {
        CaloHitList::iterator listIter = pCaloHitList->find(*hitIter);

        if (pCaloHitList->end() != listIter)
        {
            pCaloHitList->erase(listIter);
            replacementFound = true;
            continue;
        }

        allReplacementsFound = false;
    }

    if (!replacementFound)
        return STATUS_CODE_SUCCESS;

    if (!allReplacementsFound)
        std::cout << "CaloHitManager::Update - imperfect calo hit replacements made to list " << std::endl;

    for (CaloHitList::const_iterator hitIter = caloHitReplacement.m_newCaloHits.begin(), hitIterEnd = caloHitReplacement.m_newCaloHits.end();
        hitIter != hitIterEnd; ++hitIter)
    {
        if (!pCaloHitList->insert(*hitIter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::FragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1, CaloHit *&pDaughterCaloHit1, CaloHit *&pDaughterCaloHit2)
{
    pDaughterCaloHit1 = NULL; pDaughterCaloHit2 = NULL;

    if (!this->CanFragmentCaloHit(pOriginalCaloHit, fraction1))
        return STATUS_CODE_NOT_ALLOWED;

    if (RECTANGULAR == pOriginalCaloHit->GetCaloCellType())
    {
        RectangularCaloHit *pOriginalRectangularCaloHit = dynamic_cast<RectangularCaloHit *>(pOriginalCaloHit);
        pDaughterCaloHit1 = new RectangularCaloHit(pOriginalRectangularCaloHit, fraction1);
        pDaughterCaloHit2 = new RectangularCaloHit(pOriginalRectangularCaloHit, 1.f - fraction1);
    }
    else if (POINTING == pOriginalCaloHit->GetCaloCellType())
    {
        PointingCaloHit *pOriginalPointingCaloHit = dynamic_cast<PointingCaloHit *>(pOriginalCaloHit);
        pDaughterCaloHit1 = new PointingCaloHit(pOriginalPointingCaloHit, fraction1);
        pDaughterCaloHit2 = new PointingCaloHit(pOriginalPointingCaloHit, 1.f - fraction1);
    }

    if ((NULL == pDaughterCaloHit1) || (NULL == pDaughterCaloHit2))
        return STATUS_CODE_FAILURE;

    CaloHitReplacement caloHitReplacement;
    caloHitReplacement.m_oldCaloHits.insert(pOriginalCaloHit);
    caloHitReplacement.m_newCaloHits.insert(pDaughterCaloHit1); caloHitReplacement.m_newCaloHits.insert(pDaughterCaloHit2);

    if (m_nReclusteringProcesses > 0)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->Update(caloHitReplacement));
    }
    else
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(caloHitReplacement));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::MergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2, CaloHit *&pMergedCaloHit)
{
    pMergedCaloHit = NULL;

    if (!this->CanMergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2))
        return STATUS_CODE_NOT_ALLOWED;

    const float newWeight((pFragmentCaloHit1->GetWeight() + pFragmentCaloHit2->GetWeight()) / pFragmentCaloHit1->GetWeight());

    if ((RECTANGULAR == pFragmentCaloHit1->GetCaloCellType()) && (RECTANGULAR == pFragmentCaloHit2->GetCaloCellType()))
    {
        RectangularCaloHit *pOriginalRectangularCaloHit = dynamic_cast<RectangularCaloHit *>(pFragmentCaloHit1);
        pMergedCaloHit = new RectangularCaloHit(pOriginalRectangularCaloHit, newWeight);
    }
    else if ((POINTING == pFragmentCaloHit1->GetCaloCellType()) && (POINTING == pFragmentCaloHit2->GetCaloCellType()))
    {
        PointingCaloHit *pOriginalPointingCaloHit = dynamic_cast<PointingCaloHit *>(pFragmentCaloHit1);
        pMergedCaloHit = new PointingCaloHit(pOriginalPointingCaloHit, newWeight);
    }

    if (NULL == pMergedCaloHit)
        return STATUS_CODE_FAILURE;

    CaloHitReplacement caloHitReplacement;
    caloHitReplacement.m_newCaloHits.insert(pMergedCaloHit);
    caloHitReplacement.m_oldCaloHits.insert(pFragmentCaloHit1); caloHitReplacement.m_newCaloHits.insert(pFragmentCaloHit2);

    if (m_nReclusteringProcesses > 0)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->Update(caloHitReplacement));
    }
    else
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(caloHitReplacement));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::CanFragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1) const
{
    if ((fraction1 < std::numeric_limits<float>::epsilon()) || (fraction1 > 1.f))
        return false;

    if (!this->IsCaloHitAvailable(pOriginalCaloHit))
        return false;

    NameToCaloHitListMap::const_iterator iter = m_nameToCaloHitListMap.find(m_currentListName);

    if (m_nameToCaloHitListMap.end() == iter)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    if (iter->second->end() == iter->second->find(pOriginalCaloHit))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::CanMergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2) const
{
    if (pFragmentCaloHit1->GetWeight() < std::numeric_limits<float>::epsilon())
        return false;

    if (pFragmentCaloHit1->GetParentCaloHitAddress() != pFragmentCaloHit2->GetParentCaloHitAddress())
        return false;

    if (!this->IsCaloHitAvailable(pFragmentCaloHit1) || !this->IsCaloHitAvailable(pFragmentCaloHit2))
        return false;

    NameToCaloHitListMap::const_iterator iter = m_nameToCaloHitListMap.find(m_currentListName);

    if (m_nameToCaloHitListMap.end() == iter)
        throw StatusCodeException(STATUS_CODE_FAILURE);

    if ((iter->second->end() == iter->second->find(pFragmentCaloHit1)) || (iter->second->end() == iter->second->find(pFragmentCaloHit2)))
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS>
StatusCode CaloHitManager::CreateCaloHit(const PARAMETERS &parameters)
{
    try
    {
        CaloHit *pCaloHit = NULL;
        pCaloHit = this->HitInstantiation(parameters);

        NameToCaloHitListMap::iterator inputIter = m_nameToCaloHitListMap.find(INPUT_LIST_NAME);

        if ((NULL == pCaloHit) || (m_nameToCaloHitListMap.end() == inputIter) || (!inputIter->second->insert(pCaloHit).second))
            throw StatusCodeException(STATUS_CODE_FAILURE);

        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create calo hit: " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS>
CaloHit *CaloHitManager::HitInstantiation(const PARAMETERS &parameters)
{
    return NULL;
}

template <>
CaloHit *CaloHitManager::HitInstantiation(const PandoraApi::RectangularCaloHitParameters &parameters)
{
    return new RectangularCaloHit(parameters);
}

template <>
CaloHit *CaloHitManager::HitInstantiation(const PandoraApi::PointingCaloHitParameters &parameters)
{
    return new PointingCaloHit(parameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::OrderInputCaloHits()
{
    NameToCaloHitListMap::iterator existingListIter = m_nameToCaloHitListMap.find(INPUT_LIST_NAME);

    if (m_nameToCaloHitListMap.end() == existingListIter)
        return STATUS_CODE_FAILURE;

    for (CaloHitList::iterator iter = existingListIter->second->begin(), iterEnd = existingListIter->second->end(); iter != iterEnd; )
    {
        try
        {
            PseudoLayer pseudoLayer = GeometryHelper::GetPseudoLayer((*iter)->GetPositionVector());
            PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, (*iter)->SetPseudoLayer(pseudoLayer));
            ++iter;
        }
        catch (StatusCodeException &statusCodeException)
        {
            std::cout << "Failed to assign hit to pseudolayer, " << statusCodeException.ToString() << std::endl;
            delete *iter;
            existingListIter->second->erase(iter++);
        }
    }

    m_currentListName = INPUT_LIST_NAME;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CalculateCaloHitProperties() const
{
    try
    {
        NameToCaloHitListMap::const_iterator listIter = m_nameToCaloHitListMap.find(INPUT_LIST_NAME);

        if (m_nameToCaloHitListMap.end() == listIter)
            return STATUS_CODE_NOT_INITIALIZED;

        OrderedCaloHitList orderedCaloHitList;
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, orderedCaloHitList.Add(*(listIter->second)));

        for (OrderedCaloHitList::const_iterator iter = orderedCaloHitList.begin(), iterEnd = orderedCaloHitList.end(); iter != iterEnd; ++iter)
        {
            for (CaloHitList::iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
            {
                CaloHitHelper::CalculateCaloHitProperties(*hitIter, &orderedCaloHitList);
            }
        }
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "CaloHitManager: Failed to calculate calo hit properties, " << statusCodeException.ToString() << std::endl;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::GetList(const std::string &listName, const CaloHitList *&pCaloHitList) const
{
    NameToCaloHitListMap::const_iterator iter = m_nameToCaloHitListMap.find(listName);

    if (m_nameToCaloHitListMap.end() == iter)
        return STATUS_CODE_NOT_INITIALIZED;

    pCaloHitList = iter->second;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ReplaceCurrentAndAlgorithmInputLists(const Algorithm *const pAlgorithm, const std::string &caloHitListName)
{
    if (m_nameToCaloHitListMap.end() == m_nameToCaloHitListMap.find(caloHitListName))
        return STATUS_CODE_NOT_FOUND;

    if (m_savedLists.end() == m_savedLists.find(caloHitListName))
        return STATUS_CODE_NOT_ALLOWED;

    if (m_algorithmInfoMap.end() == m_algorithmInfoMap.find(pAlgorithm))
        return STATUS_CODE_FAILURE;

    m_currentListName = caloHitListName;

    for (AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.begin(), iterEnd = m_algorithmInfoMap.end(); iter != iterEnd; ++iter)
    {
        iter->second.m_parentListName = caloHitListName;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const CaloHitList &caloHitList,
    std::string &temporaryListName)
{
    if (caloHitList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    AlgorithmInfoMap::iterator iter = m_algorithmInfoMap.find(pAlgorithm);

    if (m_algorithmInfoMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    temporaryListName = TypeToString(pAlgorithm) + "_" + TypeToString(iter->second.m_numberOfListsCreated++);

    if (!iter->second.m_temporaryListNames.insert(temporaryListName).second)
        return STATUS_CODE_ALREADY_PRESENT;

    m_nameToCaloHitListMap[temporaryListName] = new CaloHitList(caloHitList);
    m_currentListName = temporaryListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ClusterList &clusterList,
    std::string &temporaryListName)
{
    if (clusterList.empty())
        return STATUS_CODE_NOT_INITIALIZED;

    CaloHitList caloHitList;

    for (ClusterList::const_iterator iter = clusterList.begin(), iterEnd = clusterList.end(); iter != iterEnd; ++iter)
    {
        Cluster *pCluster = *iter;
        pCluster->GetOrderedCaloHitList().GetCaloHitList(caloHitList);
        caloHitList.insert(pCluster->GetIsolatedCaloHitList().begin(), pCluster->GetIsolatedCaloHitList().end());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, CreateTemporaryListAndSetCurrent(pAlgorithm, caloHitList, temporaryListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::SaveList(const CaloHitList &caloHitList, const std::string &newListName)
{
    if (m_nameToCaloHitListMap.end() != m_nameToCaloHitListMap.find(newListName))
        return this->AddCaloHitsToList(newListName, caloHitList);

    if (!m_nameToCaloHitListMap.insert(NameToCaloHitListMap::value_type(newListName, new CaloHitList)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    *(m_nameToCaloHitListMap[newListName]) = caloHitList;
    m_savedLists.insert(newListName);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::AddCaloHitsToList(const std::string &listName, const CaloHitList &caloHitList)
{
    NameToCaloHitListMap::iterator listIter = m_nameToCaloHitListMap.find(listName);

    if (m_nameToCaloHitListMap.end() == listIter)
        return STATUS_CODE_NOT_FOUND;

    if (listIter->second == &caloHitList)
        return STATUS_CODE_INVALID_PARAMETER;

    for (CaloHitList::const_iterator hitIter = caloHitList.begin(), hitIterEnd = caloHitList.end(); hitIter != hitIterEnd; ++hitIter)
    {
        if (!listIter->second->insert(*hitIter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::MatchCaloHitsToMCPfoTargets(const UidToMCParticleMap &caloHitToPfoTargetMap)
{
    if (caloHitToPfoTargetMap.empty())
        return STATUS_CODE_SUCCESS;

    NameToCaloHitListMap::const_iterator inputIter = m_nameToCaloHitListMap.find(INPUT_LIST_NAME);

    if (m_nameToCaloHitListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (CaloHitList::iterator iter = inputIter->second->begin(), iterEnd = inputIter->second->end(); iter != iterEnd; ++iter)
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
        NameToCaloHitListMap::iterator iter = m_nameToCaloHitListMap.find(*listNameIter);

        if (m_nameToCaloHitListMap.end() == iter)
            return STATUS_CODE_FAILURE;

        delete iter->second;
        m_nameToCaloHitListMap.erase(iter);
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
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->EraseAllContent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::EraseAllContent()
{
    NameToCaloHitListMap::const_iterator inputIter = m_nameToCaloHitListMap.find(INPUT_LIST_NAME);

    if (m_nameToCaloHitListMap.end() == inputIter)
    {
        std::cout << "CaloHitManager::EraseAllContent cannot retrieve input calo hit list " << std::endl;
    }
    else
    {
        for (CaloHitList::iterator iter = inputIter->second->begin(), iterEnd = inputIter->second->end(); iter != iterEnd; ++iter)
            delete *iter;
    }

    for (NameToCaloHitListMap::iterator listIter = m_nameToCaloHitListMap.begin(); listIter != m_nameToCaloHitListMap.end();)
    {
        delete listIter->second;
        m_nameToCaloHitListMap.erase(listIter++);
    }

    m_currentListName = NULL_LIST_NAME;
    m_nameToCaloHitListMap.clear();
    m_savedLists.clear();

    for (ReclusterMetadataList::iterator iter = m_reclusterMetadataList.begin(), iterEnd = m_reclusterMetadataList.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_nReclusteringProcesses = 0;
    m_pCurrentReclusterMetadata = NULL;
    m_reclusterMetadataList.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CreateInitialLists()
{
    if (!m_nameToCaloHitListMap.empty() || !m_savedLists.empty())
        return STATUS_CODE_NOT_ALLOWED;

    CaloHitList caloHitList;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, SaveList(caloHitList, NULL_LIST_NAME));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, SaveList(caloHitList, INPUT_LIST_NAME));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::CaloHitMetadata::CaloHitMetadata(CaloHitList *pCaloHitList, const std::string &caloHitListName, const bool initialHitAvailability) :
    m_pCaloHitList(pCaloHitList),
    m_caloHitListName(caloHitListName)
{
    for (CaloHitList::const_iterator hitIter = pCaloHitList->begin(), hitIterEnd = pCaloHitList->end(); hitIter != hitIterEnd; ++hitIter)
    {
        if (!m_caloHitUsageMap.insert(CaloHitUsageMap::value_type(*hitIter, initialHitAvailability)).second)
            throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::CaloHitMetadata::~CaloHitMetadata()
{
    for (CaloHitReplacementList::iterator iter = m_caloHitReplacementList.begin(), iterEnd = m_caloHitReplacementList.end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::iterator hitIter = (*iter)->m_newCaloHits.begin(), hitIterEnd = (*iter)->m_newCaloHits.end(); hitIter != hitIterEnd; ++hitIter)
            delete *hitIter;

        delete *iter;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CaloHitMetadata::Update(const CaloHitMetadata &caloHitMetadata)
{
    const CaloHitReplacementList &caloHitReplacementList(caloHitMetadata.GetCaloHitReplacementList());

    for (CaloHitReplacementList::const_iterator iter = caloHitReplacementList.begin(), iterEnd = caloHitReplacementList.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Update(*(*iter)));
    }

    const CaloHitUsageMap &caloHitUsageMap(caloHitMetadata.GetCaloHitUsageMap());

    for (CaloHitUsageMap::const_iterator iter = caloHitUsageMap.begin(), iterEnd = caloHitUsageMap.end(); iter != iterEnd; ++iter)
    {
        CaloHitUsageMap::iterator usageMapIter = m_caloHitUsageMap.find(iter->first);

        if (m_caloHitUsageMap.end() == usageMapIter)
            return STATUS_CODE_FAILURE;

        usageMapIter->second = iter->second;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CaloHitMetadata::Update(const CaloHitReplacement &caloHitReplacement)
{
    for (CaloHitList::const_iterator iter = caloHitReplacement.m_newCaloHits.begin(), iterEnd = caloHitReplacement.m_newCaloHits.end();
        iter != iterEnd; ++iter)
    {
        if (!m_pCaloHitList->insert(*iter).second)
            return STATUS_CODE_ALREADY_PRESENT;

        if (!m_caloHitUsageMap.insert(CaloHitUsageMap::value_type(*iter, true)).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    for (CaloHitList::const_iterator iter = caloHitReplacement.m_oldCaloHits.begin(), iterEnd = caloHitReplacement.m_oldCaloHits.end();
        iter != iterEnd; ++iter)
    {
        CaloHitList::iterator listIter = m_pCaloHitList->find(*iter);

        if (m_pCaloHitList->end() == listIter)
            return STATUS_CODE_FAILURE;

        m_pCaloHitList->erase(listIter);

        CaloHitUsageMap::iterator mapIter = m_caloHitUsageMap.find(*iter);

        if (m_caloHitUsageMap.end() == mapIter)
            return STATUS_CODE_FAILURE;

        m_caloHitUsageMap.erase(mapIter);
    }

    m_caloHitReplacementList.push_back(new CaloHitReplacement(caloHitReplacement));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHitManager::CaloHitMetadata::Clear()
{
    for (CaloHitReplacementList::iterator iter = m_caloHitReplacementList.begin(), iterEnd = m_caloHitReplacementList.end(); iter != iterEnd; ++iter)
        delete *iter;

    m_pCaloHitList = NULL;
    m_caloHitListName.clear();
    m_caloHitUsageMap.clear();
    m_caloHitReplacementList.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const CaloHitManager::CaloHitUsageMap &CaloHitManager::CaloHitMetadata::GetCaloHitUsageMap() const
{
    return m_caloHitUsageMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const CaloHitManager::CaloHitReplacementList &CaloHitManager::CaloHitMetadata::GetCaloHitReplacementList() const
{
    return m_caloHitReplacementList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::CaloHitMetadata::IsCaloHitAvailable(CaloHit *const pCaloHit) const
{
    CaloHitUsageMap::const_iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

    if ((m_caloHitUsageMap.end()) == usageMapIter || !usageMapIter->second)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool CaloHitManager::CaloHitMetadata::AreCaloHitsAvailable(const CaloHitList &caloHitList) const
{
    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        CaloHitUsageMap::const_iterator usageMapIter = m_caloHitUsageMap.find(*iter);

        if ((m_caloHitUsageMap.end()) == usageMapIter || !usageMapIter->second)
            return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CaloHitMetadata::SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable)
{
    CaloHitUsageMap::iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

    if (m_caloHitUsageMap.end() == usageMapIter)
        return STATUS_CODE_NOT_FOUND;

    usageMapIter->second = isAvailable;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::CaloHitMetadata::SetCaloHitAvailability(CaloHitList &caloHitList, bool isAvailable)
{
    for (CaloHitList::iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        CaloHitUsageMap::iterator usageMapIter = m_caloHitUsageMap.find(*iter);

        if (m_caloHitUsageMap.end() == usageMapIter)
            return STATUS_CODE_NOT_FOUND;

        usageMapIter->second = isAvailable;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode CaloHitManager::CreateCaloHit<PandoraApi::RectangularCaloHitParameters>(const PandoraApi::RectangularCaloHitParameters &parameters);
template StatusCode CaloHitManager::CreateCaloHit<PandoraApi::PointingCaloHitParameters>(const PandoraApi::PointingCaloHitParameters &parameters);

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::ReclusterMetadata::ReclusterMetadata(CaloHitList *pCaloHitList) :
    m_pCurrentCaloHitMetadata(NULL),
    m_caloHitList(*pCaloHitList)
{
    if (m_caloHitList.empty())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::ReclusterMetadata::~ReclusterMetadata()
{
    for (NameToMetadataMap::iterator iter = m_nameToMetadataMap.begin(), iterEnd = m_nameToMetadataMap.end(); iter != iterEnd; ++iter)
        delete iter->second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ReclusterMetadata::CreateCaloHitMetadata(CaloHitList *pCaloHitList, const std::string &caloHitListName,
    const std::string &reclusterListName, const bool initialHitAvailability)
{
    m_pCurrentCaloHitMetadata = new CaloHitMetadata(pCaloHitList, caloHitListName, initialHitAvailability);

    if (!m_nameToMetadataMap.insert(NameToMetadataMap::value_type(reclusterListName, m_pCurrentCaloHitMetadata)).second)
    {
        delete m_pCurrentCaloHitMetadata;
        return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHitManager::ReclusterMetadata::ExtractCaloHitMetadata(const std::string &reclusterListName, CaloHitMetadata *&pCaloHitMetaData)
{
    NameToMetadataMap::iterator iter = m_nameToMetadataMap.find(reclusterListName);

    if (m_nameToMetadataMap.end() == iter)
        return STATUS_CODE_FAILURE;

    pCaloHitMetaData = iter->second;
    m_nameToMetadataMap.erase(iter);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const CaloHitList &CaloHitManager::ReclusterMetadata::GetCaloHitList() const
{
    return m_caloHitList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHitManager::CaloHitMetadata *CaloHitManager::ReclusterMetadata::GetCurrentCaloHitMetadata()
{
    return m_pCurrentCaloHitMetadata;
}

} // namespace pandora
