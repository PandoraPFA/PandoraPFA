/**
 *  @file   PandoraPFANew/FineGranularityContent/include/Clustering/ECalPhotonClusteringAlgorithm.h
 * 
 *  @brief  Header file for the ecal photon clustering algorithm class.
 * 
 *  $Log: $
 */
#ifndef ECAL_PHOTON_CLUSTERING_ALGORITHM_H
#define ECAL_PHOTON_CLUSTERING_ALGORITHM_H 1

#include "Pandora/Algorithm.h"

#include "Objects/Cluster.h"

#include <cmath>

namespace pandora
{
    class Axis {
    public:
    typedef std::map<float,int> BinMap;

    class WrongBinNumber {};
    class WrongLimits    {};
    class XmlError {};
    class NotAxis {};

    Axis();
    Axis( int bins, float from, float to );
    Axis( const std::vector<float>& binBorders );

    void SetDimensions( int bins, float from, float to );
    void SetDimensions( const std::vector<float>& binBorders );

    int GetBinForValue( float value );
    
    int GetNumberBins() const;
    float GetMinValue() const;
    float GetMaxValue() const;

    void WriteToXml ( TiXmlElement * xmlElement );
    void ReadFromXml( const TiXmlElement &xmlElement );

    void Print( std::ostream& );

    private:
    int numberBins;
    float minValue;
    float maxValue;
    float k;
       
    bool regularBins;

    BinMap bins;

    friend class Histogram1D;
    friend class Histogram2D;
    };

    inline float Axis::GetMinValue() const
    {
    return minValue;
    }

    inline float Axis::GetMaxValue() const 
    {
    return maxValue;
    }


    class Histogram2D;

    class Histogram1D {
    public:
    typedef std::map<int,float> MapOfBins;

    class XmlError {};
    class NotHistogram1D {};
    class DifferentBinning {};


    Histogram1D();
    Histogram1D( const TiXmlElement &xmlElement );
    Histogram1D( const std::string &histogramName, int numberBins, float from, float to );
    Histogram1D( const std::string &histogramName, const std::vector<float>& binBorders );

    void SetDimensions( const std::string &histogramName, int numberBins, float from, float to );
    void SetDimensions( const std::string &histogramName, const std::vector<float>& binBorders );

    void Fill( float value, float weight = 1.0 );

    float GetBinContent( int bin );
    float Get( float value );

    float GetSumOfEntries();
    void Scale( float value );

    void Add( Histogram1D &histogram );

    void WriteToXml ( TiXmlElement * &xmlElement );
    void ReadFromXml( const TiXmlElement &xmlElement );

    void Print( std::ostream& );

    float GetSumOfWeights() { return sumOfWeights; } // get sum of filled in weights (not changed when the histogram is scaled)

    const Axis& GetAxis() { return axis; }

    static void TokenizeString(const std::string &inputString, StringVector &tokens, const std::string &delimiter);

    private:
    void CreateEmptyBins();

    std::string name;
    Axis axis;
    
    MapOfBins bins;

    float sumOfWeights;
    };


    class Histogram2D {
    public:
    typedef std::map<int,float> MapOfBins;
    typedef std::map<int,MapOfBins> MapOfMapOfBins;

    class XmlError {};
    class NotHistogram2D {};
    class DataStructureError {};
    class DifferentBinning {};


    Histogram2D();
    Histogram2D( const TiXmlElement &xmlElement );
    Histogram2D( const std::string &histogramName, int numberBinsX, float fromX, float toX, int numberBinsY, float fromY, float toY );
    Histogram2D( const std::string &histogramName, const std::vector<float>& binBorders, int numberBinsY, float fromY, float toY );
    Histogram2D( const std::string &histogramName, int numberBinsX, float fromX, float toX, const std::vector<float>& binBorders );
    Histogram2D( const std::string &histogramName, const std::vector<float>& binBordersX, const std::vector<float>& binBordersY );

    void SetDimensions( const std::string &histogramName, int numberBinsX, float fromX, float toX, int numberBinsY, float fromY, float toY );
    void SetDimensions( const std::string &histogramName, int numberBinsX, float fromX, float toX, const std::vector<float>& binBorders );
    void SetDimensions( const std::string &histogramName, const std::vector<float>& binBorders, int numberBinsX, float fromX, float toX );
    void SetDimensions( const std::string &histogramName, const std::vector<float>& binBordersX, const std::vector<float>& binBordersY );

    void Fill( float x, float y, float weight = 1.0 );

    float GetBinContent( int binX, int binY );
    float Get( float valueX, float valueY );

    float GetSumOfEntries();
    void Scale( float value );

    void Add( Histogram2D &histogramToAdd );

    void WriteToXml ( TiXmlElement * &xmlElement );
    void ReadFromXml( const TiXmlElement &xmlElement );

    void Print( std::ostream& );
    void Print( std::string );

    float GetSumOfWeights() { return sumOfWeights; } // get sum of filled in weights (not changed when the histogram is scaled)

    const Axis& GetAxisX() { return axisX; }
    const Axis& GetAxisY() { return axisY; }

    private:
    void CreateEmptyBins();

