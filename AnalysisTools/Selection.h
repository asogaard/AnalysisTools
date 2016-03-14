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
#include "TH1F.h"

// AnalysisTools include(s).
#include "AnalysisTools/ISelection.h"
#include "AnalysisTools/ICut.h"
#include "AnalysisTools/Localised.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Cut.h"

using namespace std;

namespace AnalysisTools {
    
    template <class T, class U>
    class Selection : public ISelection , public Localised {
        
        /**
         * Base class for all selection-type objects: Pre-selection, object definition, event selection, and possibly others.
        **/
        
    public:
        
        // Constructor(s).
        Selection (const string& name) :
            Localised(name)
        {};
        
        Selection (const string& name, const vector<string>& categories) :
            Localised(name)
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
        // ...
        
        
        // Get method(s).
        unsigned               nCategories      ();
        vector<string>         categories       ();
        bool                   categoriesLocked ();
        map< string, vector< Cut<U>* > > cuts             ();
        
        
        // High-level management method(s).
        void addCategory     (const string& category);
        void addCategories   (const vector<string>& categories);
        void setCategories   (const vector<string>& categories);
        void clearCategories ();
        
        void addCut (const Cut<U>& cut);
        void addCut (const Cut<U>& cut, const string& category);

        void addPlot (CutPosition pos, const PlotMacro1D<U>& plot);
        
        void setInput (const vector<T>* candidates);
        void setInput (const vector<T>  candidates);
        /* @TODO: Only applicable to ObjectDefinition? */
        
        //template<class U>
        void addInfo (const string& name, const vector<float> * info);
        void addInfo (const string& name, const vector<int>   * info);
        void addInfo (const string& name, const vector<bool>  * info);
        /* @TODO: Do proper templating? */
        /* @TODO: Versions without vectors for EventSelection? */
        
        template <class W>
        const vector<W>* info (const string& name);
        
        virtual bool run () = 0; /* No implementation. */
        
        vector< ICut* > cuts       (const string& category);
        vector< ICut* > listCuts   ();
        
        
    protected:
        
        // Low-level management method(s).
        bool hasCategory      (const string& category);
        bool canAddCategories ();
        void lockCategories   ();
        bool hasCutflow       (const string& category);
        void setupCutflow     (const string& category);
        
        
    protected:
        
        const vector<T>* m_input = nullptr; // Not separated by category names.
        /* @TODO: Only applicable to ObjectDefinition? */
        
        map<string, const vector<float>* > m_infoFloat;
        map<string, const vector<int>* >   m_infoInt;
        map<string, const vector<bool>* >  m_infoBool;
        
        vector<string> m_categories;
        bool           m_categoriesLocked = false;
        
        map< string, vector< Cut<U>* > > m_cuts;
        
        map< string, TH1F*> m_cutflow;
        
        bool m_hasRun = false;
        
    };
    
    template <class T, class U>
    using Selections = vector< Selection<T, U> >;

    using SelectionsPtr      = vector< ISelection* >;
    
}

#endif