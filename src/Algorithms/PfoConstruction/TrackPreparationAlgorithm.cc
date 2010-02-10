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
    // Run track-cluster association daughter algorithms
    for (StringVector::const_iterator iter = m_associationAlgorithms.begin(), iterEnd = m_associationAlgorithms.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackPreparationAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "trackClusterAssociationAlgorithms",
        m_associationAlgorithms));

    return STATUS_CODE_SUCCESS;
}
