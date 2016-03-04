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
#include "TH1.h"

// Forward declaration(s).
namespace  AnalysisTools {
    template <class T>
    class Cut;
    
    template <class T>
    class Selection;
}

// AnalysisTools include(s).
#include "AnalysisTools/IPlotMacro.h"
#include "AnalysisTools/Utilities.h"

using namespace std;

namespace AnalysisTools {

    template <class T>
    class PlotMacro1D : public IPlotMacro {
        
        friend class Cut<T>;
        friend class Selection<T>;
        
    public:

        // Constructor(s).
        PlotMacro1D () {};
        PlotMacro1D (const string& name) {
            setName(name);
        };
        
        // Destructor(s).
        ~PlotMacro1D () {};
        
        
    public:
        
        // Set method(s).
        void setName      (const string& name);
        void setHistogram (const TH1& hist);
        
        void setVariable  (const string& variable);
        void setUnit      (const string& unit);
        
        void setFunction  (function< double(T) > f);

        
        // Get method(s).
        string name () const;
        TH1F* histogram ();
        TDirectory* dir ();
        
        
        // High-level management method(s).
        void fill (const T& obj);
        void fill (const T& obj, const double& weight);
        
        
    protected:
        
        // Low-level management method(s).
        void setDir (TDirectory* dir);
        void write ();
        
        
    private:
        
        string m_name     = "";
        string m_basedir  = "";
        string m_variable = "";
        string m_unit     = "";
        
        TDirectory* m_dir = nullptr;
        
        TH1F m_hist;
        
        function< double(T) > m_function;
        
    };
    
}

#endif