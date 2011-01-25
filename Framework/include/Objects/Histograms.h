/**
 *  @file   PandoraPFANew/Framework/include/Objects/Histograms.h
 * 
 *  @brief  Header file for histogram classes
 * 
 *  $Log: $
 */
#ifndef HISTOGRAMS_H
#define HISTOGRAMS_H 1

#include <map>

namespace pandora
{

/**
 *  @brief  Histogram class
 */
class Histogram
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  nBinsX number of x bins
     *  @param  xLow min binned x value
     *  @param  xHigh max binned x value
     */
    Histogram(const unsigned int nBinsX, const float xLow, const float xHigh);

    /**
     *  @brief  Copy constructor
     * 
     *  @param  rhs the histogram to copy
     */
    Histogram(const Histogram &rhs);

    /**
     *  @brief  Get the number of x bins
     * 
     *  @return The number of x bins
     */
    unsigned int GetNBinsX() const;

    /**
     *  @brief  Get the min binned x value
     * 
     *  @return The min binned x value
     */
    float GetXLow() const;

    /**
     *  @brief  Get the max binned x value
     * 
     *  @return The max binned x value
     */
    float GetXHigh() const;

    /**
     *  @brief  Get the x bin width
     * 
     *  @return The x bin width
     */
    float GetXBinWidth() const;

    /**
     *  @brief  Get the content of a specified bin
     * 
     *  @param  binX the specified bin number
     * 
     *  @return The content of the specified bin
     */
    float GetBinContent(const int binX) const;

    /**
     *  @brief  Get the mean x value of entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The mean x value
     */
    float GetMeanX() const;

    /**
     *  @brief  Get the standard deviation of entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The standard deviation
     */
    float GetStandardDeviationX() const;

    /**
     *  @brief  Get the cumulative sum of bin entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @return The cumulative sum
     */
    float GetCumulativeSumX() const;

    /**
     *  @brief  Get the maximum value in the histogram and the corresponding bin number (excludes overflow and underflow bins)
     * 
     *  @param  maximumValueX to receive the maximum value
     *  @param  maximumBinX to receive the coordinate of the bin containing the maximum value
     */
    void GetMaximumX(float &maximumValueX, int &maximumBinX) const;

    /**
     *  @brief  Set the contents of a specified bin
     * 
     *  @param  binX the specified bin number
     *  @param  value the new value for the specified bin
     */
    void SetBinContent(const int binX, const float value);

    /**
     *  @brief  Add an entry to the histogram
     * 
     *  @param  valueX the value for the entry
     *  @param  weight the weight associated with this entry
     */
    void Fill(const float valueX, const float weight = 1.f);

private:
    typedef std::map<int, float> HistogramMap;

    HistogramMap        m_histogramMap;         ///< The histogram map

    const unsigned int  m_nBinsX;               ///< The number of x bins
    const float         m_xLow;                 ///< The min binned x value
    const float         m_xHigh;                ///< The max binned x value
    float               m_xBinWidth;            ///< The x bin width
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  TwoDHistogram class
 */
class TwoDHistogram
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  nBinsX number of x bins
     *  @param  xLow min binned x value
     *  @param  xHigh max binned x value
     *  @param  nBinsY number of y bins
     *  @param  yLow min binned y value
     *  @param  yHigh max binned y value
     */
    TwoDHistogram(const unsigned int nBinsX, const float xLow, const float xHigh, const unsigned int nBinsY, const float yLow, const float yHigh);

    /**
     *  @brief  Copy constructor
     * 
     *  @param  rhs the histogram to copy
     */
    TwoDHistogram(const TwoDHistogram &rhs);

    /**
     *  @brief  Get the number of x bins
     * 
     *  @return The number of x bins
     */
    unsigned int GetNBinsX() const;

    /**
     *  @brief  Get the min binned x value
     * 
     *  @return The min binned x value
     */
    float GetXLow() const;

    /**
     *  @brief  Get the max binned x value
     * 
     *  @return The max binned x value
     */
    float GetXHigh() const;

    /**
     *  @brief  Get the x bin width
     * 
     *  @return The x bin width
     */
    float GetXBinWidth() const;

    /**
     *  @brief  Get the number of y bins
     * 
     *  @return The number of y bins
     */
    unsigned int GetNBinsY() const;

