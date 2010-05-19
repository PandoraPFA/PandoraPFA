/**
 *  @file   PandoraPFANew/include/Managers/PluginManager.h
 * 
 *  @brief  Header file for the pandora plugin manager class.
 * 
 *  $Log: $
 */
#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"

#include "StatusCodes.h"

class TiXmlHandle;

namespace pandora
{

/**
 *  @brief PluginManager class
 */
class PluginManager
{
public:
    /**
     *  @brief  Default constructor
     */
    PluginManager();

    /**
     *  @brief  Destructor
     */
    ~PluginManager();

private:
    /**
     *  @brief  Register an energy correction function
     * 
     *  @param  functionName the name/label associated with the energy correction function
     *  @param  energyCorrectionFunction pointer to an energy correction function
     */
    StatusCode RegisterEnergyCorrectionFunction(const std::string &functionName, EnergyCorrectionFunction *pEnergyCorrectionFunction);

    /**
     *  @brief  Register a particle id function
     * 
     *  @param  functionName the name/label associated with the particle id function
     *  @param  particleIdFunction pointer to a particle id function
     */
    StatusCode RegisterParticleIdFunction(const std::string &functionName, ParticleIdFunction *pParticleIdFunction);

    /**
     *  @brief  Match a vector of names/labels to energy correction functions and store pointers to these functions in a vector
     * 
     *  @param  functionNames the vector of names/labels associated with energy correction functions
     *  @param  energyCorrectionFunctionVector to receive the addresses of the energy correction functions
     */
    StatusCode AssignEnergyCorrectionFunctions(const StringVector &functionNames, EnergyCorrectionFunctionVector &energyCorrectionFunctionVector) const;

    /**
     *  @brief  Match a name/label to a particle id function and assign address of the function to a function pointer
     * 
     *  @param  functionName the name/label associated with the particle id function
     *  @param  pParticleIdFunction to receive the address of the particle id function
     */
    StatusCode AssignParticleIdFunction(const std::string &functionName, ParticleIdFunction *&pParticleIdFunction) const;

    /**
     *  @brief  Initialize plugins
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializePlugins(const TiXmlHandle *const pXmlHandle);

    typedef std::map<std::string, EnergyCorrectionFunction *> EnergyCorrectionFunctionMap;
    typedef std::map<std::string, ParticleIdFunction *> ParticleIdFunctionMap;

    EnergyCorrectionFunctionMap     m_energyCorrectionFunctionMap;      ///< The energy correction function map
    ParticleIdFunctionMap           m_particleIdFunctionMap;            ///< The particle id function map

    friend class PandoraApiImpl;
    friend class PandoraImpl;

    ADD_TEST_CLASS_FRIENDS;
};

} // namespace pandora

#endif // #ifndef PLUGIN_MANAGER_H
