#ifndef AnalysisTools_Operation_h
#define AnalysisTools_Operation_h

/**
 * @file Operation.h
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
    class Operation : public IOperation, public Localised {

        friend class Selection<TLorentzVector, T>;
        friend class Selection<T, AnalysisTools::PhysicsObject>;
        friend class Selection<T, AnalysisTools::Event>;
        friend class Selection<T, double>;
        friend class Selection<T, float>;
        friend class Selection<T, bool>;
        friend class Selection<T, int>;
    
        
    public:

        // Constructor(s).
        Operation () :
            Localised("Operation")
        {};
        
        Operation (const string& name) :
            Localised(name)
        {};
        
        
        // Destructor(s).
        ~Operation () {};
        
        
    public:
        
        // Set method(s).
        void setFunction (const function< double(T&) >& f);
        
        void clearPlots ();
        void addPlot    (const CutPosition& pos, const IPlotMacro& plot);
        
        // Get method(s).
        vector< IPlotMacro* > plots (const CutPosition& pos) const;
        vector< IPlotMacro* > plots ()                       const;
        
        
        // High-level management method(s).
        bool apply (T& obj);

        
    protected:
        
        // Low-level management method(s).
        void init  ();

        
    private:
        
        function< double(T&) > m_function;
        Ranges m_ranges;
        
        string m_variable = "";
        string m_unit     = "";
        
    };
 
    template <class T>
    using Operations = vector< Operation<T> >;
    
}

#endif