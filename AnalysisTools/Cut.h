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
#include "TTree.h"
#include "TLorentzVector.h"

// Forward declaration(s).
namespace  AnalysisTools {
    template <class T, class U>
    class Selection;
}

// AnalysisTools include(s).
#include "AnalysisTools/IOperation.h"
#include "AnalysisTools/Localised.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Range.h"
#include "AnalysisTools/PlotMacro1D.h"

using namespace std;

namespace AnalysisTools {
    
    template <class T>
    class Cut : public IOperation, public Localised {

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
        {};
        
        Cut (const string& name) :
            Localised(name)
        {};
        
        Cut (const Cut<T>& other) :
            Localised(other.m_name, other.m_dir),
            m_function(other.m_function),
            m_ranges(other.m_ranges)
        {};

        
        // Destructor(s).
        ~Cut () {};
        
        
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
        void addRange (const double& value);
        
        void addRanges (const Ranges& ranges);
        void addRanges (const vector< pair<double, double> >& vec_limits);
        
        void setFunction (const function< double(const T&) >& f);
        
        void clearPlots ();
        void addPlot    (const CutPosition& pos, IPlotMacro* plot);
        
        
        // Get method(s).
        vector< IPlotMacro* > plots (const CutPosition& pos) const;
        vector< IPlotMacro* > plots ()                       const;
        
        
        // High-level management method(s).
        bool apply (const T& obj); 

        
    protected:
        
        // Low-level management method(s).
        void init  ();
        void write ();

        
    private:
        
        function< double(const T&) > m_function;
        Ranges m_ranges;
        
        string m_variable = "";
        string m_unit     = "";

    };
 
    template <class T>
    using Cuts = vector< Cut<T> >;
    
}

#endif