// STL include(s).
#include <string>
#include <vector>
#include <iostream>
#include <memory> /* shared_ptr */
#include <cmath> /* log, pow */
#include <cassert> /* assert */
#include <algorithm> /* std::sort, std::min */

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
    
    std::vector< std::string > inputs = getDatasetsFromCommandlineArguments(argc, argv);

    if (inputs.size() == 0) {
      FCTINFO("Found 0 input files. Exiting.");
      return 0;
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
        
	// Manually built vectors.
	vector<TLorentzVector> photons;
	vector<TLorentzVector> fatjets;

	vector< float > fj_tau21DDT;

        // General, event-level information
	auto runNumber       = readBranch< unsigned >          (inputTree, "runNumber");
        auto lumiBlock       = readBranch< unsigned >          (inputTree, "lumiBlock");
        auto eventNumber     = readBranch< unsigned long long >(inputTree, "eventNumber");
        auto mcChannelNumber = readBranch< unsigned >          (inputTree, "mcChannelNumber");
        auto weight_mc       = readBranch< float >             (inputTree, "weight_mc");
	/* @TODO: Pile-up reweighting? */

	// Photon kinematic quantities
	auto ph_pt  = readBranchVector< float >(inputTree, "ph_pt");
	auto ph_eta = readBranchVector< float >(inputTree, "ph_eta");
	auto ph_phi = readBranchVector< float >(inputTree, "ph_phi");

	// Trimmed jet kinematic quantities
        auto fj_pt  = readBranchVector< float >(inputTree, "rljet_pt");
        auto fj_eta = readBranchVector< float >(inputTree, "rljet_eta");
        auto fj_phi = readBranchVector< float >(inputTree, "rljet_phi");
        auto fj_m   = readBranchVector< float >(inputTree, "rljet_m");

	// Trimmed jet substructure quantities
        auto fj_tau21 = readBranchVector< float >(inputTree, "rljet_Tau21_wta");
        auto fj_D2    = readBranchVector< float >(inputTree, "rljet_D2");

	// Untrimmed jet quantities
        auto fjut_tau21 = readBranchVector< float >(inputTree, "utrljet_Tau21_wta");
        auto fjut_pt    = readBranchVector< float >(inputTree, "utrljet_pt");

	// Trigger information
        auto HLT_g140_loose = readBranch< bool >(inputTree, "HLT_g140_loose");
        auto HLT_j380       = readBranch< bool >(inputTree, "HLT_j380");
        auto HLT_j400       = readBranch< bool >(inputTree, "HLT_j400");

	// Pre-pre-selection
	auto ISRgamma = readBranch< int >(inputTree, "ISRgamma");
	auto ISRjet   = readBranch< int >(inputTree, "ISRjet");

        
         // Get GRL.
        // -------------------------------------------------------------------       
        GRL grl2015 ("share/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.txt");
	GRL grl2016 ("share/GRL/data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.txt");
        
        
         // Get file name.
        // -------------------------------------------------------------------
        inputTree->GetEvent(0);
        
        bool     isMC = (*mcChannelNumber > 0);
        unsigned DSID = (isMC ? *mcChannelNumber : *runNumber);

        const string filedir  = "outputObjdef";
        const string filename = (string) "objdef_" + (isMC ? "MC" : "data") + "_" + to_string(DSID) + ".root";
        
        
         // Get MC event weight.
        // -------------------------------------------------------------------
        float weightDefault = 1.;
        float* weight = nullptr;
        if (isMC) {
	  weight = weight_mc.get(); //->front();
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

	// Leading fatjet tau21 (mod rhoDDT)
	PlotMacro1D<Event> plot_event_leadingFatjet_tau21DDT ("leadingfatjet_tau21DDT", [&rhoDDT](const Event& e) {
	  if (e.collection("Fatjets")->size() == 0) { return -9999.; }	  
	  return e.collection("Fatjets")->at(0).info("tau21DDT");
	});

	// Leading fatjet D2
	PlotMacro1D<Event> plot_event_leadingFatjet_D2 ("leadingfatjet_D2");
	plot_event_leadingFatjet_D2.setFunction( [](const Event& e) { 
	    if (e.collection("Fatjets")->size() == 0) { return -9999.; }
	    return e.collection("Fatjets")->at(0).info("D2"); 
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

	// Physics object rhoDDT
	PlotMacro1D<PhysicsObject> plot_object_rhoDDT ("rhoDDT");
	plot_object_rhoDDT.setFunction([&rhoDDT](const PhysicsObject& p) { 
	    return rhoDDT(p);
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
	PlotMacro1D<PhysicsObject> plot_object_tau21 ("tau21");
	plot_object_tau21.setFunction([](const PhysicsObject& p) { 
	    return p.info("tau21"); 
	  });

	// Physics object (jet) untrimmed tau21.
	PlotMacro1D<PhysicsObject> plot_object_tau21_ut ("tau21_ut", [](const PhysicsObject& p) { 
	    return p.info("tau21_ut"); 
	  });

	// Physics object (jet) untrimmed pT.
	PlotMacro1D<PhysicsObject> plot_object_pt_ut ("pt_ut");
	plot_object_pt_ut.setFunction([](const PhysicsObject& p) { 
	    return p.info("pt_ut") / 1000.; 
	  });

	// Physics object (jet) D2.
	PlotMacro1D<PhysicsObject> plot_object_D2 ("D2");
	plot_object_D2.setFunction([](const PhysicsObject& p) { 
	    return p.info("D2"); 
	  });

        
         // Pseudo-objdefs (for creating collections).
        // -------------------------------------------------------------------
	// * All fatjets.
        ObjectDefinition<TLorentzVector> AllFatjetsObjdef ("AllFatjets");

	AllFatjetsObjdef.setInput(&fatjets);
	AllFatjetsObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));
        
	// * All photons.
        ObjectDefinition<TLorentzVector> AllPhotonsObjdef ("AllPhotons");

	AllPhotonsObjdef.setInput(&photons);
	AllPhotonsObjdef.addCut(Cut<PhysicsObject>("nop", [](const PhysicsObject& p) { return true; }));

        
         // Pre-selection
        // -------------------------------------------------------------------
        EventSelection preSelection ("PreSelection");
	preSelection.setDebug(debug);	
        preSelection.addInfo("HLT_j380",       HLT_j380.get());
        preSelection.addInfo("HLT_j400",       HLT_j400.get());
        preSelection.addInfo("HLT_g140_loose", HLT_g140_loose.get());

        preSelection.addInfo("ISRgamma", ISRgamma.get());
        preSelection.addInfo("ISRjet",   ISRjet.get());

	preSelection.addCollection("Fatjets", "AllFatjets");
	preSelection.addCollection("Photons", "AllPhotons");

	// * Initial
        Cut<Event> cut_event_initial ("Initial");
        cut_event_initial.setFunction( [](const Event& e) { 
	    return e.info("ISRgamma");
	  });
        preSelection.addCut(cut_event_initial);

        // * GRL
	Cut<Event> event_grl ("GRL");
        event_grl.setFunction( [&grl2015, &grl2016, &isMC, &runNumber, &lumiBlock](const Event& e) { 
	    return isMC || grl2015.contains(*runNumber, *lumiBlock) || grl2016.contains(*runNumber, *lumiBlock);
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

	// * Trigger efficiency turn-on vs. leading jet pt.
        preSelection.addPlot(CutPosition::Pre,  plot_event_leadingFatjet_pt);
        preSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_pt);

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
   
        FatjetObjdef.addInfo("tau21",    fj_tau21);
	FatjetObjdef.addInfo("tau21_ut", fjut_tau21);
	FatjetObjdef.addInfo("pt_ut",    fjut_pt);
        FatjetObjdef.addInfo("D2",       fj_D2);

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
        FatjetObjdef.addCut(cut_fatjet_pt);

	// * Boosted regime
	Cut<PhysicsObject> cut_fatjet_BoostedRegime ("BoostedRegime");
	cut_fatjet_BoostedRegime.setFunction( [](const PhysicsObject& p) { 
	    return p.Pt() / (2. * p.M());
	  });
	cut_fatjet_BoostedRegime.addRange(1., inf);
	FatjetObjdef.addCut(cut_fatjet_BoostedRegime);

	// * rhoDDT
	Cut<PhysicsObject> cut_fatjet_rhoDDT ("rhoDDT");
	cut_fatjet_rhoDDT.setFunction( [&rhoDDT](const PhysicsObject& p) { 
	   return rhoDDT(p);
	});
	cut_fatjet_rhoDDT.addRange(1.5, inf);
	FatjetObjdef.addCut(cut_fatjet_rhoDDT);

	// * computing tau21DDT value
	Operation<PhysicsObject> operation_fatjet_tau21DDT ("compute_tau21DDT");
	operation_fatjet_tau21DDT.setFunction( [&tau21DDT](PhysicsObject& p) { 
	    p.addInfo("tau21DDT", tau21DDT(p));
	    return true;
	  });
	FatjetObjdef.addOperation(operation_fatjet_tau21DDT);

        // * Check distributions.
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_pt);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_m);
        FatjetObjdef.addPlot(CutPosition::Post, plot_object_rhoDDT);
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

        eventSelection.addCollection("Photons", "Photons");
        eventSelection.addCollection("Fatjets", "Fatjets");

        // * Photon count
        Cut<Event> cut_event_NumPhotons ("NumPhotons");
        cut_event_NumPhotons.setFunction( [](const Event& e) { 
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

	// * Fatjet dPhi
        Operation<Event> operation_event_fatjet_photon_dPhi ("dPhiFatjetPhoton");
        operation_event_fatjet_photon_dPhi.setFunction( [](const Event& e) {
	    // Since we're removing jets from the container, this function needs
	    // to be run only once. Therefore, it is implemented as an Operation
	    // rather than as a Cut.
	    for (unsigned i = e.collection("Fatjets")->size(); i --> 0; ) {
	      float dPhiMin = pi;
	      for (const auto& photon : *e.collection("Photons")) {
		float dPhi = fabs(e.collection("Fatjets")->at(i).DeltaPhi(photon));
		dPhiMin = std::min(dPhiMin, dPhi);
	      }
	      // Discared jets within |dphi| < pi/2 of _the_ signal photon
	      if (dPhiMin < pi/2.) {
		e.collection("Fatjets")->erase(e.collection("Fatjets")->begin() + i);
	      }
	    }
	    return true;
	  });
        eventSelection.addOperation(operation_event_fatjet_photon_dPhi);

        // * Fatjet count (after dPhi-cut)
        Cut<Event> cut_event_NjetsAfterDphi (cut_event_Njets);
	cut_event_NjetsAfterDphi.setName("NumFatjetsAfterDphi");
        eventSelection.addCut(cut_event_NjetsAfterDphi);      

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
	/* @TODO: Apply only for ISRjet */

	// * Blinding.
	Cut<Event> cut_event_blinding ("Blinding");
	cut_event_blinding.setFunction( [&blind](const Event& e){
	    return (!blind or e.info("isMC"));
	  });
	eventSelection.addCut(cut_event_blinding, "Pass");
       
	// * tau21DDT
	Cut<Event> cut_event_leadingFatjet_tau21DDT ("tau21DDT_0p50");
        cut_event_leadingFatjet_tau21DDT.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT.setRange(0.5, inf);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT, "Fail");

	cut_event_leadingFatjet_tau21DDT.setRange(-inf, 0.5);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT, "Pass");

	// - - -
	Cut<Event> cut_event_leadingFatjet_tau21DDT_0p45 ("tau21DDT_0p45");
        cut_event_leadingFatjet_tau21DDT_0p45.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT_0p45.setRange(-inf, 0.45);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT_0p45, "Pass");

	// - - -
	Cut<Event> cut_event_leadingFatjet_tau21DDT_0p40 ("tau21DDT_0p40");
        cut_event_leadingFatjet_tau21DDT_0p40.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT_0p40.setRange(-inf, 0.4);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT_0p40, "Pass");

	// - - -
	Cut<Event> cut_event_leadingFatjet_tau21DDT_0p35 ("tau21DDT_0p35");
        cut_event_leadingFatjet_tau21DDT_0p35.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT_0p35.setRange(-inf, 0.35);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT_0p35, "Pass");

	// - - -
	Cut<Event> cut_event_leadingFatjet_tau21DDT_0p30 ("tau21DDT_0p30");
        cut_event_leadingFatjet_tau21DDT_0p30.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT_0p30.setRange(-inf, 0.3);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT_0p30, "Pass");

	// - - -
	Cut<Event> cut_event_leadingFatjet_tau21DDT_0p25 ("tau21DDT_0p25");
        cut_event_leadingFatjet_tau21DDT_0p25.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT_0p25.setRange(-inf, 0.25);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT_0p25, "Pass");

	// - - -
	Cut<Event> cut_event_leadingFatjet_tau21DDT_0p20 ("tau21DDT_0p20");
        cut_event_leadingFatjet_tau21DDT_0p20.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT_0p20.setRange(-inf, 0.2);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT_0p20, "Pass");

	// - - -
	Cut<Event> cut_event_leadingFatjet_tau21DDT_0p10 ("tau21DDT_0p10");
        cut_event_leadingFatjet_tau21DDT_0p10.setFunction( [](const Event& e) {
	    return e.collection("Fatjets")->at(0).info("tau21DDT");
          });
	cut_event_leadingFatjet_tau21DDT_0p10.setRange(-inf, 0.1);
	eventSelection.addCut(cut_event_leadingFatjet_tau21DDT_0p10, "Pass");


	// Adding commin plotting macros.
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_pt);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_m);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_eta);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_phi);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_rhoDDT);
	eventSelection.addPlot(CutPosition::Post, plot_event_leadingFatjet_D2);
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
	  preSelection.cut("Initial", cat)->setFunction( [](const Event& e) {
	      return e.info("ISRjet");
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

	eventSelection.removeCut      ("NumPhotons");
	eventSelection.removeCut      ("NumFatjetsAfterDphi");
	eventSelection.removeOperation("dPhiFatjetPhoton");
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
            auto ret = uniqueEvents[DSID].emplace(*eventNumber);
            if (!ret.second) { continue; }

	    // Build input collections.
	    // -- Photons.
	    std::vector<float> ph_m (ph_pt->size(), 0);
	    photons = createFourVectors(*ph_pt, *ph_eta, *ph_phi, ph_m);
            
	    // -- Large radius (fat) jets
	    fatjets = createFourVectors(*fj_pt, *fj_eta, *fj_phi, *fj_m);
            
            // Run AnalysisTools.
	    for (auto* analysis : analyses) {

	      bool status = analysis->run(iEvent, nEvents, DSID);

	      // If event doesn't pass selection, do not proceed (e.g. to write objects to file).
	      if (!status) { continue; }
	      
	       // Fill output branches
	      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

	      // Photons
	      signalPhotons.clear();            
	      for (const PhysicsObject& p : *SelectedPhotons) {
                signalPhotons.push_back( (TLorentzVector) p );
	      }
	      
	      // Jets
	      signalFatjets.clear();            
	      for (const PhysicsObject& p : *SelectedFatjets) {
                signalFatjets.push_back( (TLorentzVector) p );
	      }
	      
	      // Write to output tree.
	      analysis->writeTree();
	      
	    }            

        }


	// @TODO: Improve?
	//for (auto* analysis : analyses) {
	analyses[0]->save();
	//}

    }
    
    cout << "---------------------------------------------------------------------" << endl;
    cout << " Done." << endl;
    cout << "=====================================================================" << endl;
    
    return 1;
}

