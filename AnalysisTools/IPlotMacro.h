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
#include "TNtuple.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "ILocalised.h"

using namespace std;

namespace AnalysisTools {
    
    class IPlotMacro : virtual public ILocalised {
        
        /**
         * Base interface class for all PlotMacro-type objects.
        **/
        
        friend class ICut;
        
    public:
        
        IPlotMacro () {};
        virtual ~IPlotMacro (){};
        
    
    public:
        
        // Set method(s).
        // ..

        // Get method(s).
        virtual TNtuple* ntuple () = 0;
        
        // High-level management method(s).
        // ...
        
        
    public:
        
        // Low-level management method(s).
        virtual void write  ()                = 0;
                
    };
 
}

#endif