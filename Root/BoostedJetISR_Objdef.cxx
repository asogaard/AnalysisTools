// STL include(s).
#include <string>
#include <vector>
#include <iostream>
#include <memory> /* shared_ptr */
#include <cmath> /* log, pow */
#include <cassert> /* assert */
#include <algorithm> /* std::sort */

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
    gROOT->ProcessLine(".L share/Loader.C+");
    //gROOT->ProcessLine( "#include <vector>" );
    //gROOT->LoadMacro( "share/TLorentzVectorDict.h+" );
    
    
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
        const unsigned nEvents = inputTree->GetEntries();
        if (nEvents == 0) {
            cout << " -- (File '" << input << "' is empty.)" << endl;
            continue;
        }
        
        
        // Set up addresses for reading.
        unsigned runNumber = 0;
        unsigned lumiBlock = 0;
        unsigned long long eventNumber = 0;
        unsigned mcChannelNumber = 0;
        float    weight_mc = 0;

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
	vector< float > fj_tau21DDT;
	
        // Set up branches for reading.
	TBranch *runNumberBranch, *lumiBlockBranch, *eventNumberBranch;

	TBranch *mcChannelNumberBranch, *weight_mcBranch;

	TBranch *ph_ptBranch,    *ph_etaBranch,    *ph_phiBranch;
	TBranch *fj_ptBranch, *fj_etaBranch, *fj_phiBranch, *fj_mBranch;
	TBranch *fj_tau21Branch, *fj_D2Branch;
	TBranch *fjut_tau21Branch, *fjut_ptBranch;

	TBranch *HLT_g140_looseBranch;
	TBranch *HLT_j380Branch;
	TBranch *HLT_j400Branch;
        
        
        // Connect branches to addresses.
        inputTree->SetBranchAddress( "runNumber",         &runNumber,           &runNumberBranch );
        //inputTree->SetBranchAddress( "lumiBlock",         &lumiBlock,           &lumiBlockBranch ); // @TODO: insert
        inputTree->SetBranchAddress( "eventNumber",       &eventNumber,         &eventNumberBranch );
        inputTree->SetBranchAddress( "mcChannelNumber",   &mcChannelNumber,     &mcChannelNumberBranch );
        inputTree->SetBranchAddress( "weight_mc",         &weight_mc,           &weight_mcBranch );
	/* @TODO: Pile-up reweighting? */

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
        GRL grl2015 ("share/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.txt");
	GRL grl2016 ("share/GRL/data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.txt");
        
        
         // Get file name.
        // -------------------------------------------------------------------
        inputTree->GetEvent(0);
        
        bool     isMC = (mcChannelNumber > 0);
        unsigned DSID = (isMC ? mcChannelNumber : runNumber);

        const string filedir  = "outputObjdef";
        const string filename = (string) "objdef_" + (isMC ? "MC" : "data") + "_" + to_string(DSID) + ".root";
        
        
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
	Analysis ISRgammaAnalysis ("BoostedJet+ISRgamma");
	Analysis ISRjetAnalysis   ("BoostedJet+ISRjet");
        
	const bool debug = false;

	std::vector<Analysis*> analyses = { &ISRgammaAnalysis, &ISRjetAnalysis };
	for (auto* analysis : analyses) {
	  analysis->setDebug(debug);
	}

	ISRgammaAnalysis.openOutput(filedir + "/" + filename);
	ISRjetAnalysis  .setOutput (ISRgammaAnalysis.file());

	for (auto* analysis : analyses) {
	  analysis->setWeight(weight);
	}
        
	const bool blind = true;
        
        
         // Set up output branches.
        // -------------------------------------------------------------------
	vector<TLorentzVector> signalPhotons, signalFatjets;

	for (auto* analysis : analyses) {
	  analysis->tree()->Branch("signalPhotons",      &signalPhotons,      32000, 0); /* Suppresses "TTree::Bronch" warnings */
	  analysis->tree()->Branch("signalFatjets",      &signalFatjets,      32000, 0);
	  
	  analysis->tree()->Branch("signalFatjets_tau21",   &fj_tau21);
	  analysis->tree()->Branch("signalFatjets_D2",      &fj_D2);
	
	  analysis->tree()->Branch("signalFatjets_tau21_ut",  &fjut_tau21);
	  analysis->tree()->Branch("signalFatjets_pt_ut",     &fjut_pt);
	
	  analysis->tree()->Branch("isMC",   &isMC);
	  analysis->tree()->Branch("DSID",   &DSID);
	}
       
        
         // Copy histograms.
        // -------------------------------------------------------------------
	/*
        TH1F* h_rawWeight = (TH1F*) inputFile.Get("h_rawWeight");
        analysis.file()->cd();
        h_rawWeight->Write();
	*/
        


 	 // Plotting macros.
        // -------------------------------------------------------------------

	auto rho      = [](const PhysicsObject& p) { 
	  return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 2.0));
	};
	auto rhoPrime = [](const PhysicsObject& p) { 
	  return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 1.4));
	};
	auto rhoDDT   = [](const PhysicsObject& p) { 
	  return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 1.0));
	};

	auto tau21DDT = [&rhoDDT](const PhysicsObject& p) {
	  // Linear correction function.
	  const double p0 =  0.6887;
	  const double p1 = -0.0941;
	  const double rhoDDTmin = 1.0;
	  auto linearCorrection = [&p0, &p1] (const double& x) { return p0 + p1 * x; };	  

	  // (DDT) modified tau21 value.
	  return p.info("tau21") + (linearCorrection(rhoDDTmin) - linearCorrection(rhoDDT(p)));
	};
     
	// Leading fatjet pt.
	PlotMacro1D<Event> plot_event_leadingFatjet_pt ("leadingfatjet_pt");
	plot_event_leadingFatjet_pt.setFunction( [](const Event& e) { 
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; }
	    return e.collection("Fatjets")->at(0).Pt() / 1000.; 
	  });

	// Leading fatjet mass.
	PlotMacro1D<Event> plot_event_leadingFatjet_m ("leadingfatjet_m");
	plot_event_leadingFatjet_m.setFunction( [](const Event& e) {
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; } 
	    return e.collection("Fatjets")->at(0).M() / 1000.; 
	  });

	// Leading fatjet phi.
	PlotMacro1D<Event> plot_event_leadingFatjet_phi ("leadingfatjet_phi");
	plot_event_leadingFatjet_phi.setFunction( [](const Event& e) { 
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; }
	    return e.collection("Fatjets")->at(0).Phi(); 
	  });

	// Leading fatjet eta.
	PlotMacro1D<Event> plot_event_leadingFatjet_eta ("leadingfatjet_eta");
	plot_event_leadingFatjet_eta.setFunction( [](const Event& e) { 
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; }
	    return e.collection("Fatjets")->at(0).Eta(); 
	  });

	// Leading fatjet rhoPrime.
	PlotMacro1D<Event> plot_event_leadingFatjet_rhoPrime ("leadingfatjet_rhoPrime");
	plot_event_leadingFatjet_rhoPrime.setFunction( [&rhoPrime](const Event& e) { 
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; }
	    return rhoPrime(e.collection("Fatjets")->at(0)); 
	  });

	// Leading fatjet rhoDDT.
	PlotMacro1D<Event> plot_event_leadingFatjet_rhoDDT ("leadingfatjet_rhoDDT");
	plot_event_leadingFatjet_rhoDDT.setFunction( [&rhoDDT](const Event& e) { 
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; }
	    return rhoDDT(e.collection("Fatjets")->at(0)); 
	  });

	// Leading fatjet tau21
	PlotMacro1D<Event> plot_event_leadingFatjet_tau21 ("leadingfatjet_tau21");
	plot_event_leadingFatjet_tau21.setFunction( [](const Event& e) { 
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; }
	    return e.collection("Fatjets")->at(0).info("tau21"); 
	  });

	// Photon pt.
        PlotMacro1D<Event> plot_event_leadingPhoton_pt ("leadingphoton_pt");
	plot_event_leadingPhoton_pt.setFunction([](const Event& e) { 
	    if (e.collection("Photons")->size() == 0) { return -9999.; }
	    return e.collection("Photons")->at(0).Pt() / 1000.; 
	  });

	// Delta-eta separation between photon and leading fatjet.
	PlotMacro1D<Event> plot_event_leadingFatjetPhotonDeltaEta ("leadingFatjetPhotonDeltaEta", [](const Event& e) { 
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; }
	    return std::fabs(e.collection("Fatjets")->at(0).Eta() - e.collection("Photons")->at(0).Eta()); 
	  });

	// Delta-eta separation between photon and recoil system (all fat jets).
	PlotMacro1D<Event> plot_event_recoilPhotonDeltaEta ("recoilPhotonDeltaEta", [](const Event& e) {
	    TLorentzVector recoil;
	    for (unsigned i = 0; i < e.collection("Fatjets")->size(); i++) {
	      recoil += e.collection("Fatjets")->at(i);
	    }
	    return std::fabs(recoil.Eta() - e.collection("Photons")->at(0).Eta());
	  });

	// HLT_j380 trigger decision
        PlotMacro1D<Event> plot_event_HLT_j380 ("HLT_j380");
	plot_event_HLT_j380.setFunction([](const Event& e) { 
	    return e.info("HLT_j380"); 
	  });
        
	// HLT_j400 trigger decision
        PlotMacro1D<Event> plot_event_HLT_j400 ("HLT_j400");
	plot_event_HLT_j400.setFunction([](const Event& e) { 
	    return e.info("HLT_j400"); 
	  });
        
	// HLT_g140_loose trigger decision
        PlotMacro1D<Event> plot_event_HLT_g140_loose ("HLT_g140_loose");
	plot_event_HLT_g140_loose.setFunction([](const Event& e) {
	    return e.info("HLT_g140_loose");
	  });

	// Physics object pT.
        PlotMacro1D<PhysicsObject> plot_object_pt ("pt");
	plot_object_pt.setFunction([](const PhysicsObject& p) { 
	    return p.Pt() / 1000.;
	  });

	// Physics object m.
	PlotMacro1D<PhysicsObject> plot_object_m ("m");
	plot_object_m.setFunction([](const PhysicsObject& p) { 
	    return p.M() / 1000.; 
	  });

	// Physics object eta.
	PlotMacro1D<PhysicsObject> plot_object_eta ("eta");
	plot_object_eta.setFunction([](const PhysicsObject& p) { 
	    return p.Eta();
	  });

	// Physics object phi.
	PlotMacro1D<PhysicsObject> plot_object_phi ("phi");
	plot_object_phi.setFunction([](const PhysicsObject& p) { 
	    return p.Phi();
	  });

	// Physics object (jet) tau21.
	PlotMacro1D<PhysicsObject> plot_object_tau21 ("plot_object_tau21");
	plot_object_tau21.setFunction([](const PhysicsObject& p) { 
	    return p.info("tau21"); 
	  });

	// Physics object (jet) untrimmed tau21.
	PlotMacro1D<PhysicsObject> plot_object_tau21_ut ("plot_object_tau21_ut", [](const PhysicsObject& p) { 
	    return p.info("tau21_ut"); 
	  });

	// Physics object (jet) untrimmed pT.
	PlotMacro1D<PhysicsObject> plot_object_pt_ut ("plot_object_pt_ut");
	plot_object_pt_ut.setFunction([](const PhysicsObject& p) { 
	    return p.info("pt_ut") / 1000.; 
	  });

	// Physics object (jet) D2.
	PlotMacro1D<PhysicsObject> plot_object_D2 ("plot_object_D2");
	plot_object_D2.setFunction([](const PhysicsObject& p) { 
	    return p.info("D2"); 
	  });

	// Leading fatjet tau21 (mod rhoDDT)
	PlotMacro1D<Event> plot_event_leadingFatjet_tau21DDT ("tau21DDT", [&rhoDDT](const Event& e) {
	  if (e.collection("Fatjets")->size() == 0) { return -9999.; }	  
	  //double rhoDDT_val = rhoDDT(e.collection("Fatjets")->at(0));
	  //double p0 =  0.6887;
	  //double p1 = -0.0941;
	  //double tau21 = e.collection("Fatjets")->at(0).info("tau21");
	  //auto mod = [&p0, &p1] (const double& x) { return p0 + p1 * x; };
	  //return tau21 + (mod(1.0) - mod(rhoDDT_val));
	  return e.collection("Fatjets")->at(0).info("tau21DDT");
	});
        


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
	preSelection.setDebug(debug);	
        preSelection.addInfo("HLT_j380", &HLT_j380);
        preSelection.addInfo("HLT_j400", &HLT_j400);
        preSelection.addInfo("HLT_g140_loose", &HLT_g140_loose);

        //preSelection.addCollection("Fatjets", AllFatjets);
        //preSelection.addCollection("Photons", AllPhotons);
	preSelection.addCollection("Fatjets", "AllFatjets");
	preSelection.addCollection("Photons", "AllPhotons");

        // * GRL
	Cut<Event> event_grl ("GRL");
        event_grl.setFunction( [&grl2015, &grl2016, &isMC, &runNumber, &lumiBlock](const Event& e) { 
	    return isMC || grl2015.contains(runNumber, lumiBlock) || grl2016.contains(runNumber, lumiBlock);
	  });
	preSelection.addCut(event_grl);


        // * Event cleaning
	// ...

        // * Jet cleaning
	// ...

	// * Trigger
        Cut<Event> cut_event_trigger ("Trigger");
        cut_event_trigger.setFunction( [](const Event& e) { 
	    return e.info("HLT_g140_loose"); 
	  });
        preSelection.addCut(cut_event_trigger);

        //cut_event_trigger.setFunction( [](const Event& e) { 
	//   return e.info("HLT_j380") || e.info("HLT_g140_loose"); 
	//  });
        //preSelection.addCut(cut_event_trigger);

	// * Trigger efficiency turn-on vs. leading jet pt.
        preSelection.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_pt);
        preSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_pt);

        //preSelection.addPlot(CutPosition::Pre,  plot_event_leadingPhoton_pt);
        //preSelection.addPlot(CutPosition::Post, plot_event_leadingPhoton_pt);

        preSelection.addPlot(CutPosition::Pre,  plot_event_HLT_j380);
        preSelection.addPlot(CutPosition::Post, plot_event_HLT_j380);
        
        preSelection.addPlot(CutPosition::Pre,  plot_event_HLT_j400);
        preSelection.addPlot(CutPosition::Post, plot_event_HLT_j400);
        
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
        PhotonObjdef.addPlot(CutPosition::Post, plot_object_pt);
        PhotonObjdef.addPlot(CutPosition::Post, plot_object_eta);
        PhotonObjdef.addPlot(CutPosition::Post, plot_object_phi);
        

        // Jets.
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        ObjectDefinition<TLorentzVector> FatjetObjdef ("Fatjets");

	FatjetObjdef.addCategories({"Nominal"});

        FatjetObjdef.setInput(&fatjets);
   
        FatjetObjdef.addInfo("tau21",       fj_tau21);
	FatjetObjdef.addInfo("tau21_ut",    fjut_tau21);
	FatjetObjdef.addInfo("pt_ut",       fjut_pt);
        FatjetObjdef.addInfo("D2",          fj_D2);
        FatjetObjdef.addInfo("dPhiPhoton", &fj_dphiMinPhoton);

        // * eta
        Cut<PhysicsObject> cut_fatjet_eta ("Eta");
        cut_fatjet_eta.setFunction( [](const PhysicsObject& p) { 
	    return (p.Pt() > 0. ? p.Eta() : -1000.); 
	  } );
        cut_fatjet_eta.setRange(-2.5, 2.5);
        FatjetObjdef.addCut(cut_fatjet_eta);

        // * pt
        Cut<PhysicsObject> cut_fatjet_pt ("pT");
        cut_fatjet_pt.setFunction( [](const PhysicsObject& p) { return p.Pt() / 1000.; } );
        cut_fatjet_pt.setRange(150., inf);	
	//cut_fatjet_pt.setRange(400., inf);
        FatjetObjdef.addCut(cut_fatjet_pt);

        // * dPhi(photon)
        Cut<PhysicsObject> cut_fatjet_dphi ("dPhi");
        cut_fatjet_dphi.setFunction( [](const PhysicsObject& p) { return p.info("dPhiPhoton"); } );
        cut_fatjet_dphi.setRange(pi/2., inf);
        FatjetObjdef.addCut(cut_fatjet_dphi);

	// * Boosted regime
	Cut<PhysicsObject> cut_fatjet_BoostedRegime ("BoostedRegime");
	cut_fatjet_BoostedRegime.setFunction( [](const PhysicsObject& p) { 
	    return p.Pt() > 2 * p.M();
	  });
	FatjetObjdef.addCut(cut_fatjet_BoostedRegime);

	// * rhoDDT
	Cut<PhysicsObject> cut_fatjet_rhoDDT ("rhoDDT");
	cut_fatjet_rhoDDT.setFunction( [&rhoDDT](const PhysicsObject& p) { 
	    return rhoDDT(p);
	  });
	cut_fatjet_rhoDDT.addRange(1.0, inf);
	FatjetObjdef.addCut(cut_fatjet_rhoDDT);

	// * computing tau21DDT value
	Operation<PhysicsObject> operation_fatjet_tau21DDT ("tau21DDT");
	operation_fatjet_tau21DDT.setFunction( [&tau21DDT](PhysicsObject& p) { 
	    p.addInfo("tau21DDT", tau21DDT(p));
	    return true;
	  });
	FatjetObjdef.addOperation(operation_fatjet_tau21DDT);

        // * Check distributions.
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_pt);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_m);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_eta);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_phi);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_tau21);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_tau21_ut);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_pt_ut);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_D2);


        
        // Stuff for binding selections together.
        // -------------------------------------------------------------------

	PhysicsObjects* SelectedPhotons = PhotonObjdef.result(); //"Nominal");
        PhysicsObjects* SelectedFatjets = FatjetObjdef.result("Nominal");

                
        // Event selection
        // -------------------------------------------------------------------

        EventSelection eventSelection ("EventSelection");
	eventSelection.setDebug(debug);

        eventSelection.addInfo("isMC", &isMC);

	eventSelection.addCategories({"Pass", "Fail"});

        //eventSelection.addCollection("Photons", SelectedPhotons);
        //eventSelection.addCollection("Fatjets", SelectedFatjets);
        eventSelection.addCollection("Photons", "Photons");
        eventSelection.addCollection("Fatjets", "Fatjets");

        // * Photon count
        Cut<Event> cut_event_NumPhotons ("NumPhotons");
        cut_event_NumPhotons.setFunction( [](const Event& e) { 
	    //FCTINFO("      Number of photons: %d", e.collection("Photons")->size());
	    return e.collection("Photons")->size(); 
	  });
        cut_event_NumPhotons.addRange(1);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Add plotting macros to this cut.
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_leadingFatjet_pt);
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_leadingPhoton_pt);
	cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_leadingFatjetPhotonDeltaEta);
        cut_event_NumPhotons.addPlot(CutPosition::Post, plot_event_recoilPhotonDeltaEta);

	// Actually add the cut.
        eventSelection.addCut(cut_event_NumPhotons);
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        // * Fatjet count
        Cut<Event> cut_event_Njets ("NumFatjets");
        cut_event_Njets.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->size();
	  });
        cut_event_Njets.addRange(1, inf);
        eventSelection.addCut(cut_event_Njets);
      

	// * Choose lowest-tau21DDT fat jet.
	Operation<Event> operation_event_fatjetambiguity ("FatjetAmbiguity");
	operation_event_fatjetambiguity.setFunction( [](Event& e) {
	    if (e.collection("Fatjets")->size() > 1) {
	      // Sort fat jets by ascending tau21DDT
	      std::sort(e.collection("Fatjets")->begin(), e.collection("Fatjets")->end(), 
			[](const PhysicsObject& p1, const PhysicsObject& p2) { 
			  return p1.info("tau21DDT") < p2.info("tau21DDT"); 
			});

	      // Remove all but the first element.
	      e.collection("Fatjets")->erase(e.collection("Fatjets")->begin() + 1, 
					     e.collection("Fatjets")->end());
	    }
	    return true;
	  });
	eventSelection.addOperation(operation_event_fatjetambiguity);


	// * Blinding.
	Cut<Event> cut_event_blinding ("Blinding");
	cut_event_blinding.setFunction( [&blind](const Event& e){
	    return (!blind or e.info("isMC"));
	  });
	eventSelection.addCut(cut_event_blinding, "Pass");
	

	// * tau21DDT
	Cut<Event> cut_event_leadingFatjet_tau21DDT ("tau21DDT");
        cut_event_leadingFatjet_tau21DDT.setFunction( [](const Event& e) {
	    //double rhoDDT_val = rhoDDT(e.collection("Fatjets")->at(0));
	    //double p0 =  0.6887;
	    //double p1 = -0.0941;
	    //double tau21 = e.collection("Fatjets")->at(0).info("tau21");
	    //auto mod = [&p0, &p1] (const double& x) { return p0 + p1 * x; };
	    //return tau21 + (mod(1.0) - mod(rhoDDT_val));
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT.setRange(-inf, 0.5);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT, "Pass");

	cut_event_leadingFatjet_tau21DDT.setRange(0.5, inf);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT, "Fail");

	// Adding commin plotting macros.
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_pt);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_m);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_eta);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_phi);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_rhoDDT);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_tau21);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_tau21DDT);


        // Adding selections.
        // -------------------------------------------------------------------
        
	// Pseudo-object definitions.
	ISRgammaAnalysis.addSelection(&AllFatjetsObjdef);
	ISRgammaAnalysis.addSelection(&AllPhotonsObjdef);

	ISRjetAnalysis.addSelection(&AllFatjetsObjdef);


	// Preselection.
	ISRgammaAnalysis.addSelection(&preSelection);
	
	for (const auto& cat : preSelection.categories()) {
	  preSelection.cut("Trigger", cat)->setFunction( [](const Event& e) {
	      return e.info("HLT_j380");
	    });
	}
	ISRjetAnalysis.addSelection(&preSelection);

	// Object definitions.
	ISRgammaAnalysis.addSelection(&FatjetObjdef);
	ISRgammaAnalysis.addSelection(&PhotonObjdef);

	FatjetObjdef.cut("pT")->setRange(450, inf);
	ISRjetAnalysis.addSelection(&FatjetObjdef);
	
	// Event selection.
	ISRgammaAnalysis.addSelection(&eventSelection);

	eventSelection.removeCut("NumPhotons");
	for (const auto& cat : eventSelection.categories()) {
	  eventSelection.cut("NumFatjets", cat)->setRange(2,inf);
	}
	ISRjetAnalysis.addSelection(&eventSelection);
	  
        
 
	// Duplicate event control.
	// -------------------------------------------------------------------
	map<unsigned, set<unsigned> > uniqueEvents;

        
        // Event loop.
        // -------------------------------------------------------------------

	//ISRgammaAnalysis.print();
	//ISRjetAnalysis.print();
        
        for (unsigned iEvent = 0; iEvent < nEvents; iEvent++) {
            
            inputTree->GetEvent(iEvent);

	    // Reject duplicate events.
            auto ret = uniqueEvents[DSID].emplace(eventNumber);
            if (!ret.second) { continue; }

	    // Build input collections.
	    // -- Photons.
	    photons.clear();
	    float current_pt = inf;
	    for (unsigned i = 0; i < ph_pt->size(); i++) {
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
	    for (unsigned i = 0; i < fj_pt->size(); i++) {
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
	    for (auto* analysis : analyses) {

	      bool status = analysis->run(iEvent, nEvents, DSID);

	      // If event doesn't pass selection, do not proceed (e.g. to write objects to file).
	      if (!status) { continue; }
	      
	      // Fill output branches
	      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

	      
	      // Write to output tree.
	      analysis->writeTree();
	      
	    }            

        }


	//for (auto* analysis : analyses) {
	analyses[0]->save();
	//}

    }
    
    cout << "---------------------------------------------------------------------" << endl;
    cout << " Done." << endl;
    cout << "=====================================================================" << endl;
    
    return 1;
}

