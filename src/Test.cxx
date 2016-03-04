// STL include(s).
#include <string>
#include <vector>
#include <iostream>

// ROOT include(s).
#include "TLorentzVector.h"
#include "TCanvas.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/Range.h"
#include "AnalysisTools/Cut.h"

#include "AnalysisTools/Analysis.h"
#include "AnalysisTools/ObjectDefinition.h"

using namespace std;
using namespace AnalysisTools;

int main (int argc, char* argv[]) {

    cout << "Running AnalysisTools test." << endl;

    vector<string> regions = {"SR", "CRZ", "CRT"};
    
    
    Analysis analysis ("BoostedWR");
    
    analysis.openOutput("output.root");
    

    // Pre-selection
    
    
    // Object definitions
    ObjectDefinition<TLorentzVector> Electrons ("Electrons");
    
    Electrons.setCategories( {"Loose", "Tight"} );
    
    // * pT
    Cut<TLorentzVector> el_eta;
    el_eta.setFunction( [](const TLorentzVector& p) { return p.Eta(); } );
    el_eta.setRange(-2.7, 2.7);
    Electrons.addCut(&el_eta);
    
    Cut<TLorentzVector> el_etaCrack;
    el_etaCrack.setFunction( [](const TLorentzVector& p) { return fabs(p.Eta()); } );
    el_etaCrack.addRange(0, 1.37);
    el_etaCrack.addRange(1.52, inf);
    Electrons.addCut(&el_etaCrack);
    
    Cut<TLorentzVector> el_pT;
    el_pT.setFunction( [](const TLorentzVector& p) { return p.Pt(); } );
    el_pT.setRange(7, inf);
    Electrons.addCut(&el_pT, "Loose");

    el_pT.setRange(15, inf);
    Electrons.addCut(&el_pT, "Tight");
    
    analysis.addSelection(&Electrons);
    

    // Event selection
    //Selection;
    //eventSelection.setCategories( regions );
    

    
    analysis.run();
    // - Initialise all Selections
    // --> Initialise all Cuts
    // ----> Initialise all Bookkeepers
    
    
    analysis.save();
    

    cout << "Done." << endl;
    
    return 1;
}

