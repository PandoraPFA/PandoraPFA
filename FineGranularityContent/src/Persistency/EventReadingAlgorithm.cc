/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Persistency/EventReadingAlgorithm.cc
 * 
 *  @brief  Implementation of the event reading algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Persistency/EventReadingAlgorithm.h"

#include "Utilities/FileReader.h"

using namespace pandora;

EventReadingAlgorithm::EventReadingAlgorithm() :
    m_pFileReader(NULL)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

EventReadingAlgorithm::~EventReadingAlgorithm()
{
    delete m_pFileReader;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventReadingAlgorithm::Initialize()
{
    m_pFileReader = new FileReader(*this, m_fileName);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pFileReader->GoToEvent(m_skipToEvent));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventReadingAlgorithm::Run()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pFileReader->ReadEvent());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RepeatEventPreparation(*this));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventReadingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ReadValue(xmlHandle,
        "FileName", m_fileName));

    m_skipToEvent = 0;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "SkipToEvent", m_skipToEvent));

    return STATUS_CODE_SUCCESS;
}
