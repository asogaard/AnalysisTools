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
        
        PlotMacro1D (const std::string& name) :
            Localised(name)
        {};
        
	    PlotMacro1D (const std::string& name, const std::function< float(const T&) >& f) :
            Localised(name)
        {
            setFunction(f);
        };

        /*
        PlotMacro1D (const PlotMacro1D& other) :
            Localised(other.m_name, other.m_dir),
            m_function(other.m_function)
        {};
         */
        
        // Destructor(s).
        ~PlotMacro1D () {};
        
        
    public:
        
        // Set method(s).
        //void setVariable  (const std::string& variable);
        //void setUnit      (const std::string& unit);
        void setFunction  (const std::function< float(const T&) >& f);

        
        // Get method(s).
        const std::function< float(const T&) >& function () const { return m_function; }
        
        
        // High-level management method(s).
        //void fillDirectly (const float& value);
        void fill (const T& obj);
        void fill (      T& obj);
        
        
    protected:
        
        // Low-level management method(s).
        void   setTree (TTree* tree);
        TTree* tree    ();
        
        
    private:
        
	std::function< float(const T&) > m_function;
        
        
    };
    
}

#endif
