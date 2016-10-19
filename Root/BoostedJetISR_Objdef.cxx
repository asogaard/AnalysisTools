// STL include(s).
#include <string>
#include <vector>
#include <iostream>
#include <memory> /* shared_ptr */
#include <math.h> /* log, pow */

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
            continue;
        }
        
        
        // Get number of events.
        const unsigned int nEvents = inputTree->GetEntries();
        if (nEvents == 0) {
            cout << " -- (File '" << input << "' is empty.)" << endl;
            continue;
        }
        
        
        // Set up addresses for reading.
        unsigned int runNumber = 0;
        unsigned int mcChannelNumber = 0;

	vector< float > *ph_pt  = 0;
	vector< float > *ph_eta = 0;
	vector< float > *ph_phi = 0;

	vector< float > *fj_pt  = 0;
	vector< float > *fj_eta = 0;
	vector< float > *fj_phi = 0;
	vector< float > *fj_m   = 0;

	vector< float > *fj_tau1 = 0;
	vector< float > *fj_tau2 = 0;
	vector< float > *fj_D2   = 0;

	// -- Manually built.
	vector<TLorentzVector> photons;
	vector<TLorentzVector> fatjets;

	vector< float > fj_dphiMinPhoton;
	
	/*
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
        */
        
        // Set up branches for reading.
	TBranch *runNumberBranch, *mcChannelNumberBranch;

	TBranch *ph_ptBranch,    *ph_etaBranch,    *ph_phiBranch;
	TBranch *fj_ptBranch, *fj_etaBranch, *fj_phiBranch, *fj_mBranch;
	TBranch *fj_tau1Branch, *fj_tau2Branch, *fj_D2Branch;
	/*
        TBranch *electronsBranch, *muonsBranch, *jetsBranch;
        TBranch *mu_id_mediumBranch, *mu_iso_loosetrackonlyBranch;
        TBranch *el_id_mediumBranch, *el_iso_loosetrackonlyBranch;
        TBranch *el_chargeBranch, *mu_chargeBranch;
        TBranch *el_z0sinthetaBranch, *el_d0BLsignificanceBranch;
        TBranch *mu_z0sinthetaBranch, *mu_d0BLsignificanceBranch;
        
        TBranch *METBranch, *lumiBlockBranch;
        TBranch *runNumberBranch, *mcChannelNumberBranch, *mcEventWeightBranch;
        TBranch *passedEventCleaningBranch, *passedJetCleaningBranch;
	*/
        
        
        // Connect branches to addresses.
        inputTree->SetBranchAddress( "runNumber",         &runNumber,           &runNumberBranch );
        inputTree->SetBranchAddress( "mcChannelNumber",   &mcChannelNumber,     &mcChannelNumberBranch );

        inputTree->SetBranchAddress( "ph_pt",  &ph_pt , &ph_ptBranch );
        inputTree->SetBranchAddress( "ph_eta", &ph_eta, &ph_etaBranch );
        inputTree->SetBranchAddress( "ph_phi", &ph_phi, &ph_phiBranch );

        inputTree->SetBranchAddress( "rljet_pt",  &fj_pt , &fj_ptBranch );
        inputTree->SetBranchAddress( "rljet_eta", &fj_eta, &fj_etaBranch );
        inputTree->SetBranchAddress( "rljet_phi", &fj_phi, &fj_phiBranch );
        inputTree->SetBranchAddress( "rljet_m",   &fj_m,   &fj_mBranch );

        inputTree->SetBranchAddress( "rljet_Tau1_wta", &fj_tau1, &fj_tau1Branch );
        inputTree->SetBranchAddress( "rljet_Tau2_wta", &fj_tau2, &fj_tau2Branch );
        inputTree->SetBranchAddress( "rljet_D2",       &fj_D2,    &fj_D2Branch );
	/*
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
	*/
        
        
        // Get GRL.
        // -------------------------------------------------------------------
        
        //GRL grl("share/GRL/data15_13TeV.periodAllYear_DetStatus-v73-pro19-08_DQDefects-00-01-02_PHYS_StandardGRL_All_Good_25ns.txt");
        
        
        
        // Get file name.
        // -------------------------------------------------------------------
        inputTree->GetEvent(0);
        
        bool     isMC = (mcChannelNumber > 0);
        unsigned DSID = (isMC ? mcChannelNumber : runNumber);

        string filedir  = "outputObjdef";
        string filename = (string) "objdef_" + (isMC ? "MC" : "data") + "_" + to_string(DSID) + ".root";
        
        
        
         // Get MC event weight.
        // -------------------------------------------------------------------
        float weightDefault = 1.;
        float* weight = nullptr;
        if (isMC) {
	    weight = &weightDefault; // &mcEventWeight->front();
        } else {
            weight = &weightDefault;
        }

        
        // Set up AnalysisTools
        // -------------------------------------------------------------------
        
        Analysis analysis ("BoostedJetISR");
        
        analysis.openOutput(filedir + "/" + filename);
        analysis.addTree();
        analysis.setWeight(weight);
        
        
        
        // Set up output branches.
        // -------------------------------------------------------------------

	vector<TLorentzVector> signalPhotons, signalFatjets;

        analysis.tree()->Branch("signalPhotons",      &signalPhotons,      32000, 0); /* Suppresses "TTree::Bronch" warnings */
        analysis.tree()->Branch("signalFatjets",      &signalFatjets,      32000, 0);

        analysis.tree()->Branch("signalFatjets_tau1",      &fj_tau1);
        analysis.tree()->Branch("signalFatjets_tau2",      &fj_tau2);
        analysis.tree()->Branch("signalFatjets_D2",      &fj_D2);

        /*
        vector<TLorentzVector> signalJets, signalElectrons, signalMuons;
        vector<int> signalElectrons_charge, signalMuons_charge;
        float SumET;
        
        analysis.tree()->Branch("signalJets",      &signalJets,      32000, 0); / * Suppresses "TTree::Bronch" warnings * /
        analysis.tree()->Branch("signalElectrons", &signalElectrons, 32000, 0);
        analysis.tree()->Branch("signalMuons",     &signalMuons,     32000, 0);
        
        analysis.tree()->Branch("signalElectrons_charge", &signalElectrons_charge);
        analysis.tree()->Branch("signalMuons_charge",     &signalMuons_charge);
        
        analysis.tree()->Branch("MET",    &MET);
        analysis.tree()->Branch("SumET",  &SumET);
	*/
        analysis.tree()->Branch("isMC",   &isMC);
        analysis.tree()->Branch("DSID",   &DSID);
	/*
        analysis.tree()->Branch("weight",  weight);
	*/
        
        
        
        // Copy histograms.
        // -------------------------------------------------------------------
	/*
        TH1F* h_rawWeight = (TH1F*) inputFile.Get("h_rawWeight");
        analysis.file()->cd();
        h_rawWeight->Write();
	*/
        
        
        // Pre-selection
        // -------------------------------------------------------------------
        
        EventSelection preSelection ("PreSelection");
        /*
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
        */
        
        
        // Object definitions
        // -------------------------------------------------------------------
        
        // Photons.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        ObjectDefinition<TLorentzVector> PhotonObjdef ("Photons");

        PhotonObjdef.setInput(&photons);

        // * pT
        Cut<PhysicsObject> cut_photon_pt ("pT");
        cut_photon_pt.setFunction( [](const PhysicsObject& p) { return p.Pt() / 1000.; } );
        cut_photon_pt.setRange(155., inf);
        PhotonObjdef.addCut(cut_photon_pt);
        
        
        // * Check distributions.
        PlotMacro1D<PhysicsObject> plot_photon_pt ("plot_photon_pt", [](const PhysicsObject& p) { return p.Pt() / 1000.; });
        PhotonObjdef.addPlot(CutPosition::Post, plot_photon_pt);
        

        // Jets.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -        
        ObjectDefinition<TLorentzVector> FatjetObjdef ("Fatjets");

	FatjetObjdef.addCategories({"Nominal", "D2mod", "tau21mod"});

        FatjetObjdef.setInput(&fatjets);
   
        FatjetObjdef.addInfo("tau1",  fj_tau1);
        FatjetObjdef.addInfo("tau2",  fj_tau2);
        FatjetObjdef.addInfo("D2",    fj_D2);
        FatjetObjdef.addInfo("dPhiPhoton", &fj_dphiMinPhoton);

	auto rho      = [](const PhysicsObject& p) { return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 2.0)); };
	auto rhoPrime = [](const PhysicsObject& p) { return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 1.4)); };
     
        // * eta
        Cut<PhysicsObject> cut_fatjet_eta ("Eta");
        cut_fatjet_eta.setFunction( [](const PhysicsObject& p) { return (p.Pt() > 0. ? p.Eta() : -1000.); } );
        cut_fatjet_eta.setRange(-2.5, 2.5);
        FatjetObjdef.addCut(cut_fatjet_eta);

        // * pt
        Cut<PhysicsObject> cut_fatjet_pt ("pT");
        cut_fatjet_pt.setFunction( [](const PhysicsObject& p) { return p.Pt() / 1000.; } );
        cut_fatjet_pt.setRange(250., inf);
        FatjetObjdef.addCut(cut_fatjet_pt);

        // * dPhi(photon)
        Cut<PhysicsObject> cut_fatjet_dphi ("dPhi");
        cut_fatjet_dphi.setFunction( [](const PhysicsObject& p) { return p.info("dPhiPhoton"); } );
        cut_fatjet_dphi.setRange(pi/2., inf);
        FatjetObjdef.addCut(cut_fatjet_dphi);

	// * rho
        Cut<PhysicsObject> cut_fatjet_rho ("rho");
        cut_fatjet_rho.setFunction( [&rho](const PhysicsObject& p) { return rho(p); } );
        cut_fatjet_rho.setRange(-6, -1);
        FatjetObjdef.addCut(cut_fatjet_rho, "D2mod");

	// * rho
        Cut<PhysicsObject> cut_fatjet_rhoPrime ("rhoPrime");
        cut_fatjet_rhoPrime.setFunction( [&rhoPrime](const PhysicsObject& p) { return rhoPrime(p); } );
        cut_fatjet_rhoPrime.setRange(-1, 2);
        FatjetObjdef.addCut(cut_fatjet_rhoPrime, "tau21mod");

        // * M
        Cut<PhysicsObject> cut_fatjet_m ("M");
        cut_fatjet_m.setFunction( [](const PhysicsObject& p) { return p.M() / 1000.; } );
        cut_fatjet_m.setRange(55., 75.);
        FatjetObjdef.addCut(cut_fatjet_m);


	// * Plots
        PlotMacro1D<PhysicsObject> plot_fatjet_pt ("plot_fatjet_pt", [](const PhysicsObject& p) { return p.Pt() / 1000.; });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_pt);

        PlotMacro1D<PhysicsObject> plot_fatjet_m ("plot_fatjet_m", [](const PhysicsObject& p) { return p.M() / 1000.; });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_m);

        PlotMacro1D<PhysicsObject> plot_fatjet_tau21 ("plot_fatjet_tau21", [](const PhysicsObject& p) { return p.info("tau2") / p.info("tau1"); });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_tau21);

        PlotMacro1D<PhysicsObject> plot_fatjet_tau21mod ("plot_fatjet_tau21mod", [&rhoPrime](const PhysicsObject& p) { 
	    double rhoPrimeval = rhoPrime(p);
	    double p0 =  0.477415;
	    double p1 = -0.103591;
	    auto mod = [&p0, &p1] (const double& x) { return p0 + p1 * x; };
	    return p.info("tau2") / p.info("tau1") + (mod(-1.) - mod(rhoPrimeval));
	  });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_tau21mod);

        PlotMacro1D<PhysicsObject> plot_fatjet_D2 ("plot_fatjet_D2", [](const PhysicsObject& p) { return p.info("D2"); });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_D2);

        PlotMacro1D<PhysicsObject> plot_fatjet_D2mod ("plot_fatjet_D2mod", [&rho](const PhysicsObject& p) { 
	    double rhoval = rho(p);
	    double p0 =  0.452934;
	    double p1 = -0.368133;
	    auto mod = [&p0, &p1] (const double& x) { return p0 + p1 * x; };
	    return p.info("D2") + (mod(-6.) - mod(rhoval));
	  });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_D2mod);

        PlotMacro1D<PhysicsObject> plot_fatjet_rho ("plot_fatjet_rho", [&rho](const PhysicsObject& p) { return rho(p); });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_rho);

        PlotMacro1D<PhysicsObject> plot_fatjet_rhoDDT ("plot_fatjet_rhoDDT", [](const PhysicsObject& p) { return log(p.M() * p.M() / (p.Pt() * 1000.) ); });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_rhoDDT);

        
        
        // Pseudo-objdefs (for creating collections).
        // -------------------------------------------------------------------
        ObjectDefinition<TLorentzVector> AllElectronObjdef ("AllElectrons");
	/*
	  AllElectronObjdef.setInput(electrons);
	  AllElectronObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));
	*/
        
        
        
        // Stuff for binding selections together.
        // -------------------------------------------------------------------
        
        PhysicsObjects* SelectedPhotons = PhotonObjdef.result(); //"Nominal");
        PhysicsObjects* SelectedFatjets = FatjetObjdef.result("Nominal");
        
        
        
        // Event selection
        // -------------------------------------------------------------------
        
        EventSelection eventSelection ("EventSelection");

        eventSelection.addCollection("Photons", SelectedPhotons);
        eventSelection.addCollection("Fatjets", SelectedFatjets);

	/*
        eventSelection.addCollection("AllElectrons", AllElectrons);
        
        eventSelection.addCollection("Electrons", SelectedElectrons);
        eventSelection.addCollection("Muons",     SelectedMuons);
        eventSelection.addCollection("Jets",      SelectedJets);
        
        // * Jet-electron OR
        Cut<Event> event_OR_je ("JetElectronOverlapRemoval");
        event_OR_je.setFunction( [](const Event& e) {
            / * Remove from 'Jets' if overlapping with 'AllElectrons'. * /
            AnalysisTools::OverlapRemoval(e.collection("Jets"), e.collection("AllElectrons"), 0.2, [](PhysicsObject j, PhysicsObject e) { return 2. * e.Pt() > j.Pt(); });
            return true;
        });
        eventSelection.addCut(event_OR_je);
        
        // * Electron-jet OR
        Cut<Event> event_OR_ej ("ElectronJetOverlapRemoval");
        event_OR_ej.setFunction( [](const Event& e) {
            / * Remove from 'Electrons' if overlapping with 'Jets'. * /
            AnalysisTools::OverlapRemoval(e.collection("Electrons"), e.collection("Jets"), 0.2, 0.4);
            return true;
        });
        eventSelection.addCut(event_OR_ej);
        */

        // * Fatjet count
        Cut<Event> cut_event_Njets ("NumFatjets");
        cut_event_Njets.setFunction( [](const Event& e) { return e.collection("Fatjets")->size(); });
        cut_event_Njets.addRange(1, inf);
        eventSelection.addCut(cut_event_Njets);
        
        // * Photon count
        Cut<Event> cut_event_NumPhotons ("NumPhotons");
        cut_event_NumPhotons.setFunction( [](const Event& e) { return e.collection("Photons")->size(); });
        cut_event_NumPhotons.addRange(1);

	// * -- Plots
        PlotMacro1D<Event> plot_event_leadingFatjetPt ("plot_event_leadingFatjetPt", [](const Event& e) { return e.collection("Fatjets")->at(0).Pt() / 1000.; });
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_leadingFatjetPt);

        PlotMacro1D<Event> plot_event_photonPt ("plot_event_photonPt", [](const Event& e) { return e.collection("Photons")->at(0).Pt() / 1000.; });
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_photonPt);

        eventSelection.addCut(cut_event_NumPhotons);

 
        // Adding selections.
        // -------------------------------------------------------------------
        
        analysis.addSelection(&preSelection);

        analysis.addSelection(&PhotonObjdef);
        analysis.addSelection(&FatjetObjdef);

	/*
        analysis.addSelection(&ElectronObjdef);
        analysis.addSelection(&MuonObjdef);
        analysis.addSelection(&JetObjdef);
        
        analysis.addSelection(&AllElectronObjdef);
	*/

        analysis.addSelection(&eventSelection);
        
        
        
        // Event loop.
        // -------------------------------------------------------------------
        
        for (unsigned int iEvent = 0; iEvent < nEvents; iEvent++) {
            
            inputTree->GetEvent(iEvent);

	    // Build input collections.
	    // -- Photons.
	    photons.clear();
	    float current_pt = inf;
	    for (unsigned int i = 0; i < ph_pt->size(); i++) {
	      TLorentzVector photon;
	      if (ph_pt->at(i) == 0.) { continue; }
	      photon.SetPtEtaPhiM( ph_pt ->at(i),
				   ph_eta->at(i),
				   ph_phi->at(i),
				   0 );
	      if (ph_pt->at(i) > current_pt) {
		std::cout << "WARNING: Photon " << i + 1 << "has greater pT (" << ph_pt->at(i) << ") than the previous one (" << current_pt <<  ")." << std::endl;
	      }
	      current_pt = ph_pt->at(i);
	      photons.push_back( photon );
	    }
            
	    // -- Large radius (fat) jets
	    fatjets.clear();
	    fj_dphiMinPhoton.clear();
	    current_pt = inf;
	    for (unsigned int i = 0; i < fj_pt->size(); i++) {
	      TLorentzVector fatjet;
	      if (fj_pt->at(i) == 0.) { continue; }
	      fatjet.SetPtEtaPhiM( fj_pt ->at(i),
				   fj_eta->at(i),
				   fj_phi->at(i),
				   fj_m  ->at(i) );
	      if (fj_pt->at(i) > current_pt) {
		std::cout << "WARNING: Fatjet " << i + 1 << "has greater pT (" << fj_pt->at(i) << ") than the previous one (" << current_pt <<  ")." << std::endl;
	      }
	      current_pt = fj_pt->at(i);
	      fatjets.push_back( fatjet );

	      // ---- Minimum dphi to a photon.
	      float dPhiMin = pi;
	      for (const TLorentzVector& photon : photons) {
		float dPhi = fatjet.Phi() - photon.Phi();
		while (dPhi < 0) { dPhi += 2 * pi; }
		if (dPhi > pi) { dPhi = 2 * pi - dPhi; }
		dPhiMin = (dPhi < dPhiMin ? dPhi : dPhiMin);
	      }
	      fj_dphiMinPhoton.push_back(dPhiMin);
	    }
            
            // Run AnalysisTools.
            bool status = analysis.run(iEvent, nEvents, DSID);
            
            // If event doesn't pass selection, do not proceed (e.g. to write objects to file).
            if (!status) { continue; }
            
            // Fill output branches
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            
            // -- Event-wide
	    /*
            SumET = 0;
            for (const PhysicsObject& p : *SelectedElectrons) { SumET += p.Pt(); }
            for (const PhysicsObject& p : *SelectedMuons)     { SumET += p.Pt(); }
            for (const PhysicsObject& p : *SelectedJets)      { SumET += p.Pt(); }
            */
            
            // -- Photons
            signalPhotons.clear();            
            for (const PhysicsObject& p : *SelectedPhotons) {
                signalPhotons.push_back( (TLorentzVector) p );
            }

            // -- Jets
            signalFatjets.clear();            
            for (const PhysicsObject& p : *SelectedFatjets) {
                signalFatjets.push_back( (TLorentzVector) p );
            }


            // -- Electrons
	    /*
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
	    */
            
            // -- Muons
	    /*
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
	    */
            
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

