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

StatusCode EventWritingAlgorithm::Run()
{
    FileWriter fileWriter(*this, "testfile.pndr");
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, fileWriter.WriteEvent());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventWritingAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}
