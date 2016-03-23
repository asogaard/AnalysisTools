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
#include "TH1.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/IOperation.h"
#include "AnalysisTools/ILocalised.h"

using namespace std;

namespace AnalysisTools {
    
    class ISelection : virtual public ILocalised {
        
        /**
         * Base interface class for all selection-type objects: Pre-selection, object definition, event selection, and possibly others.
        **/
        
        friend class Analysis;
        
    public:
        
        ISelection() {};
        virtual ~ISelection () {};
        
    
    public:
        
        // Set method(s).
        virtual void setWeight (const float* weight) = 0;
        
        
        // Get method(s).
        virtual vector< string > categories       () = 0;
        virtual bool             categoriesLocked () = 0;
        
        virtual OperationsPtr operations    (const string& category) = 0;
        virtual OperationsPtr allOperations () = 0;
        virtual TH1F*         cutflow       (const string& category) = 0;
        
        virtual bool hasRun () = 0;
        
        
        // High-level management method(s).
        virtual bool run () = 0;
        
        
    protected:
        
        // Low-level management method(s).
        // ...
        
        
    protected:
        
        vector< string > m_categories;
        bool             m_categoriesLocked = false;
        
        map< string, OperationsPtr > m_operations;
        map< string, TH1F* >         m_cutflow;
        
        bool m_hasRun = false;

        
    };
    
    using SelectionsPtr      = vector< ISelection* >;
 
}

#endif