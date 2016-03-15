#ifndef AnalysisTools_IOperation_h
#define AnalysisTools_IOperation_h

/**
 * @file IOperation.h
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
#include "TTree.h"

// Forward declaration(s).
// ...

// AnalysisTools include(s).
#include "AnalysisTools/IPlotMacro.h"
#include "AnalysisTools/ILocalised.h"

using namespace std;

namespace AnalysisTools {
    
    enum class CutPosition { Pre, Post};
    
    class IOperation : virtual public ILocalised{
        
        /**
         * Base interface class for all operation-type objects.
        **/
        
        friend class ISelection;
        
    public:
        
        IOperation() {};
        ~IOperation () {
            for (auto p : m_trees) {
                TTree* tree = p.second;
                if (tree) {
                    delete tree;
                    tree = nullptr;
                }
            }
        };
        
        
    public:
        
        // Set method(s).
        // ...
        
        
        // Get method(s).
        // ...
        
        
        // High-level management method(s).
        virtual vector< IPlotMacro* > plots (const CutPosition& pos) const = 0;
        virtual vector< IPlotMacro* > plots ()                       const = 0;
        
        
    public:
        
        // Low-level management method(s).
        // ...
        
        
    protected:
        
        /// Data members.
        map<CutPosition, vector< IPlotMacro* > > m_plots =  {
            {CutPosition::Pre,  vector< IPlotMacro* >()},
            {CutPosition::Post, vector< IPlotMacro* >()}
        };
        
        map<CutPosition, TTree*> m_trees = {
            {CutPosition::Pre,  nullptr},
            {CutPosition::Post, nullptr}
        };
        
        bool m_initialised = false;
        
    };
    
    using OperationsPtr = vector< IOperation* >;
 
}

#endif