/**
 *    @file PandoraPFANew/Framework/src/Objects/CaloHit.cc
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

CaloHit::CaloHit(const PandoraApi::CaloHitBaseParameters &caloHitParameters) :
    m_positionVector(caloHitParameters.m_positionVector.Get()),
    m_expectedDirection(caloHitParameters.m_expectedDirection.Get().GetUnitVector()),
    m_cellNormalVector(caloHitParameters.m_cellNormalVector.Get().GetUnitVector()),
    m_cellThickness(caloHitParameters.m_cellThickness.Get()),
    m_nCellRadiationLengths(caloHitParameters.m_nCellRadiationLengths.Get()),
    m_nCellInteractionLengths(caloHitParameters.m_nCellInteractionLengths.Get()),
    m_time(caloHitParameters.m_time.Get()),
    m_inputEnergy(caloHitParameters.m_inputEnergy.Get()),
    m_mipEquivalentEnergy(caloHitParameters.m_mipEquivalentEnergy.Get()),
    m_electromagneticEnergy(caloHitParameters.m_electromagneticEnergy.Get()),
    m_hadronicEnergy(caloHitParameters.m_hadronicEnergy.Get()),
    m_isDigital(caloHitParameters.m_isDigital.Get()),
    m_hitType(caloHitParameters.m_hitType.Get()),
    m_detectorRegion(caloHitParameters.m_detectorRegion.Get()),
    m_layer(caloHitParameters.m_layer.Get()),
    m_isInOuterSamplingLayer(caloHitParameters.m_isInOuterSamplingLayer.Get()),
    m_isPossibleMip(false),
    m_isIsolated(false),
    m_isAvailable(true),
    m_caloCellType(UNKNOWN_CELL_TYPE),
    m_pMCParticle(NULL),
    m_pParentAddress(caloHitParameters.m_pParentAddress.Get())
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

CaloHit::~CaloHit()
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

StatusCode CaloHit::SetSurroundingEnergy(float surroundingEnergy)
{
    if (!(m_surroundingEnergy = surroundingEnergy))
        return STATUS_CODE_NOT_INITIALIZED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::SetIsolatedFlag(bool isolatedFlag)
{
    m_isIsolated = isolatedFlag;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void CaloHit::SetPossibleMipFlag(bool possibleMipFlag)
{
    m_isPossibleMip = possibleMipFlag;
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

RectangularCaloHit::RectangularCaloHit(const PandoraApi::RectangularCaloHitParameters &parameters) :
    CaloHit(parameters),
    m_cellSizeU(parameters.m_cellSizeU.Get()),
    m_cellSizeV(parameters.m_cellSizeV.Get()),
    m_cellLengthScale(std::sqrt(m_cellSizeU * m_cellSizeV))
{
     m_caloCellType = RECTANGULAR;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void RectangularCaloHit::GetCellCorners(CartesianPointList &cartesianPointList) const
{
    const CartesianVector &position(this->GetPositionVector());

    CartesianVector normal(this->GetCellNormalVector());
    CartesianVector dirU((ENDCAP == this->GetDetectorRegion()) ? CartesianVector(0.f, 1.f, 0.f) : CartesianVector(0.f, 0.f, 1.f));
    CartesianVector dirV(normal.GetCrossProduct(dirU));

    dirU *= (this->GetCellSizeU() / 2.);
    dirV *= (this->GetCellSizeV() / 2.);
    normal *= (this->GetCellThickness() / 2.);

    cartesianPointList.push_back(CartesianVector(position - dirU - dirV - normal));
    cartesianPointList.push_back(CartesianVector(position + dirU - dirV - normal));
    cartesianPointList.push_back(CartesianVector(position + dirU + dirV - normal));
    cartesianPointList.push_back(CartesianVector(position - dirU + dirV - normal));
    cartesianPointList.push_back(CartesianVector(position - dirU - dirV + normal));
    cartesianPointList.push_back(CartesianVector(position + dirU - dirV + normal));
    cartesianPointList.push_back(CartesianVector(position + dirU + dirV + normal));
    cartesianPointList.push_back(CartesianVector(position - dirU + dirV + normal));
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

PointingCaloHit::PointingCaloHit(const PandoraApi::PointingCaloHitParameters &parameters) :
    CaloHit(parameters),
    m_cellSizeEta(parameters.m_cellSizeEta.Get()),
    m_cellSizePhi(parameters.m_cellSizePhi.Get()),
    m_cellLengthScale(4.f) // TODO
{
    m_caloCellType = POINTING;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void PointingCaloHit::GetCellCorners(CartesianPointList &cartesianPointList) const
{
    const CartesianVector &position(this->GetPositionVector());

    float radius(0.f), phi(0.f), theta(0.f);
    position.GetSphericalCoordinates(radius, phi, theta);

    const float centralEta(-1. * std::log(std::tan(theta / 2.)));
    const float etaMin(centralEta - this->GetCellSizeEta() / 2.);
    const float etaMax(centralEta + this->GetCellSizeEta() / 2.);

    const float thetaMin(2. * std::atan(std::exp(-1. * etaMin)));
    const float thetaMax(2. * std::atan(std::exp(-1. * etaMax)));
    const float rMin(radius - this->GetCellThickness() / 2.);
    const float rMax(radius + this->GetCellThickness() / 2.);
    const float phiMin(phi - this->GetCellSizePhi() / 2.);
    const float phiMax(phi + this->GetCellSizePhi() / 2.);

    const float sinThetaMin(std::sin(thetaMin)), cosThetaMin(std::cos(thetaMin)), sinPhiMin(std::sin(phiMin)), cosPhiMin(std::cos(phiMin));
    const float sinThetaMax(std::sin(thetaMax)), cosThetaMax(std::cos(thetaMax)), sinPhiMax(std::sin(phiMax)), cosPhiMax(std::cos(phiMax));

    cartesianPointList.push_back(CartesianVector(rMin * sinThetaMin * cosPhiMin, rMin * sinThetaMin * sinPhiMin, rMin * cosThetaMin));
    cartesianPointList.push_back(CartesianVector(rMin * sinThetaMax * cosPhiMin, rMin * sinThetaMax * sinPhiMin, rMin * cosThetaMax));
    cartesianPointList.push_back(CartesianVector(rMin * sinThetaMax * cosPhiMax, rMin * sinThetaMax * sinPhiMax, rMin * cosThetaMax));
    cartesianPointList.push_back(CartesianVector(rMin * sinThetaMin * cosPhiMax, rMin * sinThetaMin * sinPhiMax, rMin * cosThetaMin));
    cartesianPointList.push_back(CartesianVector(rMax * sinThetaMin * cosPhiMin, rMax * sinThetaMin * sinPhiMin, rMax * cosThetaMin));
    cartesianPointList.push_back(CartesianVector(rMax * sinThetaMax * cosPhiMin, rMax * sinThetaMax * sinPhiMin, rMax * cosThetaMax));
    cartesianPointList.push_back(CartesianVector(rMax * sinThetaMax * cosPhiMax, rMax * sinThetaMax * sinPhiMax, rMax * cosThetaMax));
    cartesianPointList.push_back(CartesianVector(rMax * sinThetaMin * cosPhiMax, rMax * sinThetaMin * sinPhiMax, rMax * cosThetaMin));
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

} // namespace pandora
