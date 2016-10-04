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
    
    cout << "=====================================================================" << endl;
    cout << " Running object definition." << endl;
    cout << "---------------------------------------------------------------------" << endl;
    
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
        TTree* inputTree = (TTree*) inputFile.Get("nominal"); /* "outputTree" */
        if (!inputTree) {
            cerr << "Unable to retrieve tree." << endl;
            return 0;
        }
        
        
        // Get number of events.
        const unsigned int nEvents = inputTree->GetEntries();
        if (nEvents == 0) {
            cout << " -- (File '" << input << "' is empty.)" << endl;
            continue;
        }
        
        
        // Set up addresses for reading.
        vector< TLorentzVector >* electrons = nullptr;
        vector< TLorentzVector >* muons     = nullptr;
        vector< TLorentzVector >* jets      = nullptr;
        
        vector< bool > * mu_id_medium          = nullptr;
        vector< bool > * mu_iso_loosetrackonly = nullptr;
        vector< bool > * el_id_medium          = nullptr;
        vector< bool > * el_iso_loosetrackonly = nullptr;
        
        vector< int > * el_charge = nullptr;
        vector< int > * mu_charge = nullptr;
        
        vector< float > * el_z0sintheta       = nullptr;
        vector< float > * el_d0BLsignificance = nullptr;
        vector< float > * mu_z0sintheta       = nullptr;
        vector< float > * mu_d0BLsignificance = nullptr;
        
        float MET = 0;
        unsigned int lumiBlock = 0;
        unsigned int runNumber = 0;
        unsigned int mcChannelNumber = 0;
        vector< float >* mcEventWeight = nullptr;
        bool passedEventCleaning = 0;
        bool passedJetCleaning   = 0;
        
        
        // Set up branches for reading.
        TBranch *electronsBranch, *muonsBranch, *jetsBranch;
        TBranch *mu_id_mediumBranch, *mu_iso_loosetrackonlyBranch;
        TBranch *el_id_mediumBranch, *el_iso_loosetrackonlyBranch;
        TBranch *el_chargeBranch, *mu_chargeBranch;
        TBranch *el_z0sinthetaBranch, *el_d0BLsignificanceBranch;
        TBranch *mu_z0sinthetaBranch, *mu_d0BLsignificanceBranch;
        
        TBranch *METBranch, *lumiBlockBranch;
        TBranch *runNumberBranch, *mcChannelNumberBranch, *mcEventWeightBranch;
        TBranch *passedEventCleaningBranch, *passedJetCleaningBranch;
        
        
        // Connect branches to addresses.
        inputTree->SetBranchAddress( "electrons",         &electrons, &electronsBranch );
        inputTree->SetBranchAddress( "muons",             &muons,     &muonsBranch );
        inputTree->SetBranchAddress( "AntiKt4EMTopoJets", &jets,      &jetsBranch );
        
        inputTree->SetBranchAddress( "el_id_medium",          &el_id_medium,          &el_id_mediumBranch );
        inputTree->SetBranchAddress( "el_iso_loosetrackonly", &el_iso_loosetrackonly, &el_iso_loosetrackonlyBranch );
        inputTree->SetBranchAddress( "mu_id_medium",          &mu_id_medium,          &mu_id_mediumBranch );
        inputTree->SetBranchAddress( "mu_iso_loosetrackonly", &mu_iso_loosetrackonly, &mu_iso_loosetrackonlyBranch );
        
        inputTree->SetBranchAddress( "el_charge", &el_charge, &el_chargeBranch );
        inputTree->SetBranchAddress( "mu_charge", &mu_charge, &mu_chargeBranch );
        
        inputTree->SetBranchAddress( "el_z0sintheta",       &el_z0sintheta,       &el_z0sinthetaBranch );
        inputTree->SetBranchAddress( "el_d0BLsignificance", &el_d0BLsignificance, &el_d0BLsignificanceBranch );
        inputTree->SetBranchAddress( "mu_z0sintheta",       &mu_z0sintheta,       &mu_z0sinthetaBranch );
        inputTree->SetBranchAddress( "mu_d0BLsignificance", &mu_d0BLsignificance, &mu_d0BLsignificanceBranch );
        
        inputTree->SetBranchAddress( "met",                 &MET,                 &METBranch );
        inputTree->SetBranchAddress( "lumiBlock",           &lumiBlock,           &lumiBlockBranch );
        inputTree->SetBranchAddress( "RunNumber",           &runNumber,           &runNumberBranch );
        inputTree->SetBranchAddress( "mc_channel_number",   &mcChannelNumber,     &mcChannelNumberBranch );
        inputTree->SetBranchAddress( "mc_event_weight",     &mcEventWeight,       &mcEventWeightBranch );
        inputTree->SetBranchAddress( "passedEventCleaning", &passedEventCleaning, &passedEventCleaningBranch );
        inputTree->SetBranchAddress( "passedJetCleaning",   &passedJetCleaning,   &passedJetCleaningBranch );
        
        
        // Get GRL.
        // -------------------------------------------------------------------
        
        GRL grl("share/GRL/data15_13TeV.periodAllYear_DetStatus-v73-pro19-08_DQDefects-00-01-02_PHYS_StandardGRL_All_Good_25ns.txt");
        
        
        
        // Get file name.
        // -------------------------------------------------------------------
        inputTree->GetEvent(0);
        
        bool     isMC = (mcChannelNumber > 0);
        unsigned DSID = (isMC ? mcChannelNumber : runNumber);

        string filedir  = "objdefOutput";
        string filename = (string) "objdef_" + (isMC ? "MC" : "data") + "_" + to_string(DSID) + ".root";
        
        
        
         // Get MC event weight.
        // -------------------------------------------------------------------
        float weightDefault = 1.;
        float* weight = nullptr;
        if (isMC) {
            weight = &mcEventWeight->front();
        } else {
            weight = &weightDefault;
        }

        
        // Set up AnalysisTools
        // -------------------------------------------------------------------
        
        Analysis analysis ("ResolvedWR");
        
        analysis.openOutput(filedir + "/" + filename);
        analysis.addTree();
        analysis.setWeight(weight);
        
        
        
        // Set up output branches.
        // -------------------------------------------------------------------
        
        vector<TLorentzVector> signalJets, signalElectrons, signalMuons;
        vector<int> signalElectrons_charge, signalMuons_charge;
        float SumET;
        
        analysis.tree()->Branch("signalJets",      &signalJets,      32000, 0); /* Suppresses "TTree::Bronch" warnings */
        analysis.tree()->Branch("signalElectrons", &signalElectrons, 32000, 0);
        analysis.tree()->Branch("signalMuons",     &signalMuons,     32000, 0);
        
        analysis.tree()->Branch("signalElectrons_charge", &signalElectrons_charge);
        analysis.tree()->Branch("signalMuons_charge",     &signalMuons_charge);
        
        analysis.tree()->Branch("MET",    &MET);
        analysis.tree()->Branch("SumET",  &SumET);
        analysis.tree()->Branch("isMC",   &isMC);
        analysis.tree()->Branch("DSID",   &DSID);
        analysis.tree()->Branch("weight",  weight);
        
        
        
        // Copy histograms.
        // -------------------------------------------------------------------
        
        TH1F* h_rawWeight = (TH1F*) inputFile.Get("h_rawWeight");
        analysis.file()->cd();
        h_rawWeight->Write();
        
        
        // Pre-selection
        // -------------------------------------------------------------------
        
        EventSelection preSelection ("PreSelection");
        
        // * GRL
        Cut<Event> event_grl ("GRL");
        event_grl.setFunction( [&grl, &mcChannelNumber, &lumiBlock, &runNumber](const Event& e) { return mcChannelNumber > 0 || grl.contains(runNumber, lumiBlock); } );
        preSelection.addCut(event_grl);
        
        // * Event cleaning
        Cut<Event> event_eventCleaning ("EventCleaning");
        event_eventCleaning.setFunction( [&passedEventCleaning](const Event& e) { return passedEventCleaning; } );
        preSelection.addCut(event_eventCleaning);
        
        // * Jet cleaning
        Cut<Event> event_jetCleaning ("JetCleaning");
        event_jetCleaning.setFunction( [&passedJetCleaning](const Event& e) { return passedJetCleaning; } );
        preSelection.addCut(event_jetCleaning);
        
        
        
        // Object definitions
        // -------------------------------------------------------------------
        
        // Electrons.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        
        ObjectDefinition<TLorentzVector> ElectronObjdef ("Electrons");
        
        ElectronObjdef.setInput(electrons);
        
        ElectronObjdef.addInfo("id_medium",          el_id_medium);
        ElectronObjdef.addInfo("iso_loosetrackonly", el_iso_loosetrackonly);
        ElectronObjdef.addInfo("z0sintheta",         el_z0sintheta);
        ElectronObjdef.addInfo("d0BLsignificance",   el_d0BLsignificance);
        
        // * pT
        Cut<PhysicsObject> el_pT ("pT");
        el_pT.setFunction( [](const PhysicsObject& p) { return p.Pt() / 1000.; } );
        el_pT.setRange(20., inf);
        ElectronObjdef.addCut(el_pT);
        
        // * eta
        Cut<PhysicsObject> el_eta ("Eta");
        el_eta.setFunction( [](const PhysicsObject& p) { return p.Eta(); } );
        el_eta.setRange(-2.47, 2.47);
        ElectronObjdef.addCut(el_eta);
        
        // * eta, crack-region
        Cut<PhysicsObject> el_etaCrack ("EtaCrack");
        el_etaCrack.setFunction( [](const PhysicsObject& p) { return fabs(p.Eta()); } );
        el_etaCrack.addRange(0, 1.37);
        el_etaCrack.addRange(1.52, inf);
        ElectronObjdef.addCut(el_etaCrack);
        
        // * ID (medium).
        Cut<PhysicsObject> el_ID ("MediumID");
        el_ID.setFunction( [](const PhysicsObject& p) { return p.info("id_medium"); } );
        ElectronObjdef.addCut(el_ID);
        
        // * Isolation (loose).
        Cut<PhysicsObject> el_iso ("LooseTrackOnlyIso");
        el_iso.setFunction( [](const PhysicsObject& p) { return p.info("iso_loosetrackonly"); } );
        ElectronObjdef.addCut(el_iso);
        
        // * z0 * sin(theta)
        Cut<PhysicsObject> el_z0 ("z0sintheta");
        el_z0.setFunction( [](const PhysicsObject& p) { return fabs(p.info("z0sintheta")); } );
        el_z0.addRange(0, 0.5);
        ElectronObjdef.addCut(el_z0);
        
        // * d0 (BL significance)
        Cut<PhysicsObject> el_d0 ("d0BLsignificance");
        el_d0.setFunction( [](const PhysicsObject& p) { return fabs(p.info("d0BLsignificance")); } );
        el_d0.addRange(0, 5.);
        ElectronObjdef.addCut(el_d0);
        
        
        // * Check distributions.
        PlotMacro1D<PhysicsObject> el_check_pT("CHECK_el_pT", [](const PhysicsObject& p) { return p.Pt() / 1000.; });
        ElectronObjdef.addPlot(CutPosition::Post, el_check_pT);
        
        
        // Muons.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        
        ObjectDefinition<TLorentzVector> MuonObjdef ("Muons");
        
        MuonObjdef.setInput(muons);
        
        MuonObjdef.addInfo("id_medium",          mu_id_medium);
        MuonObjdef.addInfo("iso_loosetrackonly", mu_iso_loosetrackonly);
        MuonObjdef.addInfo("z0sintheta",         mu_z0sintheta);
        MuonObjdef.addInfo("d0BLsignificance",   mu_d0BLsignificance);
        
        // * pT
        Cut<PhysicsObject> mu_pT ("pT");
        mu_pT.setFunction( [](const PhysicsObject& p) { return p.Pt() / 1000.; } );
        mu_pT.setRange(20., inf);
        MuonObjdef.addCut(mu_pT);
        
        // * eta
        Cut<PhysicsObject> mu_eta ("Eta");
        mu_eta.setFunction( [](const PhysicsObject& p) { return p.Eta(); } );
        mu_eta.setRange(-3.0, 3.0);
        MuonObjdef.addCut(mu_eta);
        
        // * ID (medium).
        Cut<PhysicsObject> mu_ID ("MediumID");
        mu_ID.setFunction( [](const PhysicsObject& p) { return p.info("id_medium"); } );
        MuonObjdef.addCut(mu_ID);
        
        // * Isolation (loose).
        Cut<PhysicsObject> mu_iso ("LooseTrackOnlyIso");
        mu_iso.setFunction( [](const PhysicsObject& p) { return p.info("iso_loosetrackonly"); } );
        MuonObjdef.addCut(mu_iso);
        
        // * z0 * sin(theta)
        Cut<PhysicsObject> mu_z0 ("z0sintheta");
        mu_z0.setFunction( [](const PhysicsObject& p) { return fabs(p.info("z0sintheta")); } );
        mu_z0.addRange(0, 0.5);
        MuonObjdef.addCut(mu_z0);
        
        // * d0 (BL significance)
        Cut<PhysicsObject> mu_d0 ("d0BLsignificance");
        mu_d0.setFunction( [](const PhysicsObject& p) { return fabs(p.info("d0BLsignificance")); } );
        mu_d0.addRange(0, 3.);
        MuonObjdef.addCut(mu_d0);
        
        // * Check distributions.
        PlotMacro1D<PhysicsObject> mu_check_pT("CHECK_mu_pT", [](const PhysicsObject& p) { return p.Pt() / 1000.; });
        MuonObjdef.addPlot(CutPosition::Post, mu_check_pT);
        
        
        // Jets.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        
        ObjectDefinition<TLorentzVector> JetObjdef ("Jets");
        
        JetObjdef.setInput(jets);
        
        // * eta
        Cut<PhysicsObject> jet_eta ("Eta");
        jet_eta.setFunction( [](const PhysicsObject& p) { return p.Eta(); } );
        jet_eta.setRange(-2.8, 2.8);
        JetObjdef.addCut(jet_eta);
        
        
        // Pseudo-objdefs (for creating collections).
        // -------------------------------------------------------------------
        ObjectDefinition<TLorentzVector> AllElectronObjdef ("AllElectrons");
        AllElectronObjdef.setInput(electrons);
        AllElectronObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));
        
        
        
        // Stuff for binding selections together.
        // -------------------------------------------------------------------
        
        PhysicsObjects* SelectedElectrons = ElectronObjdef.result("Nominal");
        PhysicsObjects* SelectedMuons     = MuonObjdef    .result("Nominal");
        PhysicsObjects* SelectedJets      = JetObjdef     .result();
        
        PhysicsObjects* AllElectrons = AllElectronObjdef.result();
        
        
        
        // Event selection
        // -------------------------------------------------------------------
        
        EventSelection eventSelection ("EventSelection");

        eventSelection.addCollection("AllElectrons", AllElectrons);
        
        eventSelection.addCollection("Electrons", SelectedElectrons);
        eventSelection.addCollection("Muons",     SelectedMuons);
        eventSelection.addCollection("Jets",      SelectedJets);
        
        // * Jet-electron OR
        Cut<Event> event_OR_je ("JetElectronOverlapRemoval");
        event_OR_je.setFunction( [](const Event& e) {
            /* Remove from 'Jets' if overlapping with 'AllElectrons'. */
            AnalysisTools::OverlapRemoval(e.collection("Jets"), e.collection("AllElectrons"), 0.2, [](PhysicsObject j, PhysicsObject e) { return 2. * e.Pt() > j.Pt(); });
            return true;
        });
        eventSelection.addCut(event_OR_je);
        
        // * Electron-jet OR
        Cut<Event> event_OR_ej ("ElectronJetOverlapRemoval");
        event_OR_ej.setFunction( [](const Event& e) {
            /* Remove from 'Electrons' if overlapping with 'Jets'. */
            AnalysisTools::OverlapRemoval(e.collection("Electrons"), e.collection("Jets"), 0.2, 0.4);
            return true;
        });
        eventSelection.addCut(event_OR_ej);
        
        // * Jet count
        Cut<Event> event_Njets ("Njets");
        event_Njets.setFunction( [](const Event& e) { return e.collection("Jets")->size(); });
        event_Njets.addRange(2, inf);
        eventSelection.addCut(event_Njets);
        
        // * Lepton count
        Cut<Event> event_Nleptons ("Nleptons");
        event_Nleptons.setFunction( [](const Event& e) { return e.collection("Electrons")->size() + e.collection("Muons")->size(); });
        event_Nleptons.addRange(2);
        eventSelection.addCut(event_Nleptons);
        
        
        
        // Adding selections.
        // -------------------------------------------------------------------
        
        analysis.addSelection(&preSelection);

        analysis.addSelection(&ElectronObjdef);
        analysis.addSelection(&MuonObjdef);
        analysis.addSelection(&JetObjdef);
        
        analysis.addSelection(&AllElectronObjdef);
        
        analysis.addSelection(&eventSelection);
        
        
        
        // Event loop.
        // -------------------------------------------------------------------
        
        for (unsigned int iEvent = 0; iEvent < nEvents; iEvent++) {
            
            inputTree->GetEvent(iEvent);
            
            // Run AnalysisTools.
            bool status = analysis.run(iEvent, nEvents, DSID);
            
            // If event doesn't pass selection, do not proceed (e.g. to write objects to file).
            if (!status) { continue; }
            
            // Fill output branches
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            
            // -- Event-wide
            SumET = 0;
            for (const PhysicsObject& p : *SelectedElectrons) { SumET += p.Pt(); }
            for (const PhysicsObject& p : *SelectedMuons)     { SumET += p.Pt(); }
            for (const PhysicsObject& p : *SelectedJets)      { SumET += p.Pt(); }
            
            
            // -- Jets
            signalJets.clear();
            
            for (const PhysicsObject& p : *SelectedJets) {
                signalJets.push_back( (TLorentzVector) p );
            }
            
            // -- Electrons
            signalElectrons       .clear();
            signalElectrons_charge.clear();
            
            for (const PhysicsObject& p : *SelectedElectrons) {
                signalElectrons.push_back( (TLorentzVector) p );
                int idx = getMatchIndex(signalElectrons.back(), electrons);
                if (idx < 0) {
                    cout << "Warning: Recieved negative match index (" << idx << " out of " << electrons->size() << ") for electrons." << endl;
                    continue;
                }
                signalElectrons_charge.push_back( el_charge->at(idx) );
            }
            
            // -- Muons
            signalMuons.clear();
            signalMuons_charge.clear();
            
            for (const PhysicsObject& p : *SelectedMuons) {
                signalMuons.push_back( (TLorentzVector) p );
                int idx = getMatchIndex(signalMuons.back(), muons);
                if (idx < 0) {
                    cout << "Warning: Recieved negative match index (" << idx << " out of " << muons->size() << ") for muons." << endl;
                    continue;
                }
                signalMuons_charge.push_back( mu_charge->at(idx) );
            }
            
            // Write to output tree.
            analysis.writeTree();
            
        }
        
        analysis.save();
        
    }
    
    cout << "---------------------------------------------------------------------" << endl;
    cout << " Done." << endl;
    cout << "=====================================================================" << endl;
    
    return 1;
}

