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
#include "AnalysisTools/IOperation.h"
#include "AnalysisTools/Localised.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Cut.h"
#include "AnalysisTools/Operation.h"

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
            cout << "<Selection::~Selection>" << endl;
            for (auto opvec : m_operations) {
                for (auto op : opvec.second) {
                    if (op) {
                        delete op;
                        op = nullptr;
                    }
                }
            }
            cout << "<Selection::~Selection> Done." << endl;
        };
        
        
    public:
        
        // Set method(s).
        // ...
        
        
        // Get method(s).
        unsigned int     nCategories      ();
        vector< string > categories       ();
        bool             categoriesLocked ();
        
        OperationsPtr operations    (const string& category);
        OperationsPtr allOperations ();
        TH1F*         cutflow       (const string& category);
        
        bool hasRun ();

        
        // High-level management method(s).
        void addCategory     (const string& category);
        void addCategories   (const vector<string>& categories);
        void setCategories   (const vector<string>& categories);
        void clearCategories ();
        
        void addCut (const Cut<U>& cut);
        void addCut (const Cut<U>& cut, const string& category);
        void addCut (const string& name, const function< double(const U&) >& f);
        void addCut (const string& name, const function< double(const U&) >& f, const string& category);
        void addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max);
        void addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max, const string& category);
        
        void addOperation (const Operation<U>& operation);
        void addOperation (const Operation<U>& operation, const string& category);
        
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
                
    };
    
    template <class T, class U>
    using Selections = vector< Selection<T, U> >;
    
}

#endif