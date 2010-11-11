/**
 *  @file   PandoraPFANew/src/Algorithms/TemplateAlgorithm.cc
 * 
 *  @brief  Implementation of the template algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TemplateAlgorithm.h"

#include "Pandora/AlgorithmHeaders.h"

using namespace pandora;

StatusCode TemplateAlgorithm::Run()
{
    // Algorithm code here

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TemplateAlgorithm::ReadSettings(const TiXmlHandle /*xmlHandle*/)
{
    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}
