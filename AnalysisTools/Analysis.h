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
#include <ctime> /* std::clock_t */
#include <iomanip> /* std::setprecision */
#include <cstdio> /* printf */


// xAOD include(s)
// ...

// ROOT include(s).
#include "TDirectory.h"
#include "TFile.h"
#include "TNtuple.h"

// Forward declaration(s).
namespace AnalysisTools {
    class ISelection;

    using SelectionsPtr = std::vector< ISelection* >;

}

// AnalysisTools include(s).
#include "AnalysisTools/ISelection.h"
#include "AnalysisTools/Localised.h"
//#include "AnalysisTools/Selection.h"

#include "AnalysisTools/PlotMacro1D.h"
#include "AnalysisTools/Range.h"

using namespace std;

namespace AnalysisTools {
    
    class Analysis : public Localised {
        
    public:
        
        // Constructor(s).
        Analysis (const string &name) :
            Localised(name)
        {};
        
        // Destructor(s).
        ~Analysis () {
            closeOutput();
        };
        
        
    public:
 
        // Set method(s).
        void addSelection    (ISelection* selection);
        void addTree         (const string& name = "outputTree");
        void setWeight       (const float* w);
        
        
        // Get method(s).
        void   clearSelections ();
        TTree* tree ();
        TFile* file ();
        void   writeTree ();
        
        // High-level management method(s).
        void openOutput  (const string& filename);
        void closeOutput ();
        bool hasOutput   ();
        
        bool run (const unsigned& current, const unsigned& maximum, const int& DSID);
        bool run (const unsigned& current, const unsigned& maximum);
        bool run ();
        
        void save ();
        
        
    private:

        TFile* m_outfile = nullptr;
        TTree* m_outtree = nullptr;

        const float* m_weight = nullptr;
        
        SelectionsPtr m_selections;
        
        std::clock_t m_start;
        
    };

}

#endif