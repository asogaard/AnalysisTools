#include "AnalysisTools/Analysis.h"

// For explicit template instantiations.
#include "TLorentzVector.h"
#include "AnalysisTools/PhysicsObject.h"
#include "AnalysisTools/ObjectDefinition.h"
#include "AnalysisTools/EventSelection.h"

namespace AnalysisTools {
    
  // Set method(s).
  template<typename T>
  void Analysis::addSelection (T* selection, const std::string& pattern) {
    DEBUG("Adding selection '%s' to categories with pattern '%s'", selection->name().c_str(), pattern.c_str());
    for (const auto& category : this->categories(pattern)) {
      m_selections[category].emplace_back(makeUniqueMove(new T(*selection)));
      this->grab(m_selections[category].back().get(), category);
    }
    DEBUG("Done");
    return;
  }
  
  void Analysis::addTree (const std::string& pattern, const std::string& name) {
    
    DEBUG("Entering");
    
    // Make sure that an output file exists.
    assert( hasOutput() );
    
    // Move to the directory of the current analysis.
    DEBUG("  Going to '%s'.", this->dir()->GetName());

    // Create a TTree in the current directory.
    for (const auto& category : this->categories(pattern)) {

      // Go to location of current analysis.
      TDirectory* putDir = this->dir();
      putDir->cd();

      // Check whether 'category' subdirectory already exists (it shouldn't).
      bool hasDir = (putDir->GetDirectory(category.c_str()) != nullptr);
      assert( !hasDir );
      
      // Create 'category' subdirectory, and move there.
      putDir = putDir->mkdir(category.c_str());

      // Create output tree for 'category'.
      m_outtree[category] = std::shared_ptr<TTree>(new TTree(name.c_str(), "Physics output tree"));
      m_outtree[category]->SetDirectory(putDir);
    }
    
    DEBUG("Exiting.");
    
    return;
  }
  
  void Analysis::setWeight (const float* weight, const std::string& pattern) {
    for (const auto& category : this->categories(pattern)) {
      m_weight[category] = weight;
    }
    return;
  }
  
  void Analysis::setSumWeights (const float* sum_weights) {
    m_sum_weights = sum_weights;
    return;
  }
  
  
  // Get method(s).
  const std::map<std::string, SelectionPtrs>&  Analysis::selections () const {
    return m_selections;
  }

  const SelectionPtrs& Analysis::selections (const std::string& category) const {
    assert( this->hasCategory(category) );
    return m_selections.at(category);
  }

  void Analysis::clearSelections () {
    m_selections.clear();
    return;
  }
  
  std::shared_ptr<TTree> Analysis::tree (const std::string& category) {
    if (category == "" && this->numCategories() == 1) {
      return trees().begin()->second;
    }
    assert( this->hasCategory(category) );
    return m_outtree.at(category);
  }
  
  const std::map<std::string, std::shared_ptr<TTree> >& Analysis::trees () {
    return m_outtree;
  }
  
  std::shared_ptr<TFile> Analysis::file () {
    assert( m_outfile );
    return m_outfile;
  }
    
  void Analysis::writeTree (const std::string& category) {
    assert( m_outtree.at(category) );
    m_outtree.at(category)->Fill();
    return;
  }
  
  void Analysis::writeTrees () {
    for (const auto& category : this->categories()) {
      writeTree(category);
    }
    return;
  }
  
  
  // High-level management method(s).
  bool Analysis::run (const std::string& category, const unsigned& current, const unsigned& maximum, const int& DSID) {
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
    bool status = run(category);
    DEBUG("Exiting.");
    return status;
  }
  
  bool Analysis::run (const std::string& category, const unsigned& current, const unsigned& maximum) {
    return run (category, current, maximum, -1);
  }
  
  bool Analysis::run (const std::string& category) {
    DEBUG("Entering (actual run method).");
    assert( this->hasCategory(category) );
    bool passed = true;
    for (auto& selection : m_selections.at(category)) {
      DEBUG("  Setting weight.");
      selection->setWeight(m_weight.at(category));
      if (m_sum_weights) {
	selection->setSumWeights(m_sum_weights);
      }
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
  
  void Analysis::print () {
    INFO("");
    INFO("Configuration for analysis '%s':", name().c_str());
    for (const auto& category : this->categories()) {
      INFO("  Category '%s':", category.c_str());
      for (const auto& selection : m_selections.at(category)) {
	selection->print();
      }
    }
    INFO("");
    return;
  }
  
  void Analysis::setup_ () {
    
    // Create a new directory, with the name of the analysis.
    this->setDir( m_outfile->mkdir(this->m_name.c_str()) );
    
    // Add output tree(s) to the newly created directory.
    for (const auto& category : this->categories()) {
      addTree(category);
    }
    
    return;
  }
  
  /// Explicitly instatiate templates.
  template void Analysis::addSelection< PseudoObjectDefinition<TLorentzVector> >(PseudoObjectDefinition<TLorentzVector>*, const std::string&);
  template void Analysis::addSelection< PseudoObjectDefinition<AnalysisTools::PhysicsObject> >(PseudoObjectDefinition<AnalysisTools::PhysicsObject>*, const std::string&);
  template void Analysis::addSelection< ObjectDefinition<TLorentzVector> >(ObjectDefinition<TLorentzVector>*, const std::string&);
  template void Analysis::addSelection< ObjectDefinition<AnalysisTools::PhysicsObject> >(ObjectDefinition<AnalysisTools::PhysicsObject>*, const std::string&);
  template void Analysis::addSelection< EventSelection >(EventSelection*, const std::string&);
  
}
