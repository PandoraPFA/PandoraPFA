/**
 *  @file   PandoraPFANew/FineGranularityContent/src/Clustering/LineClusteringAlgorithm.cc
 * 
 *  @brief  Implementation of the line clustering algorithm class.
 * 
 *  $Log: $
 */

#include "Pandora/AlgorithmHeaders.h"

#include "Clustering/LineClusteringAlgorithm.h"

using namespace pandora;

StatusCode LineClusteringAlgorithm::Run()
{
    bool useXY(true), useYZ(true);

    // Start while loop here
    while (true)
    {
        // Make list of available hits
        CaloHitList caloHitList;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetListOfAvailableCaloHits(caloHitList));

        if (caloHitList.size() < 5)
            break;

        // Create histograms
        static const float pi(std::acos(-1.f));
        const float thetaOffset(0.5f * pi / static_cast<float>(m_nLines));
        const float minTheta(-thetaOffset);
        const float maxTheta(thetaOffset + (pi * (static_cast<float>(m_nLines - 1) / static_cast<float>(m_nLines))));

        TwoDHistogram twoDHistogram_XY(m_nLines, minTheta, maxTheta, 750, -6000, 6000);
        TwoDHistogram twoDHistogram_YZ(m_nLines, minTheta, maxTheta, 750, -6000, 6000);
        this->FillHoughSpaceHistograms(caloHitList, twoDHistogram_XY, twoDHistogram_YZ);

        // Identify straight lines corresponding to narrow peak in hough space
        float maximumValue_XY(0.f);
        int maximumThetaBin_XY(-1), maximumRBin_XY(-1);
        twoDHistogram_XY.GetMaximum(maximumValue_XY, maximumThetaBin_XY, maximumRBin_XY);
        const float peakTheta_XY(twoDHistogram_XY.GetXLow() + (twoDHistogram_XY.GetXBinWidth() * static_cast<float>(0.5 + maximumThetaBin_XY)));
        const float peakR_XY(twoDHistogram_XY.GetYLow() + (twoDHistogram_XY.GetYBinWidth() * static_cast<float>(0.5 + maximumRBin_XY)));

        float maximumValue_YZ(0.f);
        int maximumThetaBin_YZ(-1), maximumRBin_YZ(-1);
        twoDHistogram_YZ.GetMaximum(maximumValue_YZ, maximumThetaBin_YZ, maximumRBin_YZ);
        const float peakTheta_YZ(twoDHistogram_YZ.GetXLow() + (twoDHistogram_YZ.GetXBinWidth() * static_cast<float>(0.5 + maximumThetaBin_YZ)));
        const float peakR_YZ(twoDHistogram_YZ.GetYLow() + (twoDHistogram_YZ.GetYBinWidth() * static_cast<float>(0.5 + maximumRBin_YZ)));

        PANDORA_MONITORING_API(DrawPandoraHistogram(twoDHistogram_XY, "colz"));
        PANDORA_MONITORING_API(DrawPandoraHistogram(twoDHistogram_YZ, "colz"));

        // Conditions to terminate clustering
        if ((maximumValue_XY < 5) || (maximumValue_YZ < 5))
        {
            std::cout << "LineClusteringAlgorithm: Too few hits on line, halting clustering." << std::endl;
            break;
        }

        // Use straight lines to create cluster
        Cluster *pCluster = NULL;
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateCluster(caloHitList, useXY, useYZ, peakR_XY, peakTheta_XY,
            peakR_YZ, peakTheta_YZ, pCluster));

        if (NULL != pCluster)
            continue;

        // Conditions to revert to 2d approach
        if (maximumValue_XY > maximumValue_YZ)
        {
            std::cout << "LineClusteringAlgorithm: Large peak discrepancy - reverting to 2d xy approach." << std::endl;
            useXY = true;
            useYZ = false;
        }
        else
        {
            std::cout << "LineClusteringAlgorithm: Large peak discrepancy - reverting to 2d yz approach." << std::endl;
            useXY = false;
            useYZ = true;
        }

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateCluster(caloHitList, useXY, useYZ, peakR_XY, peakTheta_XY,
            peakR_YZ, peakTheta_YZ, pCluster));

        // Terminate if 2d clustering is still unsuccessful
        if (NULL == pCluster)
            break;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LineClusteringAlgorithm::GetListOfAvailableCaloHits(CaloHitList &caloHitList) const
{
    const OrderedCaloHitList *pOrderedCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentOrderedCaloHitList(*this, pOrderedCaloHitList));

    for (OrderedCaloHitList::const_iterator iter = pOrderedCaloHitList->begin(), iterEnd = pOrderedCaloHitList->end(); iter != iterEnd; ++iter)
    {
        for (CaloHitList::const_iterator hitIter = iter->second->begin(), hitIterEnd = iter->second->end(); hitIter != hitIterEnd; ++hitIter)
        {
            if (CaloHitHelper::IsCaloHitAvailable(*hitIter) && (m_shouldUseIsolatedHits || !(*hitIter)->IsIsolated()))
                caloHitList.insert(*hitIter);
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void LineClusteringAlgorithm::FillHoughSpaceHistograms(const CaloHitList &caloHitList, TwoDHistogram &twoDHistogram_XY,
    TwoDHistogram &twoDHistogram_YZ) const
{
    static const float pi(std::acos(-1.f));

    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        CaloHit *pCaloHit = *iter;
        const float xPosition(pCaloHit->GetPositionVector().GetX());
        const float yPosition(pCaloHit->GetPositionVector().GetY());
        const float zPosition(pCaloHit->GetPositionVector().GetZ());

        // For each point, consider a number of straight line possibilities
        for (unsigned int iLine = 0; iLine < m_nLines; ++iLine)
        {
            const TwoDLine twoDLine_XY(xPosition, yPosition, pi * static_cast<float>(iLine) / static_cast<float>(m_nLines));
            const float houghSpaceR_XY(twoDLine_XY.GetHoughSpaceR());
            const float houghSpaceTheta_XY(twoDLine_XY.GetHoughSpaceTheta());
            twoDHistogram_XY.Fill(houghSpaceTheta_XY, houghSpaceR_XY);

            const TwoDLine twoDLine_YZ(yPosition, zPosition, pi * static_cast<float>(iLine) / static_cast<float>(m_nLines));
            const float houghSpaceR_YZ(twoDLine_YZ.GetHoughSpaceR());
            const float houghSpaceTheta_YZ(twoDLine_YZ.GetHoughSpaceTheta());
            twoDHistogram_YZ.Fill(houghSpaceTheta_YZ, houghSpaceR_YZ);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LineClusteringAlgorithm::CreateCluster(const CaloHitList &caloHitList, const bool useXY, const bool useYZ, const float peakR_XY,
    const float peakTheta_XY, const float peakR_YZ, const float peakTheta_YZ, Cluster *&pCluster) const
{
    const float cosPeakTheta_XY(std::cos(peakTheta_XY));
    const float sinPeakTheta_XY(std::sin(peakTheta_XY));

    const float cosPeakTheta_YZ(std::cos(peakTheta_YZ));
    const float sinPeakTheta_YZ(std::sin(peakTheta_YZ));

    for (CaloHitList::const_iterator iter = caloHitList.begin(), iterEnd = caloHitList.end(); iter != iterEnd; ++iter)
    {
        CaloHit *pCaloHit = *iter;

        const float hitR_XY(pCaloHit->GetPositionVector().GetX() * cosPeakTheta_XY + pCaloHit->GetPositionVector().GetY() * sinPeakTheta_XY);
        const float hitR_YZ(pCaloHit->GetPositionVector().GetY() * cosPeakTheta_YZ + pCaloHit->GetPositionVector().GetZ() * sinPeakTheta_YZ);

        if ((!useXY || std::fabs(hitR_XY - peakR_XY) < (2.f * pCaloHit->GetCellLengthScale())) &&
            (!useYZ || std::fabs(hitR_YZ - peakR_YZ) < (2.f * pCaloHit->GetCellLengthScale())) )
        {
            if (NULL == pCluster)
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, pCaloHit, pCluster));
            }
            else
            {
                PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, PandoraContentApi::AddCaloHitToCluster(*this, pCluster, pCaloHit));
            }
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode LineClusteringAlgorithm::ReadSettings(const TiXmlHandle xmlHandle)
{
    m_shouldUseIsolatedHits = false;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "ShouldUseIsolatedHits", m_shouldUseIsolatedHits));

    m_nLines = 1000;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadValue(xmlHandle,
        "NLines", m_nLines));

    if (0 == m_nLines)
        return STATUS_CODE_INVALID_PARAMETER;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

LineClusteringAlgorithm::TwoDLine::TwoDLine(const float xPointOnLine, const float yPointOnLine, const float rotationAngle) :
    m_houghSpaceR((xPointOnLine * std::cos(rotationAngle)) + (yPointOnLine * std::sin(rotationAngle))),
    m_houghSpaceTheta(rotationAngle)
{
}
