/**
 *  @file   PandoraPFANew/src/GeometryHelper.cc
 * 
 *  @brief  Implementation of the geometry helper class.
 * 
 *  $Log: $
 */
 
#include "GeometryHelper.h"

namespace pandora
{

GeometryHelper::GeometryHelper() :
    m_isInitialized(false)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryHelper::Initialize(const PandoraApi::GeometryParameters &geometryParameters)
{
    if (m_isInitialized)
        return STATUS_CODE_ALREADY_INITIALIZED;

    m_tpcInnerRadius = geometryParameters.m_tpcInnerRadius.Get();

    m_isInitialized = true;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
