/**
 *  @file   PandoraPFANew/include/PdgTable.h
 * 
 *  @brief  Header file defining masses, widths, and mc id numbers from 2002 edition of rpp
 * 
 *  $Log: $
 */
#ifndef PDG_TABLE_H
#define PDG_TABLE_H 1

namespace pandora
{

// Specify (name, pdg code, mass in GeV, width in GeV, charge)
#define PARTICLE_DATA_TABLE(d)                                                          \
    d(PHOTON,               22,             0.E+00,             0.E+00,         0)      \
    d(E_MINUS,              11,     5.10998902E-04,             0.E+00,        -1)      \
    d(E_PLUS,              -11,     5.10998902E-04,             0.E+00,         1)      \
    d(MU_MINUS,             13,     1.05658357E-01,        2.99591E-19,        -1)      \
    d(MU_PLUS,             -13,     1.05658357E-01,        2.99591E-19,         1)      \
    d(TAU_MINUS,            15,        1.77699E+00,          2.265E-12,        -1)      \
    d(TAU_PLUS,            -15,        1.77699E+00,          2.265E-12,         1)      \
    d(NU_E,                 12,             0.E+00,             0.E+00,         0)      \
    d(NU_E_BAR,            -12,             0.E+00,             0.E+00,         0)      \
    d(NU_MU,                14,             0.E+00,             0.E+00,         0)      \
    d(NU_MU_BAR,           -14,             0.E+00,             0.E+00,         0)      \
    d(NU_TAU,               16,             0.E+00,             0.E+00,         0)      \
    d(NU_TAU_BAR,          -16,             0.E+00,             0.E+00,         0)      \
    d(PI_PLUS,             211,      1.3957018E-01,         2.5284E-17,         1)      \
    d(PI_MINUS,           -211,      1.3957018E-01,         2.5284E-17,        -1)      \
    d(PI_ZERO,             111,       1.349766E-01,            7.8E-09,         0)      \
    d(PI_ZERO_BAR,        -111,       1.349766E-01,            7.8E-09,         0)      \
    d(LAMBDA,             3122,       1.115683E+00,          2.501E-15,         0)      \
    d(LAMBDA_BAR,        -3122,       1.115683E+00,          2.501E-15,         0)      \
    d(K_PLUS,              321,        4.93677E-01,          5.315E-17,         1)      \
    d(K_MINUS,            -321,        4.93677E-01,          5.315E-17,        -1)      \
    d(K_SHORT,             310,        4.97672E-01,          7.367E-15,         0)      \
    d(K_SHORT_BAR,        -310,        4.97672E-01,          7.367E-15,         0)      \
    d(K_LONG,              130,        4.97672E-01,          1.272E-17,         0)      \
    d(K_LONG_BAR,         -130,        4.97672E-01,          1.272E-17,         0)      \
    d(PROTON,             2212,      9.3827200E-01,             0.E+00,         1)      \
    d(NEUTRON,            2112,      9.3956533E-01,          7.432E-28,         0)

/**
 *  @brief  The particle type enum macro
 */
#define GET_PARTICLE_TYPE_ENTRY(a, b, c, d, e)                                          \
    a = b,

/**
 *  @brief  The name switch statement macro
 */
#define GET_PARTICLE_NAME_SWITCH(a, b, c, d, e)                                         \
    case a : return std::string(#a);

/**
 *  @brief  The pdg code switch statement macro
 */
#define GET_PARTICLE_PDG_CODE_SWITCH(a, b, c, d, e)                                     \
    case a : return b;

/**
 *  @brief  The mass switch statement macro
 */
#define GET_PARTICLE_MASS_SWITCH(a, b, c, d, e)                                         \
    case a : return c;

/**
 *  @brief  The width switch statement macro
 */
#define GET_PARTICLE_WIDTH_SWITCH(a, b, c, d, e)                                        \
    case a : return d;

/**
 *  @brief  The charge switch statement macro
 */
#define GET_PARTICLE_CHARGE_SWITCH(a, b, c, d, e)                                       \
    case a : return e;

/**
 *  @brief  The particle type enum
 */
enum ParticleType
{
    PARTICLE_DATA_TABLE(GET_PARTICLE_TYPE_ENTRY)
    NUMBER_OF_PARTICLE_TYPES
};

/**
 *  @brief  Get the name of a particle type as a string
 * 
 *  @param  particleType the particle type
 * 
 *  @return the name
 */
static std::string GetParticleName(ParticleType particleType);

/**
 *  @brief  Get the pdg code of a particle type
 * 
 *  @param  particleType the particle type
 * 
 *  @return the name
 */
static int GetParticlePdgCode(ParticleType particleType);

/**
 *  @brief  Get the mass of a particle type
 * 
 *  @param  particleType the particle type
 * 
 *  @return the mass
 */
static float GetParticleMass(ParticleType particleType);

/**
 *  @brief  Get the width of a particle type
 * 
 *  @param  particleType the particle type
 * 
 *  @return the width
 */
static float GetParticleWidth(ParticleType particleType);

/**
 *  @brief  Get the charge of a particle type
 * 
 *  @param  particleType the particle type
 * 
 *  @return the charge
 */
static int GetParticleCharge(ParticleType particleType);

//------------------------------------------------------------------------------------------------------------------------------------------

inline std::string GetParticleName(ParticleType particleType)
{
    switch (particleType)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_NAME_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int GetParticlePdgCode(ParticleType particleType)
{
    switch (particleType)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_PDG_CODE_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GetParticleMass(ParticleType particleType)
{
    switch (particleType)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_MASS_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float GetParticleWidth(ParticleType particleType)
{
    switch (particleType)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_WIDTH_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline int GetParticleCharge(ParticleType particleType)
{
    switch (particleType)
    {
        PARTICLE_DATA_TABLE(GET_PARTICLE_CHARGE_SWITCH)
        default : throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

} // namespace pandora

#endif // #ifndef PDG_TABLE_H
