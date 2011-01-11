/**
 *  @file   PandoraPFANew/Framework/include/Pandora/PandoraPlugins.h
 * 
 *  @brief  Header file detailing plugin helper functions included in pandora library
 * 
 *  $Log: $
 */
#ifndef PANDORA_PLUGINS_H
#define PANDORA_PLUGINS_H 1

#include "Helpers/ParticleIdHelper.h"

#define PANDORA_PARTICLE_ID_LIST(d)                                                                                         \
    d("PhotonFastDefault",                                  &ParticleIdHelper::IsPhotonFastDefault)                         \
    d("ElectronFastDefault",                                &ParticleIdHelper::IsElectronFastDefault)                       \
    d("MuonFastDefault",                                    &ParticleIdHelper::IsMuonFastDefault)

#define PANDORA_REGISTER_PARTICLE_ID(a, b)                                                                                  \
    {                                                                                                                       \
        StatusCode statusCode = RegisterParticleIdFunction(a, b);                                                           \
                                                                                                                            \
        if (STATUS_CODE_SUCCESS != statusCode)                                                                              \
            throw StatusCodeException(statusCode);                                                                          \
    }

#define PANDORA_REGISTER_ALL_PLUGINS()                                                                                      \
    PANDORA_PARTICLE_ID_LIST(PANDORA_REGISTER_PARTICLE_ID)

#endif // #ifndef PANDORA_PLUGINS_H
