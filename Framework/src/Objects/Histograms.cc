/**
 *  @file   PandoraPFANew/Framework/src/Objects/Histograms.cc
 * 
 *  @brief  Implementation of histogram classes.
 * 
 *  $Log: $
 */

#include "Objects/Histograms.h"

#include "Pandora/StatusCodes.h"

#include <cmath>
#include <limits>

namespace pandora
{

Histogram::Histogram(const unsigned int nBinsX, const float xLow, const float xHigh) :
    m_nBinsX(nBinsX),
    m_xLow(xLow),
    m_xHigh(xHigh)
{
    if ((0 == nBinsX) || (xHigh - xLow < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_xBinWidth = (xHigh - xLow) / static_cast<float>(nBinsX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Histogram::Histogram(const Histogram &rhs) :
    m_histogramMap(rhs.m_histogramMap),
    m_nBinsX(rhs.m_nBinsX),
    m_xLow(rhs.m_xLow),
    m_xHigh(rhs.m_xHigh),
    m_xBinWidth(rhs.m_xBinWidth)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram::GetBinContent(const int binX) const
{
    HistogramMap::const_iterator iter = m_histogramMap.find(binX);

    if (m_histogramMap.end() == iter)
        return 0.f;

    return iter->second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram::GetMeanX() const
{
    float sumEntries(0.f), sumXEntries(0.f);
    const float firstBinCenter(m_xLow + (0.5f * m_xBinWidth));

    for (HistogramMap::const_iterator iter = m_histogramMap.begin(), iterEnd = m_histogramMap.end(); iter != iterEnd; ++iter)
    {
        const int binNumber(iter->first);

        if ((binNumber < 0) || (binNumber > m_nBinsX))
            continue;

        const float binCenter(firstBinCenter + (m_xBinWidth * static_cast<float>(binNumber)));
        const float binContents(iter->second);

        sumEntries += binContents;
        sumXEntries += binContents * binCenter;
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    return (sumXEntries / sumEntries);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram::GetStandardDeviationX() const
{
    float sumEntries(0.f), sumXEntries(0.f), sumXXEntries(0.f);
    const float firstBinCenter(m_xLow + (0.5f * m_xBinWidth));

    for (HistogramMap::const_iterator iter = m_histogramMap.begin(), iterEnd = m_histogramMap.end(); iter != iterEnd; ++iter)
    {
        const int binNumber(iter->first);

        if ((binNumber < 0) || (binNumber > m_nBinsX))
            continue;

        const float binCenter(firstBinCenter + (m_xBinWidth * static_cast<float>(binNumber)));
        const float binContents(iter->second);

        sumEntries += binContents;
        sumXEntries += binContents * binCenter;
        sumXXEntries += binContents * binCenter * binCenter;
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    const float meanX(sumXEntries / sumEntries);
    const float meanXX(sumXXEntries / sumEntries);

    return std::sqrt(meanXX - (meanX * meanX));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float Histogram::GetCumulativeSumX() const
{
    float sumEntries(0.f);

    for (HistogramMap::const_iterator iter = m_histogramMap.begin(), iterEnd = m_histogramMap.end(); iter != iterEnd; ++iter)
    {
        const int binNumber(iter->first);

        if ((binNumber < 0) || (binNumber > m_nBinsX))
            continue;

        sumEntries += iter->second;
    }

    return sumEntries;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram::GetMaximumX(float &maximumValueX, int &maximumBinX) const
{
    maximumBinX = -1;
    maximumValueX = 0.f;

    for (HistogramMap::const_iterator iter = m_histogramMap.begin(), iterEnd = m_histogramMap.end(); iter != iterEnd; ++iter)
    {
        const int binNumber(iter->first);

        if ((binNumber < 0) || (binNumber > m_nBinsX))
            continue;

        const float binContents(iter->second);

        if (binContents > maximumValueX)
        {
            maximumValueX = binContents;
            maximumBinX = binNumber;
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram::SetBinContent(const int binX, const float value)
{
    if ((binX < -1) || (binX > m_nBinsX + 1))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_histogramMap[binX] = value;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Histogram::Fill(const float valueX, const float weight)
{
    const int binX(std::max(-1, std::min(static_cast<int>(m_nBinsX + 1), static_cast<int>((valueX - m_xLow) / m_xBinWidth)) ));

    if (m_histogramMap.count(binX) > 0)
    {
        m_histogramMap[binX] += weight;
    }
    else
    {
        m_histogramMap[binX] = weight;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

TwoDHistogram::TwoDHistogram(const unsigned int nBinsX, const float xLow, const float xHigh, const unsigned int nBinsY, const float yLow,
        const float yHigh) :
    m_nBinsX(nBinsX),
    m_xLow(xLow),
    m_xHigh(xHigh),
    m_nBinsY(nBinsY),
    m_yLow(yLow),
    m_yHigh(yHigh)
{
    if ((0 == nBinsX) || (xHigh - xLow < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_xBinWidth = (xHigh - xLow) / static_cast<float>(nBinsX);

    if ((0 == nBinsY) || (yHigh - yLow < std::numeric_limits<float>::epsilon()))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_yBinWidth = (yHigh - yLow) / static_cast<float>(nBinsY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

TwoDHistogram::TwoDHistogram(const TwoDHistogram &rhs) :
    m_xyHistogramMap(rhs.m_xyHistogramMap),
    m_yxHistogramMap(rhs.m_yxHistogramMap),
    m_nBinsX(rhs.m_nBinsX),
    m_xLow(rhs.m_xLow),
    m_xHigh(rhs.m_xHigh),
    m_xBinWidth(rhs.m_xBinWidth),
    m_nBinsY(rhs.m_nBinsY),
    m_yLow(rhs.m_yLow),
    m_yHigh(rhs.m_yHigh),
    m_yBinWidth(rhs.m_yBinWidth)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetBinContent(const int binX, const int binY) const
{
    TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.find(binX);

    if (m_xyHistogramMap.end() == iterX)
        return 0.f;

    HistogramMap::const_iterator iterXY = iterX->second.find(binY);

    if (iterX->second.end() == iterXY)
        return 0.f;

    return iterXY->second;
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetMeanX(const int yLowBin, const int yHighBin) const
{
    float sumEntries(0.f), sumXEntries(0.f);
    const float firstBinXCenter(m_xLow + (0.5f * m_xBinWidth));

    for (int yBin = yLowBin; yBin <= yHighBin; ++yBin)
    {
        TwoDHistogramMap::const_iterator iterY = m_yxHistogramMap.find(yBin);

        if (m_yxHistogramMap.end() == iterY)
            continue;

        for (HistogramMap::const_iterator iterX = iterY->second.begin(), iterXEnd = iterY->second.end(); iterX != iterXEnd; ++iterX)
        {
            const int binXNumber(iterX->first);

            if ((binXNumber < 0) || (binXNumber > m_nBinsX))
                continue;

            const float binXCenter(firstBinXCenter + (m_xBinWidth * static_cast<float>(binXNumber)));
            const float binContents(iterX->second);

            sumEntries += binContents;
            sumXEntries += binContents * binXCenter;
        }
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    return (sumXEntries / sumEntries);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetStandardDeviationX(const int yLowBin, const int yHighBin) const
{
    float sumEntries(0.f), sumXEntries(0.f), sumXXEntries(0.f);
    const float firstBinXCenter(m_xLow + (0.5f * m_xBinWidth));

    for (int yBin = yLowBin; yBin <= yHighBin; ++yBin)
    {
        TwoDHistogramMap::const_iterator iterY = m_yxHistogramMap.find(yBin);

        if (m_yxHistogramMap.end() == iterY)
            continue;

        for (HistogramMap::const_iterator iterX = iterY->second.begin(), iterXEnd = iterY->second.end(); iterX != iterXEnd; ++iterX)
        {
            const int binXNumber(iterX->first);

            if ((binXNumber < 0) || (binXNumber > m_nBinsX))
                continue;

            const float binXCenter(firstBinXCenter + (m_xBinWidth * static_cast<float>(binXNumber)));
            const float binContents(iterX->second);

            sumEntries += binContents;
            sumXEntries += binContents * binXCenter;
            sumXXEntries += binContents * binXCenter * binXCenter;
        }
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    const float meanX(sumXEntries / sumEntries);
    const float meanXX(sumXXEntries / sumEntries);

    return std::sqrt(meanXX - (meanX * meanX));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetCumulativeSumX(const int yLowBin, const int yHighBin) const
{
    float sumEntries(0.f);

    for (int yBin = yLowBin; yBin <= yHighBin; ++yBin)
    {
        TwoDHistogramMap::const_iterator iterY = m_yxHistogramMap.find(yBin);

        if (m_yxHistogramMap.end() == iterY)
            continue;

        for (HistogramMap::const_iterator iterX = iterY->second.begin(), iterXEnd = iterY->second.end(); iterX != iterXEnd; ++iterX)
        {
            const int binXNumber(iterX->first);

            if ((binXNumber < 0) || (binXNumber > m_nBinsX))
                continue;

            sumEntries += iterX->second;
        }
    }

    return sumEntries;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::GetMaximumX(const int yLowBin, const int yHighBin, float &maximumValueX, int &maximumBinX) const
{
    maximumBinX = -1;
    maximumValueX = 0.f;

    for (int yBin = yLowBin; yBin <= yHighBin; ++yBin)
    {
        TwoDHistogramMap::const_iterator iterY = m_yxHistogramMap.find(yBin);

        if (m_yxHistogramMap.end() == iterY)
            continue;

        for (HistogramMap::const_iterator iterX = iterY->second.begin(), iterXEnd = iterY->second.end(); iterX != iterXEnd; ++iterX)
        {
            const int binXNumber(iterX->first);

            if ((binXNumber < 0) || (binXNumber > m_nBinsX))
                continue;

            const float binContents(iterX->second);

            if (binContents > maximumValueX)
            {
                maximumValueX = binContents;
                maximumBinX = binXNumber;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetMeanY(const int xLowBin, const int xHighBin) const
{
    float sumEntries(0.f), sumYEntries(0.f);
    const float firstBinYCenter(m_yLow + (0.5f * m_yBinWidth));

    for (int xBin = xLowBin; xBin <= xHighBin; ++xBin)
    {
        TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.find(xBin);

        if (m_xyHistogramMap.end() == iterX)
            continue;

        for (HistogramMap::const_iterator iterY = iterX->second.begin(), iterYEnd = iterX->second.end(); iterY != iterYEnd; ++iterY)
        {
            const int binYNumber(iterY->first);

            if ((binYNumber < 0) || (binYNumber > m_nBinsY))
                continue;

            const float binYCenter(firstBinYCenter + (m_yBinWidth * static_cast<float>(binYNumber)));
            const float binContents(iterY->second);

            sumEntries += binContents;
            sumYEntries += binContents * binYCenter;
        }
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    return (sumYEntries / sumEntries);
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetStandardDeviationY(const int xLowBin, const int xHighBin) const
{
    float sumEntries(0.f), sumYEntries(0.f), sumYYEntries(0.f);
    const float firstBinYCenter(m_yLow + (0.5f * m_yBinWidth));

    for (int xBin = xLowBin; xBin <= xHighBin; ++xBin)
    {
        TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.find(xBin);

        if (m_xyHistogramMap.end() == iterX)
            continue;

        for (HistogramMap::const_iterator iterY = iterX->second.begin(), iterYEnd = iterX->second.end(); iterY != iterYEnd; ++iterY)
        {
            const int binYNumber(iterY->first);

            if ((binYNumber < 0) || (binYNumber > m_nBinsY))
                continue;

            const float binYCenter(firstBinYCenter + (m_yBinWidth * static_cast<float>(binYNumber)));
            const float binContents(iterY->second);

            sumEntries += binContents;
            sumYEntries += binContents * binYCenter;
            sumYYEntries += binContents * binYCenter * binYCenter;
        }
    }

    if (std::fabs(sumEntries) < std::numeric_limits<float>::epsilon())
        return 0.f;

    const float meanY(sumYEntries / sumEntries);
    const float meanYY(sumYYEntries / sumEntries);

    return std::sqrt(meanYY - (meanY * meanY));
}

//------------------------------------------------------------------------------------------------------------------------------------------

float TwoDHistogram::GetCumulativeSumY(const int xLowBin, const int xHighBin) const
{
    float sumEntries(0.f);

    for (int xBin = xLowBin; xBin <= xHighBin; ++xBin)
    {
        TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.find(xBin);

        if (m_xyHistogramMap.end() == iterX)
            continue;

        for (HistogramMap::const_iterator iterY = iterX->second.begin(), iterYEnd = iterX->second.end(); iterY != iterYEnd; ++iterY)
        {
            const int binYNumber(iterY->first);

            if ((binYNumber < 0) || (binYNumber > m_nBinsY))
                continue;

            sumEntries += iterY->second;
        }
    }

    return sumEntries;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::GetMaximumY(const int xLowBin, const int xHighBin, float &maximumValueY, int &maximumBinY) const
{
    maximumBinY = -1;
    maximumValueY = 0.f;

    for (int xBin = xLowBin; xBin <= xHighBin; ++xBin)
    {
        TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.find(xBin);

        if (m_xyHistogramMap.end() == iterX)
            continue;

        for (HistogramMap::const_iterator iterY = iterX->second.begin(), iterYEnd = iterX->second.end(); iterY != iterYEnd; ++iterY)
        {
            const int binYNumber(iterY->first);

            if ((binYNumber < 0) || (binYNumber > m_nBinsY))
                continue;

            const float binContents(iterY->second);

            if (binContents > maximumValueY)
            {
                maximumValueY = binContents;
                maximumBinY = binYNumber;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::GetMaximumXY(float &maximumValueXY, int &maximumBinX, int &maximumBinY) const
{
    maximumBinX = -1;
    maximumBinY = -1;
    maximumValueXY = 0.f;

    for (TwoDHistogramMap::const_iterator iterX = m_xyHistogramMap.begin(), iterXEnd = m_xyHistogramMap.end(); iterX != iterXEnd; ++iterX)
    {
        const int binXNumber(iterX->first);

        if ((binXNumber < 0) || (binXNumber > m_nBinsX))
            continue;

        for (HistogramMap::const_iterator iterY = iterX->second.begin(), iterYEnd = iterX->second.end(); iterY != iterYEnd; ++iterY)
        {
            const int binYNumber(iterY->first);

            if ((binYNumber < 0) || (binYNumber > m_nBinsY))
                continue;

            const float binContents(iterY->second);

            if (binContents > maximumValueXY)
            {
                maximumValueXY = binContents;
                maximumBinX = binXNumber;
                maximumBinY = binYNumber;
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::SetBinContent(const int binX, const int binY, const float value)
{
    if ((binX < -1) || (binX > m_nBinsX + 1) || (binY < -1) || (binY > m_nBinsY + 1))
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);

    m_xyHistogramMap[binX][binY] = value;
    m_yxHistogramMap[binY][binX] = value;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void TwoDHistogram::Fill(const float valueX, const float valueY, const float weight)
{
    const int binX(std::max(-1, std::min(static_cast<int>(m_nBinsX + 1), static_cast<int>((valueX - m_xLow) / m_xBinWidth)) ));
    const int binY(std::max(-1, std::min(static_cast<int>(m_nBinsY + 1), static_cast<int>((valueY - m_yLow) / m_yBinWidth)) ));

    HistogramMap &yHistogramMap(m_xyHistogramMap[binX]);

    if (yHistogramMap.count(binY) > 0)
    {
        yHistogramMap[binY] += weight;
    }
    else
    {
        yHistogramMap[binY] = weight;
    }

    m_yxHistogramMap[binY][binX] = yHistogramMap[binY];
}

} // namespace pandora
