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
#include "TNtuple.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/IPlotMacro.h"
#include "AnalysisTools/ILocalised.h"

using namespace std;

namespace AnalysisTools {
    
    class ICut : virtual public ILocalised{
        
        /**
         * Base interface class for all cut-type objects.
        **/
        
        friend class ISelection;
        
    public:
        
        ICut() {};
        virtual ~ICut (){};
        
    public:
        
        // Set method(s).
        // ...
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        virtual vector< TNtuple* > ntuples  () = 0;
        virtual vector< IPlotMacro* > plots () const = 0;
        
        //virtual bool select (const TLorentzVector& p) const = 0;
        
        
    public:
        
        // Low-level management method(s).
        virtual void write   ()                 = 0;
        
    };
 
}

#endif