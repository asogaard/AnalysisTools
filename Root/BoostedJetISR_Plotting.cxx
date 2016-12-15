// STL include(s).
#include <string>
#include <vector>
#include <iostream>

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

    
    // Get input files. (@TODO: Turn into utility function.)
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
    
    
    // Setup PlottingHelper.
    string outdir  = "outputPlotting/";

    TFile outfile ((outdir + "plotting.root").c_str(), "RECREATE");

    { // Restricted scope.
      PlottingHelper<TH1F> ph ("BoostedJetISR/Photons/Nominal/Cutflow", inputs);
      ph.setOutfile(&outfile);
      ph.setYaxisTitle("Events");
      ph.setPrintLines({"HLT_j380"});
      ph.draw();
    } // end: restricted scope 

    { // Restricted scope.
      PlottingHelper<TH1F> ph ("BoostedJetISR/Fatjets/Nominal/Cutflow", inputs);
      ph.setOutfile(&outfile);
      ph.setYaxisTitle("Events");
      ph.setPrintLines({"Trimmed anti-k_{t}^{R=1.0} jets", "HLT_j380", "Req. 1 #gamma, p_{T} > 155 GeV"});
      ph.draw();
    } // end: restricted scope 

    { // Restricted scope.
      PlottingHelper<TH1F> ph ("BoostedJetISR/EventSelection/Pass/Cutflow", inputs);
      ph.setOutfile(&outfile);
      ph.setYaxisTitle("Events");
      ph.setPrintLines({"Trimmed anti-k_{t}^{R=1.0} jets", "HLT_j380", "Req. 1 #gamma, p_{T} > 155 GeV", "Req. #geq 1 jet, p_{T} > 400 GeV", "#Delta#phi(J,#gamma) > #pi/2"});
      ph.draw();
    } // end: restricted scope 

    std::vector<std::string> lines = {"Trimmed anti-k_{t}^{R=1.0} jets", "HLT_j380", "Req. 1 #gamma, p_{T} > 155 GeV", "Req. #geq 1 jet, p_{T} > 400 GeV", "#Delta#phi(J,#gamma) > #pi/2", "Leading jet p_{T} > 2 M"};

    { // Restricted scope.
      PlottingHelper<TH1F> ph ("BoostedJetISR/EventSelection/Pass/BoostedRegime/Postcut:leadingfatjet_pt", inputs);
      ph.setOutfile(&outfile);
      ph.setAxis(50, 0, 1500.);
      ph.setXaxisTitle("Leading jet p_{T} [GeV]");
      ph.setYaxisTitle("Events");
      ph.setPrintLines(lines);
      ph.draw();
    } // end: restricted scope 


    { // Restricted scope.
      PlottingHelper<TH1F> ph ("BoostedJetISR/EventSelection/Pass/rhoDDT/Precut:CutVariable", inputs);
      ph.setOutfile(&outfile);
      ph.setAxis(50, -2., 8.);
      ph.setXaxisTitle("Leading jet #rho^{DDT}");
      ph.setYaxisTitle("Events");
      ph.setPrintLines(lines);
      ph.draw();
    } // end: restricted scope 

    lines.push_back("#rho^{DDT} > 1");

    { // Restricted scope.
      PlottingHelper<TH1F> ph ("BoostedJetISR/EventSelection/Fail/tau21_mod_rhoDDT/Precut:CutVariable", inputs);
      ph.setOutfile(&outfile);
      ph.setAxis(50, 0, 1.5);
      ph.setXaxisTitle("Leading jet #tau_{21}^{DDT}");
      ph.setYaxisTitle("Events");
      ph.setPrintLines(lines);
      ph.draw();
    } // end: restricted scope 

    lines.push_back("#tau_{21}^{DDT} > 0.5 (fail)");

    { // Restricted scope.
      PlottingHelper<TH1F> ph ("BoostedJetISR/EventSelection/Fail/tau21_mod_rhoDDT/Postcut:leadingfatjet_m", inputs);
      ph.setOutfile(&outfile);
      ph.setAxis(50, 0, 300.);
      ph.setXaxisTitle("Leading jet mass [GeV]");
      ph.setYaxisTitle("Events");
      ph.setPrintLines(lines);
      ph.setPadding(2.6);
      ph.draw();
    } // end: restricted scope 

    /*
    PlottingHelper ph_rhoPrime ("BoostedJetISR/EventSelection/rhoPrime/rhoPrime/Postcut:CutVariable", inputs);
    ph_rhoPrime.setOutfile(outfile);
    ph_rhoPrime.setAxis(50, -3.5, 4.0);
    ph_rhoPrime.setAxisTitles("Leading jet #rho'", "Events");
    ph_rhoPrime.drawRatioPad(false);
    ph_rhoPrime.setLuminosity(0.0);
    ph_rhoPrime.setNormalised(true);
    ph_rhoPrime.setScaleSignal(1.);
    ph_rhoPrime.setLog(false);
    ph_rhoPrime.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Req. 1 #gamma, p_{T} > 155 GeV", "Req. #geq 1 jet, p_{T} > 150 GeV", "#Delta#phi(J,#gamma) > #pi/2"});
    ph_rhoPrime.draw();

    PlottingHelper ph_rhoDDT ("BoostedJetISR/EventSelection/rhoDDT/rhoDDT/Postcut:CutVariable", inputs);
    ph_rhoDDT.setOutfile(outfile);
    ph_rhoDDT.setAxis(50, -1.0, 8.0);
    ph_rhoDDT.setAxisTitles("Leading jet #rho^{DDT}", "Events");
    ph_rhoDDT.drawRatioPad(false);
    ph_rhoDDT.setLuminosity(0.0);
    ph_rhoDDT.setNormalised(true);
    ph_rhoDDT.setScaleSignal(1.);
    ph_rhoDDT.setLog(false);
    ph_rhoDDT.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Req. 1 #gamma, p_{T} > 155 GeV", "Req. #geq 1 jet, p_{T} > 150 GeV", "#Delta#phi(J,#gamma) > #pi/2"});
    ph_rhoDDT.draw();
    */

    /*
    PlottingHelper ph_tau21ModRhoPrime ("BoostedJetISR/EventSelection/rhoPrime/tau21_mod_rhoPrime/Precut:CutVariable", inputs);
    ph_tau21ModRhoPrime.setOutfile(outfile);
    ph_tau21ModRhoPrime.setAxis(50, 0.0, 1.5);
    ph_tau21ModRhoPrime.setAxisTitles("Leading jet #tilde{#tau}_{21}'", "Events");
    ph_tau21ModRhoPrime.drawRatioPad(false);
    ph_tau21ModRhoPrime.setLuminosity(0.0);
    ph_tau21ModRhoPrime.setNormalised(true); // true
    ph_tau21ModRhoPrime.setScaleSignal(1.);
    ph_tau21ModRhoPrime.setPadding(2.6);
    //ph_tau21ModRhoPrime.computeImprovement(1);
    ph_tau21ModRhoPrime.setLog(false);
    ph_tau21ModRhoPrime.setPrintLines({ 
	"Z'(q#bar{q}) + ISR #gamma",	
	  "Trimmed anti-k_{t}^{R=1.0} jets", 
	  "Req. 1 #gamma, p_{T} > 155 GeV", 
	  "Req. #geq 1 jet, p_{T} > 150 GeV", 
	  / *"#Delta#phi(J,#gamma) > #pi/2", * /
	  "Jet M < p_{T} / 2",
	  "#rho' > -1.5"});
    ph_tau21ModRhoPrime.draw();


    PlottingHelper ph_tau21ModRhoDDT ("BoostedJetISR/EventSelection/rhoDDT/tau21_mod_rhoDDT/Precut:CutVariable", inputs);
    ph_tau21ModRhoDDT.setOutfile(outfile);
    ph_tau21ModRhoDDT.setAxis(50, 0.0, 1.5);
    ph_tau21ModRhoDDT.setAxisTitles("Leading jet #tilde{#tau}_{21}^{DDT}", "Events");
    ph_tau21ModRhoDDT.drawRatioPad(false);
    ph_tau21ModRhoDDT.setLuminosity(0.0);
    ph_tau21ModRhoDDT.setNormalised(true); // true
    ph_tau21ModRhoDDT.setScaleSignal(1.);
    ph_tau21ModRhoDDT.setPadding(2.6);
    //ph_tau21ModRhoDDT.computeImprovement(1);
    ph_tau21ModRhoDDT.setLog(false);
    ph_tau21ModRhoDDT.setPrintLines({
	"Z'(q#bar{q}) + ISR #gamma", 
	  "Trimmed anti-k_{t}^{R=1.0} jets", 
	  "Req. 1 #gamma, p_{T} > 155 GeV", 
	  "Req. #geq 1 jet, p_{T} > 150 GeV", 
	  / *"#Delta#phi(J,#gamma) > #pi/2", * /
	  "Jet M < p_{T} / 2",
	  "#rho^{DDT} > 1"});
    ph_tau21ModRhoDDT.draw();
    */

    /*
    PlottingHelper ph_cutflow_fatjets ("BoostedJetISR/Fatjets/Nominal/Cutflow", inputs);
    ph_cutflow_fatjets.setOutfile(outfile);
    //ph_cutflow_fatjets.setAxis(25, 0., 4.0);
    //ph_cutflow_fatjets.setAxisTitles("|#Delta#eta(#gamma, lead. J)|", "Events");
    ph_cutflow_fatjets.drawRatioPad(false);
    ph_cutflow_fatjets.setLuminosity(0.0);
    //ph_cutflow_fatjets.setLog(false);
    ph_cutflow_fatjets.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_cutflow_fatjets.draw();

    PlottingHelper ph_cutflow_photons ("BoostedJetISR/Photons/Nominal/Cutflow", inputs);
    ph_cutflow_photons.setOutfile(outfile);
    //ph_cutflow_photons.setAxis(25, 0., 4.0);
    //ph_cutflow_photons.setAxisTitles("|#Delta#eta(#gamma, lead. J)|", "Events");
    ph_cutflow_photons.drawRatioPad(false);
    ph_cutflow_photons.setLuminosity(0.0);
    //ph_cutflow_photons.setLog(false);
    ph_cutflow_photons.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_cutflow_photons.draw();

    PlottingHelper ph_cutflow_eventSelection ("BoostedJetISR/EventSelection/Nominal/Cutflow", inputs);
    ph_cutflow_eventSelection.setOutfile(outfile);
    //ph_cutflow_eventSelection.setAxis(25, 0., 4.0);
    //ph_cutflow_eventSelection.setAxisTitles("|#Delta#eta(#gamma, lead. J)|", "Events");
    ph_cutflow_eventSelection.drawRatioPad(false);
    ph_cutflow_eventSelection.setLuminosity(0.0);
    //ph_cutflow_eventSelection.setLog(false);
    ph_cutflow_eventSelection.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_cutflow_eventSelection.draw();
    */


    /*
    PlottingHelper ph_dEta ("BoostedJetISR/EventSelection/Nominal/NumPhotons/Postcut:plot_event_leadingFatjetPhotonDeltaEta", inputs);
    ph_dEta.setOutfile(outfile);
    ph_dEta.setAxis(25, 0., 4.0);
    ph_dEta.setAxisTitles("|#Delta#eta(#gamma, lead. J)|", "Events");
    ph_dEta.drawRatioPad(false);
    ph_dEta.setLuminosity(0.0);
    ph_dEta.setNormalised(true);
    ph_dEta.setLog(false);
    ph_dEta.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Req. 1 #gamma, p_{T} > 155 GeV", "Req. #geq 1 jet, p_{T} > 150 GeV", "#Delta#phi(J,#gamma) > #pi/2"});
    ph_dEta.draw();

    PlottingHelper ph_dEta_improvement ("BoostedJetISR/EventSelection/Nominal/NumPhotons/Postcut:plot_event_leadingFatjetPhotonDeltaEta", inputs);
    ph_dEta_improvement.setOutfile(outfile);
    ph_dEta_improvement.setAxis(25, 0., 4.0);
    ph_dEta_improvement.setAxisTitles("|#Delta#eta(#gamma, lead. J)|", "Events");
    ph_dEta_improvement.drawRatioPad(false);
    ph_dEta_improvement.setLuminosity(0.0);
    ph_dEta_improvement.computeImprovement(1);
    ph_dEta_improvement.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Req. 1 #gamma, p_{T} > 155 GeV", "Req. #geq 1 jet, p_{T} > 150 GeV", "#Delta#phi(J,#gamma) > #pi/2"});
    ph_dEta_improvement.draw();

    PlottingHelper ph_dEtaRecoil ("BoostedJetISR/EventSelection/Nominal/NumPhotons/Postcut:plot_event_recoilPhotonDeltaEta", inputs);
    ph_dEtaRecoil.setOutfile(outfile);
    ph_dEtaRecoil.setAxis(25, 0., 4.0);
    ph_dEtaRecoil.setAxisTitles("|#Delta#eta(#gamma, recoil)|", "Events");
    ph_dEtaRecoil.drawRatioPad(false);
    ph_dEtaRecoil.setLuminosity(0.0);
    ph_dEtaRecoil.setNormalised(true);
    ph_dEtaRecoil.setLog(false);
    ph_dEtaRecoil.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Req. 1 #gamma, p_{T} > 155 GeV", "Req. #geq 1 jet, p_{T} > 150 GeV", "#Delta#phi(J,#gamma) > #pi/2"});
    ph_dEtaRecoil.draw();

    PlottingHelper ph_dEtaRecoil_improvement ("BoostedJetISR/EventSelection/Nominal/NumPhotons/Postcut:plot_event_recoilPhotonDeltaEta", inputs);
    ph_dEtaRecoil_improvement.setOutfile(outfile);
    ph_dEtaRecoil_improvement.setAxis(25, 0., 4.0);
    ph_dEtaRecoil_improvement.setAxisTitles("|#Delta#eta(#gamma, recoil)|", "Events");
    ph_dEtaRecoil_improvement.drawRatioPad(false);
    ph_dEtaRecoil_improvement.setLuminosity(0.0);
    ph_dEtaRecoil_improvement.computeImprovement(1);
    ph_dEtaRecoil_improvement.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Req. 1 #gamma, p_{T} > 155 GeV", "Req. #geq 1 jet, p_{T} > 150 GeV", "#Delta#phi(J,#gamma) > #pi/2"});
    ph_dEtaRecoil_improvement.draw();
    */

    /*
    PlottingHelper ph1 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_pt", inputs);
    ph1.setOutfile(outfile);
    ph1.setAxis(100, 0., 2000.);
    ph1.setAxisTitles("Jet p_{T} [GeV]", "Jets");
    ph1.drawRatioPad(false);
    ph1.setLuminosity(0.0);
    ph1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph1.draw();
    PlottingHelper ph2 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_m", inputs);
    ph2.setOutfile(outfile);
    ph2.setAxis(100, 0., 160.);
    ph2.setAxisTitles("Jet M [GeV]", "Jets");
    ph2.drawRatioPad(false);
    ph2.setLuminosity(0.0);
    ph2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph2.draw();
    */

    /*
    // tau21
    PlottingHelper ph_tau21_1 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_tau21", inputs);
    ph_tau21_1.setOutfile(outfile);
    ph_tau21_1.setAxis(50, 0.0, 1.3);
    ph_tau21_1.setAxisTitles("Jet #tau_{21}", "Jets");
    ph_tau21_1.drawRatioPad(false);
    ph_tau21_1.setLuminosity(0.0);
    ph_tau21_1.setNormalised(true);
    ph_tau21_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV"});
    ph_tau21_1.draw();
    
    PlottingHelper ph_tau21_2 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_tau21", inputs);
    ph_tau21_2.setOutfile(outfile);
    ph_tau21_2.setAxis(50, 0.0, 1.3);
    ph_tau21_2.setAxisTitles("Jet #tau_{21}", "Jets");
    ph_tau21_2.drawRatioPad(false);
    ph_tau21_2.setLuminosity(0.0);
    ph_tau21_2.setNormalised(true);
    ph_tau21_2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "M_{jet} #in  [55, 75] GeV"});
    ph_tau21_2.draw();
    
    PlottingHelper ph_tau21_3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_tau21", inputs);
    ph_tau21_3.setOutfile(outfile);
    ph_tau21_3.setAxis(50, -0.2, 1.3);
    ph_tau21_3.setAxisTitles("Jet #tau_{21}", "Jets");
    ph_tau21_3.drawRatioPad(false);
    ph_tau21_3.setLuminosity(0.0);
    ph_tau21_3.computeImprovement(1);
    //ph_tau21_3.setNormalised(true);
    ph_tau21_3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "M_{jet} #in  [55, 75] GeV"});
    ph_tau21_3.draw();
    
    // tau21mod
    PlottingHelper ph_tau21mod_1 ("BoostedJetISR/Fatjets/tau21mod/dPhi/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_1.setOutfile(outfile);
    ph_tau21mod_1.setAxis(50, -0.5, 2.0);
    ph_tau21mod_1.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_1.drawRatioPad(false);
    ph_tau21mod_1.setLuminosity(0.0);
    ph_tau21mod_1.setNormalised(true);
    ph_tau21mod_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV"});
    ph_tau21mod_1.draw();
    
    PlottingHelper ph_tau21mod_2 ("BoostedJetISR/Fatjets/tau21mod/rhoPrime/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_2.setOutfile(outfile);
    ph_tau21mod_2.setAxis(50, -0.5, 2.0);
    ph_tau21mod_2.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_2.drawRatioPad(false);
    ph_tau21mod_2.setLuminosity(0.0);
    ph_tau21mod_2.setNormalised(true);
    ph_tau21mod_2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "#rho' #in  [-1, 2]"});
    ph_tau21mod_2.draw();
    
    PlottingHelper ph_tau21mod_3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_3.setOutfile(outfile);
    ph_tau21mod_3.setAxis(50, -0.5, 2.0);
    ph_tau21mod_3.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_3.drawRatioPad(false);
    ph_tau21mod_3.setLuminosity(0.0);
    ph_tau21mod_3.setNormalised(true);
    ph_tau21mod_3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV","M_{jet} #in  [55, 75] GeV"});
    ph_tau21mod_3.draw();
    
    PlottingHelper ph_tau21mod_4 ("BoostedJetISR/Fatjets/tau21mod/M/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_4.setOutfile(outfile);
    ph_tau21mod_4.setAxis(50, -0.5, 2.0);
    ph_tau21mod_4.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_4.drawRatioPad(false);
    ph_tau21mod_4.setLuminosity(0.0);
    ph_tau21mod_4.setNormalised(true);
    ph_tau21mod_4.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "#rho' #in  [-1, 2]", "M_{jet} #in  [55, 75] GeV"});
    ph_tau21mod_4.draw();
    
    PlottingHelper ph_tau21mod_5 ("BoostedJetISR/Fatjets/tau21mod/M/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_5.setOutfile(outfile);
    ph_tau21mod_5.setAxis(50, 0., 1.3);
    ph_tau21mod_5.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_5.drawRatioPad(false);
    ph_tau21mod_5.setLuminosity(0.0);
    ph_tau21mod_5.computeImprovement(1);
    //ph_tau21mod_5.setNormalised(true);
    ph_tau21mod_5.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "#rho' #in  [-1, 2]", "M_{jet} #in  [55, 75] GeV"});
    ph_tau21mod_5.draw();
    
    PlottingHelper ph_tau21mod_6 ("BoostedJetISR/Fatjets/tau21mod/D2mod/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_6.setOutfile(outfile);
    ph_tau21mod_6.setAxis(50, 0., 1.3);
    ph_tau21mod_6.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_6.drawRatioPad(false);
    ph_tau21mod_6.setLuminosity(0.0);
    ph_tau21mod_6.computeImprovement(1);
    //ph_tau21mod_6.setNormalised(true);
    ph_tau21mod_6.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "#rho' #in  [-1, 2]", "M_{jet} #in  [55, 75] GeV", "#rho #in  [-6, -1] and #tilde{D}_{2} < 2.4"});
    ph_tau21mod_6.draw();
    
    // D2
    PlottingHelper ph_D2_1 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_D2", inputs);
    ph_D2_1.setOutfile(outfile);
    ph_D2_1.setAxis(50, 0., 5.0);
    ph_D2_1.setAxisTitles("Jet D_{2}^{#beta=1}", "Jets");
    ph_D2_1.drawRatioPad(false);
    ph_D2_1.setLuminosity(0.0);
    ph_D2_1.setNormalised(true);
    ph_D2_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV"});
    ph_D2_1.draw();

    PlottingHelper ph_D2_2 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_D2", inputs);
    ph_D2_2.setOutfile(outfile);
    ph_D2_2.setAxis(50, 0., 5.0);
    ph_D2_2.setAxisTitles("Jet D_{2}^{#beta=1}", "Jets");
    ph_D2_2.drawRatioPad(false);
    ph_D2_2.setLuminosity(0.0);
    ph_D2_2.setNormalised(true);
    ph_D2_2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "M_{jet} #in  [55, 75] GeV"});
    ph_D2_2.draw();

    PlottingHelper ph_D2_3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_D2", inputs);
    ph_D2_3.setOutfile(outfile);
    ph_D2_3.setAxis(50, 0., 5.0);
    ph_D2_3.setAxisTitles("Jet D_{2}^{#beta=1}", "Jets");
    ph_D2_3.drawRatioPad(false);
    ph_D2_3.setLuminosity(0.0);
    ph_D2_3.computeImprovement(1);
    //ph_D2_3.setNormalised(true);
    ph_D2_3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "M_{jet} #in  [55, 75] GeV"});
    ph_D2_3.draw();

    // D2mod
    PlottingHelper ph_D2mod_1 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_1.setOutfile(outfile);
    ph_D2mod_1.setAxis(50, 0., 7.0);
    ph_D2mod_1.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_1.drawRatioPad(false);
    ph_D2mod_1.setLuminosity(0.0);
    ph_D2mod_1.setNormalised(true);
    ph_D2mod_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV"});
    ph_D2mod_1.draw();

    PlottingHelper ph_D2mod_2 ("BoostedJetISR/Fatjets/D2mod/rho/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_2.setOutfile(outfile);
    ph_D2mod_2.setAxis(50, 0., 7.0);
    ph_D2mod_2.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_2.drawRatioPad(false);
    ph_D2mod_2.setLuminosity(0.0);
    ph_D2mod_2.setNormalised(true);
    ph_D2mod_2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "#rho #in  [-6, -1]"});
    ph_D2mod_2.draw();

    PlottingHelper ph_D2mod_3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_3.setOutfile(outfile);
    ph_D2mod_3.setAxis(50, 0., 7.0);
    ph_D2mod_3.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_3.drawRatioPad(false);
    ph_D2mod_3.setLuminosity(0.0);
    ph_D2mod_3.setNormalised(true);
    ph_D2mod_3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "M_{jet} #in  [55, 75] GeV"});
    ph_D2mod_3.draw();

    PlottingHelper ph_D2mod_4 ("BoostedJetISR/Fatjets/D2mod/M/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_4.setOutfile(outfile);
    ph_D2mod_4.setAxis(50, 0., 7.0);
    ph_D2mod_4.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_4.drawRatioPad(false);
    ph_D2mod_4.setLuminosity(0.0);
    ph_D2mod_4.setNormalised(true);
    ph_D2mod_4.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "#rho #in  [-6, -1]","M_{jet} #in  [55, 75] GeV"});
    ph_D2mod_4.draw();

    PlottingHelper ph_D2mod_5 ("BoostedJetISR/Fatjets/D2mod/M/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_5.setOutfile(outfile);
    ph_D2mod_5.setAxis(50, 1.0, 6.0);
    ph_D2mod_5.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_5.drawRatioPad(false);
    ph_D2mod_5.setLuminosity(0.0);
    ph_D2mod_5.computeImprovement(1);
    //ph_D2mod_5.setNormalised(true);
    ph_D2mod_5.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "#rho #in  [-6, -1]","M_{jet} #in  [55, 75] GeV"});
    ph_D2mod_5.draw();

    PlottingHelper ph_D2mod_6 ("BoostedJetISR/Fatjets/D2mod/tau21mod/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_6.setOutfile(outfile);
    ph_D2mod_6.setAxis(50, 1.0, 6.0);
    ph_D2mod_6.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_6.drawRatioPad(false);
    ph_D2mod_6.setLuminosity(0.0);
    ph_D2mod_6.computeImprovement(1);
    //ph_D2mod_6.setNormalised(true);
    ph_D2mod_6.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV", "#rho #in  [-6, -1]","M_{jet} #in  [55, 75] GeV", "#rho' #in  [-1, 2] and #tilde{#tau}_{21} < 0.6"});
    ph_D2mod_6.draw();


    PlottingHelper ph7 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_rho", inputs);
    ph7.setOutfile(outfile);
    ph7.setAxis(50, -15., 5.0);
    ph7.setAxisTitles("Jet #rho", "Jets");
    ph7.drawRatioPad(false);
    ph7.setLuminosity(0.0);
    ph7.setNormalised(true);
    ph7.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV"});
    ph7.draw();

    PlottingHelper ph8 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_rhoDDT", inputs);
    ph8.setOutfile(outfile);
    ph8.setAxis(50, -10., 5.0);
    ph8.setAxisTitles("Jet #rho^{DDT}", "Jets");
    ph8.drawRatioPad(false);
    ph8.setLuminosity(0.0);
    ph8.setNormalised(true);
    ph8.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV"});
    ph8.draw();
    */

    /*
    PlottingHelper ph_rho_1 ("BoostedJetISR/Fatjets/D2mod/rho/Precut:CutVariable", inputs);
    ph_rho_1.setOutfile(outfile);
    ph_rho_1.setAxis(50, -10., 2.);
    ph_rho_1.setAxisTitles("Jet #rho", "Jets");
    ph_rho_1.drawRatioPad(false);
    ph_rho_1.setLuminosity(0.0);
    ph_rho_1.setNormalised(true);
    ph_rho_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV"});
    ph_rho_1.draw();
    
    PlottingHelper ph_rhoPrime_1 ("BoostedJetISR/Fatjets/tau21mod/rhoPrime/Precut:CutVariable", inputs);
    ph_rhoPrime_1.setOutfile(outfile);
    ph_rhoPrime_1.setAxis(50, -5., 7.);
    ph_rhoPrime_1.setAxisTitles("Jet #rho'", "Jets");
    ph_rhoPrime_1.drawRatioPad(false);
    ph_rhoPrime_1.setLuminosity(0.0);
    ph_rhoPrime_1.setNormalised(true);
    ph_rhoPrime_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "Jet p_{#perp}  > 250 GeV"});
    ph_rhoPrime_1.draw();
    */


    // Finalise.
    outfile.Write();
    outfile.Close();
    
    /*
    if (outfile) {
        delete outfile;
        outfile = nullptr;
    }
    */
    
    cout << "---------------------------------------------------------------------" << endl;
    cout << " Done." << endl;
    cout << "=====================================================================" << endl;
    
    return 1;
}