    /**
     *  @brief  Get the min binned y value
     * 
     *  @return The min binned y value
     */
    float GetYLow() const;

    /**
     *  @brief  Get the max binned y value
     * 
     *  @return The max binned y value
     */
    float GetYHigh() const;

    /**
     *  @brief  Get the y bin width
     * 
     *  @return The y bin width
     */
    float GetYBinWidth() const;

    /**
     *  @brief  Get the content of a specified bin
     * 
     *  @param  binX the specified x bin number
     *  @param  binY the specified y bin number
     * 
     *  @return The content of the specified bin
     */
    float GetBinContent(const int binX, const int binY) const;

    /**
     *  @brief  Get the mean x value of entries in the histogram (excluding overflow and underflow bins) corresponding to
     *          the specified y range
     * 
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     * 
     *  @return The mean x value
     */
    float GetMeanX() const;
    float GetMeanX(const int yBin) const;
    float GetMeanX(const int yLowBin, const int yHighBin) const;

    /**
     *  @brief  Get the standard deviation of x entries in the histogram (excluding overflow and underflow bins) corresponding to
     *          the specified y range
     * 
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     * 
     *  @return The standard deviation
     */
    float GetStandardDeviationX() const;
    float GetStandardDeviationX(const int yBin) const;
    float GetStandardDeviationX(const int yLowBin, const int yHighBin) const;

    /**
     *  @brief  Get the cumulative sum of x bin entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     * 
     *  @return The cumulative sum
     */
    float GetCumulativeSumX() const;
    float GetCumulativeSumX(const int yBin) const;
    float GetCumulativeSumX(const int yLowBin, const int yHighBin) const;

    /**
     *  @brief  Get the maximum value in the x histogram and the corresponding bin number (excludes overflow and underflow bins)
     * 
     *  @param  yLowBin bin at start of specified y range
     *  @param  yHighBin bin at end of specified y range
     *  @param  maximumValueX to receive the maximum value
     *  @param  maximumBinX to receive the coordinate of the bin containing the maximum value
     */
    void GetMaximumX(float &maximumValueX, int &maximumBinX) const;
    void GetMaximumX(const int yBin, float &maximumValueX, int &maximumBinX) const;
    void GetMaximumX(const int yLowBin, const int yHighBin, float &maximumValueX, int &maximumBinX) const;

    /**
     *  @brief  Get the mean y value of entries in the histogram (excluding overflow and underflow bins) corresponding to
     *          the specified x range
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     * 
     *  @return The mean y value
     */
    float GetMeanY() const;
    float GetMeanY(const int xBin) const;
    float GetMeanY(const int xLowBin, const int xHighBin) const;

    /**
     *  @brief  Get the standard deviation of yentries in the histogram (excluding overflow and underflow bins) corresponding to
     *          the specified x range
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     * 
     *  @return The standard deviation
     */
    float GetStandardDeviationY() const;
    float GetStandardDeviationY(const int xBin) const;
    float GetStandardDeviationY(const int xLowBin, const int xHighBin) const;

    /**
     *  @brief  Get the cumulative sum of y bin entries in the histogram (excluding overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     * 
     *  @return The cumulative sum
     */
    float GetCumulativeSumY() const;
    float GetCumulativeSumY(const int xBin) const;
    float GetCumulativeSumY(const int xLowBin, const int xHighBin) const;

    /**
     *  @brief  Get the maximum value in they histogram and the corresponding bin number (excludes overflow and underflow bins)
     * 
     *  @param  xLowBin bin at start of specified x range
     *  @param  xHighBin bin at end of specified x range
     *  @param  maximumValueY to receive the maximum value
     *  @param  maximumBinY to receive the coordinate of the bin containing the maximum value
     */
    void GetMaximumY(float &maximumValueY, int &maximumBin) const;
    void GetMaximumY(const int xBin, float &maximumValueY, int &maximumBinY) const;
    void GetMaximumY(const int xLowBin, const int xHighBin, float &maximumValueY, int &maximumBinY) const;

    /**
     *  @brief  Get the maximum value in the histogram and the corresponding bin numbers
     * 
     *  @param  maximumValueXY to receive the maximum value
     *  @param  maximumBinX to receive the x coordinate of the bin containing the maximum value
     *  @param  maximumBinY to receive the y coordinate of the bin containing the maximum value
     */
    void GetMaximumXY(float &maximumValueXY, int &maximumBinX, int &maximumBinY) const;

