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

#include "Pandora/PandoraInternal.h"

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
     *  @brief  Operator< now orders by calo hit energy
     * 
     *  @param  rhs calo hit to compare with
     */
    bool operator< (const CaloHit &rhs) const;

    /**
     *  @brief  Get the position vector of center of calorimeter cell, units mm
     * 
     *  @return the position vector
     */
    const CartesianVector &GetPositionVector() const;

    /**
     *  @brief  Get the unit vector normal to the sampling layer, pointing outwards from the origin
     * 
     *  @return the normal vector
     */
    const CartesianVector &GetNormalVector() const;

    /**
     *  @brief  Get the u dimension of cell (u = x in ENDCAP, u = r in BARREL), units mm
     * 
     *  @return the u dimension of cell
     */
    float GetCellSizeU() const;

    /**
     *  @brief  Get the v dimension of cell (v = y in ENDCAP, v = r*phi in BARREL), units mm
     * 
     *  @return the v dimension of cell
     */
    float GetCellSizeV() const;

    /**
     *  @brief  Get the z dimension of cell
     * 
     *  @return the z dimension of cell
     */
    float GetCellSizeZ() const;

    /**
     *  @brief  Get the absorber material in front of cell, units radiation lengths
     * 
     *  @return the absorber material in front of cell in radiation lengths
     */
    float GetNRadiationLengths() const;

    /**
     *  @brief  Get the absorber material in front of cell, units interaction lengths
     * 
     *  @return the absorber material in front of cell in interaction lengths
     */
    float GetNInteractionLengths() const;

    /**
     *  @brief  Get the corrected energy of the calorimeter cell, units GeV
     * 
     *  @return the corrected energy of the calorimeter cell
     */
    float GetEnergy() const;

    /**
     *  @brief  Get the time of (earliest) energy deposition in this cell, units ns
     * 
     *  @return the time of (earliest) energy deposition in this cell
     */
    float GetTime() const;

    /**
     *  @brief  Whether cell should be treated as digital
     * 
     *  @return boolean
     */
    bool IsDigital() const;

    /**
     *  @brief  Get the calorimeter hit type
     * 
     *  @return the calorimeter hit type
     */
    HitType GetHitType() const;

    /**
     *  @brief  Get the region of the detector in which the calo hit is located
     * 
     *  @return the detector region
     */
    DetectorRegion GetDetectorRegion() const;

    /**
     *  @brief  Get the subdetector readout layer number
     * 
     *  @return the subdetector readout layer number
     */
    unsigned int GetLayer() const;

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
    StatusCode GetMCParticle(MCParticle *&pMCParticle) const;

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

    const CartesianVector   m_positionVector;           ///< Position vector of center of calorimeter cell, units mm
    const CartesianVector   m_normalVector;             ///< Unit normal to the sampling layer, pointing outwards from the origin

    const float             m_cellSizeU;                ///< Dimension of cell (u = x in ENDCAP, u = r in BARREL), units mm
    const float             m_cellSizeV;                ///< Dimension of cell (v = y in ENDCAP, v = r*phi in BARREL), units mm
    const float             m_cellSizeZ;                ///< Dimension of cell, z-coordinate, units mm

    const float             m_nRadiationLengths;        ///< Absorber material in front of cell, units radiation lengths
    const float             m_nInteractionLengths;      ///< Absorber material in front of cell, units interaction lengths

    const float             m_energy;                   ///< Corrected energy of the calorimeter cell, units GeV
    const float             m_time;                     ///< Time of (earliest) energy deposition in this cell, units ns

    const bool              m_isDigital;                ///< Whether cell should be treated as digital (implies constant cell energy)
    const HitType           m_hitType;                  ///< The type of calorimeter hit
    const DetectorRegion    m_detectorRegion;           ///< Region of the detector in which the calo hit is located

    const unsigned int      m_layer;                    ///< The subdetector readout layer number
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

inline const CartesianVector &CaloHit::GetPositionVector() const
{
    return m_positionVector;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &CaloHit::GetNormalVector() const
{
    return m_normalVector;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetCellSizeU() const
{
    return m_cellSizeU;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetCellSizeV() const
{
    return m_cellSizeV;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetCellSizeZ() const
{
    return m_cellSizeZ;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetNRadiationLengths() const
{
    return m_nRadiationLengths;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetNInteractionLengths() const
{
    return m_nInteractionLengths;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetEnergy() const
{
    return m_energy;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float CaloHit::GetTime() const
{
    return m_time;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHit::IsDigital() const
{
    return m_isDigital;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline HitType CaloHit::GetHitType() const
{
    return m_hitType;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline DetectorRegion CaloHit::GetDetectorRegion() const
{
    return m_detectorRegion;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int CaloHit::GetLayer() const
{
    return m_layer;
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

inline StatusCode CaloHit::GetMCParticle(MCParticle *&pMCParticle) const
{
    if (NULL == m_pMCParticle)
        return STATUS_CODE_NOT_INITIALIZED;

    pMCParticle = m_pMCParticle;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const void *CaloHit::GetParentCaloHitAddress() const
{
    return m_pParentAddress;
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
