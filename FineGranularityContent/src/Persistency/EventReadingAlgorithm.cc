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

StatusCode EventReadingAlgorithm::Run()
{
    FileReader fileReader(*this, "testfile.pndr");
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, fileReader.ReadEvent());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EventReadingAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}
