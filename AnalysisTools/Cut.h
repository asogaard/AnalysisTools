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
#include "AnalysisTools/ICut.h"
#include "AnalysisTools/Localised.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Range.h"
#include "AnalysisTools/PlotMacro1D.h"

using namespace std;

namespace AnalysisTools {
    
    enum class CutPosition { Pre, Post};
    
    template <class T>
    class Cut : public ICut, public Localised {

        friend class Selection<TLorentzVector, T>;
        friend class Selection<T, AnalysisTools::PhysicsObject>;
        friend class Selection<T, AnalysisTools::Event>;
        friend class Selection<T, double>;
        friend class Selection<T, float>;
        friend class Selection<T, bool>;
        friend class Selection<T, int>;
    
        /* *
         * @TODO: Implement 'addPlot(CutPosition, PlotMacro1D)' function.
         */
        
        
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
        ~Cut () {
            for (auto pos_tree : m_trees) {
                TTree* tree = pos_tree.second;
                if (tree) {
                    delete tree;
                    tree = nullptr;
                }
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
        void addPlot (CutPosition pos, IPlotMacro* plot);
        
        void prependName    (const string& prefix); // @asogaard: Move to Localised?
        
        // Get method(s).
        vector< IPlotMacro* > plots () const;
        vector< IPlotMacro* > plots (const CutPosition& pos) const;
        
        
        // High-level management method(s).
        bool select (const T& obj);

        
    protected:
        
        // Low-level management method(s).
        void init  ();
        void write ();

        
    private:
        
        unsigned m_nBins    = 10;
        double   m_axisDown =  0.;
        double   m_axisUp   =  1.;

        function< double(T) > m_function;
        Ranges m_ranges;
        
        string m_variable = "";
        string m_unit     = "";
        
        map<CutPosition, vector< IPlotMacro* > > m_plots =  { {CutPosition::Pre, vector< IPlotMacro* >()}, {CutPosition::Post, vector< IPlotMacro* >()} };
;
        
        map<CutPosition, TTree*> m_trees = { {CutPosition::Pre, nullptr}, {CutPosition::Post, nullptr} };
        
        bool m_initialised = false;
    };
 
    template <class T>
    using Cuts = vector< Cut<T> >;
 
    using CutsPtr = vector< ICut* >;
    
}

#endif