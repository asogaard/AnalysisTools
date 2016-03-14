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
    template <class T, class U>
    class Selection;
}

// AnalysisTools include(s).
#include "AnalysisTools/ICut.h"
#include "AnalysisTools/Localised.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Range.h"
#include "AnalysisTools/PlotMacro1D.h"

using namespace std;

namespace AnalysisTools {
    
    template <class T>
    class Cut : public ICut, public Localised {

        friend class Selection<TLorentzVector, T>;
        friend class Selection<T, AnalysisTools::PhysicsObject>;
        friend class Selection<T, AnalysisTools::Event>;
        friend class Selection<T, double>;
        friend class Selection<T, float>;
        friend class Selection<T, bool>;
        friend class Selection<T, int>;
        
        
    public:

        // Constructor(s).
        Cut () :
            Localised("Cut")
        {
            cout << "<Cut::Cut> Entering (default)." << endl;
            setBasePlots();
            cout << "<Cut::Cut> Exiting." << endl;
        };
        
        Cut (const string& name) :
            Localised(name)
        {
            cout << "<Cut::Cut> Entering (name)." << endl;
            cout << "<Cut::Cut>   Name: '" << name << "'" << endl;
            setBasePlots();
            cout << "<Cut::Cut> Exiting." << endl;
        };
        
        Cut (const Cut<T>& other) :
            Localised(other.m_name, other.m_dir),
            m_function(other.m_function),
            m_ranges(other.m_ranges)
        {
            cout << "<Cut::Cut> Entering (copy)." << endl;
            setBasePlots();
            cout << "<Cut::Cut> Exiting." << endl;
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
        
        //void setName        (const string& name);
        void prependName    (const string& prefix);
        
        // Get method(s).
        vector< IPlotMacro* > plots      () const;
        vector< TNtuple* >    ntuples ();
        
        
        // High-level management method(s).
        bool select (const T& obj) const;

        
    protected:
        
        // Low-level management method(s).
        void setBasePlots ();
        void write  ();

        
    private:
        
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