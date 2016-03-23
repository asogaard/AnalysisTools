#ifndef AnalysisTools_PlottingHelper_h
#define AnalysisTools_PlottingHelper_h

/**
 * @file PlottingHelper.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <iostream>
#include <vector>
#include <map>
#include <utility> /* std::pair */
#include <assert.h> /* assert */
#include <fstream> /* ifsteam */
#include <sstream> /* stringstream */
#include <iomanip> /* std::setprecision */
#include <algorithm> /* std::max */
#include <regex> 
#include <cmath> /* log */

// ROOT include(s).
#include "TCanvas.h"
#include "TPad.h"
#include "TFile.h"
#include "TH1.h"
#include "THStack.h"
#include "TTree.h"
#include "TBranch.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TROOT.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"

using namespace std;

namespace AnalysisTools {

    struct SampleInfo;
    enum class PlotType { Background, Signal, Data };
    
    class PlottingHelper {
        
    public:

        // Constructor(s).
        PlottingHelper (const string& input)
        {
            setInputName(input);
        };
        
        PlottingHelper (const string& input, const vector<string>& filenames) :
            m_filenames(filenames)
        {
            setInputName(input);
        };
        
        PlottingHelper (const string& input, const vector<string>& filenames, const double& lumi) :
            m_filenames(filenames),
            m_lumi(lumi)
        {
            setInputName(input);
        };
        
        // Destructor(s).
        ~PlottingHelper () {
            closePads();
        };
        
        
    public:
        
        // Set method(s).
        void setInputName  (const string& input);
        void setFilenames  (const vector<string>& filenames);
        
        void setOutfile  (TFile* outfile);
        
        void setXsecFile     (const string& xsecfile);
        void setSettingsFile (const string& settingsfile);

        void setLuminosity (const double& lumi);
        
        void setPrintLines (const vector<string>& printlines);
        
        void setXaxisTitle (const string& title);
        void setYaxisTitle (const string& title);
        void setZaxisTitle (const string& title);
        
        void setAxisTitles (const string& xtitle, const string& ytitle);
        void setAxisTitles (const string& xtitle, const string& ytitle, const string& ztitle);

        void setAxis (const unsigned& nbins, const double& xmin, const double& xmax);
        void setAxis (const vector<double>& bins);
        
        void setXaxis (const unsigned& nbins, const double& xmin, const double& xmax);
        void setXaxis (const vector<double>& bins);

        void setYaxis (const unsigned& nbins, const double& xmin, const double& xmax);
        void setYaxis (const vector<double>& bins);

        void drawRatioPad (const bool& ratio);
        
        void rebin (const unsigned& nrebin);
        
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        void draw ();
        void save (const string& path);

        
    protected:
        
        // Low-level management method(s).
        void closePads      ();
        void loadHistograms ();
        void loadXsec ();
        
        void styleHist (TH1F* hist, const bool& isMC, const string& name); // const unsigned& DSID = 0);
        
        
    private:
        
        string m_input  = "";
        string m_branch = "";
        vector<string> m_filenames;
        
        TFile* m_outfile = nullptr;
        
        string m_xsecfile     = "share/crossSections.csv";
        string m_settingsfile = "share/plotSettings.csv";
        
        double m_lumi = -1;
        
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
        
        bool m_log = true; /* Create accessors */
        
        // Completely internal.
        TCanvas*           m_canvas = nullptr;
        pair<TPad*, TPad*> m_pads = {nullptr, nullptr};
        
        unsigned m_W = 600;
        unsigned m_H = 700;
        
        TH1F* m_sum = nullptr;
        TH1F* m_data = nullptr;
        map<string, TH1F*> m_backgrounds;
        map<string, TH1F*> m_signals;
        
        map< PlotType, map<string, vector<TH1F*> > > m_hists = {
            { PlotType::Background, map<string, vector<TH1F*> >() },
            { PlotType::Signal,     map<string, vector<TH1F*> >() },
            { PlotType::Data,       map<string, vector<TH1F*> >() }
        };
        map<string, TH1F* > m_ratiohists;
        
        map< unsigned, SampleInfo > m_info;
        
        // Style stuff.
        const double m_fontSizeS = 0.040;
        const double m_fontSizeM = 0.040;
        const double m_fontSizeL = 0.050;

        
    };
    
    struct SampleInfo {
        PlotType type;
        unsigned DSID;
        double   lumi;
        double   xsec;
        string   name;
        unsigned evts;
    };

    using PlottingHelpers = vector<PlottingHelper>;
    
}

#endif