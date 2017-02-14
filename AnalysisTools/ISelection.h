#ifndef AnalysisTools_ISelection_h
#define AnalysisTools_ISelection_h

/**
 * @file ISelection.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <memory> /* std::unique_ptr, std::shared_ptr */

// ROOT include(s).
#include "TH1.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/IOperation.h"
#include "AnalysisTools/Localised.h"
#include "AnalysisTools/ValuesCache.h"

using namespace std;

namespace AnalysisTools {
    
  class ISelection : virtual public ILocalised {
        
        /**
         * Base interface class for all selection-type objects: Pre-selection, object definition, event selection, and possibly others.
        **/
        
        friend class Analysis;
        
    public:
        
        //virtual  ISelection () = 0;
        virtual ~ISelection () {};


    public:
        
        // Set method(s).
        virtual void setWeight (const float* weight) = 0;
	virtual bool required () const = 0;
        
        // Get method(s).
        virtual vector< string > categories       () = 0;
        virtual bool             categoriesLocked () = 0;
        
        virtual std::vector<IOperation*> operations    (const string& category) const = 0;
        virtual std::vector<IOperation*> allOperations () = 0;
        virtual TH1F*                    cutflow       (const string& category) = 0;
        
        virtual bool hasRun () = 0;

	inline ValuesCache* valuesCache () { return &m_valuesCache; }
	inline const bool& performCaching () const { return m_performCaching; }
        
        // High-level management method(s).
        virtual bool run () = 0;

	virtual void print () const = 0;
        
    protected:
        
        // Low-level management method(s).
        // ...
        
	
    protected:
     
	/// Data member(s).
	std::vector< std::string > m_categories;
        bool m_categoriesLocked = false;
        
        map< string, std::vector< std::unique_ptr<IOperation> > > m_operations;
        map< string, std::unique_ptr<TH1F> > m_cutflow;
        
        bool m_hasRun = false;

	const float* m_weight = nullptr;

	bool m_required = false; // Whether the analysis will stop if this selection isn't passed

	/**
	 * Cache for storing the function return-values used e.g. in PlotMacro1D. This is based on the idea that all plottin-macros with name 'xyz' within the same selection will (hopefully!) return the same value, which means that we only need to evaluate it once.	 
	 */
	ValuesCache m_valuesCache;
	bool m_performCaching = false;

    };
    
    //using SelectionsPtr = std::vector< std::unique_ptr<ISelection> >;
    //using SelectionsPtr = std::vector< ISelection* >;
 
}

#endif
