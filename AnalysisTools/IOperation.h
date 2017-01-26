#ifndef AnalysisTools_IOperation_h
#define AnalysisTools_IOperation_h

/**
 * @file IOperation.h
 * @author Andreas Sogaard
 */

// STL include(s).
#include <iostream>
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
#include "AnalysisTools/IPlotMacro.h"
#include "AnalysisTools/ILocalised.h"

using namespace std;

namespace AnalysisTools {
    
    enum class CutPosition { Pre, Post };
    
    class IOperation : virtual public ILocalised {
        
        /**
         * Base interface class for all operation-type objects.
        **/
        
        friend class ISelection;
        
    public:
        
        //IOperation () {};
        virtual ~IOperation () {};

        
    public:
        
        // Set method(s).
        // ...
        
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        virtual std::vector< IPlotMacro* > plots (const CutPosition& pos) const = 0;
        virtual std::vector< IPlotMacro* > plots ()                       const = 0;
        
	virtual void print () const = 0;
	

    public:
        
        // Low-level management method(s).
        // ...
        
        
    protected:
        
        /// Data members.
        map< CutPosition, std::vector< std::unique_ptr<IPlotMacro> > > m_plots;/* = {
	  {CutPosition::Pre,  std::vector< std::unique_ptr<IPlotMacro> >()},
	  {CutPosition::Post, std::vector< std::unique_ptr<IPlotMacro> >()}
	  };*/
        
        map< CutPosition, std::unique_ptr<TTree> > m_trees;/* = {
            {CutPosition::Pre,  nullptr},
            {CutPosition::Post, nullptr}
	    };*/
        
        bool m_initialised = false;
        
    };
    
    //using OperationsPtr = std::vector< IOperation* >;
    using OperationPtrs = std::vector< std::unique_ptr<IOperation> >;
 
}

#endif
