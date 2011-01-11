/**
 *  @file   PandoraPFANew/Framework/include/Pandora/PandoraPlugins.h
 * 
 *  @brief  Header file detailing plugin helper functions included in pandora library
 * 
 *  $Log: $
 */
#ifndef PANDORA_PLUGINS_H
#define PANDORA_PLUGINS_H 1

#include "Helpers/EnergyCorrectionsHelper.h"
#include "Helpers/ParticleIdHelper.h"
    
#define PANDORA_ENERGY_CORRECTION_LIST(d)                                                                                   \
    d("CleanClusters",              HADRONIC,               &EnergyCorrectionsHelper::CleanCluster)                         \
    d("ScaleHotHadrons",            HADRONIC,               &EnergyCorrectionsHelper::ScaleHotHadronEnergy)                 \
    d("MuonCoilCorrection",         HADRONIC,               &EnergyCorrectionsHelper::ApplyMuonEnergyCorrection)

#define PANDORA_PARTICLE_ID_LIST(d)                                                                                         \
    d("PhotonFastDefault",                                  &ParticleIdHelper::IsPhotonFastDefault)                         \
    d("ElectronFastDefault",                                &ParticleIdHelper::IsElectronFastDefault)                       \
    d("MuonFastDefault",                                    &ParticleIdHelper::IsMuonFastDefault)

#define PANDORA_REGISTER_ENERGY_CORRECTION(a, b, c)                                                                         \
    {                                                                                                                       \
        StatusCode statusCode = RegisterEnergyCorrectionFunction(a, b, c);                                                  \
                                                                                                                            \
        if (STATUS_CODE_SUCCESS != statusCode)                                                                              \
            throw StatusCodeException(statusCode);                                                                          \
    }

#define PANDORA_REGISTER_PARTICLE_ID(a, b)                                                                                  \
    {                                                                                                                       \
        StatusCode statusCode = RegisterParticleIdFunction(a, b);                                                           \
                                                                                                                            \
        if (STATUS_CODE_SUCCESS != statusCode)                                                                              \
            throw StatusCodeException(statusCode);                                                                          \
    }

#define PANDORA_REGISTER_ALL_PLUGINS()                                                                                      \
    PANDORA_ENERGY_CORRECTION_LIST(PANDORA_REGISTER_ENERGY_CORRECTION)                                                      \
    PANDORA_PARTICLE_ID_LIST(PANDORA_REGISTER_PARTICLE_ID)

#endif // #ifndef PANDORA_PLUGINS_H
