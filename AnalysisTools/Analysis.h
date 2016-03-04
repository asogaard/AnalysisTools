#ifndef AnalysisTools_Analysis_h
#define AnalysisTools_Analysis_h

/**
 * @file Analysis.h
 * @author Andreas Sogaard
 **/

// STL include(s).
#include <iostream>
#include <string>
#include <vector>
#include <memory> /* std::unique_ptr */

// xAOD include(s)
// ...

// ROOT include(s).
#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"

// Forward declaration(s).
namespace AnalysisTools {
    class ISelection;

    using SelectionsPtr = std::vector< ISelection* >;

}

// AnalysisTools include(s).
#include "AnalysisTools/ISelection.h"
//#include "AnalysisTools/Selection.h"

#include "AnalysisTools/PlotMacro1D.h"
#include "AnalysisTools/Range.h"

using namespace std;

namespace AnalysisTools {
    
    class Analysis {
        
    public:
        
        // Constructor(s).
        Analysis (const string &name) :
            m_name(name)
        {};
        
        // Destructor(s).
        ~Analysis () {
            closeOutput();
        };
        
        
    public:
 
        // Set method(s).
        // ...
        
        
        // Get method(s).
        string name () const;
        
        void clearSelections ();
        void addSelection (ISelection* selection);
        
        
        // High-level management method(s).
        void openOutput  (const string& filename);
        void closeOutput ();
        bool hasOutput   ();
        
        void run ();
        
        void save ();
        
        void grab (ISelection* selection);
        
        vector< TH1F* > histograms();
        
        
    private:

        string m_name;
        
        TDirectory* m_dir     = nullptr;
        TFile*      m_outfile = nullptr;

        SelectionsPtr m_selections;

        //vector< TH1F& > m_histograms;
        
    };

}

#endif