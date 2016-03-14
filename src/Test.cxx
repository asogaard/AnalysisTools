// STL include(s).
#include <string>
#include <vector>
#include <iostream>

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
    cout << " Running AnalysisTools test." << endl;
    cout << "----------------------------------------------------------------------" << endl;
    
    if (argc < 2) {
        cout << "Please provide at least one input file." << endl;
        return 0;
    }

    /* *
     * Loop input files.
     */
    
    string input = string(argv[1]);
    

    
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
    
    
    // Load dictionaries and stuff.
    gROOT->ProcessLine( "#include <vector>" );
    gROOT->LoadMacro( "share/TLorentzVectorDict.h+" );
    
    
    // Get number of events.
    const unsigned int nEvents = inputTree->GetEntries();
    
    
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
    TBranch *runNumberBranch, *mcChannelNumberBranch;
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
    inputTree->SetBranchAddress( "passedEventCleaning", &passedEventCleaning, &passedEventCleaningBranch );
    inputTree->SetBranchAddress( "passedJetCleaning",   &passedJetCleaning,   &passedJetCleaningBranch );
    
    
    
     // Get GRL.
    // -------------------------------------------------------------------
    
    GRL grl("share/GRL/data15_13TeV.periodAllYear_DetStatus-v73-pro19-08_DQDefects-00-01-02_PHYS_StandardGRL_All_Good_25ns.txt");
    
    
    
     // Set up AnalysisTools
    // -------------------------------------------------------------------
    
    Analysis analysis ("BoostedWR");
    
    analysis.openOutput("output.root");

    /* *
     * Only the input should be templated. All cuts should not, all using the
     * PhysicsObject class internally.
     */
    
    
    
     // Pre-selection
    // -------------------------------------------------------------------
    
    // ...
    
    
    
     // Object definitions
    // -------------------------------------------------------------------

    ObjectDefinition<TLorentzVector> ElectronObjdef ("Electrons");
    
    ElectronObjdef.setInput(electrons);

    ElectronObjdef.addInfo("id_medium",          el_id_medium);
    ElectronObjdef.addInfo("iso_loosetrackonly", el_iso_loosetrackonly);
    ElectronObjdef.addInfo("z0sintheta",         el_z0sintheta);
    ElectronObjdef.addInfo("d0BLsignificance",   el_d0BLsignificance);

    //ElectronObjdef.setCategories( {"Loose", "Tight"} ); // Just nominal

    // * pT
    Cut<PhysicsObject> el_pT ("pT");
    el_pT.setFunction( [](PhysicsObject p) { return p.Pt() / 1000.; } );
    el_pT.setRange(20., inf);
    ElectronObjdef.addCut(el_pT); // , "Loose");

    // * eta
    Cut<PhysicsObject> el_eta ("Eta");
    el_eta.setFunction( [](PhysicsObject p) { return p.Eta(); } );
    el_eta.setRange(-2.47, 2.47);
    ElectronObjdef.addCut(el_eta);
    
    // * eta, crack-region
    Cut<PhysicsObject> el_etaCrack ("EtaCrack");
    el_etaCrack.setFunction( [](PhysicsObject p) { return fabs(p.Eta()); } );
    el_etaCrack.addRange(0, 1.37);
    el_etaCrack.addRange(1.52, inf);
    ElectronObjdef.addCut(el_etaCrack);
    
    // * ID (medium).
    Cut<PhysicsObject> el_ID ("MediumID");
    el_ID.setFunction( [](PhysicsObject p) { return p.info("id_medium"); } );
    ElectronObjdef.addCut(el_ID);
    
    // * Isolation (loose).
    Cut<PhysicsObject> el_iso ("LooseTrackOnlyIso");
    el_iso.setFunction( [](PhysicsObject p) { return p.info("iso_loosetrackonly"); } );
    ElectronObjdef.addCut(el_iso);
    
    // * z0 * sin(theta)
    Cut<PhysicsObject> el_z0 ("z0sintheta");
    el_z0.setFunction( [](PhysicsObject p) { return fabs(p.info("z0sintheta")); } );
    el_z0.addRange(0, 0.5);
    ElectronObjdef.addCut(el_z0);
    
    // * d0 (BL significance)
    Cut<PhysicsObject> el_d0 ("d0BLsignificance");
    el_d0.setFunction( [](PhysicsObject p) { return fabs(p.info("d0BLsignificance")); } );
    el_d0.addRange(0, 5.);
    ElectronObjdef.addCut(el_d0);

    
    // * Check distributions.
    PlotMacro1D<PhysicsObject> el_check_pT("CHECK_el_pT", [](PhysicsObject p) { return p.Pt() / 1000.; });
    ElectronObjdef.addPlot(CutPosition::Post, el_check_pT);
    
    // Event selection

    EventSelection eventSelection ("EventSelection");
    vector<string> regions = {"SR", "CRZ", "CRT"};
    eventSelection.setCategories( regions );

    //eventSelection.addCollect(SelectedElectrons); // Somehow...
    /*
    eventSelection.addInfo("eventCleaning", passedEventCleaning);
    eventSelection.addInfo("jetCleaning",   passedJetCleaning);
    eventSelection.addInfo("LB",            lumiBlock);
    eventSelection.addInfo("run",           runNumber);
    eventSelection.addInfo("DSID",          mc_channel_number);
     */

    // * GRL
    Cut<Event> event_grl ("GRL");
    event_grl.setFunction( [grl, mcChannelNumber, lumiBlock, runNumber](Event e) { return true; }); //mcChannelNumber > 0 || grl.contains(lumiBlock, runNumber); } );
    eventSelection.addCut(event_grl);

    // * Event cleaning
    Cut<Event> event_eventCleaning ("EventCleaning");
    event_eventCleaning.setFunction( [passedEventCleaning](Event e) { return passedEventCleaning; } );
    eventSelection.addCut(event_eventCleaning);

    // * Jet cleaning
    Cut<Event> event_jetCleaning ("JetCleaning");
    event_jetCleaning.setFunction( [passedJetCleaning](Event e) { return passedJetCleaning; } );
    eventSelection.addCut(event_jetCleaning);

    
    
     // Adding analyses.
    // -------------------------------------------------------------------
    
    analysis.addSelection(&ElectronObjdef);
    analysis.addSelection(&eventSelection);
    

    
     // Stuff for binding selections together.
    // -------------------------------------------------------------------
    
    /* *
     * This should not change dynamically!
     */
    
    //PhysicsObjects* SelectedElectrons = ElectronObjdef.result("Nominal");
    
    
    
     // Event loop.
    // -------------------------------------------------------------------
    
    for (unsigned int iEvent = 0; iEvent < nEvents; iEvent++) {
        
        if (iEvent == nEvents) { break; }
        
        inputTree->GetEvent(iEvent);
        
        // Run AnalysisTools.
        bool status = analysis.run(iEvent, nEvents, mcChannelNumber);
        
        // If event doesn't pass selection, do not proceed (e.g. to write objects to file).
        if (!status) { continue; }
        
        //SelectedElectrons = ElectronObjdef.result("Nominal");
        
        
        
    }
    
    analysis.save();
    

    cout << "----------------------------------------------------------------------" << endl;
    cout << " Done." << endl;
    cout << "======================================================================" << endl;
    


    
    return 1;
}

