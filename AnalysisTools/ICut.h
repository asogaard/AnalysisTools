#ifndef AnalysisTools_ICut_h
#define AnalysisTools_ICut_h

/**
 * @file ICut.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <memory> /* std::unique_ptr */

// ROOT include(s).
#include "TDirectory.h"
#include "TH1.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/IPlotMacro.h"

using namespace std;

namespace AnalysisTools {
    
    class ICut {
        
        /**
         * Base interface class for all cut-type objects.
        **/
        
        friend class ISelection;
        
    public:
        
        ICut() {};
        virtual ~ICut (){};
        
    public:
        
        // Set method(s).
        virtual void setName (const string& name) = 0;
        
        
        // Get method(s).
        virtual string name () const = 0;
        
        //virtual vector< IPlotMacro* > plots () const = 0;
        
        
        // High-level management method(s).
        virtual vector< TH1F* > histograms  () = 0;
        virtual vector< IPlotMacro* > plots () = 0;
        
        //virtual bool select (const TLorentzVector& p) const = 0;
        
        
    public:
        
        // Low-level management method(s).
        virtual void setDir  (TDirectory* dir)  = 0;
        virtual void grab    (IPlotMacro* plot) = 0;
        virtual void write   ()                 = 0;
        
        
    protected:
        
        string      m_name = "";
        TDirectory* m_dir  = nullptr;
        
    };
 
}

#endif