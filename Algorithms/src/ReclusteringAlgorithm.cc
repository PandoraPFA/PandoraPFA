/**
 *  @file   PandoraPFANew/Algorithms/src/ReclusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the reclustering algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ReclusteringAlgorithm.h"

using namespace pandora;

StatusCode ReclusteringAlgorithm::Run()
{
    // Run reclustering daughter algorithms
    for (StringVector::const_iterator iter = m_reclusteringAlgorithms.begin(), iterEnd = m_reclusteringAlgorithms.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ReclusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "reclusteringAlgorithms",
        m_reclusteringAlgorithms));

    return STATUS_CODE_SUCCESS;
}
