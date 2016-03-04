#ifndef AnalysisTools_Selection_h
#define AnalysisTools_Selection_h

/**
 * @file Selection.h
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
#include "TLorentzVector.h"

// AnalysisTools include(s).
#include "AnalysisTools/ISelection.h"
#include "AnalysisTools/ICut.h"
#include "AnalysisTools/Cut.h"

using namespace std;

namespace AnalysisTools {
    
    template <class T>
    class Selection : public ISelection {
        
        /**
         * Base class for all selection-type objects: Pre-selection, object definition, event selection, and possibly others.
        **/
        
    public:
        
        // Constructor(s).
        Selection (const string& name) :
            m_name(name)
        {};
        
        // Destructor(s).
        ~Selection () {};
        
        
    public:
        
        // Set method(s).
        void setName    (const string& name);
        
        
        // Get method(s).
        bool locked ();
        
        string name    ();
        string basedir ();
        
        unsigned               nCategories      ();
        vector<string>         categories       ();
        bool                   categoriesLocked ();
        map< string, CutsPtr > cuts             ();
        
        
        // High-level management method(s).
        void addCategory     (const string& category);
        void addCategories   (const vector<string>& categories);
        void setCategories   (const vector<string>& categories);
        void clearCategories ();
        
        void addCut (ICut* cut);
        void addCut (ICut* cut, const string& category);
        
        virtual void run () {};
        
        vector< TH1F* > histograms ();
        CutsPtr         listCuts   ();
        
        
    protected:
        
        // Low-level management method(s).
        void setDir     (TDirectory* dir);
        
        bool hasCategory      (const string& category);
        bool canAddCategories ();
        void lockCategories   ();
        
        void lock ();
        
        void grab (ICut* cut);
        
        void write ();
        
        
    protected:
        
        bool m_locked = false;
        
        string m_name    = "";
        
        TDirectory* m_dir = nullptr;
        
        vector<string> m_categories;
        bool           m_categoriesLocked = false;
        
        map< string, CutsPtr > m_cuts;
        
        bool m_hasRun = false;
        
    };
    
    template <class T>
    using Selections = vector< Selection<T> >;

    using SelectionsPtr = vector< ISelection* >;
    
}

#endif