    std::string name;
    Axis axisX, axisY;
    
    MapOfMapOfBins bins;

    float sumOfWeights;

    };



    class PhotonIDLikelihoodCalculator
    {
    public:

        class FileNotFound {};

    static PhotonIDLikelihoodCalculator* Instance();
    float  PID(float E, float rms, float frac, float start);
    void Delete();

    void WriteXmlSig( const std::string& fileName );
    void WriteXmlBkg( const std::string& fileName );
    void LoadXml(  const pandora::StringVector& fileNamesSig, const pandora::StringVector& fileNamesBkg, bool dontTakeLastFileName = false );

    Histogram1D energySig;
    Histogram1D energyBkg;

    Histogram2D rmsSig;
    Histogram2D fracSig;
    Histogram2D startSig;
    Histogram2D rmsBkg;
    Histogram2D fracBkg;
    Histogram2D startBkg;


    protected:
        PhotonIDLikelihoodCalculator() {  }

    private:
        static PhotonIDLikelihoodCalculator* _instance;
    
    static bool fromXml;

    void ReadXmlSignal(      const std::string& fileNameSig );
    void ReadXmlBackground(  const std::string& fileNameBkg );

    };



    typedef struct
    {
        float energy;
        int   du;
        int   dv;
        float dmin;
        int   showerDepth90;
        int   showerDepth25;
        int   showerStartDepth;
        float rms;
        int peakNumber;
    } protoClusterPeaks_t;

} // namespace pandora

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief ECalPhotonClusteringAlgorithm class
 */
class ECalPhotonClusteringAlgorithm : public pandora::Algorithm
{
    /**
     * @brief provides the calculation of a running mean and RMS
     *
     */
    class RunningMeanRMS
    {
    public:
        /**
         * @brief Constructor
         */
    RunningMeanRMS() : m_mean(0), m_nEvents(0), m_weights(0), m_s2(0),m_Rms(0)
        {
        }

        /**
         * @brief getter for current RMS
         *
         * @return (double) RMS
         */
        double GetRMS()
        {
            return m_Rms;
        }

        /**
         * @brief getter for current mean value
         *
         * @return (double) mean value
         */
        double GetMean()
        {
            return m_mean;
        }

        /**
         * @brief getter for current sum of event-weights
         *
         * @return (double) weights
         */
        double GetWeightSum()
        {
            return m_weights;
        }

        /**
         * @brief getter for current number of events
         *
         * @return (int) number of events
         */
        double GetN()
        {
            return m_nEvents;
        }
        
        /**
         * @brief reset everything
         */
        void Clear()
        {
            m_mean = 0;
            m_nEvents = 0;
            m_weights = 0;
            m_s2 = 0;
            m_Rms = 0;
        }

        /**
         * @brief calculate new mean and RMS value
         *
         * @param value
         * @param eventWeight
         */
        void Fill( double value, double eventWeight )
        {
            // calculate the mean-value
            int nNew = m_nEvents + 1;
            double meanNew = CalcMean( m_mean, m_nEvents, value, m_weights, eventWeight );

            double weightsumNew = m_weights + eventWeight;
            // calculate rms
            // S = SQRT( ((n-1)*sOld2 + (xNew-mNew)*(xnew-mOld) )/n )
            // calculate the rms
            double s2New = CalcS2( m_mean, meanNew, nNew, m_s2, value );

            m_Rms = sqrt( s2New );
            m_mean = meanNew;
            m_nEvents = nNew;
            m_weights = weightsumNew;
            m_s2 = s2New;
        }

    private:
        double m_mean;
        int    m_nEvents;
        double m_weights;
        double m_s2;
        double m_Rms;

        /**
         * @brief calculate the new mean value 
         *
         * @param meanOld
         * @param n
         * @param valNew
         * @param weightsumOld
         * @param evweight
         * @return
         */
        double CalcMean( double meanOld, double n, double valNew, double weightsumOld, double evweight ){
        const double epsilon = 1e-7;
        if( fabs(n - 0.)<epsilon ) 
            return valNew;
            //        double meanNew = meanOld + ( valNew - meanOld )/(n +1 );
            double meanNew = (weightsumOld*meanOld + evweight*valNew) / (weightsumOld + evweight);
            return meanNew;
        }

        /**
         * @brief calculate the new s2 value (necessary for calculating the RMS)
         *
         * @param meanOld
         * @param meanNew
         * @param nNew
         * @param s2Old
         * @param valNew
         * @return
         */
        double CalcS2( double meanOld, double meanNew, double nNew, double s2Old, double valNew )
        {
        const double epsilon = 1e-7;
            if( fabs(nNew - 0.)<epsilon ) return 0.;
            double s2New = ( (nNew-1)*s2Old + (valNew-meanNew)*(valNew-meanOld) )/nNew;
            return s2New;
        }
    };

    /**
     * @brief stores photon Id properties
     *
     */
    class PhotonIdProperties
    {
    public:
        float GetLongProfileShowerStart()  {return m_photonLongShowerStart;};
        float GetLongProfilePhotonFraction(){return m_photonLongProfileFraction;};
        void  SetLongProfileShowerStart(   float photonLongShowerStart    ){ m_photonLongShowerStart = photonLongShowerStart;          }
        void  SetLongProfilePhotonFraction( float photonLongProfileFraction){  m_photonLongProfileFraction = photonLongProfileFraction; }

