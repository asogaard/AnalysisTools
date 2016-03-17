#include "AnalysisTools/Analysis.h"

namespace AnalysisTools {
    
    // Set method(s).
    void Analysis::addSelection (ISelection* selection) {
        m_selections.push_back( selection );
        this->grab(selection);
        return;
    }
    
    void Analysis::addTree (const string& name) {
        assert( hasOutput() );
        m_outfile->cd();
        m_outtree = new TTree(name.c_str(), "Physics output tree");
        return;
    }

    
    // Get method(s).
    void Analysis::clearSelections () {
        m_selections.clear();
        return;
    }
    
    TTree* Analysis::tree () {
        assert( m_outtree );
        return m_outtree;
    }

    void Analysis::writeTree () {
        assert( m_outtree );
        m_outtree->Fill();
        return;
    }

    
    // High-level management method(s).
    bool Analysis::run (const unsigned& current, const unsigned& maximum, const int& DSID) {
        // * Progress bar.

        int barWidth = 67;
        
        float progress = 0., prevProgress = 0.;
        if (maximum == 1) {
            progress = 1;
        } else {
            prevProgress = ((float) (current - 1) / (float) (maximum - 1));
            progress     = ((float) current / (float) (maximum - 1));
        }
        
        bool update = (progress == 1) || (progress == 0) || (int(progress*100.) != int(prevProgress*100.)) || (int(barWidth*progress) != int(barWidth*prevProgress));
        
        if (progress == 0) {
            m_start = std::clock();
        }
        
        if (update) {
            if (DSID > 0) {
                cout << DSID << " | ";
                barWidth -= 9;
            }
            if (progress < 1) {
                std::cout << "[";
                if (progress < 0.33) {
                    cout << "\033[0;31m";
                } else if (progress < 0.66) {
                    cout << "\033[0;33m";
                } else  {
                    cout << "\033[0;32m";
                }
                int pos = barWidth * progress;
                for (int i = 0; i < barWidth; ++i) {
                    if (i < pos) std::cout << "=";
                    else if (i == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::cout << "\033[0m";
                std::cout << "] " << int(progress * 100.0) << " %\r";
                std::cout.flush();
            } else {
                std::cout << "[";
                int pos = barWidth * progress;
                for (int i = 0; i < barWidth; ++i) {
                    if (i < pos) std::cout << "=";
                    else if (i == pos) std::cout << ">";
                    else std::cout << " ";
                }
                std::clock_t stop = std::clock();
                printf("] %7d | %6.1f s | %5.3f ms/evt\n", maximum, (stop - m_start) / (double)(CLOCKS_PER_SEC), (stop - m_start) / (double)(CLOCKS_PER_SEC) * 1000. / double(maximum));
            }
        }
        
        // * Running.
        return run();
    }
    
    bool Analysis::run (const unsigned& current, const unsigned& maximum) {
        return run (current, maximum, -1);;
    }
    
    bool Analysis::run () {
        bool status = true;
        for (ISelection* selection : m_selections) {
            status &= selection->run();
            if (!status) { break; }
        }
        return status;
    }
    
    void Analysis::openOutput  (const string& filename) {
        /* Perform checks. */
        /* Allow for adding to another file? */
        /* Separate histogram and physics output? */
        
        if (strcmp(filename.substr(0,1).c_str(), "/") == 0) {
            cout << "WARNING: File '" << filename << "' not accepted. Only accepting realtive paths." << endl;
            return;
        }
        
        if (filename.find("/") != string::npos) {
            string dir = filename.substr(0,filename.find_last_of("/")); // ...
            if (!dirExists(dir)) {
                cout << "WARNING: Directory '" << dir << "' does not exist. Creating it." << endl;
                system(("mkdir -p " + dir).c_str());
            }
        }
        
        
        m_outfile = new TFile(filename.c_str(), "RECREATE");
        
        this->m_dir = m_outfile->mkdir(this->m_name.c_str());

        return;
    }
    
    void Analysis::closeOutput () {
        if (m_outfile) {
            m_outfile->Close();
            m_outfile = nullptr;
        }
        return;
    }
    
    bool Analysis::hasOutput () {
        return (bool) m_outfile;
    }
    
    void Analysis::save () {
        assert( hasOutput() );
        m_outfile->Write();
        return;
    }
    
}