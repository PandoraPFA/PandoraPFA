/**
 *  @file   PandoraPFANew/FineGranularityContent/src/FragmentRemovalAlgorithm.cc
 * 
 *  @brief  Implementation of the fragment removal algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "FragmentRemovalAlgorithm.h"

using namespace pandora;

StatusCode FragmentRemovalAlgorithm::Run()
{
    // Run fragment removal daughter algorithms
    for (StringVector::const_iterator iter = m_fragmentRemovalAlgorithms.begin(), iterEnd = m_fragmentRemovalAlgorithms.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode FragmentRemovalAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "fragmentRemovalAlgorithms",
        m_fragmentRemovalAlgorithms));

    return STATUS_CODE_SUCCESS;
}