        float m_photonLongProfileFraction;
        float m_photonLongShowerStart;
    };

    /**
     * @brief stores cluster properties
     */
    class ClusterProperties
    {
    public:
        float electromagneticEnergy;

        float m_hitMean[3]; // alyways x,y,z
        float m_centroid[3];
        float m_centroid10[3];
        float m_centroid20[3];

        float m_centroidEnergy;
        float m_centroid10Energy;
        float m_centroid20Energy;

        pandora::CartesianVector m_centroidFirstLayer; 

        int   m_innerPseudoLayer;
    };

public:
    /**
     *  @brief  Factory class for instantiating algorithm
     */
    class Factory : public pandora::AlgorithmFactory
    {
    public:
        pandora::Algorithm *CreateAlgorithm() const;
    };

    ECalPhotonClusteringAlgorithm();

    ~ECalPhotonClusteringAlgorithm();

    /**
     *  @brief  Perform any operations that must occur after reading settings, but before running the algorithm
     */
    virtual pandora::StatusCode Initialize();

 private:
    bool IsPhoton( pandora::Cluster* &pPhotonCandidateCluster, const pandora::OrderedCaloHitList& pOriginalOrderedCaloHitList, 
        pandora::protoClusterPeaks_t& peak, ClusterProperties& originalClusterProperties, bool& useOriginalCluster, int& peaksSize); 

    pandora::StatusCode TransverseProfile(const pandora::Cluster* cluster, std::vector<pandora::protoClusterPeaks_t> &peaks, int maxLayers);
    pandora::Cluster* TransverseProfile( const ClusterProperties& clusterProperties, const pandora::OrderedCaloHitList& pOrderedCaloHitList, const int peakForProtoCluster, const unsigned int maxLayers, const int extraLayers = 0);

    float GetTruePhotonContribution(const pandora::Cluster* cluster);
    void GetClusterProperties(const pandora::Cluster* cluster, ClusterProperties& clusterProperties );
    void DistanceToPositionAndDirection(const pandora::CartesianVector& position, const pandora::CartesianVector& referencePosition,
        const pandora::CartesianVector& referenceDirection, float& longitudinalComponent, float& perpendicularComponent);

    void CreateOrSaveLikelihoodHistograms(bool create);

    void ComputeTransverseVectors( const pandora::CartesianVector& direction, pandora::CartesianVector& transverseUnitVectorU,
        pandora::CartesianVector& transverseUnitVectorV);

    pandora::StatusCode Run();
    pandora::StatusCode ReadSettings(const TiXmlHandle xmlHandle);

    static const unsigned int m_maximumNumberOfLayers;

    unsigned int            m_nECalLayers;
    unsigned int            m_minimumHitsInClusters;
    int                     m_producePrintoutStatements;
    int                     m_makingPhotonIdLikelihoodHistograms;

    std::string             m_clusterListName;              ///< name of the initial cluster list (only needed when clustering algorithm is run)
    bool                    m_preserveClusters;             ///< preserve clusters which were not split and split which are not photons

    bool                    m_cheatingTrueFractionForPid;   ///< Cheating! Set PID to true photon fraction

    std::string             m_monitoringFileName;           ///< filename for file where for monitoring information is stored
    pandora::StringVector   m_configurationFileNamesBkg;    ///< filenames for file where the configuration of the photon clustering is stored : background
    pandora::StringVector   m_configurationFileNamesSig;    ///< filenames for file where the configuration of the photon clustering is stored : signal
    float                   m_likelihoodRatioCut;           ///< cut on likelihood ratio

    std::string             m_strategy;                     ///< The strategy used for photon recognition

    std::string             m_clusteringAlgorithmName;      ///< The name of the clustering algorithm to run

    pandora::StringVector   m_energyBins;                   ///< borders of bins of energy binning (only important at production of configuration xml files)
    pandora::StringVector   m_dimensionsRms;                ///< dimensions of photon candidate cluster Rms histogram (e.g. "20 0 5")
    pandora::StringVector   m_dimensionsFraction;           ///< dimensions of photon fraction histogram (e.g. "20 0 1.0")
    pandora::StringVector   m_dimensionsStart;              ///< dimensions of photon start histogram (e.g. "20 0 10.0" )

    std::string             m_produceConfigurationFiles;    ///< produce the configuration file (using the provided events) ("signal" events, "background" events, "signal" and "background" events, to be split by "fraction", "combine" takes a list of input files and writes into the last given filename

    bool                    m_isAlreadyInitialized;         ///< set to true if initialisation has taken place

    pandora::TrackList     *pTrackList;                     ///< tracks to be are checked if they are too close to the photoncandidate for a photon
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline pandora::Algorithm *ECalPhotonClusteringAlgorithm::Factory::CreateAlgorithm() const
{
    return new ECalPhotonClusteringAlgorithm();
}

#endif // #ifndef ECAL_PHOTON_CLUSTERING_ALGORITHM_H
