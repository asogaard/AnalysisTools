#ifndef AnalysisTools_Cut_h
#define AnalysisTools_Cut_h

/**
 * @file Cut.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <iostream>
#include <string>
#include <vector>
#include <functional> /* std::function */
#include <assert.h> /* assert */

// xAOD include(s)
// ...

// ROOT include(s).
#include "TDirectory.h"
#include "TLorentzVector.h"

// Forward declaration(s).
namespace  AnalysisTools {
    template <class T>
    class Selection;
}

// AnalysisTools include(s).
#include "AnalysisTools/ICut.h"
#include "AnalysisTools/PlotMacro1D.h"
#include "AnalysisTools/Range.h"

using namespace std;

namespace AnalysisTools {
    
    template <class T>
    class Cut : public ICut {
        
        friend class Selection<T>;
        
    public:

        // Constructor(s).
        Cut () {
            setName("Cut");
            setBasePlots();
        };
        
        Cut (const string& name) {
            setName(name);
            setBasePlots();
        };
        
        // Destructor(s).
        ~Cut () {
            for (auto& plot : m_plots) {
                delete plot;
            }
        };
        
        
    public:
        
        // Set method(s).
        void clearRanges ();
        
        void setRange (const Range& range);
        void setRange (const pair<double, double>& limits);
        void setRange (const double& down, const double& up);
        
        void setRanges (const Ranges& ranges);
        void setRanges (const vector< pair<double, double> >& vec_limits);
        
        void addRange (const Range& range);
        void addRange (const pair<double, double>& limits);
        void addRange (const double& down, const double& up);
        
        void addRanges (const Ranges& ranges);
        void addRanges (const vector< pair<double, double> >& vec_limits);
        
        void setFunction (function< double(T) > f);
        
        void clearPlots ();
        void addPlot (IPlotMacro* plot);
        
        void setName        (const string& name);
        void prependName    (const string& prefix);
        
        // Get method(s).
        string name    () const;
        
        vector< IPlotMacro* > plots      ();
        vector< TH1F* >       histograms ();
        
        
        // High-level management method(s).
        bool select (const T& obj) const;

        // histogrammin methods
        
    protected:
        
        // Low-level management method(s).
        void setDir (TDirectory* dir);
        void setBasePlots ();
        void grab   (IPlotMacro* plot);
        void write  ();

        
    private:
        
        string m_name     = "";
        
        TDirectory* m_dir = nullptr;
        
        unsigned m_nBins    = 10;
        double   m_axisDown =  0.;
        double   m_axisUp   =  1.;

        function< double(T) > m_function;
        Ranges m_ranges;
        
        string m_variable = "";
        string m_unit     = "";
        
        vector< IPlotMacro* > m_plots;
        
    };
 
    template <class T>
    using Cuts = vector< Cut<T> >;
 
    using CutsPtr = vector< ICut* >;
    
}

#endif