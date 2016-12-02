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
#include <utility> /* std::pair, std::move */
#include <cassert> /* assert */
#include <fstream> /* ifsteam */
#include <sstream> /* stringstream */
#include <iomanip> /* std::setprecision */
#include <algorithm> /* std::max */
#include <regex> /* std::regex */
#include <cmath> /* log */
#include <memory> /* std::unique_ptr, std::shared_ptr */

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

// Typedef(s).
typedef std::unique_ptr<TH1>     upTH1;
typedef std::unique_ptr<TPad>    upTPad;
typedef std::unique_ptr<TCanvas> upTCanvas;
typedef std::shared_ptr<TH1>     spTH1;
typedef std::shared_ptr<TPad>    spTPad;
typedef std::shared_ptr<TCanvas> spTCanvas;

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
  class PlottingHelper {

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
    
    void rebin  (const unsigned& nrebin);
    void setLog (const bool&     doLog);
    
    void computeImprovement (const int&  improvementDirection);
    void printBinContents   (const bool& print);
    void setScaleSignal     (const double& scaleSignal);
    void setPadding     (const double& padding);
    
    void setNormalised (const bool& normalised);
    

    /// Get method(s).
    // ...
    
    
    /// High-level management method(s).
    void draw ();
    void save (const string& path);
    
    
  protected:
    
    /// Low-level management method(s).
    void closePads_ ();
    bool setupCanvas_ ();
    std::unique_ptr<HistType> getHistogram_ (TFile* file, const std::string& path);
    void loadHistograms_ ();
    void loadSampleInfo_ ();
    
    void styleHist_ (HistType* hist, const bool& isMC, const string& name); // const unsigned& DSID = 0);
    
    
  private:

    /// Data member(s).
    string m_input  = "";
    string m_branch = "";
    vector<string> m_filenames;
    
    TFile* m_outfile = nullptr;
    string m_outdir  = "./";
    
    string m_sampleinfofile = "share/weightsMC.csv"; // "crossSections.csv";
    string m_settingsfile   = "share/plotSettings.csv";
    
    double m_lumi = 0.;
    
    vector<string> m_printlines;
    
    vector<string> m_axistitles = {"", "", ""};
    
    unsigned m_nbinsx = 1; /* For use only when reading from tree. */
    unsigned m_nbinsy = 1; /* For use only when reading from tree. */
    double   m_xmin   = 0.;
    double   m_xmax   = 1.;
    double   m_ymin   = 0.;
    double   m_ymax   = 1.;
    
    bool m_ratio = true;
    
    unsigned m_rebin = 1; /* For use only when reading from histogram. */
    
    bool m_log = true;
    bool m_1D  = true; /* make accessor */
    
    double m_padding = 2.3;
    
    bool m_print = false;
    
    bool m_normalised = false;
    
    // Completely internal.
    upTCanvas m_canvas;
    pair< upTPad, upTPad > m_pads = {nullptr, nullptr};
    
    unsigned m_W = 600;
    unsigned m_H = 700;
    
    upHistType m_sum  = nullptr;
    upHistType m_data = nullptr;
    map<string, upHistType > m_backgrounds;
    map<string, upHistType > m_signals;
    vector< pair<string, HistType* > > m_backgroundsSorted;

    map<string, upHistType > m_ratiohists;
    
    map< unsigned, SampleInfo > m_info;
    
    int m_improvementDirection = -1;
    double m_scaleSignal = 10.;
    
        
  };


  // Convenient class-specific typedef(s).
  template<class HistType>
  using PlottingHelpers = std::vector< PlottingHelper<HistType> >;
  
}

#endif
