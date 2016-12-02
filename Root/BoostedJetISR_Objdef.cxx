// STL include(s).
#include <string>
#include <vector>
#include <iostream>
#include <memory> /* shared_ptr */
#include <math.h> /* log, pow */
#include <assert.h> /* assert */

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
    //gROOT->ProcessLine(".L share/Loader.C+");
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
        float        weight_mc = 0;

	vector< float > *ph_pt  = 0;
	vector< float > *ph_eta = 0;
	vector< float > *ph_phi = 0;

	vector< float > *fj_pt  = 0;
	vector< float > *fj_eta = 0;
	vector< float > *fj_phi = 0;
	vector< float > *fj_m   = 0;

	vector< float > *fj_tau21 = 0;
	vector< float > *fj_D2    = 0;

	vector< float > *fjut_tau21 = 0;
	vector< float > *fjut_pt    = 0;

	bool HLT_g140_loose = false;
	bool HLT_j380 = false;
	bool HLT_j400 = false;

	// -- Manually built.
	vector<TLorentzVector> photons;
	vector<TLorentzVector> fatjets;

	vector< float > fj_dphiMinPhoton;
	
        // Set up branches for reading.
	TBranch *runNumberBranch, *mcChannelNumberBranch, *weight_mcBranch;

	TBranch *ph_ptBranch,    *ph_etaBranch,    *ph_phiBranch;
	TBranch *fj_ptBranch, *fj_etaBranch, *fj_phiBranch, *fj_mBranch;
	TBranch *fj_tau21Branch, *fj_D2Branch;
	TBranch *fjut_tau21Branch, *fjut_ptBranch;

	TBranch *HLT_g140_looseBranch;
	TBranch *HLT_j380Branch;
	TBranch *HLT_j400Branch;
        
        
        // Connect branches to addresses.
        inputTree->SetBranchAddress( "runNumber",         &runNumber,           &runNumberBranch );
        inputTree->SetBranchAddress( "mcChannelNumber",   &mcChannelNumber,     &mcChannelNumberBranch );
        inputTree->SetBranchAddress( "weight_mc",   &weight_mc,     &weight_mcBranch );

	// Photon kinematic quantities
        inputTree->SetBranchAddress( "ph_pt",  &ph_pt , &ph_ptBranch );
        inputTree->SetBranchAddress( "ph_eta", &ph_eta, &ph_etaBranch );
        inputTree->SetBranchAddress( "ph_phi", &ph_phi, &ph_phiBranch );

	// Trimmed jet kinematic quantities
        inputTree->SetBranchAddress( "rljet_pt",  &fj_pt , &fj_ptBranch );
        inputTree->SetBranchAddress( "rljet_eta", &fj_eta, &fj_etaBranch );
        inputTree->SetBranchAddress( "rljet_phi", &fj_phi, &fj_phiBranch );
        inputTree->SetBranchAddress( "rljet_m",   &fj_m,   &fj_mBranch );

	// Trimmed jet substructure quantities
        inputTree->SetBranchAddress( "rljet_Tau21_wta", &fj_tau21, &fj_tau21Branch );
        inputTree->SetBranchAddress( "rljet_D2",        &fj_D2,    &fj_D2Branch );

	// Untrimmed jet quantities
        inputTree->SetBranchAddress( "utrljet_Tau21_wta", &fjut_tau21, &fjut_tau21Branch );
        inputTree->SetBranchAddress( "utrljet_pt",        &fjut_pt,    &fjut_ptBranch );

	// Trigger information
        inputTree->SetBranchAddress( "HLT_g140_loose", &HLT_g140_loose, &HLT_g140_looseBranch );
        inputTree->SetBranchAddress( "HLT_j380",       &HLT_j380,       &HLT_j380Branch );
        inputTree->SetBranchAddress( "HLT_j400",       &HLT_j400,       &HLT_j400Branch );

        
        // Get GRL.
        // -------------------------------------------------------------------
        
        //GRL grl("share/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.txt");
        
        
        
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
	  weight = &weight_mc; //->front();
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

        analysis.tree()->Branch("signalFatjets_tau21",   &fj_tau21);
        analysis.tree()->Branch("signalFatjets_D2",      &fj_D2);

        analysis.tree()->Branch("signalFatjets_tau21_ut",  &fjut_tau21);
        analysis.tree()->Branch("signalFatjets_pt_ut",     &fjut_pt);

        analysis.tree()->Branch("isMC",   &isMC);
        analysis.tree()->Branch("DSID",   &DSID);

        
        
        // Copy histograms.
        // -------------------------------------------------------------------
	/*
        TH1F* h_rawWeight = (TH1F*) inputFile.Get("h_rawWeight");
        analysis.file()->cd();
        h_rawWeight->Write();
	*/
        

        // Pseudo-objdefs (for creating collections).
        // -------------------------------------------------------------------

	// * All fatjets.
        ObjectDefinition<TLorentzVector> AllFatjetsObjdef ("AllFatjets");

	AllFatjetsObjdef.setInput(&fatjets);
	AllFatjetsObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));

        PhysicsObjects* AllFatjets = AllFatjetsObjdef.result();
        
	// * All photons.
        ObjectDefinition<TLorentzVector> AllPhotonsObjdef ("AllPhotons");

	AllPhotonsObjdef.setInput(&photons);
	AllPhotonsObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));

        PhysicsObjects* AllPhotons = AllPhotonsObjdef.result();
        
        
        // Pre-selection
        // -------------------------------------------------------------------
        
        EventSelection preSelection ("PreSelection");

        preSelection.addInfo("HLT_j380", &HLT_j380);
        preSelection.addInfo("HLT_j400", &HLT_j400);
        preSelection.addInfo("HLT_g140_loose", &HLT_g140_loose);

        preSelection.addCollection("Fatjets", AllFatjets);
        preSelection.addCollection("Photons", AllPhotons);

        // * GRL
	/*
	Cut<Event> event_grl ("GRL");
        event_grl.setFunction( [&grl, &mcChannelNumber, &lumiBlock, &runNumber](const Event& e) { return mcChannelNumber > 0 || grl.contains(runNumber, lumiBlock); } );
	preSelection.addCut(event_grl);
	*/

        // * Event cleaning
        // * Jet cleaning

	// * Trigger
        Cut<Event> cut_event_trigger ("Trigger");
        cut_event_trigger.setFunction( [](const Event& e) { return e.info("HLT_j380"); });
        preSelection.addCut(cut_event_trigger);

	// * Trigger efficiency turn-on vs. leading jet pt.
        PlotMacro1D<Event> plot_event_leadingFatjetPt ("plot_event_leadingFatjetPt", [](const Event& e) { 
	    return e.collection("Fatjets")->at(0).Pt() / 1000.; 
	  });
        preSelection.addPlot(CutPosition::Pre,  plot_event_leadingFatjetPt);
        preSelection.addPlot(CutPosition::Post, plot_event_leadingFatjetPt);

        PlotMacro1D<Event> plot_event_leadingPhotonPt ("plot_event_leadingPhotonPt", [](const Event& e) { 
	    return e.collection("Photons")->at(0).Pt() / 1000.; 
	  });
        preSelection.addPlot(CutPosition::Pre,  plot_event_leadingPhotonPt);
        preSelection.addPlot(CutPosition::Post, plot_event_leadingPhotonPt);

        PlotMacro1D<Event> plot_event_HLT_j380 ("plot_event_HLT_j380", [](const Event& e) { 
	    return e.info("HLT_j380"); 
	  });
        preSelection.addPlot(CutPosition::Pre,  plot_event_HLT_j380);
        preSelection.addPlot(CutPosition::Post, plot_event_HLT_j380);
        
        PlotMacro1D<Event> plot_event_HLT_j400 ("plot_event_HLT_j400", [](const Event& e) { 
	    return e.info("HLT_j400"); 
	  });
        preSelection.addPlot(CutPosition::Pre,  plot_event_HLT_j400);
        preSelection.addPlot(CutPosition::Post, plot_event_HLT_j400);
        
        PlotMacro1D<Event> plot_event_HLT_g140_loose ("plot_event_HLT_g140_loose", [](const Event& e) { 
	    return e.info("HLT_g140_loose"); 
	  });
        preSelection.addPlot(CutPosition::Pre,  plot_event_HLT_g140_loose);
        preSelection.addPlot(CutPosition::Post, plot_event_HLT_g140_loose);
        
        
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

	FatjetObjdef.addCategories({"Nominal"}); //, "D2mod", "tau21mod"});

        FatjetObjdef.setInput(&fatjets);
   
        FatjetObjdef.addInfo("tau21",    fj_tau21);
	FatjetObjdef.addInfo("tau21_ut", fjut_tau21);
	FatjetObjdef.addInfo("pt_ut",    fjut_pt);
        FatjetObjdef.addInfo("D2",       fj_D2);
        FatjetObjdef.addInfo("dPhiPhoton", &fj_dphiMinPhoton);

	auto rho      = [](const PhysicsObject& p) { return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 2.0)); };
	auto rhoPrime = [](const PhysicsObject& p) { return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 1.4)); };
	auto rhoDDT   = [](const PhysicsObject& p) { return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 1.0)); };
     
        // * eta
        Cut<PhysicsObject> cut_fatjet_eta ("Eta");
        cut_fatjet_eta.setFunction( [](const PhysicsObject& p) { return (p.Pt() > 0. ? p.Eta() : -1000.); } );
        cut_fatjet_eta.setRange(-2.5, 2.5);
        FatjetObjdef.addCut(cut_fatjet_eta);

        // * pt
        Cut<PhysicsObject> cut_fatjet_pt ("pT");
        cut_fatjet_pt.setFunction( [](const PhysicsObject& p) { return p.Pt() / 1000.; } );
        //cut_fatjet_pt.setRange(150., inf);
	cut_fatjet_pt.setRange(400., inf);
        FatjetObjdef.addCut(cut_fatjet_pt);

        // * dPhi(photon)
        Cut<PhysicsObject> cut_fatjet_dphi ("dPhi");
        cut_fatjet_dphi.setFunction( [](const PhysicsObject& p) { return p.info("dPhiPhoton"); } );
        cut_fatjet_dphi.setRange(pi/2., inf);


	 // Fatjet property plots; for use with substructureProfiles.py
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// * pt
	PlotMacro1D<PhysicsObject> plot_fatjet_pt ("plot_fatjet_pt", [](const PhysicsObject& p) { 
	    return p.Pt() / 1000.; 
	  });
        cut_fatjet_dphi.addPlot(CutPosition::Post, plot_fatjet_pt);

	// * m
	PlotMacro1D<PhysicsObject> plot_fatjet_m ("plot_fatjet_m", [](const PhysicsObject& p) { 
	    return p.M() / 1000.;
	  });
        cut_fatjet_dphi.addPlot(CutPosition::Post, plot_fatjet_m);

	// * tau21
	PlotMacro1D<PhysicsObject> plot_fatjet_tau21 ("plot_fatjet_tau21", [](const PhysicsObject& p) { 
	    return p.info("tau21");
	  });
        cut_fatjet_dphi.addPlot(CutPosition::Post, plot_fatjet_tau21);

	// * tau21_ut
	PlotMacro1D<PhysicsObject> plot_fatjet_tau21_ut ("plot_fatjet_tau21_ut", [](const PhysicsObject& p) { 
	    return p.info("tau21_ut"); 
	  });
        cut_fatjet_dphi.addPlot(CutPosition::Post, plot_fatjet_tau21_ut);

	// * pt_ut
	PlotMacro1D<PhysicsObject> plot_fatjet_pt_ut ("plot_fatjet_pt_ut", [](const PhysicsObject& p) { 
	    return p.info("pt_ut") / 1000.; 
	  });
        cut_fatjet_dphi.addPlot(CutPosition::Post, plot_fatjet_pt_ut);

	// * D2
	PlotMacro1D<PhysicsObject> plot_fatjet_D2 ("plot_fatjet_D2", [](const PhysicsObject& p) { 
	    return p.info("D2"); 
	  });
        cut_fatjet_dphi.addPlot(CutPosition::Post, plot_fatjet_D2);



	 // Actually add the cut.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        FatjetObjdef.addCut(cut_fatjet_dphi);


	/*
	// * rho
        Cut<PhysicsObject> cut_fatjet_rho ("rho");
        cut_fatjet_rho.setFunction( [&rho](const PhysicsObject& p) { return rho(p); } );
        cut_fatjet_rho.setRange(-6, -1);
        FatjetObjdef.addCut(cut_fatjet_rho, "D2mod");

	// * rhoPrime
        Cut<PhysicsObject> cut_fatjet_rhoPrime ("rhoPrime");
        cut_fatjet_rhoPrime.setFunction( [&rhoPrime](const PhysicsObject& p) { return rhoPrime(p); } );
        cut_fatjet_rhoPrime.setRange(-1, 2);
        FatjetObjdef.addCut(cut_fatjet_rhoPrime, "tau21mod");

        // * M
        Cut<PhysicsObject> cut_fatjet_m ("M");
        cut_fatjet_m.setFunction( [](const PhysicsObject& p) { return p.M() / 1000.; } );
        cut_fatjet_m.setRange(55., 75.);
        FatjetObjdef.addCut(cut_fatjet_m);

	// * BONUS: rho (applied before looking at tau21mod distribution).
        FatjetObjdef.addCut(cut_fatjet_rho, "tau21mod");

	// * BONUS: D2mod (applied before looking at tau21mod distribution).
        Cut<PhysicsObject> cut_fatjet_D2mod ("D2mod");
        cut_fatjet_D2mod.setFunction( [&rho](const PhysicsObject& p) { 
	    double rhoval = rho(p);
            double p0 =  0.452934;
            double p1 = -0.368133;
            auto mod = [&p0, &p1] (const double& x) { return p0 + p1 * x; };
            return p.info("D2") + (mod(-6.) - mod(rhoval));
	  });
        cut_fatjet_D2mod.setRange(-inf, 2.4);
        FatjetObjdef.addCut(cut_fatjet_D2mod, "tau21mod");

	// * BONUS: rhoPrime (applied before looking at D2mod distribution).
        FatjetObjdef.addCut(cut_fatjet_rhoPrime, "D2mod");

	// * tau21mod (applied before looking at D2mod distribution).
        Cut<PhysicsObject> cut_fatjet_tau21mod ("tau21mod");
        cut_fatjet_tau21mod.setFunction( [&rhoPrime](const PhysicsObject& p) { 
	    double rhoPrimeval = rhoPrime(p);
            double p0 =  0.477415;
            double p1 = -0.103591;
            auto mod = [&p0, &p1] (const double& x) { return p0 + p1 * x; };
            return p.info("tau2") / p.info("tau1") + (mod(-1.) - mod(rhoPrimeval));
	  });
	cut_fatjet_tau21mod.setRange(-inf, 0.6);
        FatjetObjdef.addCut(cut_fatjet_tau21mod, "D2mod");


	// * Plots
        PlotMacro1D<PhysicsObject> plot_fatjet_pt ("plot_fatjet_pt", [](const PhysicsObject& p) { return p.Pt() / 1000.; });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_pt);

        PlotMacro1D<PhysicsObject> plot_fatjet_m ("plot_fatjet_m", [](const PhysicsObject& p) { return p.M() / 1000.; });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_m);

        PlotMacro1D<PhysicsObject> plot_fatjet_tau21 ("plot_fatjet_tau21", [](const PhysicsObject& p) { return p.info("tau2") / p.info("tau1"); });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_tau21);

        PlotMacro1D<PhysicsObject> plot_fatjet_tau21_ut ("plot_fatjet_tau21_ut", [](const PhysicsObject& p) { return p.info("tau21_ut"); });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_tau21_ut);

        PlotMacro1D<PhysicsObject> plot_fatjet_pt_ut ("plot_fatjet_pt_ut", [](const PhysicsObject& p) { return p.info("pt_ut") / 1000.; });
        FatjetObjdef.addPlot(CutPosition::Post, plot_fatjet_pt_ut);

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
	*/
        
        
        
        
        // Stuff for binding selections together.
        // -------------------------------------------------------------------
        
        PhysicsObjects* SelectedPhotons = PhotonObjdef.result(); //"Nominal");
        PhysicsObjects* SelectedFatjets = FatjetObjdef.result("Nominal");
        
        
        
        // Event selection
        // -------------------------------------------------------------------
        
        EventSelection eventSelection ("EventSelection");

	eventSelection.addCategories({"Nominal", "rhoPrime", "rhoDDT"});

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
	// Previously defined.
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_leadingFatjetPt);

        PlotMacro1D<Event> plot_event_photonPt ("plot_event_photonPt", [](const Event& e) { return e.collection("Photons")->at(0).Pt() / 1000.; });
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_photonPt);

        PlotMacro1D<Event> plot_event_leadingFatjetPhotonDeltaEta ("plot_event_leadingFatjetPhotonDeltaEta", [](const Event& e) { return std::fabs(e.collection("Fatjets")->at(0).Eta() - e.collection("Photons")->at(0).Eta()); });
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_leadingFatjetPhotonDeltaEta);

        PlotMacro1D<Event> plot_event_recoilPhotonDeltaEta ("plot_event_recoilPhotonDeltaEta", [](const Event& e) {
	    TLorentzVector recoil;
	    for (unsigned int i = 0; i < e.collection("Fatjets")->size(); i++) {
	      recoil += e.collection("Fatjets")->at(i);
	    }
	    return std::fabs(recoil.Eta() - e.collection("Photons")->at(0).Eta()); });
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_recoilPhotonDeltaEta);


	 // Actually add the cut.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        eventSelection.addCut(cut_event_NumPhotons);


	// * Boosted regime
	Cut<Event> cut_event_leadingFatjet_BoostedRegime ("BoostedRegime");
	cut_event_leadingFatjet_BoostedRegime.setFunction( [](const Event& e) { 
	    return e.collection("Fatjets")->at(0).M() < e.collection("Fatjets")->at(0).Pt() / 2.; 
	  });
	eventSelection.addCut(cut_event_leadingFatjet_BoostedRegime);

	// * rhoPrime
	Cut<Event> cut_event_leadingFatjet_rhoPrime ("rhoPrime");
	cut_event_leadingFatjet_rhoPrime.setFunction( [&rhoPrime](const Event& e) { 
	    return rhoPrime(e.collection("Fatjets")->at(0)); 
	  });
	cut_event_leadingFatjet_rhoPrime.addRange(-1.5, inf);
	eventSelection.addCut(cut_event_leadingFatjet_rhoPrime, "rhoPrime");

	// * rhoDDT
	Cut<Event> cut_event_leadingFatjet_rhoDDT ("rhoDDT");
	cut_event_leadingFatjet_rhoDDT.setFunction( [&rhoDDT](const Event& e) { 
	    return rhoDDT(e.collection("Fatjets")->at(0)); 
	  });
	cut_event_leadingFatjet_rhoDDT.addRange(1.0, inf);
	eventSelection.addCut(cut_event_leadingFatjet_rhoDDT, "rhoDDT");

	// *** Plots ***
	PlotMacro1D<Event> plot_event_leadingFatjet_pt ("leadingfatjet_pt");
	plot_event_leadingFatjet_pt.setFunction( [](const Event& e) { 
	    return e.collection("Fatjets")->size() == 0 ? -1 : e.collection("Fatjets")->at(0).Pt() / 1000.; 
	  });
	PlotMacro1D<Event> plot_event_leadingFatjet_m ("leadingfatjet_m");
	plot_event_leadingFatjet_m.setFunction( [](const Event& e) { 
	    return e.collection("Fatjets")->size() == 0 ? -1 : e.collection("Fatjets")->at(0).M() / 1000.; 
	  });

	// * tau21
	Cut<Event> cut_event_leadingFatjet_tau21 ("tau21");
        cut_event_leadingFatjet_tau21.setFunction( [&rhoPrime](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21");
          });
	cut_event_leadingFatjet_tau21.addRange(-inf, 0.6);


	eventSelection.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_pt);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_pt);

	eventSelection.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_m);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_m);

	eventSelection.addCut(cut_event_leadingFatjet_tau21, "Nominal");


	// * tau21_mod_rhoPrime
	Cut<Event> cut_event_leadingFatjet_tau21_mod_rhoPrime ("tau21_mod_rhoPrime");
        cut_event_leadingFatjet_tau21_mod_rhoPrime.setFunction( [&rhoPrime](const Event& e) {
	    double rhoPrime_val = rhoPrime(e.collection("Fatjets")->at(0));
	    double p0 =  0.4877;
	    double p1 = -0.0943;
	    double tau21 = e.collection("Fatjets")->at(0).info("tau21");
	    auto mod = [&p0, &p1] (const double& x) { return p0 + p1 * x; };
	    return tau21 + (mod(-1.5) - mod(rhoPrime_val));
          });
	cut_event_leadingFatjet_tau21_mod_rhoPrime.addRange(-inf, 0.6);

	// --* Plot: rhoPrime
	PlotMacro1D<Event> plot_event_leadingFatjet_rhoPrime ("rhoPrime");
	plot_event_leadingFatjet_rhoPrime.setFunction( [&rhoPrime](const Event& e) { 
	    return rhoPrime(e.collection("Fatjets")->at(0)); 
	  });
	cut_event_leadingFatjet_tau21_mod_rhoPrime.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_rhoPrime);
	cut_event_leadingFatjet_tau21_mod_rhoPrime.addPlot(CutPosition::Post, plot_event_leadingFatjet_rhoPrime);

       	// --* Plot: pt
	cut_event_leadingFatjet_tau21_mod_rhoPrime.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_pt);
	cut_event_leadingFatjet_tau21_mod_rhoPrime.addPlot(CutPosition::Post, plot_event_leadingFatjet_pt);

       	// --* Plot: m
	cut_event_leadingFatjet_tau21_mod_rhoPrime.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_m);
	cut_event_leadingFatjet_tau21_mod_rhoPrime.addPlot(CutPosition::Post, plot_event_leadingFatjet_m);

	// --* Actually add the cut
	eventSelection.addCut(cut_event_leadingFatjet_tau21_mod_rhoPrime, "rhoPrime");

	// * tau21_mod_rhoDDT
	Cut<Event> cut_event_leadingFatjet_tau21_mod_rhoDDT ("tau21_mod_rhoDDT");
        cut_event_leadingFatjet_tau21_mod_rhoDDT.setFunction( [&rhoDDT](const Event& e) {
	    double rhoDDT_val = rhoDDT(e.collection("Fatjets")->at(0));
	    double p0 =  0.6887;
	    double p1 = -0.0941;
	    double tau21 = e.collection("Fatjets")->at(0).info("tau21");
	    auto mod = [&p0, &p1] (const double& x) { return p0 + p1 * x; };
	    return tau21 + (mod(1.0) - mod(rhoDDT_val));
          });
	cut_event_leadingFatjet_tau21_mod_rhoDDT.addRange(-inf, 0.6);

	// --* Plot: rhoDDT
	PlotMacro1D<Event> plot_event_leadingFatjet_rhoDDT ("rhoDDT");
	plot_event_leadingFatjet_rhoDDT.setFunction( [&rhoDDT](const Event& e) { 
	    return rhoDDT(e.collection("Fatjets")->at(0)); 
	  });
	cut_event_leadingFatjet_tau21_mod_rhoDDT.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_rhoDDT);
	cut_event_leadingFatjet_tau21_mod_rhoDDT.addPlot(CutPosition::Post, plot_event_leadingFatjet_rhoDDT);

       	// --* Plot: pt
	cut_event_leadingFatjet_tau21_mod_rhoDDT.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_pt);
	cut_event_leadingFatjet_tau21_mod_rhoDDT.addPlot(CutPosition::Post, plot_event_leadingFatjet_pt);

	// --* Plot: m
	cut_event_leadingFatjet_tau21_mod_rhoDDT.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_m);
	cut_event_leadingFatjet_tau21_mod_rhoDDT.addPlot(CutPosition::Post, plot_event_leadingFatjet_m);

	// --* Actually add the cut
        eventSelection.addCut(cut_event_leadingFatjet_tau21_mod_rhoDDT, "rhoDDT");



 
        // Adding selections.
        // -------------------------------------------------------------------
        
	// Pseudo objdefs
        analysis.addSelection(&AllFatjetsObjdef);
        analysis.addSelection(&AllPhotonsObjdef);

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

