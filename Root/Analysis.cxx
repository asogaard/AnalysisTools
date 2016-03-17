// STL include(s).
#include <string>
#include <vector>
#include <iostream>
#include <memory> /* shared_ptr */

// ROOT include(s).
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/Event.h"
#include "AnalysisTools/Range.h"
#include "AnalysisTools/GRL.h"
#include "AnalysisTools/Cut.h"

#include "AnalysisTools/Analysis.h"
#include "AnalysisTools/ObjectDefinition.h"
#include "AnalysisTools/EventSelection.h"

using namespace std;
using namespace AnalysisTools;

int main (int argc, char* argv[]) {
    
    cout << "======================================================================" << endl;
    cout << " Running analysis." << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    if (argc < 2) {
        cout << "Please provide at least one input file." << endl;
        return 0;
    }
    
    
    // Load dictionaries and stuff.
    gROOT->ProcessLine( "#include <vector>" );
    gROOT->LoadMacro( "share/TLorentzVectorDict.h+" );
    

    // Get input files.
    vector<string> inputs;
    
    if (argc == 2) {
        string path = string(argv[1]);
        if (endsWith(path, ".root")) {
            inputs.push_back(path);
        } else {
            /* Assuming list of file names */
            ifstream file ( path.c_str() );
            if (!file.is_open()) {
                cout << "ERROR: Input file list not found. Exiting." << endl;
                return 0;
            }
            string filename = "";
            while ( file.good() ) {
                std::getline ( file, filename );
                if (filename == "") { continue; }
                inputs.push_back(filename);
            }
            file.close();
        }
    } else {
        for (unsigned i = 1; i < argc; i++) {
            inputs.push_back( string(argv[i]) );
        }
    }
    
    // Loop input files.
    for (const string& input : inputs) {
        
        // Get input file.
        TFile inputFile(input.c_str(), "READ");
        if ( !inputFile.IsOpen() ) {
            cerr << "Unable to open file." << endl;
            return 0;
        }
        
        
        // Get input tree.
        TTree* inputTree = (TTree*) inputFile.Get("outputTree");
        if (!inputTree) {
            cerr << "Unable to retrieve tree." << endl;
            return 0;
        }
        
        
        // Get number of events.
        const unsigned int nEvents = inputTree->GetEntries();
        if (nEvents == 0) {
            cout << " -- " << endl;
            continue;
        }
        
        
        // Set up addresses for reading.
        vector< TLorentzVector >* electrons = nullptr;
        vector< TLorentzVector >* muons     = nullptr;
        vector< TLorentzVector >* jets      = nullptr;
        
        vector< int > * el_charge = nullptr;
        vector< int > * mu_charge = nullptr;
        
        float    MET  = 0;
        bool     isMC = 0;
        unsigned DSID = 0;
        
        
        // Set up branches for reading.
        TBranch *electronsBranch, *muonsBranch, *jetsBranch;
        TBranch *el_chargeBranch, *mu_chargeBranch;
        
        TBranch *METBranch;
        TBranch *isMCBranch, *DSIDBranch;
        
        
        // Connect branches to addresses.
        inputTree->SetBranchAddress( "signalElectrons", &electrons, &electronsBranch );
        inputTree->SetBranchAddress( "signalMuons",     &muons,     &muonsBranch );
        inputTree->SetBranchAddress( "signalJets",      &jets,      &jetsBranch );
        
        inputTree->SetBranchAddress( "signalElectrons_charge", &el_charge, &el_chargeBranch );
        inputTree->SetBranchAddress( "signalMuons_charge",     &mu_charge, &mu_chargeBranch );
        
        inputTree->SetBranchAddress( "MET",  &MET,  &METBranch );
        inputTree->SetBranchAddress( "isMC", &isMC, &isMCBranch );
        inputTree->SetBranchAddress( "DSID", &DSID, &DSIDBranch );
        
        
        
        // Get file name.
        // -------------------------------------------------------------------
        inputTree->GetEvent(0);
        
        string filedir  = "analysis_output/";
        string filename = (string) "analysis_" + (isMC ? "MC" : "data") + "_" + to_string(DSID) + ".root";
        
        
        
        // Set up AnalysisTools
        // -------------------------------------------------------------------
        
        Analysis analysis ("ResolvedWR");
        
        analysis.openOutput(filedir + filename);
        
        
        
        // Object definitions
        // -------------------------------------------------------------------
        
        // Electrons.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        
        ObjectDefinition<TLorentzVector> ElectronObjdef ("Electrons");
        ElectronObjdef.setInput(electrons);
        ElectronObjdef.addInfo("charge", el_charge);
        ElectronObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));
        
        // Muons.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        
        ObjectDefinition<TLorentzVector> MuonObjdef ("Muons");
        MuonObjdef.setInput(muons);
        MuonObjdef.addInfo("charge", mu_charge);
        MuonObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));
        
        // Muons.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        
        ObjectDefinition<TLorentzVector> JetObjdef ("Jets");
        JetObjdef.setInput(jets);
        JetObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));
        
        
        // Stuff for binding selections together.
        // -------------------------------------------------------------------
        
        /* Just make a 'Collection' class instead? */
        
        PhysicsObjects* SelectedElectrons = ElectronObjdef.result();
        PhysicsObjects* SelectedMuons     = MuonObjdef    .result();
        PhysicsObjects* SelectedJets      = JetObjdef     .result();
        
        
        
        // Event selection
        // -------------------------------------------------------------------
        
        EventSelection eventSelection ("EventSelection");
        
        eventSelection.addCategories({"SR_SS_ee", "SR_SS_mm", "SR_OS_ee", "SR_OS_mm"});
        
        eventSelection.addCollection("Electrons", SelectedElectrons);
        eventSelection.addCollection("Muons",     SelectedMuons);
        eventSelection.addCollection("Jets",      SelectedJets);
        
        // * Leptons
        Cut<Event> event_ee ("ee");
        event_ee.setFunction( [](const Event& e) { return e.collection("Electrons")->size(); });
        event_ee.addRange(2);
        eventSelection.addCut(event_ee, "SR_SS_ee");
        eventSelection.addCut(event_ee, "SR_OS_ee");
        
        Cut<Event> event_mm ("mm");
        event_mm.setFunction( [](const Event& e) { return e.collection("Muons")->size(); });
        event_mm.addRange(2);
        eventSelection.addCut(event_mm, "SR_SS_mm");
        eventSelection.addCut(event_mm, "SR_OS_mm");
        
        // * Charge
        Cut<Event> event_SS_ee ("SS_ee");
        event_SS_ee.setFunction( [](const Event& e) {
            return e.collection("Electrons")->at(0).info("charge") * e.collection("Electrons")->at(1).info("charge");
        });
        event_SS_ee.addRange(0, inf);
        eventSelection.addCut(event_SS_ee, "SR_SS_ee");
        
        Cut<Event> event_SS_mm ("SS_mm");
        event_SS_mm.setFunction( [](const Event& e) {
            return e.collection("Muons")->at(0).info("charge") * e.collection("Muons")->at(1).info("charge");
        });
        event_SS_mm.addRange(0, inf);
        eventSelection.addCut(event_SS_mm, "SR_SS_mm");
        
        Cut<Event> event_OS_ee ("OS_ee");
        event_OS_ee.setFunction( [](const Event& e) {
            return e.collection("Electrons")->at(0).info("charge") * e.collection("Electrons")->at(1).info("charge");
        });
        event_OS_ee.addRange(-inf, 0);
        eventSelection.addCut(event_OS_ee, "SR_OS_ee");
        
        Cut<Event> event_OS_mm ("OS_mm");
        event_OS_mm.setFunction( [](const Event& e) {
            return e.collection("Muons")->at(0).info("charge") * e.collection("Muons")->at(1).info("charge");
        });
        event_OS_mm.addRange(-inf, 0);
        eventSelection.addCut(event_OS_mm, "SR_OS_mm");
        
        
        // * Recombination (hadronic, leptonic).
        Operation<Event> recomb_jj ("Recomb_jj");
        recomb_jj.setFunction([](Event& e) {
            e.setParticle("jj", e.collection("Jets")->at(0) + e.collection("Jets")->at(1));
            return true;
        });
        eventSelection.addOperation(recomb_jj);
        
        
        Operation<Event> recomb_ll ("Recomb_ll");
        recomb_ll.setFunction([](Event& e) {
            if (e.collection("Electrons")->size() > 0) {
                e.setParticle("ll", e.collection("Electrons")->at(0) + e.collection("Electrons")->at(1));
            } else {
                e.setParticle("ll", e.collection("Muons")->at(0)     + e.collection("Muons")->at(1));
            }
            return true;
        });
        eventSelection.addOperation(recomb_ll);
        
        
        // * Check distributions.
        PlotMacro1D<Event> event_Mlljj("Mlljj", [](const Event& e) {
            return (e.particle("ll") + e.particle("jj")).M() / 1000.;
        });
        //MuonObjdef.addPlot(CutPosition::Post, mu_check_pT);
        
        
        
        // * Z veto (hadronic).
        Cut<Event> event_Zhad_veto ("Zhad_veto");
        event_Zhad_veto.setFunction( [](const Event& e) { return e.particle("jj").M() / 1000.; });
        event_Zhad_veto.addRange(110., inf);
        eventSelection.addCut(event_Zhad_veto); // "SR*"
        
        
        Cut<Event> event_Zhad_sel ("Zhad_selection");
        event_Zhad_sel.setFunction( [](const Event& e) { return e.particle("jj").M() / 1000.; });
        event_Zhad_sel.addRange(0, 110.);
        //eventSelection.addCut(event_Zhad_sel, "CR*");
        
        // * Z veto (leptonic).
        Cut<Event> event_Zlep_veto ("Zlep_veto");
        event_Zlep_veto.setFunction( [](const Event& e) { return e.particle("ll").M() / 1000.; });
        event_Zlep_veto.addRange(110., inf);
        event_Zlep_veto.addPlot(CutPosition::Pre,  event_Mlljj);
        event_Zlep_veto.addPlot(CutPosition::Post, event_Mlljj);
        eventSelection.addCut(event_Zlep_veto); // "SR*"
        
        Cut<Event> event_Zlep_sel ("Zlep_selection");
        event_Zlep_sel.setFunction( [](const Event& e) { return e.particle("ll").M() / 1000.; });
        event_Zlep_sel.addRange(0, 110.);
        //eventSelection.addCut(event_Zlep_sel, "CR*");
        
        // * SumET
        
        // * Check distributions.
        PlotMacro1D<Event> event_check_Njet("CHECK_event_Njet", [](const Event& e) { return e.collection("Jets")->size(); });
        eventSelection.addPlot(CutPosition::Pre,  event_check_Njet);
        eventSelection.addPlot(CutPosition::Post, event_check_Njet);
        
        PlotMacro1D<Event> event_check_Nel("CHECK_event_Nel", [](const Event& e) { return e.collection("Electrons")->size(); });
        eventSelection.addPlot(CutPosition::Pre,  event_check_Nel);
        eventSelection.addPlot(CutPosition::Post, event_check_Nel);
        
        
        
        
        // Adding analyses.
        // -------------------------------------------------------------------
        
        analysis.addSelection(&ElectronObjdef);
        analysis.addSelection(&MuonObjdef);
        analysis.addSelection(&JetObjdef);
        analysis.addSelection(&eventSelection);
        
        
        
        // Event loop.
        // -------------------------------------------------------------------
        
        for (unsigned int iEvent = 0; iEvent < nEvents; iEvent++) {
            
            inputTree->GetEvent(iEvent);
            
            // Run AnalysisTools.
            bool status = analysis.run(iEvent, nEvents, DSID);
            
            // If event doesn't pass selection, do not proceed (e.g. to write objects to file).
            if (!status) { continue; }
            
            
        }
        
        analysis.save();
        
    }
    
    cout << "----------------------------------------------------------------------" << endl;
    cout << " Done." << endl;
    cout << "======================================================================" << endl;
    
    return 1;
}

