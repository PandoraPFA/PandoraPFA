/**
 *  @file   PandoraPFANew/include/Objects/CaloHit.h
 * 
 *  @brief  Header file for the calo hit class.
 * 
 *  $Log: $
 */
#ifndef CALO_HIT_H
#define CALO_HIT_H 1

#include "Api/PandoraApi.h"

#include "Typedefs.h"

namespace pandora
{

class MCParticle;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  CaloHit class
 */
class CaloHit
{
public:
    /**
     *  @brief  Calorimeter hit type enum
     */
    enum HitType
    {
        ECAL,
        HCAL,
        MUON
    };

    /**
     *  @brief  DetectorRegion enum
     */
    enum DetectorRegion
    {
        BARREL,
        ENDCAP
    };

    /**
     *  @brief  Operator< now orders by calo hit energy
     * 
     *  @param  rhs calo hit to compare with
     */
    bool operator< (const CaloHit &rhs) const;

    /**
     *  @brief  Get the calo hit energy
     * 
     *  @return the calo hit energy
     */
    float GetEnergy() const;

    /**
     *  @brief  Get pseudo layer for the calo hit
     * 
     *  @param  pseudoLayer to receive the pseudo layer
     */
    StatusCode GetPseudoLayer(PseudoLayer &pseudoLayer) const;
    
    /**
     *  @brief  Get address of the mc particle associated with the calo hit
     * 
     *  @param  pMCParticle to receive the address of the mc particle
     */
    StatusCode GetMCParticle(MCParticle *pMCParticle) const;

    /**
     *  @brief  Get the address of the parent calo hit in the user framework
     */
     const void *GetParentCaloHitAddress() const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  parameters the calo hit parameters
     */
    CaloHit(const PandoraApi::CaloHitParameters &caloHitParameters);

    /**
     *  @brief  Destructor
     */
    ~CaloHit();

    /**
     *  @brief  Set the mc particle associated with the calo hit
     * 
     *  @param  pMCParticle address of the mc particle
     */
    StatusCode SetMCParticle(MCParticle *const pMCParticle);

    /**
     *  @brief  Set the mc pseudo layer for the calo hit
     * 
     *  @param  pseudoLayer the pseudo layer
     */
    StatusCode SetPseudoLayer(const PseudoLayer &pseudoLayer);

//    const CartesianVector   m_position;                 ///< Position vector of center of calorimeter cell, units mm
//    const CartesianVector   m_normalVector;             ///< Unit normal to the sampling layer, pointing outwards from the origin
//
//    const float             m_du;                       ///< Dimension of readout cell (u = x in ENDCAP, u = r in BARREL), units mm
//    const float             m_dv;                       ///< Dimension of readout cell (v = y in ENDCAP, v = r*phi in BARREL), units mm
//    const float             m_dz;                       ///< Dimension of readout cell, z-coordinate, units mm
//
//    const float             m_nRadiationLengths;        ///< Absorber material in front of this cell in fraction of a radiation length
//    const float             m_nInteractionLengths;      ///< Absorber material in front of this cell in fraction of an interaction length
//
    const float             m_energy;                   ///< Corrected energy of the calorimeter cell, units GeV
//    const float             m_time;                     ///< Time of (earliest) energy deposition in this cell, units ns
//
//    const bool              m_isDigital;                ///< Whether cell should be treated as digital (implies constant cell energy)
//    const HitType           m_hitType;                  ///< The type of calorimeter hit
//    const Region            m_region;                   ///< Region of the detector in which the calo hit is located

    const unsigned int      m_layer;                    ///< The actual subdetector readout layer number
    PseudoLayer             m_pseudoLayer;              ///< The pseudo layer to which the calo hit has been assigned
    bool                    m_isSortedIntoPseudoLayer;  ///< Whether the calo hit has been sorted into a pseudo layer

    MCParticle              *m_pMCParticle;             ///< The associated MC particle
    const void              *m_pParentAddress;          ///< The address of the parent calo hit in the user framework

    friend class CaloHitManager;
};

/**
 *  @brief  Operator to dump calo hit properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  caloHit the calo hit
 */
std::ostream &operator<<(std::ostream &stream, const CaloHit &caloHit);

//------------------------------------------------------------------------------------------------------------------------------------------

inline const void *CaloHit::GetParentCaloHitAddress() const
{
    return m_pParentAddress;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHit::GetPseudoLayer(PseudoLayer &pseudoLayer) const
{
    if (!m_isSortedIntoPseudoLayer)
        return STATUS_CODE_NOT_INITIALIZED;

    pseudoLayer = m_pseudoLayer;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHit::GetMCParticle(MCParticle *pMCParticle) const
{
    if (NULL == m_pMCParticle)
        return STATUS_CODE_NOT_INITIALIZED;

    pMCParticle = m_pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHit::SetMCParticle(MCParticle *const pMCParticle)
{
    if (NULL == pMCParticle)
        return STATUS_CODE_FAILURE;

    m_pMCParticle = pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHit::SetPseudoLayer(const PseudoLayer &pseudoLayer)
{
    m_pseudoLayer = pseudoLayer;
    m_isSortedIntoPseudoLayer = true;

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora

#endif // #ifndef CALO_HIT_H
