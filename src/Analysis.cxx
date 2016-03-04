#include "AnalysisTools/Analysis.h"

namespace AnalysisTools {
    
    // Set method(s).
    // ...
    
    
    // Get method(s).
    string Analysis::name () const {
        return m_name;
    }
    
    void Analysis::clearSelections () {
        m_selections.clear();
        return;
    }
    
    void Analysis::addSelection (ISelection* selection) {
        m_selections.push_back( selection );
        this->grab(selection);
        return;
    }
    
    
    // High-level management method(s).
    void Analysis::run () {
        for (ISelection* selection : m_selections) {
            selection->run();
        }
        return;
    }
    
    void Analysis::openOutput  (const string& filename) {
        /* Perform checks. */
        /* Allow for adding to another file? */
        /* Separate histogram and physics output? */
        m_outfile = new TFile(filename.c_str(), "RECREATE");
        
        m_dir = m_outfile->mkdir(m_name.c_str());

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
    
    void Analysis::grab (ISelection* selection) {
        selection->lock();
        assert(m_dir);
        selection->setDir( m_dir->mkdir(selection->name().c_str()) );
        for (auto& cut : selection->listCuts()) {
            selection->grab(cut);
        }
        return;
    }
    
    vector< TH1F* > Analysis::histograms () {
        vector< TH1F* > hists;
        for (ISelection* selection : m_selections) {
            vector< TH1F* > newHists = selection->histograms();
            hists.insert( hists.end(), newHists.begin(), newHists.end() );
        }
        return hists;
    }
    
}