    /**
     *  @brief  Set the contents of a specified bin
     * 
     *  @param  binX the specified x bin number
     *  @param  binY the specified y bin number
     *  @param  value the new value for the specified bin
     */
    void SetBinContent(const int binX, const int binY, const float value);

    /**
     *  @brief  Add an entry to the histogram
     * 
     *  @param  valueX the x value for the entry
     *  @param  valueY the y value for the entry
     *  @param  weight the weight associated with this entry
     */
    void Fill(const float valueX, const float valueY, const float weight = 1.f);

private:
    typedef std::map<int, float> HistogramMap;
    typedef std::map<int, HistogramMap> TwoDHistogramMap;

    TwoDHistogramMap    m_xyHistogramMap;       ///< The x->y->value 2d histogram map
    TwoDHistogramMap    m_yxHistogramMap;       ///< The y->x->value 2d histogram map

    unsigned int        m_nBinsX;               ///< The number of x bins
    float               m_xLow;                 ///< The min binned x value
    float               m_xHigh;                ///< The max binned x value
    float               m_xBinWidth;            ///< The x bin width

    unsigned int        m_nBinsY;               ///< The number of y bins
    float               m_yLow;                 ///< The min binned y value
    float               m_yHigh;                ///< The max binned y value
    float               m_yBinWidth;            ///< The y bin width
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int Histogram::GetNBinsX() const
{
    return m_nBinsX;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetXLow() const
{
    return m_xLow;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetXHigh() const
{
    return m_xHigh;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float Histogram::GetXBinWidth() const
{
    return m_xBinWidth;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int TwoDHistogram::GetNBinsX() const
{
    return m_nBinsX;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetXLow() const
{
    return m_xLow;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetXHigh() const
{
    return m_xHigh;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetXBinWidth() const
{
    return m_xBinWidth;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline unsigned int TwoDHistogram::GetNBinsY() const
{
    return m_nBinsY;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetYLow() const
{
    return m_yLow;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetYHigh() const
{
    return m_yHigh;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetYBinWidth() const
{
    return m_yBinWidth;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetMeanX() const
{
    return this->GetMeanX(0, m_nBinsY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetMeanX(const int yBin) const
{
    return this->GetMeanX(yBin, yBin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetStandardDeviationX() const
{
    return this->GetStandardDeviationX(0, m_nBinsY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetStandardDeviationX(const int yBin) const
{
    return this->GetStandardDeviationX(yBin, yBin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetCumulativeSumX() const
{
    return this->GetCumulativeSumX(0, m_nBinsY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetCumulativeSumX(const int yBin) const
{
    return this->GetCumulativeSumX(yBin, yBin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void TwoDHistogram::GetMaximumX(float &maximumValueX, int &maximumBinX) const
{
    return this->GetMaximumX(0, m_nBinsY, maximumValueX, maximumBinX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void TwoDHistogram::GetMaximumX(const int yBin, float &maximumValueX, int &maximumBinX) const
{
    return this->GetMaximumX(yBin, yBin, maximumValueX, maximumBinX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetMeanY() const
{
    return this->GetMeanY(0, m_nBinsX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetMeanY(const int xBin) const
{
    return this->GetMeanY(xBin, xBin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetStandardDeviationY() const
{
    return this->GetStandardDeviationY(0, m_nBinsX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetStandardDeviationY(const int xBin) const
{
    return this->GetStandardDeviationY(xBin, xBin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetCumulativeSumY() const
{
    return this->GetCumulativeSumY(0, m_nBinsX);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline float TwoDHistogram::GetCumulativeSumY(const int xBin) const
{
    return this->GetCumulativeSumY(xBin, xBin);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void TwoDHistogram::GetMaximumY(float &maximumValueY, int &maximumBinY) const
{
    return this->GetMaximumX(0, m_nBinsX, maximumValueY, maximumBinY);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline void TwoDHistogram::GetMaximumY(const int xBin, float &maximumValueY, int &maximumBinY) const
{
    return this->GetMaximumX(xBin, xBin, maximumValueY, maximumBinY);
}

} // namespace pandora

#endif // #ifndef HISTOGRAMS_H
