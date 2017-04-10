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
#include "AnalysisTools/CollectionRetriever.h"
#include "AnalysisTools/EventRetriever.h"
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

  const bool debug = false;
     
  // Load dictionaries and stuff.
  gROOT->ProcessLine(".L share/Loader.C+");
  
  std::vector<std::string> inputs = getDatasetsFromCommandlineArguments(argc, argv);
  
  if (inputs.size() == 0) {
    FCTINFO("Found 0 input files. Exiting.");
    return 0;
  }
  
  // Loop input files.
  for (const std::string& input : inputs) {
    
    // Get input file.
    TFile inputFile(input.c_str(), "READ");
    if ( !inputFile.IsOpen() ) {
      cerr << "Unable to open file." << endl;
      return 0;
    }
    
    // Get input tree(s).
    std::map<std::string, TTree*> inputTree;
    TTree* sumWeightsTree = nullptr;

    try {
      inputTree["Tight"] = retrieveTree("nominal",       &inputFile);
      inputTree["Loose"] = retrieveTree("nominal_Loose", &inputFile);
      sumWeightsTree     = retrieveTree("sumWeights",    &inputFile);
    } catch (...) { 
      FCTWARNING("One or more trees couldn't bee retrieved.");
      continue; 
    }

    // Analysis categories.
    const std::vector<std::string> categories = { "Tight", "Loose" };

    // Get number of events.
    std::map<std::string, unsigned> nEvents;
    bool empty = true;
    for (const auto& pair : inputTree) {
      nEvents[pair.first] = pair.second->GetEntries();
      if (nEvents[pair.first] > 0) { empty = false; }
    }
    if (empty) {
      cout << " -- (File '" << input << "' is empty.)" << endl;
      continue;
    }

    // Pointers to data from retrievers.
    Event* pEvent = nullptr;
    std::vector<PhysicsObject>* pPhotons = nullptr;
    std::vector<PhysicsObject>* pLargeRadiusJets = nullptr;
    std::vector<PhysicsObject>* pSmallRadiusJets = nullptr;

    // Helper functions
    auto rho      = [](const PhysicsObject& p) { 
      return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 2.0));
    };
    auto rhoDDT   = [](const PhysicsObject& p) { 
      return log(pow(p.M() / 1000., 2.0) / pow(p.Pt() / 1000., 1.0));
    };
    
    auto tau21DDT = [&rhoDDT](const PhysicsObject& p) {
      // Linear correction function.
      /* pT > 150 GeV
	 const float p0 =  0.6887;
	 const float p1 = -0.0941;
      */
      /* pT > 200 GeV */
      const float p0 =  0.705;
      const float p1 = -0.100;

      const float rhoDDTmin = 1.0;
      auto linearCorrection = [&p0, &p1] (const float& x) { return p0 + p1 * x; };	  
      
      // (DDT) modified tau21 value.
      return p.info("tau21") + (linearCorrection(rhoDDTmin) - linearCorrection(rhoDDT(p)));
    };
    
    auto dPhiPhoton = [&pPhotons] (const PhysicsObject& p) {
      float dphi = 9999.;
      if (pPhotons->size() > 0) {
	dphi = std::abs(p.DeltaPhi(pPhotons->at(0)));
      } 
      return dphi;
    };

    // Data retrievers
    EventRetriever eventRetriever ({"mcChannelNumber", "HLT_j380", "HLT_j400", "HLT_g140_loose", "ISRgamma", "eventNumber",  "runNumber", "lumiBlock"});
    eventRetriever.addInfo("isMC", [](const Event& e) { return e.info("mcChannelNumber") > 0; });
    eventRetriever.setDebug(debug);
    pEvent = eventRetriever.result();

    CollectionRetriever photonsRetriever (FromPtEtaPhiE(), "ph_");
    photonsRetriever.addInfo({"iso"}, "ph_");
    photonsRetriever.setDebug(debug);
    pPhotons = photonsRetriever.result();

    CollectionRetriever largeRadiusJetsRetriever (FromPtEtaPhiM(), "rljet_");
    largeRadiusJetsRetriever.addInfo({"Tau21_wta", "D2"}, "rljet_");
    largeRadiusJetsRetriever.rename("Tau21_wta", "tau21");
    largeRadiusJetsRetriever.addInfo("rhoDDT",     rhoDDT);
    largeRadiusJetsRetriever.addInfo("tau21DDT",   tau21DDT);
    largeRadiusJetsRetriever.addInfo("dPhiPhoton", dPhiPhoton);
    largeRadiusJetsRetriever.setDebug(debug);
    pLargeRadiusJets = largeRadiusJetsRetriever.result();

    CollectionRetriever smallRadiusJetsRetriever (FromPtEtaPhiE("pt", "eta", "phi", "e"), "jet_");
    smallRadiusJetsRetriever.setDebug(debug);
    pSmallRadiusJets = smallRadiusJetsRetriever.result();
  
    // General, event-level information
    std::map<std::string, std::unique_ptr<float> > weight_mc;
    for (const auto& category : categories) {
      weight_mc[category] = readBranch<float>(inputTree[category], "weight_mc");
      /* @TODO: Pile-up reweighting? */
    }
    
        
    // Get GRL.
    // -------------------------------------------------------------------       
    GRL grl2015 ("share/GRL/data15_13TeV.periodAllYear_DetStatus-v79-repro20-02_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.txt");
    GRL grl2016 ("share/GRL/data16_13TeV.periodAllYear_DetStatus-v83-pro20-15_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.txt");
    
    
    // Get file name.
    // -------------------------------------------------------------------
    eventRetriever.setTree(inputTree[categories.at(0)]);
    inputTree[categories.at(0)]->GetEvent(0);
    eventRetriever.retrieve();
    
    bool     isMC = pEvent->info("isMC");
    unsigned DSID = pEvent->info("isMC") ? pEvent->info("mcChannelNumber") : pEvent->info("runNumber");
    
    const string filedir  = "outputObjdef";
    const string filename = (string) "objdef_" + (isMC ? "MC" : "data") + "_" + to_string(DSID) + ".root";
    
    
    // Get MC event weight.
    // -------------------------------------------------------------------
    float weightDefault = 1.;
    std::map<std::string, float*> weight;
    std::map<std::string, float> sum_weights;
    for (const auto& category : categories) {
      sum_weights[category] = 0.;
      if (isMC) {
	// MC weight
	weight[category] = weight_mc[category].get(); 

	// Sum of weights, for normalisation
	auto part_sum_weights = readBranch<float>(sumWeightsTree, "totalEventsWeighted");
	for (unsigned i = 0; i < sumWeightsTree->GetEntries(); i++) {
	  sumWeightsTree->GetEntry(i);
	  sum_weights[category] += *part_sum_weights.get();
	}
      } else {
	weight[category] = &weightDefault;
      }
    }
    
    // Set up AnalysisTools
    // -------------------------------------------------------------------       
    Analysis ISRgammaAnalysis ("BoostedJet+ISRgamma");
    
    ISRgammaAnalysis.addCategories(categories);
    
    std::vector<Analysis*> analyses = { &ISRgammaAnalysis }; 
    for (auto* analysis : analyses) {
      analysis->setDebug(debug);
    }
    
    ISRgammaAnalysis.openOutput(filedir + "/" + filename);
    
    for (auto* analysis : analyses) {
      for (const auto& category : categories) {
	analysis->setWeight(weight[category], category);
	analysis->setSumWeights(&sum_weights[category]);
      }
    }
    
    const bool blind = true;
    
    
    // Set up output branches.
    // -------------------------------------------------------------------
    std::map<std::string, std::vector<TLorentzVector> > signalPhotons, signalLargeRadiusJets, signalSmallRadiusJets;
    std::map<std::string, std::vector<float> > signalLargeRadiusJets_tau21DDT;
    std::map<std::string, std::map<std::string, bool> > passes;
    std::map<std::string, float> leadingLargeRadiusJet_tau21DDT, leadingLargeRadiusJet_pt, leadingLargeRadiusJet_eta, photon_iso;
    std::map<std::string, unsigned> numSmallRadiusJetsInTransverseRegion;
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

	analysis->tree(category)->Branch("leadingLargeRadiusJet_tau21DDT", &leadingLargeRadiusJet_tau21DDT[category]);
	analysis->tree(category)->Branch("leadingLargeRadiusJet_pt", &leadingLargeRadiusJet_pt[category]);
	analysis->tree(category)->Branch("leadingLargeRadiusJet_eta", &leadingLargeRadiusJet_eta[category]);
	analysis->tree(category)->Branch("numSmallRadiusJetsInTransverseRegion", &numSmallRadiusJetsInTransverseRegion[category]);

	analysis->tree(category)->Branch("photon_iso", &photon_iso[category]);

	//analysis->tree(category)->Branch("Pass", &passes[category]["Pass"]);
	//analysis->tree(category)->Branch("Fail", &passes[category]["Fail"]);

	analysis->tree(category)->Branch("weight", weight[category]);
	
	analysis->tree(category)->Branch("isMC",   &isMC);
	analysis->tree(category)->Branch("DSID",   &DSID);
      }
    }        
    
    
    // Plotting macros.
    // -------------------------------------------------------------------
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
    
    
    
    // Pseudo-objdefs (for creating collections).
    // -------------------------------------------------------------------
    // * All largeRadiusJets.
    PseudoObjectDefinition<PhysicsObject> AllLargeRadiusJetsObjdef("AllLargeRadiusJets");
    AllLargeRadiusJetsObjdef.setInput(pLargeRadiusJets);

    // * All photons.
    PseudoObjectDefinition<PhysicsObject> AllPhotonsObjdef("AllPhotons");
    AllPhotonsObjdef.setInput(pPhotons);    
    

    // Pre-selection
    // -------------------------------------------------------------------
    EventSelection preSelection ("PreSelection");
    preSelection.setInput(pEvent);
    preSelection.setDebug(debug);	
    
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
    ObjectDefinition<PhysicsObject> PhotonObjdef ("Photons");
    PhotonObjdef.setInput(pPhotons);
    
    // * pT
    PhotonObjdef.addCut(cut_pt.withRange(155., inf));
    
    // * Check distributions.
    PhotonObjdef.addPlot(CutPosition::Post, plot_object_pt);
    PhotonObjdef.addPlot(CutPosition::Post, plot_object_eta);
    PhotonObjdef.addPlot(CutPosition::Post, plot_object_phi);
    
    
    // Small-radius jets.
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ObjectDefinition<PhysicsObject> SmallRadiusJetObjdef ("SmallRadiusJets");
    SmallRadiusJetObjdef.setInput(pSmallRadiusJets);
    
    // * eta
    SmallRadiusJetObjdef.addCut(cut_eta.withRange(-2.5, 2.5));
    
    // * pt
    SmallRadiusJetObjdef.addCut(cut_pt.withRange(25., inf));
    
    
    // Large-radius jets
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    ObjectDefinition<PhysicsObject> LargeRadiusJetObjdef ("LargeRadiusJets");    
    LargeRadiusJetObjdef.setInput(pLargeRadiusJets);
    
    // * eta
    LargeRadiusJetObjdef.addCut(cut_eta.withRange(-2., 2.));
    
    // * pt
    LargeRadiusJetObjdef.addCut(cut_pt.withRange(200., inf));

    // * dphi
    LargeRadiusJetObjdef.addCut(get_cut_object_info("dPhiPhoton").withRange(pi/2., inf));

    // * Boosted regime
    Cut<PhysicsObject> cut_largeRadiusJet_BoostedRegime ("BoostedRegime");
    cut_largeRadiusJet_BoostedRegime.setFunction( [](const PhysicsObject& p) { 
	return p.Pt() / (2. * p.M());
      });
    cut_largeRadiusJet_BoostedRegime.addRange(1., inf);
    LargeRadiusJetObjdef.addCut(cut_largeRadiusJet_BoostedRegime);

    // * rhoDDT
    LargeRadiusJetObjdef.addCut(get_cut_object_info("rhoDDT").withRange(1.5, inf));
    
    // * Check distributions.
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, plot_object_pt);
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, plot_object_eta);
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, plot_object_phi);
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, plot_object_m);
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("rhoDDT"));
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("tau21"));
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("tau21DDT"));
    
    
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
    eventSelection.setInput(pEvent);
    eventSelection.setDebug(debug);
    
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
	return (!blind or e.info("isMC") or e.collection("LargeRadiusJets").at(0)->M() / 1000. < 100.);
      });
    eventSelection.addCut(cut_event_blinding, "Pass");

    // * tau21DDT
    Cut<Event> cut_event_leadingLargeRadiusJet_tau21DDT = get_cut_event_leading_info("LargeRadiusJets", "tau21DDT");
    cut_event_leadingLargeRadiusJet_tau21DDT.setRange(0.5, inf);
    eventSelection.addCut(cut_event_leadingLargeRadiusJet_tau21DDT, "Fail");
    
    cut_event_leadingLargeRadiusJet_tau21DDT.setRange(-inf, 0.5);
    eventSelection.addCut(cut_event_leadingLargeRadiusJet_tau21DDT, "Pass");    

    // * Check distributions
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_pt ("LargeRadiusJets"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_m  ("LargeRadiusJets"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_eta("LargeRadiusJets"));    
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_phi("LargeRadiusJets"));    

    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_info("LargeRadiusJets", "rhoDDT"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_info("LargeRadiusJets", "tau21DDT"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_info("LargeRadiusJets", "tau21"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_info("LargeRadiusJets", "D2"));

    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_E  ("Photons"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_pt ("Photons"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_eta("Photons"));    
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_phi("Photons"));    


    
    
    // Adding selections.
    // -------------------------------------------------------------------
    
    for (const auto& category : categories) {
      
      // Pseudo-object definitions.
      // -- Large-radius jets
      ISRgammaAnalysis.addSelection(&AllLargeRadiusJetsObjdef, category);
      
      // -- Photons
      ISRgammaAnalysis.addSelection(&AllPhotonsObjdef, category);
      
      // Preselection.
      ISRgammaAnalysis.addSelection(&preSelection, category);
      
      // Object definitions.
      // -- Small-radius jets
      ISRgammaAnalysis.addSelection(&SmallRadiusJetObjdef, category);
      
      // -- Large-radius jets
      ISRgammaAnalysis.addSelection(&LargeRadiusJetObjdef, category);
      
      // -- Photons
      ISRgammaAnalysis.addSelection(&PhotonObjdef, category);
      
      // Event selection.
      ISRgammaAnalysis.addSelection(&eventSelection, category);	
      
    }
    
    
    // Event loop.
    // -------------------------------------------------------------------
    
    for (const auto& category : categories) {

      // Set correct retriever trees.
      eventRetriever          .setTree(inputTree[category]);
      photonsRetriever        .setTree(inputTree[category]);
      largeRadiusJetsRetriever.setTree(inputTree[category]);
      smallRadiusJetsRetriever.setTree(inputTree[category]);

      // Duplicate event control.
      map<unsigned, set<unsigned> > uniqueEvents;
      
      // Loop events
      for (unsigned iEvent = 0; iEvent < nEvents[category]; iEvent++) {
	inputTree[category]->GetEvent(iEvent);

	// Retrieve collections and events
	eventRetriever          .retrieve();
	photonsRetriever        .retrieve();
	largeRadiusJetsRetriever.retrieve();
	smallRadiusJetsRetriever.retrieve();

	// Reject duplicate events.
	auto ret = uniqueEvents[DSID].emplace(pEvent->info("eventNumber"));
	if (!ret.second) { continue; }
	
	// Run AnalysisTools.
	for (auto* analysis : analyses) {
	  
	  bool status = analysis->run(category, iEvent, nEvents[category], DSID);
	  
	  // If event doesn't pass selection, do not proceed (e.g. to write objects to file).
	  if (!status) { continue; }
	  
	  // Fill output branches
	  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	  
	  // Get pointer to object definitions
	  ObjectDefinition<PhysicsObject> *pObjdef, *pPhotonsObjdef, *pSmallRadiusJetsObjdef, *pLargeRadiusJetsObjdef;
	  for (const auto& pSelection : analysis->selections(category)) {
	    pObjdef = nullptr;
	    if (pObjdef = dynamic_cast<ObjectDefinition<PhysicsObject>*>(pSelection.get())) {
	      if (pObjdef->name() == "Photons")         { pPhotonsObjdef         = pObjdef; }
	      if (pObjdef->name() == "SmallRadiusJets") { pSmallRadiusJetsObjdef = pObjdef; }
	      if (pObjdef->name() == "LargeRadiusJets") { pLargeRadiusJetsObjdef = pObjdef; }
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
	  photon_iso[category] = -9999.;
	  assert( pPhotonsObjdef );
	  for (const PhysicsObject& p : *pPhotonsObjdef->result()) {
	    signalPhotons[category].push_back( (TLorentzVector) p );
	    photon_iso[category] = p.info("iso");
	  }
	  
	  // Large-radius jets
	  signalLargeRadiusJets         [category].clear();            
	  signalLargeRadiusJets_tau21DDT[category].clear();            
	  assert( pLargeRadiusJetsObjdef );
	  for (const PhysicsObject& p : *pLargeRadiusJetsObjdef->result()) {
	    signalLargeRadiusJets         [category].push_back( (TLorentzVector) p );
	    signalLargeRadiusJets_tau21DDT[category].push_back( p.info("tau21DDT") );
	  }
	  leadingLargeRadiusJet_tau21DDT[category] = pLargeRadiusJetsObjdef->result()->at(0).info("tau21DDT");
	  leadingLargeRadiusJet_pt      [category] = pLargeRadiusJetsObjdef->result()->at(0).Pt() / 1000.;
	  leadingLargeRadiusJet_eta     [category] = pLargeRadiusJetsObjdef->result()->at(0).Eta();
	  
	  // Small-radius jets
	  signalSmallRadiusJets[category].clear();            
	  numSmallRadiusJetsInTransverseRegion[category] = 0;
	  assert( pSmallRadiusJetsObjdef );
	  for (const PhysicsObject& p : *pSmallRadiusJetsObjdef->result()) { 
	    signalSmallRadiusJets[category].push_back( (TLorentzVector) p );
	    float dphi = std::abs(p.DeltaPhi(signalPhotons[category].at(0)));
	    if (dphi > pi/3. && dphi < 2.*pi/3.) { numSmallRadiusJetsInTransverseRegion[category]++; }
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
  
