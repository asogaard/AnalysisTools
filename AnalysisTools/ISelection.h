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
#include "TDirectory.h"
#include "TNtuple.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/ICut.h"

using namespace std;

namespace AnalysisTools {
    
    class ISelection {
        
        /**
         * Base interface class for all selection-type objects: Pre-selection, object definition, event selection, and possibly others.
        **/
        
        friend class Analysis;
        
    public:
        
        ISelection() {};
        virtual ~ISelection (){};
        
    
    public:
        
        // Set method(s).
        virtual void setName    (const string& name) = 0;
        
        
        // Get method(s).
        virtual string name   () = 0;
        virtual bool   locked () = 0;
        virtual vector< string > categories () = 0;
        
        // High-level management method(s).
        virtual void run () = 0;
        
        virtual vector< TNtuple* > ntuples () = 0;
        virtual vector< ICut* > listCuts   () = 0;
        virtual vector< ICut* > cuts       (const string& category) = 0;
        
        
    protected:
        
        // Low-level management method(s).
        virtual void setDir (TDirectory* dir) = 0;
        virtual void grab   (const string& category, ICut* cut) = 0;
        virtual void lock   () = 0;
        virtual void write  () = 0;
        
        
    protected:
        
        string m_name    = "";
        
        TDirectory* m_dir = nullptr;
        
    };
 
}

#endif