#include "AnalysisTools/Utilities.h"

namespace AnalysisTools {
  
  TTree* retrieveTree (const std::string& name, TFile* file) {
    TTree* tree = (TTree*) file->Get(name.c_str());
    if (!tree) {
      FCTWARNING("Tree '%s' could not be retrieved from file '%s'.", name.c_str(), file->GetName());
      throw std::exception();
    }
    return tree;
  }
    
}
