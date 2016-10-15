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
    */
    PlottingHelper ph2 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_m", inputs);
    ph2.setOutfile(outfile);
    ph2.setAxis(100, 0., 160.);
    ph2.setAxisTitles("Jet M [GeV]", "Jets");
    ph2.drawRatioPad(false);
    ph2.setLuminosity(0.0);
    ph2.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"/* ,"M_{jet} #in  [55, 75] GeV"*/});
    ph2.draw();

    PlottingHelper ph3 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_tau21", inputs);
    ph3.setOutfile(outfile);
    ph3.setAxis(50, 0., 1.3);
    ph3.setAxisTitles("Jet #tau_{21}", "Jets");
    ph3.drawRatioPad(false);
    ph3.setLuminosity(0.0);
    ph3.setNormalised(true);
    ph3.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets" ,"M_{jet} #in  [55, 75] GeV"});
    ph3.draw();
    
    PlottingHelper ph4 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_D2", inputs);
    ph4.setOutfile(outfile);
    ph4.setAxis(50, 0., 5.0);
    ph4.setAxisTitles("Jet D_{2}^{#beta=1}", "Jets");
    ph4.drawRatioPad(false);
    ph4.setLuminosity(0.0);
    ph4.setNormalised(true);
    ph4.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets" ,"M_{jet} #in  [55, 75] GeV"});
    ph4.draw();

    PlottingHelper ph5 ("BoostedJetISR/Fatjets/Nominal/M/Postcut:plot_fatjet_D2mod", inputs);
    ph5.setOutfile(outfile);
    ph5.setAxis(50, 0., 7.0);
    ph5.setAxisTitles("Jet #tilde{D}_{2}^{#beta=1}", "Jets");
    ph5.drawRatioPad(false);
    ph5.setLuminosity(0.0);
    ph5.setNormalised(true);
    ph5.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"/*, "#rho #in  [-6, -1)"*/ ,"M_{jet} #in  [55, 75] GeV"});
    ph5.draw();

    /*
    PlottingHelper ph6 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_rho", inputs);
    ph6.setOutfile(outfile);
    ph6.setAxis(50, -15., 5.0);
    ph6.setAxisTitles("Jet #rho", "Jets");
    ph6.drawRatioPad(false);
    ph6.setLuminosity(0.0);
    ph6.setNormalised(true);
    ph6.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph6.draw();

    PlottingHelper ph7 ("BoostedJetISR/Fatjets/Nominal/dPhi/Postcut:plot_fatjet_rhoDDT", inputs);
    ph7.setOutfile(outfile);
    ph7.setAxis(50, -10., 5.0);
    ph7.setAxisTitles("Jet #rho^{DDT}", "Jets");
    ph7.drawRatioPad(false);
    ph7.setLuminosity(0.0);
    ph7.setNormalised(true);
    ph7.setPrintLines({"Z'(q#bar{q}) + ISR #gamma", "Trimmed anti-k_{t}^{R=1.0} jets"});
    ph7.draw();
    */
    
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

