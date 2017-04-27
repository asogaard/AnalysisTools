#ifndef AnalysisTools_PlottingHelper_h
#define AnalysisTools_PlottingHelper_h

/**
 * @file   PlottingHelper.h
 * @author Andreas Sogaard
 * @brief  Class for creating pretty analysis plots.
 */

// STL include(s).
#include <iostream>
#include <vector> /* std::vector */
#include <string> /* std::string */
#include <map> /* std::map */
#include <utility> /* std::pair, std::move, std::tuple */
#include <cassert> /* assert */
#include <fstream> /* ifsteam */
#include <sstream> /* stringstream */
#include <iomanip> /* std::setprecision */
#include <algorithm> /* std::max, std::sort, std::reverse, std::find */
#include <regex> /* std::regex */
#include <cmath> /* std::log, std::log10, std::pow */
#include <memory> /* std::unique_ptr, std::shared_ptr */
#include <exception> /* std::exception */

// ROOT include(s).
#include "TCanvas.h"
#include "TPad.h"
#include "TFile.h"
#include "TH1.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TTree.h"
#include "TBranch.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TArrow.h"
#include "TROOT.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/Logger.h"
#include "AnalysisTools/Range.h"

// Typedef(s).
typedef std::unique_ptr<TH1>     upTH1;
typedef std::unique_ptr<TPad>    upTPad;
typedef std::unique_ptr<TCanvas> upTCanvas;

// Making an exception for now...
using namespace std;


namespace AnalysisTools {

  // Utility function for creating and moving unique pointer.
  template<typename T>
  std::unique_ptr<T> makeUniqueMove (T* p);

  // Enum for indicating different types of samples
  enum class SampleType { Background, Signal, Data };

  // Struct for storing information about difference data/MC samples.
  struct SampleInfo {
    SampleType type;
    unsigned   DSID; // Dataset ID (MC channel number or data run number).
    double     lumi; // Integrated luminosity. Data only
    double     xsec; // Cross section. MC only.
    string     name; // Sample name, for grouping and legend entries.
    unsigned   evts; // Number of events, for normalisation.
  };
  

  /**
   * @brief Class for creating pretty analysis plots.
   *
   * ...
   *
   * This template class allows for plotting any type of ROOT histogram
   */
  template<class HistType>
  class PlottingHelper : public Logger {

    typedef std::unique_ptr<HistType> upHistType;

  public: 

    /// Static members.
    // Font size variable(s).
    static constexpr double s_fontSizeS = 0.035; // 0.040
    static constexpr double s_fontSizeM = 0.038; // 0.040
    static constexpr double s_fontSizeL = 0.042; // 0.050
 
    /// Constructor(s).
    PlottingHelper (const string& input) {
	setInputName(input);
      };
    
    PlottingHelper (const string& input, const vector<string>& filenames) {
      setInputName(input);
      setFilenames(filenames);
    };
    
    /// Destructor.
    ~PlottingHelper () {};
    
    
  public:
    
    /// Set method(s).
    void setInputName  (const string& input);
    void setFilenames  (const vector<string>& filenames);
    
    void setOutfile  (TFile* outfile);
    
    void setSampleInfoFile     (const string& sampleinfofile);
    void setSettingsFile (const string& settingsfile);
    
    void setPrintLines (const vector<string>& printlines);
    
    void setXaxisTitle (const string& title);
    void setYaxisTitle (const string& title);
    void setZaxisTitle (const string& title);
    
    void setAxis (const unsigned& nbins, const double& xmin, const double& xmax);
    void setAxis (const vector<double>& bins);
    
    void setXaxis (const unsigned& nbins, const double& xmin, const double& xmax);
    void setXaxis (const vector<double>& bins);
    
    void setYaxis (const unsigned& nbins, const double& xmin, const double& xmax);
    void setYaxis (const vector<double>& bins);
    
    void drawRatioPad (const bool& ratio);
    
    inline void setPlotmin (const double& plotmin) { m_plotmin = plotmin; }
    inline void setPlotmax (const double& plotmax) { m_plotmax = plotmax; }

    void rebin  (const unsigned& nrebin);
    void setLog (const bool&     doLog);
    
    void computeImprovement (const int&  improvementDirection);
    void printBinContents   (const bool& print);
    void setScaleSignal     (const double& scaleSignal);
    void setScaleBackground (const std::string& name, const double& scale);
    void setPadding         (const double& padding);
    void setPulls           (const bool& pulls);
    
    void setNormalised (const bool& normalised);
    void setSortBackgrounds (const bool& sortBackgrounds);

    inline void setLuminosity (const double& lumi) { m_lumi = lumi; }
    
