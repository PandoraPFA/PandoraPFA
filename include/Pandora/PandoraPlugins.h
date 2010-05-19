/**
 *  @file   PandoraPFANew/include/Pandora/PandoraPlugins.h
 * 
 *  @brief  Header file detailing plugin helper functions included in pandora library
 * 
 *  $Log: $
 */
#ifndef PANDORA_PLUGINS_H
#define PANDORA_PLUGINS_H 1

#include "Helpers/EnergyCorrectionsHelper.h"
#include "Helpers/ParticleIdHelper.h"
    
#define PANDORA_PLUGIN_LIST(d)                                                                                              \
    d(EnergyCorrectionFunction,     "CleanClusters",            &EnergyCorrectionsHelper::CleanCluster)                     \
    d(EnergyCorrectionFunction,     "ScaleHotHadrons",          &EnergyCorrectionsHelper::ScaleHotHadronEnergy)             \
    d(EnergyCorrectionFunction,     "MuonCoilCorrection",       &EnergyCorrectionsHelper::ApplyMuonEnergyCorrection)        \
    d(ParticleIdFunction,           "PhotonFastDefault",        &ParticleIdHelper::IsPhotonFastDefault)                     \
    d(ParticleIdFunction,           "ElectronFastDefault",      &ParticleIdHelper::IsElectronFastDefault)

#define PANDORA_REGISTER_PLUGIN(a, b, c)                                                                                    \
    {                                                                                                                       \
        StatusCode statusCode = Register##a(b, c);                                                                          \
                                                                                                                            \
        if (STATUS_CODE_SUCCESS != statusCode)                                                                              \
            throw StatusCodeException(statusCode);                                                                          \
    }

#define PANDORA_REGISTER_ALL_PLUGINS()                                                                                      \
    PANDORA_PLUGIN_LIST(PANDORA_REGISTER_PLUGIN)

#endif // #ifndef PANDORA_PLUGINS_H
