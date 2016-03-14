#ifndef AnalysisTools_PlotMacro1D_h
#define AnalysisTools_PlotMacro1D_h

/**
 * @file PlotMacro1D.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <iostream>
#include <string>
#include <vector>
#include <functional> /* std::function */
#include <assert.h> /* assert */

// ROOT include(s).
#include "TDirectory.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TBranch.h"

// Forward declaration(s).
namespace  AnalysisTools {
    template <class T>
    class Cut;
    
    template <class T, class U>
    class Selection;
}

// AnalysisTools include(s).
#include "AnalysisTools/IPlotMacro.h"
#include "AnalysisTools/Localised.h"
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"

using namespace std;

namespace AnalysisTools {

    template <class T>
    class PlotMacro1D : public IPlotMacro, public Localised {
        
        friend class Cut<T>;
        friend class Selection<T, AnalysisTools::PhysicsObject>;
        friend class Selection<T, AnalysisTools::Event>;
        friend class Selection<T, double>;
        friend class Selection<T, float>;
        friend class Selection<T, bool>;
        friend class Selection<T, int>;
        
    public:

        // Constructor(s).
        PlotMacro1D () :
            Localised()
        {};
        
        PlotMacro1D (const string& name) :
            Localised(name)
        {};
        
        // Destructor(s).
        ~PlotMacro1D () {};
        
        
    public:
        
        // Set method(s).
        //void setVariable  (const string& variable);
        //void setUnit      (const string& unit);
        
        void setFunction  (function< double(T) > f);

        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        void fill (const T& obj);
        
        
    protected:
        
        // Low-level management method(s).
        void   setTree (TTree* tree);
        TTree* tree    ();
        
        
    private:
        
        function< double(T) > m_function;
        
        
    };
    
}

#endif