    inline void setArrowsDown (const std::vector<float>& arrowsDown)  { m_arrowsDown  = arrowsDown; }
    inline void setArrowsRight(const std::vector<float>& arrowsRight) { m_arrowsRight = arrowsRight; }
    inline void setArrowsLeft (const std::vector<float>& arrowsLeft)  { m_arrowsLeft  = arrowsLeft; }

    void addRegion(const std::vector<float>& limits, const std::string& title, const bool& snap = true, const bool& light = false);

    inline void subtract (const std::vector<std::string>& subtract) { m_subtract = subtract; }

    inline void setIncludeOverflow (const bool& includeOverflow) { m_includeOverflow = includeOverflow; }

    void addSystematic (const TH1F& hist);
    void addSystematic (const std::function< void(TH1F* syst, const TH1F* data, const TH1F* background) >& f);
    void addSystematic (const std::string& cat_up, const std::string& cat_down);
    
    /// Get method(s).
    float getDataEstimateAgreement (const std::vector<unsigned>& bins);
    
    
    /// High-level management method(s).
    bool draw ();
    void save (const string& path);
    
    
  protected:
    
    /// Low-level management method(s).
    bool setupCanvas_ ();
    std::unique_ptr<HistType> getHistogram_ (TFile* file, const std::string& path);
    bool loadHistograms_ (const std::string& input);
    bool loadSampleInfo_ ();
    
    void styleHist_ (HistType* hist, const bool& isMC, const string& name); // const unsigned& DSID = 0);
    
    void computeSystematics_ ();
    void computeSystematic_  (const std::function< void(TH1F* syst, const TH1F* data, const TH1F* background) >& f);

    void addToSystematicsSum_ (HistType* syst);

    void getSystematicsFromCategories_ ();

    upHistType getSumOfBackgrounds_ ();
    upHistType getLeadingBackground_ ();

    void drawArrows_  (const float& plotmin, const float& plotmax);
    void drawRegions_ (const float& plotmin, const float& plotmax);
    

  private:

    /// Data member(s).
    string m_input  = "";
    string m_branch = "";
    vector<string> m_filenames;
    
    TFile* m_outfile = nullptr;
    string m_outdir  = "./";
    
    string m_sampleinfofile = "share/sampleInfo.csv";
    string m_settingsfile   = "share/plotSettings.csv";
    
    double m_lumi = 0.;
    
    vector<string> m_printlines;
    
    vector<string> m_axistitles = {"", "", ""};
    
    double m_plotmin = -9999.;
    double m_plotmax = -9999.;

    unsigned m_nbinsx = 1; /* For use only when reading from tree. */
    unsigned m_nbinsy = 1; /* For use only when reading from tree. */
    double   m_xmin   = 0.;
    double   m_xmax   = 1.;
    double   m_ymin   = 0.;
    double   m_ymax   = 1.;
    const double*  m_xbins  = nullptr;
    
    bool m_ratio = true;
    bool m_pulls = false;
    
    unsigned m_rebin = 1; /* For use only when reading from histogram. */
    
    bool m_log = true;
    bool m_1D  = true; /* make accessor */
   
    double m_padding = 2.0;
    
    bool m_print = false;
    
    bool m_normalised = false;
    bool m_sortBackgrounds = true;
 
    std::vector<float> m_arrowsDown  = {};
    std::vector<float> m_arrowsRight = {};
    std::vector<float> m_arrowsLeft  = {};

    //std::vector< std::pair<Range, std::string> > m_regions;
    std::vector< std::tuple<Range, std::string, bool, bool> > m_regions;

    // Completely internal.
    upTCanvas m_canvas;
    pair< upTPad, upTPad > m_pads = {nullptr, nullptr};
    
    unsigned m_width  = 600;
    unsigned m_height = 700;
    
    upHistType m_sum  = nullptr;
    upHistType m_background  = nullptr;
    upHistType m_data = nullptr;
    map<string, upHistType > m_backgrounds;
    map<string, upHistType > m_signals;
    vector< pair<string, HistType* > > m_backgroundsSorted;

    map<string, upHistType > m_ratiohists;
    
    map< unsigned, SampleInfo > m_info;
    
    int m_improvementDirection = -1;
    double m_scaleSignal = 10.;
    std::map<std::string, double> m_scaleBackground;

    bool m_includeOverflow = true;
            
    std::vector<std::string> m_subtract = {};
    
    std::vector< upHistType > m_systematics = {};
    std::vector< std::function< void(TH1F* syst, const TH1F* data, const TH1F* background) > > m_systematicCalls = {};
    std::vector< std::pair<std::string, std::string> > m_systematicsCategories;
    upHistType m_systematicsSum = nullptr;

  };


  /// Convenient, class-specific typedef(s).
  template<class HistType>
  using PlottingHelpers = std::vector< PlottingHelper<HistType> >;
  
}

#endif
