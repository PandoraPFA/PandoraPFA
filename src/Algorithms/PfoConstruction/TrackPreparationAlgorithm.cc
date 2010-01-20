/**
 *  @file   PandoraPFANew/src/Algorithms/PfoConstruction/TrackPreparationAlgorithm.cc
 * 
 *  @brief  Implementation of the track preparation algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/PfoConstruction/TrackPreparationAlgorithm.h"

using namespace pandora;

StatusCode TrackPreparationAlgorithm::Run()
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_trackClusterAssociationAlgName));

    return STATUS_CODE_SUCCESS;
}
