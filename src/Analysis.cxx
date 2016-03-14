#include "AnalysisTools/Analysis.h"

namespace AnalysisTools {
    
    // Set method(s).
    // ...
    
    
    // Get method(s).
    void Analysis::clearSelections () {
        m_selections.clear();
        return;
    }
    
    void Analysis::addSelection (ISelection* selection) {
        cout <<  "<Analysis::addSelection> Entering." << endl;
        m_selections.push_back( selection );
        this->grab(selection);
        cout << "<Analysis::addSelection> Exiting." << endl;
        return;
    }
    
    
    // High-level management method(s).
    void Analysis::run (const unsigned& current, const unsigned& maximum, const int& DSID) {
        // * Progress bar.

        int barWidth = 68;
        float progress = ((float) current / (float) (maximum - 1));
        
        if (progress == 0) {
            m_start = std::clock();
        }
        
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
            std::cout << "] " << maximum;
            std::cout << " | " << std::setprecision(3) << (stop - m_start) / (double)(CLOCKS_PER_SEC) << " s";
            std::cout << " | " << std::setprecision(3) << (stop - m_start) / (double)(CLOCKS_PER_SEC) / 1000. / double(maximum) << " ms/evt" << endl;
        }
        
        // * Running.
        run();
    }
    
    void Analysis::run (const unsigned& current, const unsigned& maximum) {
        run (current, maximum, -1);
        return;
    }
    
    void Analysis::run () {
        cout << "<Analysis::run> Entering." << endl;
        int it = 0;
        for (ISelection* selection : m_selections) {
            cout << "<Analysis::run>   Running selection " << ++it << "/" << m_selections.size() << endl;
            selection->run();
        }
        cout << "<Analysis::run> Exiting." << endl;
        return;
    }
    
    void Analysis::openOutput  (const string& filename) {
        /* Perform checks. */
        /* Allow for adding to another file? */
        /* Separate histogram and physics output? */
        m_outfile = new TFile(filename.c_str(), "RECREATE");
        
        this->m_dir = m_outfile->mkdir(this->m_name.c_str());

        return;
    }
    
    void Analysis::closeOutput () {
        if (m_outfile) {
            m_outfile->Close();
            delete m_outfile;
            m_outfile = nullptr;
        }
        return;
    }
    
    bool Analysis::hasOutput () {
        return (bool) m_outfile;
    }
    
    void Analysis::save () {
        assert( hasOutput() );
        for (auto selection : m_selections) {
            selection->write();
        }
        m_outfile->Write();
        return;
    }
  
     vector< TNtuple* > Analysis::ntuples () {
        vector< TNtuple* > ntuples;
        for (ISelection* selection : m_selections) {
            vector< TNtuple* > newNtuples = selection->ntuples();
            ntuples.insert( ntuples.end(), newNtuples.begin(), newNtuples.end() );
        }
        return ntuples;
    }
    
}