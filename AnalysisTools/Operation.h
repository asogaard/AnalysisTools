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
	    Operation("Operation")
        {};
        
        Operation (const string& name) :
            Localised(name)
        {
	  // Can't use initialiser lists with unique_ptr.
	  this->m_plots.clear();
	  this->m_plots[CutPosition::Pre]  = std::vector< std::unique_ptr<IPlotMacro> >();
          this->m_plots[CutPosition::Post] = std::vector< std::unique_ptr<IPlotMacro> >();
	};

	Operation (const string& name, const function< double(T&) >& f) :
            Operation(name)
	{
	    m_function = f;
	};
	    
	Operation (const Operation<T>& other) :
            Operation(other.m_name)
        {
	  this->m_initialised = false;
	  this->m_trees.clear();
	  	  
	  this->m_function = other.m_function;
	  this->m_ranges   = other.m_ranges;
	  
	  // Copy plotting macros.
	  for (const auto& pos : { CutPosition::Pre, CutPosition::Post }) {
	    for (IPlotMacro* plot : other.plots(pos)) {
	      addPlot(pos, *plot);
	    }
	  }
	};
        
        
        // Destructor(s).
        ~Operation () {};
        
        
    public:
        
        // Set method(s).
        void setFunction (const function< double(T&) >& f);
        
        void clearPlots ();
        void addPlot    (const CutPosition& pos, const IPlotMacro& plot);
        
        // Get method(s).
	virtual std::vector< IPlotMacro* > plots (const CutPosition& pos) const;
        virtual std::vector< IPlotMacro* > plots ()                       const;
        
	virtual void print () const;
        
        // High-level management method(s).
        bool apply (T& obj, const float& w = 1.);

        
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
