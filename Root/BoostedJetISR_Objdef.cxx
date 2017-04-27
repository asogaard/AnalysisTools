// STL include(s).
#include <string>
#include <vector>
#include <iostream>
#include <cmath> /* log, pow, abs */
#include <algorithm> /* std::sort, std::min */

// ROOT include(s).
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

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

  // Debug level.
  const bool debug = false;

  // Blinding.
  const bool blind = true;

  // Analysis categories.
  const std::vector<std::string> categories = {
    "Nominal" //,
    /*
    "LARGER_JET_Comb_Baseline_Kin__1up",
    "LARGER_JET_Comb_Baseline_Kin__1down",
    "LARGER_JET_Comb_Modelling_Kin__1up",
    "LARGER_JET_Comb_Modelling_Kin__1down",
    "LARGER_JET_Comb_TotalStat_Kin__1up",
    "LARGER_JET_Comb_TotalStat_Kin__1down",
    "LARGER_JET_Comb_Tracking_Kin__1up",
    "LARGER_JET_Comb_Tracking_Kin__1down",
    "LARGER_JET_Rtrk_Baseline_Sub__1up",
    "LARGER_JET_Rtrk_Baseline_Sub__1down",
    "LARGER_JET_Rtrk_Modelling_Sub__1up",
    "LARGER_JET_Rtrk_Modelling_Sub__1down",
    "LARGER_JET_Rtrk_TotalStat_Sub__1up",
    "LARGER_JET_Rtrk_TotalStat_Sub__1down",
    "LARGER_JET_Rtrk_Tracking_Sub__1up",
    "LARGER_JET_Rtrk_Tracking_Sub__1down",
    "PHOTON_EG_RESOLUTION_ALL__1down",
    "PHOTON_EG_RESOLUTION_ALL__1up",
    "PHOTON_EG_SCALE_ALL__1down",
    "PHOTON_EG_SCALE_ALL__1up",
    */
  };
     
  // Load dictionaries and stuff.
  gROOT->ProcessLine(".L share/Loader.C+");

  // Get input files.
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
    
    // Get input tree(s) and metadata.
    std::map<std::string, TTree*> inputTree;
    TH1F* metadataHist = nullptr;

    try {
      for (const auto& category : categories) {
	inputTree[category] = retrieveTree(category, &inputFile);
      }
      metadataHist = retrieveHist<TH1F>("MetaData", &inputFile);
    } catch (...) { 
      FCTWARNING("One or more trees couldn't bee retrieved.");
      continue; 
    }

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
      return log(pow(p.M(), 2.0) / pow(p.Pt(), 2.0));
    };
    auto rhoDDT   = [](const PhysicsObject& p) { 
      return log(pow(p.M(), 2.0) / pow(p.Pt(), 1.0));
    };
    
    auto tau21DDT = [&rhoDDT](const PhysicsObject& p) {
      // Linear correction function.
      const float p0 =  0.687;
      const float p1 = -0.0936;

      const float rhoDDTmin = 1.5;
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
    EventRetriever eventRetriever ({"mcChannelNumber", "eventNumber",  "runNumber", "lumiBlock", "passedTriggers"});
    eventRetriever.addInfo("isMC", [](const Event& e) { return e.info("mcChannelNumber") > 0; });
    eventRetriever.setDebug(debug);
    pEvent = eventRetriever.result();

    CollectionRetriever photonsRetriever (FromPtEtaPhiM(), "ph_");
    //photonsRetriever.addInfo({"isTight"}, "ph_");
    photonsRetriever.setDebug(debug);
    pPhotons = photonsRetriever.result();

    CollectionRetriever largeRadiusJetsRetriever (FromPtEtaPhiE("pt", "eta", "phi", "E"), "fatjet_");
    largeRadiusJetsRetriever.addInfo({"tau21_wta", "D2", "pt_ungroomed", "tau21_wta_ungroomed"}, "fatjet_");
    largeRadiusJetsRetriever.rename("tau21_wta", "tau21");
    largeRadiusJetsRetriever.rename("tau21_wta_ungroomed", "tau21_ungroomed");
    largeRadiusJetsRetriever.addInfo("rho",        rho);
    largeRadiusJetsRetriever.addInfo("rhoDDT",     rhoDDT);
    largeRadiusJetsRetriever.addInfo("tau21DDT",   tau21DDT);
    largeRadiusJetsRetriever.addInfo("dPhiPhoton", dPhiPhoton);
    largeRadiusJetsRetriever.setDebug(debug);
    pLargeRadiusJets = largeRadiusJetsRetriever.result();
  
    // General, event-level information
    std::map<std::string, std::unique_ptr<float> > weight_mc;
    for (const auto& category : categories) {
      weight_mc[category] = readBranch<float>(inputTree[category], "mcEventWeight");
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
	weight     [category] = weight_mc[category].get(); 
	sum_weights[category] = metadataHist->GetBinContent(5);
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
    
    
    // Set up output branches.
    // -------------------------------------------------------------------
    for (auto* analysis : analyses) {
      for (const auto& category : categories) {
	analysis->tree(category)->Branch("weight", weight[category]);	
	analysis->tree(category)->Branch("isMC",   &isMC);
	analysis->tree(category)->Branch("DSID",   &DSID);
      }
    }        
    
    
    // Pre-selection
    // -------------------------------------------------------------------
    EventSelection preSelection ("PreSelection");
    preSelection.setInput(pEvent);
    preSelection.setDebug(debug);	
    
    // * GRL
    Cut<Event> event_grl ("GRL");
    event_grl.setFunction( [&grl2015, &grl2016](const Event& e) { 
	return e.info("isMC") || grl2015.contains(e.info("runNumber"), e.info("lumiBlock")) 
	                      || grl2016.contains(e.info("runNumber"), e.info("lumiBlock"));
      });
    preSelection.addCut(event_grl);
    
    // * Trigger
    preSelection.addCut(get_cut_event_hasInfo("HLT_g140_loose"));
    
    
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
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("rho"));
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("rhoDDT"));
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("tau21"));
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("tau21DDT"));
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("tau21_ungroomed"));
    LargeRadiusJetObjdef.addPlot(CutPosition::Post, get_plot_object_info("pt_ungroomed"));
    
    
    // Event selection.
    // -------------------------------------------------------------------
    EventSelection eventSelection ("EventSelection");
    eventSelection.setInput(pEvent);
    eventSelection.setDebug(debug);
    
    eventSelection.addCategories({"Pass", "Fail"});
    
    eventSelection.addCollection("Photons", "Photons");
    eventSelection.addCollection("LargeRadiusJets", "LargeRadiusJets");
    
    // * OPERATION: Choose lowest-tau21DDT jet
    eventSelection.addOperation("jetAmbiguity", [](Event& e) {
        const PhysicsObject* J = nullptr;
	/* leading
	if (e.collection("LargeRadiusJets").size() > 0) {
	  J = e.collection("LargeRadiusJets").at(0);
	}
	*/
	/* smallest tau21DDT */
        float minTau21DDT = inf;
	for (const PhysicsObject* p : e.collection("LargeRadiusJets")) {
          if (p->info("tau21DDT") < minTau21DDT) {
            minTau21DDT = p->info("tau21DDT");
            J = p;
          }
	} /**/
	if (J) { e.setParticle("Jet", *J); }
        return true;
      });

    // * Photon count
    eventSelection.addCut(get_cut_num("Photons").withRange(1));
    
    // * LargeRadiusJet count
    eventSelection.addCut(get_cut_num("LargeRadiusJets").withRange(1,inf));
    
    // * Blinding.
    Cut<Event> cut_event_blinding ("Blinding");
    cut_event_blinding.setFunction( [&blind](const Event& e){
	return (!blind or e.info("isMC") or e.particle("Jet").M() < 110.);
      });
    eventSelection.addCut(cut_event_blinding, "Pass");

    // * tau21DDT
    Cut<Event> cut_event_leadingLargeRadiusJet_tau21DDT = get_cut_event_particle_info("Jet", "tau21DDT");
    cut_event_leadingLargeRadiusJet_tau21DDT.setRange(0.5, inf);
    eventSelection.addCut(cut_event_leadingLargeRadiusJet_tau21DDT, "Fail");
    
    cut_event_leadingLargeRadiusJet_tau21DDT.setRange(-inf, 0.5);
    eventSelection.addCut(cut_event_leadingLargeRadiusJet_tau21DDT, "Pass");    

    // * Check distributions
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_pt ("Jet"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_m  ("Jet"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_eta("Jet"));    
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_phi("Jet"));    

    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_info("Jet", "rho"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_info("Jet", "rhoDDT"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_info("Jet", "tau21DDT"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_info("Jet", "tau21"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_info("Jet", "D2"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_info("Jet", "tau21_ungroomed"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_particle_info("Jet", "pt_ungroomed"));

    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_E  ("Photons"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_pt ("Photons"));
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_eta("Photons"));    
    eventSelection.addPlot(CutPosition::Post, get_plot_event_leading_phi("Photons"));    

    
    // Adding selections (to all categories).
    // -------------------------------------------------------------------          
    // Preselection.
    ISRgammaAnalysis.addSelection(&preSelection);
    
    // Object definition(s).
    // -- Large-radius jets
    ISRgammaAnalysis.addSelection(&LargeRadiusJetObjdef);
    
    // -- Photons
    ISRgammaAnalysis.addSelection(&PhotonObjdef);
    
    // Event selection.
    ISRgammaAnalysis.addSelection(&eventSelection);
      
    
    // Event loop.
    // -------------------------------------------------------------------
    for (const auto& category : categories) {

      // Set correct retriever trees.
      eventRetriever          .setTree(inputTree[category]);
      photonsRetriever        .setTree(inputTree[category]);
      largeRadiusJetsRetriever.setTree(inputTree[category]);

      // Duplicate event control.
      map<unsigned, set<unsigned> > uniqueEvents;
      
      // Loop events
      for (unsigned iEvent = 0; iEvent < nEvents[category]; iEvent++) {
	inputTree[category]->GetEvent(iEvent);

	// Retrieve collections and events
	eventRetriever          .retrieve();
	photonsRetriever        .retrieve();
	largeRadiusJetsRetriever.retrieve();

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
	      if (pObjdef->name() == "LargeRadiusJets") { pLargeRadiusJetsObjdef = pObjdef; }
	    }
	  }

	  // ...

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
