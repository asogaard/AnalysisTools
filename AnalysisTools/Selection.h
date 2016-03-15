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
            for (auto category : m_categories) {
                for (auto op : m_operations[category]) {
                    if (op) {
                        delete op;
                        op = nullptr;
                    }
                }
                if (m_cutflow[category]) {
                    delete m_cutflow[category];
                }
            }
        };
        
        
    public:
        
        // Set method(s).
        void addCategory     (const string& category);
        void addCategories   (const vector<string>& categories);
        void setCategories   (const vector<string>& categories);
        void clearCategories ();
        
        void addCut (const Cut<U>& cut);
        void addCut (const Cut<U>& cut, const string& category, const bool& common = false);
        void addCut (const string& name, const function< double(const U&) >& f);
        void addCut (const string& name, const function< double(const U&) >& f, const string& category);
        void addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max);
        void addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max, const string& category);
        
        void addOperation (const Operation<U>& operation);
        void addOperation (const Operation<U>& operation, const string& category, const bool& common = false);
        void addOperation (const string& name, const function< double(U&) >& f);
        void addOperation (const string& name, const function< double(U&) >& f, const string& category);

        void addPlot (const CutPosition& pos, const PlotMacro1D<U>& plot);
        
        
        // Get method(s).
        unsigned int     nCategories      ();
        vector< string > categories       ();
        bool             categoriesLocked ();
        
        OperationsPtr operations    (const string& category);
        OperationsPtr allOperations ();
        TH1F*         cutflow       (const string& category);
        
        bool hasRun ();

        
        // High-level management method(s).
        virtual bool run () = 0; /* No implementation. */
        
        
    protected:
        
        // Low-level management method(s).
        bool hasCategory      (const string& category);
        bool canAddCategories ();
        void lockCategories   ();
        bool hasCutflow       (const string& category);
        void setupCutflow     (const string& category);
       
        
    protected:

        int  m_branch = -1;
        bool m_hasRun = false;
        
    };
    
    template <class T, class U>
    using Selections = vector< Selection<T, U> >;
    
}

#endif