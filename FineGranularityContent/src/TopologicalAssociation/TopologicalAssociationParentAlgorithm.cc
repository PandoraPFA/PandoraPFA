/**
 *  @file   PandoraPFANew/FineGranularityContent/src/TopologicalAssociation/TopologicalAssociationParentAlgorithm.cc
 * 
 *  @brief  Implementation of the topological association parent algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "TopologicalAssociation/TopologicalAssociationParentAlgorithm.h"

using namespace pandora;

StatusCode TopologicalAssociationParentAlgorithm::Run()
{
    // Run topological daughter association algorithms
    for (StringVector::const_iterator iter = m_associationAlgorithms.begin(), iterEnd = m_associationAlgorithms.end(); iter != iterEnd; ++iter)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TopologicalAssociationParentAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, XmlHelper::ProcessAlgorithmList(*this, xmlHandle, "associationAlgorithms",
        m_associationAlgorithms));

    return STATUS_CODE_SUCCESS;
}
