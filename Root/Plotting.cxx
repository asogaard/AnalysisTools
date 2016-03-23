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
    TFile* outfile = new TFile("plotting.root", "RECREATE");
    
    PlottingHelper ph ("ResolvedWR/EventSelection/CRL_OS_ee/Zlep_selection/Postcut:CutVariable", inputs); // Mlljj
    ph.setOutfile(outfile);
    ph.setAxis(40, 0., 180.);
    ph.setAxisTitles("M_{lljj} [GeV]", "Events");
    ph.drawRatioPad(true);
    ph.setLuminosity(3.2);
    ph.setPrintLines({"Leptonic control region", "Opposite sign ee"});
    ph.draw();
    
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

