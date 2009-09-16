/**
 *    @file PandoraPFANew/src/Objects/CaloHit.cc
 * 
 *    @brief Implementation of the calo hit class.
 * 
 *  $Log: $
 */

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

namespace pandora
{

bool CaloHit::operator< (const CaloHit &rhs) const
{
    return m_energy > rhs.m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::CaloHit(const PandoraApi::CaloHitParameters &caloHitParameters) :
    m_positionVector(caloHitParameters.m_positionVector.Get()),
    m_normalVector(caloHitParameters.m_normalVector.Get()),
    m_cellSizeU(caloHitParameters.m_cellSizeU.Get()),
    m_cellSizeV(caloHitParameters.m_cellSizeV.Get()),
    m_cellSizeZ(caloHitParameters.m_cellSizeZ.Get()),
    m_nRadiationLengths(caloHitParameters.m_nRadiationLengths.Get()),
    m_nInteractionLengths(caloHitParameters.m_nInteractionLengths.Get()),
    m_energy(caloHitParameters.m_energy.Get()),
    m_time(caloHitParameters.m_time.Get()),
    m_isDigital(caloHitParameters.m_isDigital.Get()),
    m_hitType(caloHitParameters.m_hitType.Get()),
    m_detectorRegion(caloHitParameters.m_detectorRegion.Get()),
    m_layer(caloHitParameters.m_layer.Get()),
    m_isSortedIntoPseudoLayer(false),
    m_pMCParticle(NULL),
    m_pParentAddress(caloHitParameters.m_pParentAddress.Get())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::~CaloHit()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetMCParticle(MCParticle *const pMCParticle)
{
    if (NULL == pMCParticle)
        return STATUS_CODE_FAILURE;

    m_pMCParticle = pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &stream, const CaloHit &caloHit)
{
    stream  << " CaloHit: " << std::endl
            << " position " << caloHit.GetPositionVector()
            << " energy   " << caloHit.GetEnergy() << std::endl;

    return stream;
}

} // namespace pandora
