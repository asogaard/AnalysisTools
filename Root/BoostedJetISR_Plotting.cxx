// STL include(s).
#include <string>
#include <vector>
#include <iostream>
#include <algorithm> /* std::max */

// ROOT include(s).
#include "TFile.h"

// AnalysisTools include(s).
#include "AnalysisTools/Utilities.h"
#include "AnalysisTools/PlottingHelper.h"

using namespace std;
using namespace AnalysisTools;

int main (int argc, char* argv[]) {
    
    cout << "=====================================================================" << endl;
    cout << " Running plotting." << endl;
    cout << "---------------------------------------------------------------------" << endl;
    
    if (argc < 2) {
        cout << "Please provide at least one input file." << endl;
        return 0;
    }

    
    std::vector< std::string > inputs = getDatasetsFromCommandlineArguments(argc, argv);

    
    if (inputs.size() == 0) {
      FCTINFO("Found 0 input files. Exiting.");
      return 0;
    }

    const double lumi2015 = 3.1267209095999995;
    const double lumi2016 = 33.13138242028672;
    const double lumi = lumi2015 + lumi2016;
    
    const float backgroundScale = 1.300;

    bool fullSelection = false;
    bool jetMass       = true;

    // Setup PlottingHelper.
    string outdir  = "outputPlotting/";

    TFile outfile ((outdir + "plotting.root").c_str(), "RECREATE");

    //std::vector<std::string> analyses = { "BoostedJet+ISRgamma", "BoostedJet+ISRjet" };
    std::vector<std::string> analyses = { "BoostedJet+ISRgamma/Tight" };
    //std::vector<std::string> analyses = { "BoostedJet+ISRjet" };

    // Loop analyses.
    for (const std::string& analysis : analyses) {

      // Definitions
      const bool ISRgamma = analysis.find("gamma") < analysis.size();
      const std::string analysis_name = (ISRgamma ? "ISR #gamma" : "ISR jet");
      const std::string jet_name = "Trimmed anti-k_{t}^{R=1.0} jets";


       // Cutflows
      //////////////////////////////////////////////////////////////////////////
      
      // -- Pre-selection cutflow
      if (fullSelection) {
	PlottingHelper<TH1F> ph (analysis + "/PreSelection/Nominal/Cutflow", inputs);
	ph.setOutfile(&outfile);
	ph.setYaxisTitle("Events");
	ph.setPrintLines({jet_name});
	ph.setSortBackgrounds(true);
	ph.setLuminosity(lumi);

	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      }

      // -- Photon cutflow
      if (fullSelection && ISRgamma) {
	PlottingHelper<TH1F> ph (analysis + "/Photons/Nominal/Cutflow", inputs);
	ph.setOutfile(&outfile);
	ph.setYaxisTitle("Events");
	ph.setPrintLines({analysis_name + " pre-selection"});
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      }

      // -- LargeRadiusJet cutflow
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/Cutflow", inputs);
	ph.setOutfile(&outfile);
	ph.setYaxisTitle("Events");
	ph.setPrintLines({jet_name, analysis_name + " pre-selection"});
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      }

      // -- Event selection cutflow
      if (fullSelection) {
	PlottingHelper<TH1F> ph (analysis + "/EventSelection/Pass/Cutflow", inputs);
	ph.setOutfile(&outfile);
	ph.setYaxisTitle("Events");
	ph.setPrintLines({jet_name, analysis_name + " pre-selection"});
	ph.setSortBackgrounds(true);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      }
      

       // Cut variables
      //////////////////////////////////////////////////////////////////////////

      std::vector<std::string> lines;
      unsigned nBins = 50;

       // Photon object definition 
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      if (ISRgamma) {
	
	lines = { analysis_name + " pre-selection" };
	
	// -- pT
	if (fullSelection) { 
	  PlottingHelper<TH1F> ph (analysis + "/Photons/Nominal/pt/Precut:CutVariable", inputs);
	  ph.setOutfile(&outfile);
	  ph.setAxis(nBins, 0, 1500.);
	  ph.setXaxisTitle("Photon p_{T} [GeV]");
	  ph.setYaxisTitle("Photons");
	  ph.setPrintLines(lines);
	  ph.setLuminosity(lumi);
	  ph.setArrowsRight({150.});
	  ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	  ph.draw();
	} 

	// -- Check distribution(s)
	// ---- eta
	if (fullSelection) { 
	  PlottingHelper<TH1F> ph (analysis + "/Photons/Nominal/pt/Postcut:eta", inputs);
	  ph.setOutfile(&outfile);
	  ph.setAxis(nBins, -3, 3);
	  ph.setXaxisTitle("Photon #eta");
	  ph.setYaxisTitle("Photons");
	  ph.setPrintLines(lines);
	  ph.setLuminosity(lumi);
	  ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	  ph.draw();
	} 
	
      }
      
       // LargeRadiusJet object definition 
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

      lines = {	jet_name, analysis_name + " pre-selection" };
      
      // -- eta
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/eta/Precut:CutVariable", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, -3, 3);
	ph.setXaxisTitle("Jet #eta");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setArrowsRight({-2.5});
        ph.setArrowsLeft ({ 2.5});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 

      // -- pt
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/pt/Precut:CutVariable", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 1500);
	ph.setXaxisTitle("Jet p_{T} [GeV]");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setArrowsRight({float(ISRgamma ? 200. : 450.)});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      
      // -- Boosted regime
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/BoostedRegime/Precut:CutVariable", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 4);
	ph.setXaxisTitle("Jet p_{T} / 2 M");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setArrowsRight({1.});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 

      // -- Check distributions
      lines.push_back("Before #rho^{DDT} cut");
      // ---- Jet pT
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/BoostedRegime/Postcut:pt", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 1500);
	ph.setXaxisTitle("Jet p_{T} [GeV]");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 

      // ---- Jet eta
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/BoostedRegime/Postcut:eta", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, -3, 3);
	ph.setXaxisTitle("Jet #eta");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      
      // ---- Jet mass
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/BoostedRegime/Postcut:m", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 300);
	ph.setXaxisTitle("Jet mass [GeV]");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setLog(false);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      
      // ---- Jet tau21
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/BoostedRegime/Postcut:tau21", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, -0.2, 1.0);
	ph.setXaxisTitle("Jet #tau_{21}");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      
      // ---- Jet tau21DDT
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/BoostedRegime/Postcut:tau21DDT", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 1.2);
	ph.setXaxisTitle("Jet #tau_{21}^{DDT}");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      lines.pop_back();      
      
      // -- rhoDDT
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/rhoDDT/Precut:CutVariable", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, -2, 7);
	ph.setXaxisTitle("Jet #rho^{DDT}");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setArrowsRight({1.5});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 

      // -- Check distributions
      lines.push_back("After #rho^{DDT} cut");
      // ---- Jet pT
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/rhoDDT/Postcut:pt", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 1500);
	ph.setXaxisTitle("Jet p_{T} [GeV]");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 

      // ---- Jet eta
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/rhoDDT/Postcut:eta", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, -3, 3);
	ph.setXaxisTitle("Jet #eta");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      
      // ---- Jet mass
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/rhoDDT/Postcut:m", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 300);
	ph.setXaxisTitle("Jet mass [GeV]");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setLog(false);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 

      // ---- Jet tau21
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/rhoDDT/Postcut:tau21", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, -0.2, 1.0);
	ph.setXaxisTitle("Jet #tau_{21}");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 

      // ---- Jet tau21DDT
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/rhoDDT/Postcut:tau21DDT", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 1.2);
	ph.setXaxisTitle("Jet #tau_{21}^{DDT}");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      lines.pop_back();      
      
      // -- dPhiPhoton
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/LargeRadiusJets/Nominal/dPhiPhoton/Precut:CutVariable", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, pi);
	ph.setXaxisTitle("#Delta#phi(Jet, #gamma)");
	ph.setYaxisTitle("Jets");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setArrowsRight({pi/2.});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      

       // Event selection
      // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

      lines = {	jet_name, analysis_name + " pre-selection" };
      
      // -- NumPhotons
      if (fullSelection && ISRgamma) { 
	PlottingHelper<TH1F> ph (analysis + "/EventSelection/Pass/NumPhotons/Precut:CutVariable", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(3, -0.5, 2.5);
	ph.setXaxisTitle("Number of photons");
	ph.setYaxisTitle("Events");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setArrowsDown({1});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
   
      // -- NumLargeRadiusJets
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/EventSelection/Pass/NumLargeRadiusJets/Precut:CutVariable", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(3, -0.5, 2.5);
	ph.setXaxisTitle("Number of jets");
	ph.setYaxisTitle("Events");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setArrowsRight({float(ISRgamma ? 0.5 : 1.5)});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
   
      // -- tau21DDT
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/EventSelection/Pass/NumLargeRadiusJets/Postcut:leading_LargeRadiusJets_tau21DDT", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0., 1.2);
	ph.setXaxisTitle("Leading jet #tau_{21}^{DDT}");
	ph.setYaxisTitle("Events");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleSignal(10);
        ph.setArrowsLeft({float(ISRgamma ? 0.5 : 0.5)});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 

      // -- tau21
      if (fullSelection) { 
	PlottingHelper<TH1F> ph (analysis + "/EventSelection/Pass/NumLargeRadiusJets/Postcut:leading_LargeRadiusJets_tau21", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, -0.2, 1.0);
	ph.setXaxisTitle("Leading jet #tau_{21}");
	ph.setYaxisTitle("Events");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setScaleSignal(10);
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      
      //lines.pop_back();

      // -- mass
      lines.back() = analysis_name + " selection";
      lines.push_back("Before substructure cut");
      std::vector<double> mass_bins = { 
	  0.0,   20.0,   21.0,   22.1,   23.2,   24.4,   25.6,   26.9,   28.2,   29.6,
	 31.1,   32.6,   34.3,   36.0,   37.8,   39.6,   41.6,   43.7,   45.9,   48.2,
	 50.6,   53.1,   55.8,   58.6,   61.5,   64.6,   67.8,   71.2,   74.7,   78.5,
	 82.4,   86.5,   90.8,   95.3,  100.1,  105.1,  110.4,  115.9,  121.7,  127.8,
	134.1,  140.8,  147.9,  155.3,  163.0,  171.2,  179.8,  188.7,  198.2,  208.1,
	218.5,  229.4,  240.9,  252.9,  265.5,  278.8,  300.0
      };
      std::vector<double>::const_iterator first = mass_bins.begin();
      std::vector<double>::const_iterator last  = mass_bins.begin() + 43;
      std::vector<double> mass_bins_red(first, last);
      mass_bins_red.back() = 150.;
      /*
      { 
	PlottingHelper<TH1F> ph (analysis + "/EventSelection/Pass/NumLargeRadiusJets/Postcut:leading_LargeRadiusJets_m", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(30, 0., 150.);
	//ph.setAxis(mass_bins);
	ph.setXaxisTitle("Leading jet mass [GeV]");
	ph.setYaxisTitle("Events");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setSortBackgrounds(false);
	ph.setScaleSignal(1.);
	ph.setScaleLeadingBackground(1.30);
	ph.setLog(false);
	//ph.subtract({"Incl. #gamma"});
	ph.setScaleBackground("Incl. #gamma", backgroundScale); 
	ph.draw();
      } 
      */
      float mass = 80.;
      float w    = 0.2;
      lines.back() = "#tau_{21}^{DDT} < 0.50 (pass)";
      if (jetMass) {
	PlottingHelper<TH1F> ph (analysis + "/EventSelection/Pass/tau21DDT_0p50/Postcut:leading_LargeRadiusJets_m", inputs);
	ph.setOutfile(&outfile);
	//ph.setAxis(60, 0, 300);
	//ph.setAxis(14, 30, 100);
	ph.setAxis(25, 50, 100);
	//ph.setAxis(mass_bins);
	//ph.setAxis(mass_bins_red);
	ph.setXaxisTitle("Leading jet mass [GeV]");
	ph.setYaxisTitle("Events");
	ph.setPrintLines(lines);
	ph.setLuminosity(lumi);
	ph.setSortBackgrounds(false);
	ph.setScaleSignal(1.);
	ph.setLog(false);
	//ph.setPulls(true);
	ph.setIncludeOverflow(false);
	ph.setScaleBackground("#gamma + W", 1.28); 
	ph.setScaleBackground("#gamma + Z", 1.38); 
	
	ph.subtract({"Incl. #gamma (d.d.)"});
	ph.setPlotmax(800.);
	ph.setPlotmin(-100.);

	ph.setArrowsDown({mass * (1 - w), mass * (1 - float(0.5 * w)), mass * ( 1 + float(0.5 * w)), mass * (1 + w)});
	
	/**/
	ph.addSystematic([&mass, &w](TH1F* syst, const TH1F* data, const TH1F* background) { 

	    // Get SR bins
	    std::vector<int> VRbins = {};
	    int sr_low  = const_cast<TH1F*>(data)->FindBin(mass * (1. - 0.5 * w)) + 1;
	    int sr_high = const_cast<TH1F*>(data)->FindBin(mass * (1. + 0.5 * w)) - 1;
	    if (data->GetXaxis()->GetBinUpEdge (sr_low)  <= mass * (1. - 0.5 * w)) { sr_low  += 1; }
	    if (data->GetXaxis()->GetBinLowEdge(sr_high) >= mass * (1. + 0.5 * w)) { sr_high -= 1; }
	    
	    // Get VR bins
	    int vr_low_low   = const_cast<TH1F*>(data)->FindBin(mass * (1. - w)) + 1;
	    int vr_low_high  = sr_low  - 1;
	    int vr_high_low  = sr_high + 1;
	    int vr_high_high = const_cast<TH1F*>(data)->FindBin(mass * (1. + w)) - 1;
	    if (data->GetXaxis()->GetBinUpEdge (vr_low_low)   <= mass * (1. - w)) { vr_low_low   += 1; }
	    if (data->GetXaxis()->GetBinLowEdge(vr_high_high) >= mass * (1. + w)) { vr_high_high -= 1; }

	    FCTINFO("VR: [%.1f, %.1f] u [%.1f, %.1f]", 
		    data->GetXaxis()->GetBinLowEdge(vr_low_low), 
		    data->GetXaxis()->GetBinUpEdge(vr_low_high),
		    data->GetXaxis()->GetBinLowEdge(vr_high_low),
		    data->GetXaxis()->GetBinUpEdge(vr_high_high));
	    FCTINFO("SR: [%.1f, %.1f]", 
		    data->GetXaxis()->GetBinLowEdge(sr_low), 
		    data->GetXaxis()->GetBinUpEdge(sr_high));

	    // Compute delta TF
	    float delta_TF = 0.;
	    float avg_stat = 0.;

	    auto compute_pull = [](const TH1F* h1, const TH1F* h2, const unsigned& bin) {
	      float c1 = h1->GetBinContent(bin);
	      float c2 = h2->GetBinContent(bin);
	      float e1 = h1->GetBinError  (bin);
	      float e2 = h2->GetBinError  (bin);
	      //return std::abs(c1 - c2) / std::sqrt( std::pow(e1, 2) + std::pow(e2, 2) );
	      return std::abs(c1 - c2) / e2;
	    };
	   
	    for (int bin = vr_low_low; bin <= vr_low_high; bin++) {
	      float pull = compute_pull(data, background, bin);
	      delta_TF += pull;
	      avg_stat += background->GetBinError(bin);
	      FCTINFO("Bin %2d in lower  VR: Pull = %.3f (m in [%.1f, %.1f] GeV)", bin, pull, data->GetXaxis()->GetBinLowEdge(bin), data->GetXaxis()->GetBinUpEdge(bin));
	      FCTINFO("  Data:  %.1f", data->GetBinContent(bin));
	      FCTINFO("  Bkg.:  %.1f", background->GetBinContent(bin));
	      FCTINFO("  Ratio: %.3f", data->GetBinContent(bin) / background->GetBinContent(bin));
	    }

	    for (int bin = vr_high_low; bin <= vr_high_high; bin++) {
	      float pull = compute_pull(data, background, bin);
	      delta_TF += pull;
	      avg_stat += background->GetBinError(bin);
	      FCTINFO("Bin %2d in higher VR: Pull = %.3f (m in [%.1f, %.1f] GeV)", bin, pull, data->GetXaxis()->GetBinLowEdge(bin), data->GetXaxis()->GetBinUpEdge(bin));
	      FCTINFO("  Data:  %.1f", data->GetBinContent(bin));
	      FCTINFO("  Bkg.:  %.1f", background->GetBinContent(bin));
	      FCTINFO("  Ratio: %.3f", data->GetBinContent(bin) / background->GetBinContent(bin));
	    }

	    unsigned num_bins = (vr_high_high - vr_high_low + 1) + (vr_low_high - vr_low_low + 1);

	    delta_TF /= float(num_bins);
	    avg_stat /= float(num_bins);

	    FCTINFO("delta_TF = %.2f", delta_TF);
	    FCTINFO("avg_stat = %.2f", avg_stat);

	    for (unsigned bin = sr_low; bin <= sr_high; bin++) {
	      syst->SetBinError(bin, avg_stat * std::max(delta_TF, float(2.)));
	    }

	  });
	/**/

	ph.draw();
      } 


       // Jet mass spectra
      //////////////////////////////////////////////////////////////////////////
      /*
      std::vector<float> tau21DDT_cuts = { 0.50, 0.45, 0.40, 0.35, 0.30, 0.25, 0.20 }; //, 0.10};

      lines.push_back("");
      for (const float& cut : tau21DDT_cuts) {
	std::string cut_name = formatNumber(cut, 3, 1);
	lines.back() = "#tau_{21}^{DDT} < " + cut_name + " (pass)";
	cut_name = replaceAll(cut_name, ".", "p");
	PlottingHelper<TH1F> ph (analysis + "/EventSelection/Pass/tau21DDT_" + cut_name + "/Postcut:leadingfatjet_m", inputs);
	ph.setOutfile(&outfile);
	ph.setAxis(nBins, 0, 300.);
	ph.setXaxisTitle("Leading jet mass [GeV]");
	ph.setYaxisTitle("Events");
	ph.setPrintLines(lines);
	ph.setLog(true);
	ph.setNormalised(true);
	ph.setSortBackgrounds(false);
	ph.setPlotmin(ISRgamma ? 1.0E-05 : 1.0E-03);
	ph.setPlotmax(ISRgamma ? 1.0E+03 : 1.0E+01);
	ph.setLuminosity(lumi);
	ph.draw();
      }
      */
   
    } // end: loop analyses


    // Finalise.
    outfile.Write();
    outfile.Close();
    
    cout << "---------------------------------------------------------------------" << endl;
    cout << " Done." << endl;
    cout << "=====================================================================" << endl;
    
    return 1;
}
