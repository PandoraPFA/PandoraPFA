/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Persistency/EventWritingAlgorithm.cc
 * 
 *  @brief  Implementation of the event writing algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Persistency/EventWritingAlgorithm.h"

#include "Utilities/FileWriter.h"

using namespace pandora;

EventWritingAlgorithm::EventWritingAlgorithm() :
    m_pFileWriter(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

EventWritingAlgorithm::~EventWritingAlgorithm()
{
    delete m_pFileWriter;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventWritingAlgorithm::Initialize()
{
    const FileMode fileMode(m_shouldOverwrite ? OVERWRITE : APPEND);
    m_pFileWriter = new FileWriter(*(this->GetPandora()), m_fileName, fileMode);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventWritingAlgorithm::Run()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pFileWriter->WriteEvent());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventWritingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "FileName", m_fileName));

    m_shouldOverwrite = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldOverwrite", m_shouldOverwrite));

    return STATUS_CODE_SUCCESS;
}
