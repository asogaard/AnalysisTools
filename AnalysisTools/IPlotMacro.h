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
#include <cassert>
#include <memory> /* std::unique_ptr */

// ROOT include(s).
#include "TDirectory.h"
#include "TTree.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "ILocalised.h"

using namespace std;

namespace AnalysisTools {
    
    class IPlotMacro : virtual public ILocalised {
        
        /**
         * Base interface class for all PlotMacro-type objects.
         */
        
        friend class ICut;
        
    public:
        
        IPlotMacro () {};
        virtual ~IPlotMacro () {};
        
    
    public:
        
        // Set method(s).
        // ...

        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        // ...
	inline void fillDirectly (const double& value) { assert( m_tree ); m_value = value; }
        
        
    public:
        
        // Low-level management method(s).
        virtual void   setTree (TTree* tree) = 0;
        virtual TTree* tree    ()            = 0;

        
    protected:
        
        double m_value;
        TTree* m_tree = nullptr;

        
    };
 
}

#endif
