#ifndef AnalysisTools_ISelection_h
#define AnalysisTools_ISelection_h

/**
 * @file ISelection.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <memory> /* std::unique_ptr */

// ROOT include(s).
#include "TNtuple.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/ICut.h"
#include "AnalysisTools/ILocalised.h"

using namespace std;

namespace AnalysisTools {
    
    class ISelection : virtual ILocalised {
        
        /**
         * Base interface class for all selection-type objects: Pre-selection, object definition, event selection, and possibly others.
        **/
        
        friend class Analysis;
        
    public:
        
        ISelection() {};
        virtual ~ISelection (){};
        
    
    public:
        
        // Set method(s).
        // ...
        
        // Get method(s).
        virtual vector< string > categories () = 0;
        
        // High-level management method(s).
        virtual void run () = 0;
        
        virtual vector< TNtuple* > ntuples () = 0; // @TODO: Should parent classes have access to children class PlotMacros?
        virtual vector< ICut* > listCuts   () = 0;
        virtual vector< ICut* > cuts       (const string& category) = 0;
        
        
    protected:
        
        // Low-level management method(s).
        virtual void write  () = 0;
        
    };
 
}

#endif