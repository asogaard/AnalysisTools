#ifndef AnalysisTools_Selection_h
#define AnalysisTools_Selection_h

/**
 * @file Selection.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <memory> /* std::unique_ptr */
#include <utility> /* std::make_pair */
#include <regex>
#include <iterator> /* std::advance */

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
    class Selection : public ISelection, public Localised {
        
        /**
         * Base class for all selection-type objects: Pre-selection, object definition, event selection, and possibly others.
	 */
        
    public:
        
        // Constructor(s).
        Selection (const string& name) :
            Localised(name)
	{};
        
        Selection (const string& name, const vector<string>& categories) :
            Selection(name)
	{ 
	  addCategories(categories); 
	}

	Selection (const Selection<T,U>& other) :
	    Selection(other.m_name)
	{
	  this->m_weight   = other.m_weight;
	  this->m_children = {};
	  this->m_dir      = nullptr;
	  this->m_parent   = nullptr;

	  addCategories(other.m_categories);
	  for (const auto& category : m_categories) {

	    // Adding cuts (thereby copying them, instead of just copying the pointer).
	    for (IOperation* iop : other.operations(category)) {
	      if        (const Cut<U>*       cut = dynamic_cast< const Cut<U>* >      (iop)) {
		addCut(*cut, category, false);
	      } else if (const Operation<U>* op  = dynamic_cast< const Operation<U>* >(iop)) {
		addOperation(*op, category, false);
	      } else {
		WARNING("Couldn't cast IOperation '%s'.", iop->name().c_str());
	      }
	    }

	  }
	};

        // Destructor(s).
	~Selection () {};
        
        
    public:
        
        // Set method(s).
        void setWeight (const float* weight);
        
        void addCategory     (const string& category);
        void addCategories   (const vector<string>& categories);
        void setCategories   (const vector<string>& categories);
        void clearCategories ();
        
        void addCut (const Cut<U>& cut);
        void addCut (const Cut<U>& cut,  const string& pattern, const bool& common = false);
        void addCut (const string& name, const function< double(const U&) >& f);
        void addCut (const string& name, const function< double(const U&) >& f, const string& category);
        void addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max);
        void addCut (const string& name, const function< double(const U&) >& f, const double& min, const double& max, const string& category);

	void removeOperation (const unsigned& loc);
	void removeOperation (const unsigned& loc,  const string& category);
	void removeOperation (const string&   name);
	void removeOperation (const string&   name, const string& category);

	void removeCut (const unsigned& loc);
	void removeCut (const unsigned& loc,  const string& category);
	void removeCut (const string&   name);
	void removeCut (const string&   name, const string& category);

        void addOperation (const Operation<U>& operation);
        void addOperation (const Operation<U>& operation, const string& pattern, const bool& common = false);
        void addOperation (const string& name, const function< double(U&) >& f);
        void addOperation (const string& name, const function< double(U&) >& f, const string& category);

        void addPlot (const CutPosition& pos, const PlotMacro1D<U>& plot);
        
        
        // Get method(s).
        unsigned int     nCategories      ();
        vector< string > categories       ();
        bool             categoriesLocked ();
        
	Cut<U>* cut (const unsigned& loc,  const string& category = "Nominal"); 
	Cut<U>* cut (const string&   name, const string& category = "Nominal"); 

	IOperation* operation (const unsigned& loc,  const string& category = "Nominal"); 
	IOperation* operation (const string&   name, const string& category = "Nominal"); 

        virtual std::vector<IOperation*> operations    (const string& category) const;
        virtual std::vector<IOperation*> allOperations ();
        virtual TH1F*                    cutflow       (const string& category);
	
        bool hasRun ();

        
        // High-level management method(s).
        virtual bool run () = 0; /* No implementation. */
        
        
    protected:
        
        // Low-level management method(s).
        bool hasCategory      (const string& category) const;
        bool canAddCategories ()                       const;
        void lockCategories   ();
        bool hasCutflow       (const string& category) const ;
        void setupCutflow     (const string& category);
       
        
    protected:

        int  m_branch = -1;
        //bool m_hasRun = false;

        //const float* m_weight = nullptr;

    };
    
    template <class T, class U>
    using Selections = vector< Selection<T, U> >;
    
}

#endif
