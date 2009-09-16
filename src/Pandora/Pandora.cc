/**
 *  @file   PandoraPFANew/src/Pandora/Pandora.cc
 * 
 *  @brief  Implementation of the pandora class.
 * 
 *  $Log: $
 */

#include "Api/PandoraApi.h"
#include "Api/PandoraApiImpl.h"
#include "Api/PandoraContentApi.h"
#include "Api/PandoraContentApiImpl.h"

#include "Helpers/GeometryHelper.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/TrackManager.h"

#include "Pandora/Pandora.h"
#include "Pandora/PandoraImpl.h"
#include "Pandora/PandoraSettings.h"

#include "Xml/tinyxml.h"

namespace pandora
{

Pandora::Pandora() :
    m_pAlgorithmManager(new AlgorithmManager(this)),
    m_pCaloHitManager(new CaloHitManager),
    m_pClusterManager(new ClusterManager),
    m_pGeometryHelper(new GeometryHelper),
    m_pMCManager(new MCManager),
    m_pParticleFlowObjectManager(new ParticleFlowObjectManager),
    m_pTrackManager(new TrackManager),
    m_pPandoraSettings(new PandoraSettings),
    m_pPandoraApiImpl(new PandoraApiImpl(this)),
    m_pPandoraContentApiImpl(new PandoraContentApiImpl(this)),
    m_pPandoraImpl(new PandoraImpl(this))
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

Pandora::~Pandora()
{
    delete m_pAlgorithmManager;
    delete m_pCaloHitManager;
    delete m_pClusterManager;
    delete m_pGeometryHelper;
    delete m_pMCManager;
    delete m_pParticleFlowObjectManager;
    delete m_pTrackManager;
    delete m_pPandoraSettings;
    delete m_pPandoraApiImpl;
    delete m_pPandoraContentApiImpl;
    delete m_pPandoraImpl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::ProcessEvent()
{
    std::cout << "Pandora process event" << std::endl;

    // Prepare event
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->MatchObjectsToMCPfoTargets());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->AssociateTracks());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->OrderInputCaloHits());

    // Loop over algorithms
    const StringVector *const pPandoraAlgorithms = m_pAlgorithmManager->GetPandoraAlgorithms();

    for (StringVector::const_iterator iter = pPandoraAlgorithms->begin(), iterEnd = pPandoraAlgorithms->end(); iter != iterEnd; ++iter)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->RunAlgorithm(*iter));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode Pandora::ReadSettings(const std::string &xmlFileName)
{
    try
    {
        TiXmlDocument xmlDocument(xmlFileName);

        if (!xmlDocument.LoadFile())
        {
            std::cout << "Pandora::ReadSettings - Invalid xml file." << std::endl;
            throw StatusCodeException(STATUS_CODE_FAILURE);
        }

        const TiXmlHandle xmlDocumentHandle(&xmlDocument);
        const TiXmlHandle xmlHandle = TiXmlHandle(xmlDocumentHandle.FirstChildElement().Element());

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->InitializeAlgorithms(&xmlHandle));
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, m_pPandoraImpl->ReadPandoraSettings(&xmlHandle));
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failure in reading pandora settings, " << statusCodeException.ToString() << std::endl;
        return STATUS_CODE_FAILURE;
    }
    catch (...)
    {
        std::cout << "Failure in reading pandora settings, unrecognized exception" << std::endl;
        return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraApiImpl *const Pandora::GetPandoraApiImpl() const
{
    return m_pPandoraApiImpl;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraContentApiImpl *const Pandora::GetPandoraContentApiImpl() const
{
    return m_pPandoraContentApiImpl;
}

} // namespace pandora