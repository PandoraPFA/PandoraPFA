/**
 *    @file PandoraPFANew/src/Objects/CaloHit.cc
 * 
 *    @brief Implementation of the calo hit class.
 * 
 *  $Log: $
 */

#include "Objects/CaloHit.h"
#include "Objects/MCParticle.h"

#include <cmath>

namespace pandora
{

CaloHit::CaloHit(const PandoraApi::CaloHitParameters &caloHitParameters) :
    m_positionVector(caloHitParameters.m_positionVector.Get()),
    m_normalVector(caloHitParameters.m_normalVector.Get()),
    m_cellSizeU(caloHitParameters.m_cellSizeU.Get()),
    m_cellSizeV(caloHitParameters.m_cellSizeV.Get()),
    m_cellThickness(caloHitParameters.m_cellThickness.Get()),
    m_cellLengthScale(std::sqrt(m_cellSizeU * m_cellSizeV)),
    m_nRadiationLengths(caloHitParameters.m_nRadiationLengths.Get()),
    m_nInteractionLengths(caloHitParameters.m_nInteractionLengths.Get()),
    m_time(caloHitParameters.m_time.Get()),
    m_inputEnergy(caloHitParameters.m_inputEnergy.Get()),
    m_mipEquivalentEnergy(caloHitParameters.m_mipEquivalentEnergy.Get()),
    m_electromagneticEnergy(caloHitParameters.m_electromagneticEnergy.Get()),
    m_hadronicEnergy(caloHitParameters.m_hadronicEnergy.Get()),
    m_isDigital(caloHitParameters.m_isDigital.Get()),
    m_hitType(caloHitParameters.m_hitType.Get()),
    m_detectorRegion(caloHitParameters.m_detectorRegion.Get()),
    m_layer(caloHitParameters.m_layer.Get()),
    m_surroundingEnergy(0),
    m_isPossibleMip(false),
    m_isIsolated(false),
    m_isAvailable(true),
    m_pMCParticle(NULL),
    m_pParentAddress(caloHitParameters.m_pParentAddress.Get())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetPseudoLayer(PseudoLayer pseudoLayer)
{
    if (!(m_pseudoLayer = pseudoLayer))
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode CaloHit::SetDensityWeight(float densityWeight)
{
    if (!(m_densityWeight = densityWeight))
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::AddSurroundingEnergy(float surroundingEnergy)
{
    m_surroundingEnergy += surroundingEnergy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::SetPossibleMipFlag(bool possibleMipFlag)
{
    m_isPossibleMip = possibleMipFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::SetIsolatedFlag(bool isolatedFlag)
{
    m_isIsolated = isolatedFlag;
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
            << " energy   " << caloHit.GetInputEnergy() << std::endl;

    return stream;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode SortByEnergy(const CaloHitList &caloHitList, EnergySortedCaloHitList &energySortedCaloHitList)
{
    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        if (!energySortedCaloHitList.insert(*iter).second)
            return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
