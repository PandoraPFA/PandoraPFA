/**
 *  @file   PandoraPFANew/include/PandoraImpl.h
 * 
 *  @brief  Header file for the pandora impl class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_IMPL_H
#define PANDORA_IMPL_H 1

namespace pandora
{

/**
 *    @brief PandoraImpl class
 */
class PandoraImpl
{
private:
    /**
     *  @brief  Match tracks and calo hits to their correct mc particles for particle flow
     */
    StatusCode MatchObjectsToMCPfoTargets() const;

    /**
     *  @brief  Add track associations (parent-daughter and sibling)
     */
    StatusCode AssociateTracks() const;

    /**
     *  @brief  Order input calo hits by pseudo layer
     */
    StatusCode OrderInputCaloHits() const;

    /**
     *  @brief  Initialize pandora algorithms
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializeAlgorithms(const TiXmlHandle *const pXmlHandle) const;

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode ReadPandoraSettings(const TiXmlHandle *const pXmlHandle) const;

    /**
     *  @brief  Run an algorithm registered with pandora
     * 
     *  @param  algorithmName the name of the algorithm instance to run
     */
    StatusCode RunAlgorithm(const std::string &algorithmName) const;

private:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the pandora object to interface
     */
    PandoraImpl(Pandora *pPandora);

    Pandora    *m_pPandora;    ///< The pandora object to provide an interface to

    friend class Pandora;
};

} // namespace pandora

#endif // #ifndef PANDORA_IMPL_H
