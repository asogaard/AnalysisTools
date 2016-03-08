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
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Cut.h"

using namespace std;

namespace AnalysisTools {
    
    template <class T, class U>
    class Selection : public ISelection {
        
        /**
         * Base class for all selection-type objects: Pre-selection, object definition, event selection, and possibly others.
        **/
        
    public:
        
        // Constructor(s).
        Selection (const string& name) :
            m_name(name)
        {};
        
        Selection (const string& name, const vector<string>& categories) :
            m_name(name)
        {
            addCategories(categories);
        };
        
        // Destructor(s).
        ~Selection () {
            for (auto cutvec : m_cuts) {
                for (auto cut : cutvec.second) {
                    if (cut) {
                        delete cut;
                        cut = nullptr;
                    }
                }
            }
        };
        
        
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
        map< string, vector< Cut<U>* > > cuts             ();
        
        
        // High-level management method(s).
        void addCategory     (const string& category);
        void addCategories   (const vector<string>& categories);
        void setCategories   (const vector<string>& categories);
        void clearCategories ();
        
        void addCut (Cut<U>* cut);
        void addCut (Cut<U>* cut, const string& category);
        
        void setInput (const vector<T>* candidates);
        void setInput (const vector<T>  candidates);
        
        //template<class U>
        void addInfo (const string& name, const vector<float> * info);
        void addInfo (const string& name, const vector<int>   * info);
        void addInfo (const string& name, const vector<bool>  * info);
        
        template <class W>
        const vector<W>* info (const string& name);
        
        virtual void run () {};
        
        vector< TNtuple* > ntuples ();
        vector< ICut* > cuts       (const string& category);
        vector< ICut* > listCuts   ();
        
        
    protected:
        
        // Low-level management method(s).
        void setDir (TDirectory* dir);
        
        bool hasCategory      (const string& category);
        bool canAddCategories ();
        void lockCategories   ();
        
        void lock ();
        
        void grab (const string& category, ICut* cut);
        
        void write ();
        
        
    protected:
        
        bool m_locked = false;
        
        string m_name    = "";
        
        const vector<T>* m_input = nullptr; // Not separated by category names.
        
        map<string, const vector<float>* > m_infoFloat;
        map<string, const vector<int>* >   m_infoInt;
        map<string, const vector<bool>* >  m_infoBool;
        
        vector<string> m_categories;
        bool           m_categoriesLocked = false;
        
        map< string, vector< Cut<U>* > > m_cuts;
        
        bool m_hasRun = false;
        
    };
    
    template <class T, class U>
    using Selections = vector< Selection<T, U> >;

    using SelectionsPtr = vector< ISelection* >;
    
}

#endif