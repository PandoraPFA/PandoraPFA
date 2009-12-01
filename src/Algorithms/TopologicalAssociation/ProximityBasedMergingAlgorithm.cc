/**
 *  @file   PandoraPFANew/src/Algorithms/TopologicalAssociation/ProximityBasedMergingAlgorithm.cc
 * 
 *  @brief  Implementation of the proximity based merging algorithm class.
 * 
 *  $Log: $
 */

#include "Algorithms/TopologicalAssociation/ProximityBasedMergingAlgorithm.h"

using namespace pandora;

StatusCode ProximityBasedMergingAlgorithm::Run()
{
    // Begin by recalculating track-cluster associations
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, m_trackClusterAssociationAlgName));

    // Algorithm code here

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ProximityBasedMergingAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessFirstAlgorithm(*this, xmlHandle, m_trackClusterAssociationAlgName));

    // Read settings from xml file here

    return STATUS_CODE_SUCCESS;
}
