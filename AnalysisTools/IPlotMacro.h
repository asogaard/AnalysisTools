#ifndef AnalysisTools_IPlotMacro_h
#define AnalysisTools_IPlotMacro_h

/**
 * @file IPlotMacro.h
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
// ..

using namespace std;

namespace AnalysisTools {
    
    class IPlotMacro {
        
        /**
         * Base interface class for all PlotMacro-type objects.
        **/
        
        friend class ICut;
        
    public:
        
        IPlotMacro () {};
        virtual ~IPlotMacro (){};
        
    
    public:
        
        // Set method(s).
        virtual void setName (const string& name) = 0;
        
        
        // Get method(s).
        virtual string name () const = 0;
        
        virtual TH1F* histogram () = 0;
        
        virtual TDirectory* dir () = 0;
        
        
        // High-level management method(s).
        // ...
        
        
    public:
        
        // Low-level management method(s).
        virtual void setDir (TDirectory* dir) = 0;
        virtual void write  ()                = 0;
        
        
    protected:
        
        string      m_name = "";
        TDirectory* m_dir  = nullptr;
        
    };
 
}

#endif