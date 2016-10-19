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

    TFile* outfile = new TFile((outdir + "plotting.root").c_str(), "RECREATE");

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


    // tau21
    PlottingHelper ph_tau21_1 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_tau21", inputs);
    ph_tau21_1.setOutfile(outfile);
    ph_tau21_1.setAxis(50, 0.0, 1.3);
    ph_tau21_1.setAxisTitles("Jet #tau_{21}", "Jets");
    ph_tau21_1.drawRatioPad(false);
    ph_tau21_1.setLuminosity(0.0);
    ph_tau21_1.setNormalised(true);
    ph_tau21_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_tau21_1.draw();
    
    PlottingHelper ph_tau21_2 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_tau21", inputs);
    ph_tau21_2.setOutfile(outfile);
    ph_tau21_2.setAxis(50, 0.0, 1.3);
    ph_tau21_2.setAxisTitles("Jet #tau_{21}", "Jets");
    ph_tau21_2.drawRatioPad(false);
    ph_tau21_2.setLuminosity(0.0);
    ph_tau21_2.setNormalised(true);
    ph_tau21_2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "M_{jet} #in  [55, 75] GeV"});
    ph_tau21_2.draw();
    
    PlottingHelper ph_tau21_3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_tau21", inputs);
    ph_tau21_3.setOutfile(outfile);
    ph_tau21_3.setAxis(50, -0.2, 1.3);
    ph_tau21_3.setAxisTitles("Jet #tau_{21}", "Jets");
    ph_tau21_3.drawRatioPad(false);
    ph_tau21_3.setLuminosity(0.0);
    ph_tau21_3.computeImprovement(1);
    //ph_tau21_3.setNormalised(true);
    ph_tau21_3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "M_{jet} #in  [55, 75] GeV"});
    ph_tau21_3.draw();
    
    // tau21mod
    PlottingHelper ph_tau21mod_1 ("BoostedJetISR/Fatjets/tau21mod/dPhi/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_1.setOutfile(outfile);
    ph_tau21mod_1.setAxis(50, -0.5, 2.0);
    ph_tau21mod_1.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_1.drawRatioPad(false);
    ph_tau21mod_1.setLuminosity(0.0);
    ph_tau21mod_1.setNormalised(true);
    ph_tau21mod_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_tau21mod_1.draw();
    
    PlottingHelper ph_tau21mod_2 ("BoostedJetISR/Fatjets/tau21mod/rhoPrime/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_2.setOutfile(outfile);
    ph_tau21mod_2.setAxis(50, -0.5, 2.0);
    ph_tau21mod_2.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_2.drawRatioPad(false);
    ph_tau21mod_2.setLuminosity(0.0);
    ph_tau21mod_2.setNormalised(true);
    ph_tau21mod_2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "#rho' #in  [-1, 2]"});
    ph_tau21mod_2.draw();
    
    PlottingHelper ph_tau21mod_3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_3.setOutfile(outfile);
    ph_tau21mod_3.setAxis(50, -0.5, 2.0);
    ph_tau21mod_3.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_3.drawRatioPad(false);
    ph_tau21mod_3.setLuminosity(0.0);
    ph_tau21mod_3.setNormalised(true);
    ph_tau21mod_3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets","M_{jet} #in  [55, 75] GeV"});
    ph_tau21mod_3.draw();
    
    PlottingHelper ph_tau21mod_4 ("BoostedJetISR/Fatjets/tau21mod/M/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_4.setOutfile(outfile);
    ph_tau21mod_4.setAxis(50, -0.5, 2.0);
    ph_tau21mod_4.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_4.drawRatioPad(false);
    ph_tau21mod_4.setLuminosity(0.0);
    ph_tau21mod_4.setNormalised(true);
    ph_tau21mod_4.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "#rho' #in  [-1, 2]", "M_{jet} #in  [55, 75] GeV"});
    ph_tau21mod_4.draw();
    
    PlottingHelper ph_tau21mod_5 ("BoostedJetISR/Fatjets/tau21mod/M/Postcut:plot_fatjet_tau21mod", inputs);
    ph_tau21mod_5.setOutfile(outfile);
    ph_tau21mod_5.setAxis(50, 0., 1.3);
    ph_tau21mod_5.setAxisTitles("Jet #tilde{#tau}_{21}", "Jets");
    ph_tau21mod_5.drawRatioPad(false);
    ph_tau21mod_5.setLuminosity(0.0);
    ph_tau21mod_5.computeImprovement(1);
    //ph_tau21mod_5.setNormalised(true);
    ph_tau21mod_5.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "#rho' #in  [-1, 2]", "M_{jet} #in  [55, 75] GeV"});
    ph_tau21mod_5.draw();
    
    // D2
    PlottingHelper ph_D2_1 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_D2", inputs);
    ph_D2_1.setOutfile(outfile);
    ph_D2_1.setAxis(50, 0., 5.0);
    ph_D2_1.setAxisTitles("Jet D_{2}^{#beta=1}", "Jets");
    ph_D2_1.drawRatioPad(false);
    ph_D2_1.setLuminosity(0.0);
    ph_D2_1.setNormalised(true);
    ph_D2_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_D2_1.draw();

    PlottingHelper ph_D2_2 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_D2", inputs);
    ph_D2_2.setOutfile(outfile);
    ph_D2_2.setAxis(50, 0., 5.0);
    ph_D2_2.setAxisTitles("Jet D_{2}^{#beta=1}", "Jets");
    ph_D2_2.drawRatioPad(false);
    ph_D2_2.setLuminosity(0.0);
    ph_D2_2.setNormalised(true);
    ph_D2_2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "M_{jet} #in  [55, 75] GeV"});
    ph_D2_2.draw();

    PlottingHelper ph_D2_3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_D2", inputs);
    ph_D2_3.setOutfile(outfile);
    ph_D2_3.setAxis(50, 0., 5.0);
    ph_D2_3.setAxisTitles("Jet D_{2}^{#beta=1}", "Jets");
    ph_D2_3.drawRatioPad(false);
    ph_D2_3.setLuminosity(0.0);
    ph_D2_3.computeImprovement(1);
    //ph_D2_3.setNormalised(true);
    ph_D2_3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "M_{jet} #in  [55, 75] GeV"});
    ph_D2_3.draw();

    // D2mod
    PlottingHelper ph_D2mod_1 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_1.setOutfile(outfile);
    ph_D2mod_1.setAxis(50, 0., 7.0);
    ph_D2mod_1.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_1.drawRatioPad(false);
    ph_D2mod_1.setLuminosity(0.0);
    ph_D2mod_1.setNormalised(true);
    ph_D2mod_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_D2mod_1.draw();

    PlottingHelper ph_D2mod_2 ("BoostedJetISR/Fatjets/D2mod/rho/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_2.setOutfile(outfile);
    ph_D2mod_2.setAxis(50, 0., 7.0);
    ph_D2mod_2.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_2.drawRatioPad(false);
    ph_D2mod_2.setLuminosity(0.0);
    ph_D2mod_2.setNormalised(true);
    ph_D2mod_2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "#rho #in  [-6, -1]"});
    ph_D2mod_2.draw();

    PlottingHelper ph_D2mod_3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_3.setOutfile(outfile);
    ph_D2mod_3.setAxis(50, 0., 7.0);
    ph_D2mod_3.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_3.drawRatioPad(false);
    ph_D2mod_3.setLuminosity(0.0);
    ph_D2mod_3.setNormalised(true);
    ph_D2mod_3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "M_{jet} #in  [55, 75] GeV"});
    ph_D2mod_3.draw();

    PlottingHelper ph_D2mod_4 ("BoostedJetISR/Fatjets/D2mod/M/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_4.setOutfile(outfile);
    ph_D2mod_4.setAxis(50, 0., 7.0);
    ph_D2mod_4.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_4.drawRatioPad(false);
    ph_D2mod_4.setLuminosity(0.0);
    ph_D2mod_4.setNormalised(true);
    ph_D2mod_4.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "#rho #in  [-6, -1]","M_{jet} #in  [55, 75] GeV"});
    ph_D2mod_4.draw();

    PlottingHelper ph_D2mod_5 ("BoostedJetISR/Fatjets/D2mod/M/Postcut:plot_fatjet_D2mod", inputs);
    ph_D2mod_5.setOutfile(outfile);
    ph_D2mod_5.setAxis(50, 1.0, 6.0);
    ph_D2mod_5.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph_D2mod_5.drawRatioPad(false);
    ph_D2mod_5.setLuminosity(0.0);
    ph_D2mod_5.computeImprovement(1);
    //ph_D2mod_5.setNormalised(true);
    ph_D2mod_5.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets", "#rho #in  [-6, -1]","M_{jet} #in  [55, 75] GeV"});
    ph_D2mod_5.draw();

    /*
    PlottingHelper ph7 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_rho", inputs);
    ph7.setOutfile(outfile);
    ph7.setAxis(50, -15., 5.0);
    ph7.setAxisTitles("Jet #rho", "Jets");
    ph7.drawRatioPad(false);
    ph7.setLuminosity(0.0);
    ph7.setNormalised(true);
    ph7.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph7.draw();

    PlottingHelper ph8 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_rhoDDT", inputs);
    ph8.setOutfile(outfile);
    ph8.setAxis(50, -10., 5.0);
    ph8.setAxisTitles("Jet #rho^{DDT}", "Jets");
    ph8.drawRatioPad(false);
    ph8.setLuminosity(0.0);
    ph8.setNormalised(true);
    ph8.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph8.draw();
    */

    PlottingHelper ph_rho_1 ("BoostedJetISR/Fatjets/D2mod/rho/Precut:CutVariable", inputs);
    ph_rho_1.setOutfile(outfile);
    ph_rho_1.setAxis(50, -10., 2.);
    ph_rho_1.setAxisTitles("Jet #rho", "Jets");
    ph_rho_1.drawRatioPad(false);
    ph_rho_1.setLuminosity(0.0);
    ph_rho_1.setNormalised(true);
    ph_rho_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_rho_1.draw();
    
    PlottingHelper ph_rhoPrime_1 ("BoostedJetISR/Fatjets/tau21mod/rhoPrime/Precut:CutVariable", inputs);
    ph_rhoPrime_1.setOutfile(outfile);
    ph_rhoPrime_1.setAxis(50, -5., 7.);
    ph_rhoPrime_1.setAxisTitles("Jet #rho'", "Jets");
    ph_rhoPrime_1.drawRatioPad(false);
    ph_rhoPrime_1.setLuminosity(0.0);
    ph_rhoPrime_1.setNormalised(true);
    ph_rhoPrime_1.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph_rhoPrime_1.draw();
    
    outfile->Write();
    outfile->Close();
    
    if (outfile) {
        delete outfile;
        outfile = nullptr;
    }
    
    cout << "---------------------------------------------------------------------" << endl;
    cout << " Done." << endl;
    cout << "=====================================================================" << endl;
    
    return 1;
}

