#include "AnalysisTools/Analysis.h"

// For explicit template instantiations.
#include "TLorentzVector.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/ObjectDefinition.h"
#include "AnalysisTools/EventSelection.h"

namespace AnalysisTools {
    
    // Set method(s).
    template<typename T>
    void Analysis::addSelection (T* selection) { // ISelection*
        m_selections.emplace_back(makeUniqueMove(new T(*selection))); // std::move(std::unique_ptr<ISelection>(new T(*selection))) );
	this->grab(m_selections.back().get());
        //m_selections.push_back( selection );
	//this->grab(selection);
        return;
    }
    
    void Analysis::addTree (const string& name) {

        DEBUG("Entering");

	// Make sure that an output file exists.
        assert( hasOutput() );

	// Move to the directory of the current analysis.
	DEBUG("  Going to '%s'.", this->dir()->GetName());
	this->dir()->cd();

	// Create a TTree in the current directory.
        m_outtree = std::shared_ptr<TTree>(new TTree(name.c_str(), "Physics output tree"));

	DEBUG("Exiting.");

        return;
    }
    
    void Analysis::setWeight (const float* weight) {
        m_weight = weight;
        return;
    }

    
    // Get method(s).
    void Analysis::clearSelections () {
        m_selections.clear();
        return;
    }
    
    std::shared_ptr<TTree> Analysis::tree () {
        assert( m_outtree );
        return m_outtree;
    }
    
    std::shared_ptr<TFile> Analysis::file () {
        assert( m_outfile );
        return m_outfile;
    }


    void Analysis::writeTree () {
        assert( m_outtree );
        m_outtree->Fill();
        return;
    }

    
    // High-level management method(s).
    bool Analysis::run (const unsigned& current, const unsigned& maximum, const int& DSID) {
        DEBUG("Entering.");
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
	DEBUG("  Calling the actual 'run' method.");
        return run();
	DEBUG("Exiting.");
    }
    
    bool Analysis::run (const unsigned& current, const unsigned& maximum) {
        return run (current, maximum, -1);
    }
    
    bool Analysis::run () {
	DEBUG("Entering (actual run method).");
        bool passed = true;
        for (auto& selection : m_selections) {
	    DEBUG("  Setting weight.");
            selection->setWeight(m_weight);
	    DEBUG("  Running %s.", selection->name().c_str());
            passed &= selection->run();
            if (!passed && selection->required()) { break; }
        }
	DEBUG("Exiting (actual run method).");
        return passed;
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
        
        
        m_outfile = std::shared_ptr<TFile>( new TFile(filename.c_str(), "RECREATE"));
        
        //this->m_dir = m_outfile->mkdir(this->m_name.c_str());
	setup_();

        return;
    }

    void Analysis::setOutput (std::shared_ptr<TFile> outfile) {
      m_outfile = outfile;
      setup_();
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

        DEBUG("Entering.");

	// Make sure that an output file exists.
        assert( hasOutput() );

	// Save file.
        m_outfile->Write();

	DEBUG("Exiting.");

        return;
    }

    void Analysis::print () const {
      INFO("");
      INFO("Configuration for analysis '%s':", name().c_str());
      for (const auto& selection : m_selections) {
	selection->print();
      }
      INFO("");
      return;
    }

    void Analysis::setup_ () {

      // Create a new directory, with the name of the analysis.
      this->setDir( m_outfile->mkdir(this->m_name.c_str()) );

      // Add output tree to the newly created directory.
      addTree();

      return;
    }

    /// Explicitly instatiate templates.
    template void Analysis::addSelection< PseudoObjectDefinition<TLorentzVector> >(PseudoObjectDefinition<TLorentzVector>*);
    template void Analysis::addSelection< ObjectDefinition<TLorentzVector> >(ObjectDefinition<TLorentzVector>*);
    template void Analysis::addSelection< EventSelection >(EventSelection*);
    
}
