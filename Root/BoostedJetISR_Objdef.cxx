// STL include(s).
#include <string>
#include <vector>
#include <iostream>
#include <memory> /* shared_ptr */
#include <cmath> /* log, pow, abs */
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

#include "AnalysisTools/CommonOperations.h"
#include "AnalysisTools/CommonPlots.h"

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
    std::map<std::string, TTree*> inputTree;
    inputTree["Tight"] = (TTree*) inputFile.Get("nominal"); 
    if (!inputTree["Tight"]) {
      cerr << "Unable to retrieve tree." << endl;
      continue;
    }
    inputTree["Loose"] = (TTree*) inputFile.Get("nominal_Loose");
    if (!inputTree["Loose"]) {
      cerr << "Unable to retrieve tree." << endl;
      continue;
    }

    const std::vector<std::string> categories = { "Tight", "Loose" };
    //const std::vector<std::string> categories = { "Loose", "Tight" }; // { "Tight", "Loose" };
    
    // Get number of events.
    std::map<std::string, unsigned> nEvents; // = 10000; //inputTree->GetEntries();
    bool empty = true;
    for (const auto& pair : inputTree) {
      nEvents[pair.first] = pair.second->GetEntries();
      if (nEvents[pair.first] > 0) { empty = false; }
    }
    if (empty) {
      cout << " -- (File '" << input << "' is empty.)" << endl;
      continue;
    }
    
    // Manually built vectors.
    std::vector<TLorentzVector> photons;
    std::vector<TLorentzVector> largeRadiusJets;
    std::vector<TLorentzVector> smallRadiusJets;
    std::vector<float> lj_dphiMinPhoton;

    
    // General, event-level information
    std::map<std::string, std::unique_ptr<unsigned> > runNumber, lumiBlock, mcChannelNumber;
    std::map<std::string, std::unique_ptr<unsigned long long> > eventNumber;
    std::map<std::string, std::unique_ptr<float> > weight_mc;
    for (const auto& category : categories) {
      runNumber      [category] = readBranch<unsigned>          (inputTree[category], "runNumber");
      lumiBlock      [category] = readBranch<unsigned>          (inputTree[category], "lumiBlock");
      eventNumber    [category] = readBranch<unsigned long long>(inputTree[category], "eventNumber");
      mcChannelNumber[category] = readBranch<unsigned>          (inputTree[category], "mcChannelNumber");
      weight_mc      [category] = readBranch<float>             (inputTree[category], "weight_mc");
      /* @TODO: Pile-up reweighting? */
    }
    
    // Photon kinematic quantities
    std::map<std::string, std::vector<float>* > ph_pt, ph_eta, ph_phi, ph_e;
    //std::map<std::string, std::unique_ptr< std::vector<float> > > ph_pt, ph_eta, ph_phi, ph_e;
    for (const auto& category : categories) {
      /* -- * /
      ph_pt [category] = readBranchVector< float >(inputTree[category], "ph_pt");
      ph_eta[category] = readBranchVector< float >(inputTree[category], "ph_eta");
      ph_phi[category] = readBranchVector< float >(inputTree[category], "ph_phi");
      ph_e  [category] = readBranchVector< float >(inputTree[category], "ph_e");
      / * -- */

      /* -- */
      ph_pt [category] = nullptr;
      inputTree[category]->SetBranchAddress("ph_pt", &ph_pt[category]);

      ph_eta [category] = nullptr;
      inputTree[category]->SetBranchAddress("ph_eta", &ph_eta[category]);

      ph_phi [category] = nullptr;
      inputTree[category]->SetBranchAddress("ph_phi", &ph_phi[category]);

      ph_e [category] = nullptr;
      inputTree[category]->SetBranchAddress("ph_e", &ph_e[category]);
      /* -- */

      /* THIS WORKS! * /
      std::string var;

      ph_pt [category] = nullptr;
      var = "ph_pt";
      inputTree[category]->SetBranchAddress(var.c_str(), &ph_pt[category]);

      ph_eta [category] = nullptr;
      var = "ph_eta";
      inputTree[category]->SetBranchAddress(var.c_str(), &ph_eta[category]);

      ph_phi [category] = nullptr;
      var = "ph_phi";
      inputTree[category]->SetBranchAddress(var.c_str(), &ph_phi[category]);

      ph_e [category] = nullptr;
      var = "ph_e";
      inputTree[category]->SetBranchAddress(var.c_str(), &ph_e[category]);
      /* -- */
    }

    /*
    for (const auto& category : categories) {
      std::cout << "===" << category << "===" << std::endl;
      std::cout << ph_pt[category]->size() << std::endl;
      for (unsigned i = 0; i < 10; i ++) {
	inputTree[category]->GetEntry(i);
	std::cout << ph_pt[category]->size() << std::endl;
      }
    }
    */
    
    // Trimmed large-radius jet kinematic quantities
    std::map<std::string, std::vector<float>* > lj_pt, lj_eta, lj_phi, lj_m;
    //std::map<std::string, std::unique_ptr< std::vector<float> > > lj_pt, lj_eta, lj_phi, lj_m;
    for (const auto& category : categories) {
      lj_pt [category] = nullptr;
      inputTree[category]->SetBranchAddress("rljet_pt", &lj_pt [category]);
      lj_eta[category] = nullptr;
      inputTree[category]->SetBranchAddress("rljet_eta", &lj_eta[category]);
      lj_phi[category] = nullptr;
      inputTree[category]->SetBranchAddress("rljet_phi", &lj_phi[category]);
      lj_m  [category] = nullptr;
      inputTree[category]->SetBranchAddress("rljet_m", &lj_m  [category]);
      /*
	lj_pt [category] = readBranchVector< float >(inputTree[category], "rljet_pt");
      lj_eta[category] = readBranchVector< float >(inputTree[category], "rljet_eta");
      lj_phi[category] = readBranchVector< float >(inputTree[category], "rljet_phi");
      lj_m  [category] = readBranchVector< float >(inputTree[category], "rljet_m");
      */
    }
    
    // Small-radius jet kinematic quantities
    std::map<std::string, std::vector<float>* > sj_pt, sj_eta, sj_phi, sj_e;
    //std::map<std::string, std::unique_ptr< std::vector<float> > > sj_pt, sj_eta, sj_phi, sj_e;
    for (const auto& category : categories) {
      sj_pt [category] = nullptr;
      inputTree[category]->SetBranchAddress("jet_pt", &sj_pt [category]);
      sj_eta[category] = nullptr;
      inputTree[category]->SetBranchAddress("jet_eta", &sj_eta[category]);
      sj_phi[category] = nullptr;
      inputTree[category]->SetBranchAddress("jet_phi", &sj_phi[category]);
      sj_e  [category] = nullptr;
      inputTree[category]->SetBranchAddress("jet_e", &sj_e  [category]);
      /*
      sj_pt [category] = readBranchVector< float >(inputTree[category], "jet_pt");
      sj_eta[category] = readBranchVector< float >(inputTree[category], "jet_eta");
      sj_phi[category] = readBranchVector< float >(inputTree[category], "jet_phi");
      sj_e  [category] = readBranchVector< float >(inputTree[category], "jet_e");
      */
    }
    
    // Trimmed jet substructure quantities
    std::map<std::string, std::vector<float>* > lj_tau21, lj_D2;
    //std::map<std::string, std::unique_ptr< std::vector<float> > > lj_tau21, lj_D2;
    for (const auto& category : categories) {
      lj_tau21 [category] = nullptr;
      inputTree[category]->SetBranchAddress("rljet_Tau21_wta", &lj_tau21 [category]);
      lj_D2[category] = nullptr;
      inputTree[category]->SetBranchAddress("rljet_D2", &lj_D2[category]);
      /*
      lj_tau21[category] = readBranchVector< float >(inputTree[category], "rljet_Tau21_wta");
      lj_D2   [category] = readBranchVector< float >(inputTree[category], "rljet_D2");
      */
    }
    
    // Trigger information
    std::map<std::string, std::unique_ptr<bool> > HLT_g140_loose, HLT_j380, HLT_j400;
    for (const auto& category : categories) {
      HLT_g140_loose[category] = readBranch< bool >(inputTree[category], "HLT_g140_loose");
      HLT_j380      [category] = readBranch< bool >(inputTree[category], "HLT_j380");
      HLT_j400      [category] = readBranch< bool >(inputTree[category], "HLT_j400");
    }
    
    // Pre-pre-selection
    std::map<std::string, std::unique_ptr<int> > ISRgamma, ISRjet;
    for (const auto& category : categories) {
      ISRgamma     [category] = readBranch< int >(inputTree[category], "ISRgamma");
      ISRjet       [category] = readBranch< int >(inputTree[category], "ISRjet");
    }
    
    
    // Get GRL.
    // -------------------------------------------------------------------       
    GRL grl2015 ("share/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.txt");
    GRL grl2016 ("share/GRL/data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.txt");
    
    
    // Get file name.
    // -------------------------------------------------------------------
    inputTree[categories.at(0)]->GetEvent(0);
    
    bool     isMC = (*mcChannelNumber[categories.at(0)] > 0);
    unsigned DSID = (isMC ? *mcChannelNumber[categories.at(0)] : *runNumber[categories.at(0)]);
    
    const string filedir  = "outputObjdef";
    const string filename = (string) "objdef_" + (isMC ? "MC" : "data") + "_" + to_string(DSID) + ".root";
    
    
    // Get MC event weight.
    // -------------------------------------------------------------------
    float weightDefault = 1.;
    std::map<std::string, float*> weight;
    for (const auto& category : categories) {
      if (isMC) {
	weight[category] = weight_mc[category].get(); 
      } else {
	weight[category] = &weightDefault;
      }
    }
    
    // Set up AnalysisTools
    // -------------------------------------------------------------------       
    Analysis ISRgammaAnalysis ("BoostedJet+ISRgamma");
    
    ISRgammaAnalysis.addCategories(categories);
    
    const bool debug = false;
    
    std::vector<Analysis*> analyses = { &ISRgammaAnalysis }; 
    for (auto* analysis : analyses) {
      analysis->setDebug(debug);
    }
    
    ISRgammaAnalysis.openOutput(filedir + "/" + filename);
    
    for (auto* analysis : analyses) {
      for (const auto& category : categories) {
	analysis->setWeight(weight[category], category);
      }
    }
    
    const bool blind = true;
    
    
    // Set up output branches.
    // -------------------------------------------------------------------
    std::map<std::string, std::vector<TLorentzVector> > signalPhotons, signalLargeRadiusJets, signalSmallRadiusJets;
    std::map<std::string, std::vector<float> > signalLargeRadiusJets_tau21DDT;
    std::map<std::string, std::map<std::string, bool> > passes;
    for (const auto& category : categories) {
      passes[category]["Pass"] = false;
      passes[category]["Fail"] = false;
    }
    
    for (auto* analysis : analyses) {
      for (const auto& category : categories) {
	analysis->tree(category)->Branch("photons",         &signalPhotons[category],         32000, 0); /* Suppresses "TTree::Bronch" warnings */
	analysis->tree(category)->Branch("largeRadiusJets", &signalLargeRadiusJets[category], 32000, 0);
	analysis->tree(category)->Branch("smallRadiusJets", &signalSmallRadiusJets[category], 32000, 0);

	analysis->tree(category)->Branch("largeRadiusJets_tau21DDT", &signalLargeRadiusJets_tau21DDT[category]);

	analysis->tree(category)->Branch("Pass", &passes[category]["Pass"]);
	analysis->tree(category)->Branch("Fail", &passes[category]["Fail"]);
	
	analysis->tree(category)->Branch("isMC",   &isMC);
	analysis->tree(category)->Branch("DSID",   &DSID);
      }
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
      const float p0 =  0.6887;
      const float p1 = -0.0941;
      const float rhoDDTmin = 1.0;
      auto linearCorrection = [&p0, &p1] (const float& x) { return p0 + p1 * x; };	  
      
      // (DDT) modified tau21 value.
      return p.info("tau21") + (linearCorrection(rhoDDTmin) - linearCorrection(rhoDDT(p)));
    };
    
    
    // Leading largeRadiusJet rhoPrime.
    PlotMacro1D<Event> plot_event_leadingLargeRadiusJet_rhoPrime ("leading_LargeRadiusJets_rhoPrime");
    plot_event_leadingLargeRadiusJet_rhoPrime.setFunction( [&rhoPrime](const Event& e) { 
	if (e.collection("LargeRadiusJets").size() == 0) { return -9999.; }
	return rhoPrime(*e.collection("LargeRadiusJets").at(0)); 
      });
    
    // Leading largeRadiusJet rhoDDT.
    PlotMacro1D<Event> plot_event_leadingLargeRadiusJet_rhoDDT ("leading_LargeRadiusJets_rhoDDT");
    plot_event_leadingLargeRadiusJet_rhoDDT.setFunction( [&rhoDDT](const Event& e) { 
	if (e.collection("LargeRadiusJets").size() == 0) { return -9999.; }
	return rhoDDT(*e.collection("LargeRadiusJets").at(0)); 
      });
    
    // Delta-eta separation between photon and leading largeRadiusJet.
    PlotMacro1D<Event> plot_event_leadingLargeRadiusJetPhotonDeltaEta ("leadingLargeRadiusJetPhotonDeltaEta", [](const Event& e) { 
	if (e.collection("LargeRadiusJets").size() == 0) { return -9999.; }
	if (e.collection("Photons").size() == 0) { return  9999.; }
	return std::fabs(e.collection("LargeRadiusJets").at(0)->Eta() - e.collection("Photons").at(0)->Eta()); 
      });
    
    // Delta-eta separation between photon and recoil system (all fat jets).
    PlotMacro1D<Event> plot_event_recoilPhotonDeltaEta ("recoilPhotonDeltaEta", [](const Event& e) {
	TLorentzVector recoil;
	if (e.collection("Photons").size() == 0) { return 9999.; }
	for (unsigned i = 0; i < e.collection("LargeRadiusJets").size(); i++) {
	  recoil += *e.collection("LargeRadiusJets").at(i);
	}
	return std::fabs(recoil.Eta() - e.collection("Photons").at(0)->Eta());
      });
    
    // Physics object rhoDDT
    PlotMacro1D<PhysicsObject> plot_object_rhoDDT ("rhoDDT");
    plot_object_rhoDDT.setFunction([&rhoDDT](const PhysicsObject& p) { 
	return rhoDDT(p);
      });
    
    
    // Pseudo-objdefs (for creating collections).
    // -------------------------------------------------------------------
    // * All largeRadiusJets.
    PseudoObjectDefinition<TLorentzVector> AllLargeRadiusJetsObjdef("AllLargeRadiusJets");
    AllLargeRadiusJetsObjdef.setInput(&largeRadiusJets);

    // * All photons.
    PseudoObjectDefinition<TLorentzVector> AllPhotonsObjdef("AllPhotons");
    AllPhotonsObjdef.setInput(&photons);    
    

    // Pre-selection
    // -------------------------------------------------------------------
    EventSelection preSelection ("PreSelection");
    preSelection.setDebug(debug);	
    
    preSelection.addInfo("isMC", &isMC);
    
    preSelection.addCollection("LargeRadiusJets", "AllLargeRadiusJets");
    preSelection.addCollection("Photons", "AllPhotons");
    
    
    // * Initial
    /* Important, in order to use the right trigger! */
    preSelection.addCut(get_cut_event_info("ISRgamma"));
    
    
    // * GRL
    Cut<Event> event_grl ("GRL");
    event_grl.setFunction( [&grl2015, &grl2016](const Event& e) { 
	return e.info("isMC") || grl2015.contains(e.info("runNumber"), e.info("lumiBlock")) 
	                      || grl2016.contains(e.info("runNumber"), e.info("lumiBlock"));
      });
    preSelection.addCut(event_grl);
    
    
    // * Event cleaning
    // ...
    
    // * Jet cleaning
    // ...
    
    // * Trigger
    preSelection.addCut(get_cut_event_info("HLT_g140_loose"));
    
    // * Trigger efficiency turn-on vs. leading jet pt.
    // ...
    
    
    
    
    // Object definitions
    // -------------------------------------------------------------------
    
    // Photons
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ObjectDefinition<TLorentzVector> PhotonObjdef ("Photons");
    
    PhotonObjdef.setInput(&photons);
    
    // * pT
    PhotonObjdef.addCut(cut_pt.withRange(155., inf));
    
    // * Check distributions.
    PhotonObjdef.addPlot(CutPosition::Post, plot_object_pt);
    PhotonObjdef.addPlot(CutPosition::Post, plot_object_eta);
    PhotonObjdef.addPlot(CutPosition::Post, plot_object_phi);
    
    
    // Small-radius jets.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ObjectDefinition<TLorentzVector> SmallRadiusJetObjdef ("SmallRadiusJets");
    
    SmallRadiusJetObjdef.setInput(&smallRadiusJets);
    
    // * eta
    SmallRadiusJetObjdef.addCut(cut_eta.withRange(-2.5, 2.5));
    
    // * pt
    SmallRadiusJetObjdef.addCut(cut_pt.withRange(25., inf));
    
    
    // Large-radius jets
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ObjectDefinition<TLorentzVector> LargeRadiusJetObjdef ("LargeRadiusJets");
    
    LargeRadiusJetObjdef.setInput(&largeRadiusJets);
    
    LargeRadiusJetObjdef.addInfo("dPhiPhoton", &lj_dphiMinPhoton);
    
    // * eta
    LargeRadiusJetObjdef.addCut(cut_eta.withRange(-2.5, 2.5));
    
    // * pt
    LargeRadiusJetObjdef.addCut(cut_pt.withRange(150., inf));
    
    // * Boosted regime
    Cut<PhysicsObject> cut_largeRadiusJet_BoostedRegime ("BoostedRegime");
    cut_largeRadiusJet_BoostedRegime.setFunction( [](const PhysicsObject& p) { 
	return p.Pt() / (2. * p.M());
      });
    cut_largeRadiusJet_BoostedRegime.addRange(1., inf);
    LargeRadiusJetObjdef.addCut(cut_largeRadiusJet_BoostedRegime);
    
    // * rhoDDT
    Cut<PhysicsObject> cut_largeRadiusJet_rhoDDT ("rhoDDT");
    cut_largeRadiusJet_rhoDDT.setFunction( [&rhoDDT](const PhysicsObject& p) { 
	return rhoDDT(p);
      });
    cut_largeRadiusJet_rhoDDT.addRange(1.5, inf);
    LargeRadiusJetObjdef.addCut(cut_largeRadiusJet_rhoDDT);

    // * dphi
    LargeRadiusJetObjdef.addCut(get_cut_object_info("dPhiPhoton").withRange(2.*pi/3., inf));
    
    // * computing tau21DDT value
    Operation<PhysicsObject> operation_largeRadiusJet_tau21DDT ("compute_tau21DDT");
    operation_largeRadiusJet_tau21DDT.setFunction( [&tau21DDT](PhysicsObject& p) { 
	p.addInfo("tau21DDT", tau21DDT(p));
	return true;
      });
    LargeRadiusJetObjdef.addOperation(operation_largeRadiusJet_tau21DDT);
    
    // * Check distributions.
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, plot_object_pt);
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, plot_object_m);
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, plot_object_eta);
    
    
    // Stuff for binding selections together.
    // -------------------------------------------------------------------
    // Doesn't work -- need to point to the _copies_ stored in the analysis.
    /**
     * @TODO: Switch _back_ to pointer-based selections management? It's sooo much more easy and transparent...
     */
    //PhysicsObjects* SelectedPhotons         = PhotonObjdef.result();
    //PhysicsObjects* SelectedLargeRadiusJets = LargeRadiusJetObjdef.result("Nominal");
    //PhysicsObjects* SelectedSmallRadiusJets = SmallRadiusJetObjdef.result();
    
    
    // Event selection (nominal)
    // -------------------------------------------------------------------
    EventSelection eventSelection ("EventSelection");
    eventSelection.setDebug(debug);
    
    eventSelection.addInfo("isMC", &isMC);
    
    eventSelection.addCategories({"Pass", "Fail"});
    
    eventSelection.addCollection("Photons", "Photons");
    eventSelection.addCollection("LargeRadiusJets", "LargeRadiusJets");
    
    // * Photon count
    eventSelection.addCut(get_cut_num("Photons").withRange(1));
    
    // * LargeRadiusJet count
    eventSelection.addCut(get_cut_num("LargeRadiusJets").withRange(1,inf));
    
    // * Blinding.
    Cut<Event> cut_event_blinding ("Blinding");
    cut_event_blinding.setFunction( [&blind](const Event& e){
	return (!blind or e.info("isMC"));
      });
    eventSelection.addCut(cut_event_blinding, "Pass");
    
    // * tau21DDT
    Cut<Event> cut_event_leadingLargeRadiusJet_tau21DDT ("tau21DDT_0p50");
    cut_event_leadingLargeRadiusJet_tau21DDT.setFunction( [](const Event& e) {
	return e.collection("LargeRadiusJets").at(0)->info("tau21DDT");
      });
    cut_event_leadingLargeRadiusJet_tau21DDT.setRange(0.5, inf);
    eventSelection.addCut(cut_event_leadingLargeRadiusJet_tau21DDT, "Fail");
    
    cut_event_leadingLargeRadiusJet_tau21DDT.setRange(-inf, 0.5);
    eventSelection.addCut(cut_event_leadingLargeRadiusJet_tau21DDT, "Pass");
    
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_pt ("LargeRadiusJets"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_m  ("LargeRadiusJets"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_eta("LargeRadiusJets"));    
    eventSelection.addPlot(CutPosition::Post, plot_event_leadingLargeRadiusJet_rhoDDT);
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_info("LargeRadiusJets", "tau21DDT"));
    
    
    // Adding selections.
    // -------------------------------------------------------------------
    
    for (const auto& category : categories) {
      
      // Pseudo-object definitions.
      // -- Large-radius jets
      ISRgammaAnalysis.addSelection(&AllLargeRadiusJetsObjdef, category);
      
      // -- Photons
      ISRgammaAnalysis.addSelection(&AllPhotonsObjdef, category);
      
      // Preselection.
      preSelection.addInfo("HLT_j380",       HLT_j380      [category].get(), true);
      preSelection.addInfo("HLT_j400",       HLT_j400      [category].get(), true);
      preSelection.addInfo("HLT_g140_loose", HLT_g140_loose[category].get(), true);
      preSelection.addInfo("ISRgamma",       ISRgamma      [category].get(), true);
      preSelection.addInfo("runNumber",      runNumber     [category].get(), true);
      preSelection.addInfo("lumiBlock",      lumiBlock     [category].get(), true);
      ISRgammaAnalysis.addSelection(&preSelection, category);
      
      // Object definitions.
      // -- Small-radius jets
      ISRgammaAnalysis.addSelection(&SmallRadiusJetObjdef, category);
      
      // -- Large-radius jets
      LargeRadiusJetObjdef.addInfo("tau21",    lj_tau21  [category], true);
      ISRgammaAnalysis.addSelection(&LargeRadiusJetObjdef, category);
      
      // -- Photons
      ISRgammaAnalysis.addSelection(&PhotonObjdef, category);
      
      // Event selection.
      ISRgammaAnalysis.addSelection(&eventSelection, category);	
      
    }
    
    
    // Event loop.
    // -------------------------------------------------------------------
    
    for (const auto& category : categories) {

      // Duplicate event control.
      map<unsigned, set<unsigned> > uniqueEvents;
      
      // Loop events
      for (unsigned iEvent = 0; iEvent < nEvents[category]; iEvent++) {
	
	inputTree[category]->GetEvent(iEvent);
	
	// Reject duplicate events.
	auto ret = uniqueEvents[DSID].emplace(*eventNumber[category]);
	if (!ret.second) { continue; }
	
	// Build input collections.
	photons         = createFourVectorsE(ph_pt[category], ph_eta[category], ph_phi[category], ph_e[category]);  
	smallRadiusJets = createFourVectorsE(sj_pt[category], sj_eta[category], sj_phi[category], sj_e[category]);
	largeRadiusJets = createFourVectorsM(lj_pt[category], lj_eta[category], lj_phi[category], lj_m[category]);

	// Compute delta-phi separation between large-radius jets and leading photon
	lj_dphiMinPhoton.clear();
	for (const TLorentzVector& jet : largeRadiusJets) {
	  if (photons.size() > 0) {
	    lj_dphiMinPhoton.push_back(std::abs(jet.DeltaPhi(photons.at(0))));
	  } else {
	    lj_dphiMinPhoton.push_back(9999.);
	  }
	}

	
	// Run AnalysisTools.
	for (auto* analysis : analyses) {
	  
	  bool status = analysis->run(category, iEvent, nEvents[category], DSID);
	  
	  // If event doesn't pass selection, do not proceed (e.g. to write objects to file).
	  if (!status) { continue; }
	  
	  // Fill output branches
	  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	  
	  // Get pointer to object definitions
	  ObjectDefinition<TLorentzVector> *pObjdef, *pPhotonsObjdef, *pSmallRadiusJetsObjdef, *pLargeRadiusJetsObjdef;
	  for (const auto& pSelection : analysis->selections(category)) {
	    pObjdef = nullptr;
	    if (pObjdef = dynamic_cast<ObjectDefinition<TLorentzVector>*>(pSelection.get())) {
	      if (pObjdef->name() == "Photons") {
		pPhotonsObjdef = pObjdef;
	      }
	      if (pObjdef->name() == "SmallRadiusJets") {
		pSmallRadiusJetsObjdef = pObjdef;
	      }
	      if (pObjdef->name() == "LargeRadiusJets") {
		pLargeRadiusJetsObjdef = pObjdef;
	      }
	    }
	  }

	  /* -- */
	  // Category
	  ISelection* selection = analysis->selections(category).back().get();
	  for (const auto& selection_category : selection->categories()) {
	    passes[category][selection_category] = selection->passes(selection_category);
	  }
	  
	  // Photons
	  signalPhotons[category].clear();            
	  assert( pPhotonsObjdef );
	  for (const PhysicsObject& p : *pPhotonsObjdef->result()) {
	    signalPhotons[category].push_back( (TLorentzVector) p );
	  }
	  
	  // Large-radius jets
	  signalLargeRadiusJets         [category].clear();            
	  signalLargeRadiusJets_tau21DDT[category].clear();            
	  assert( pLargeRadiusJetsObjdef );
	  for (const PhysicsObject& p : *pLargeRadiusJetsObjdef->result()) {
	    signalLargeRadiusJets         [category].push_back( (TLorentzVector) p );
	    signalLargeRadiusJets_tau21DDT[category].push_back( p.info("tau21DDT") );
	  }
	  
	  // Small-radius jets
	  signalSmallRadiusJets[category].clear();            
	  assert( pSmallRadiusJetsObjdef );
	  for (const PhysicsObject& p : *pSmallRadiusJetsObjdef->result()) { 
	    signalSmallRadiusJets[category].push_back( (TLorentzVector) p );
	  }
	  /* -- */
	  
	  // Write to output tree.
	  analysis->writeTree(category);
		  
	} // end loop: events
      
      } // end loop: analyses (SWITCH?)

    } // end loop: categories (SWTICH?)
      
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
  
